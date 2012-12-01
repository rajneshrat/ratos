#include "log.h"

static uint32 GlobalTicks;
static uint32 totalLogs = 0;

struct Log{
	uint32 time;
	char Message[100];
};

typedef struct Log KernelLog;

KernelLog kernelLogs[100];

void AddTicks(uint32 ticks)
{
	GlobalTicks = GlobalTicks + ticks;
}

uint32 GetTicks()
{
	return GlobalTicks;
}

void AddKernelMesg(char *message)
{
	if(totalLogs<100){
		kernelLogs[totalLogs].time = GetTicks;
		copy(kernelLogs[totalLogs].Message,message,100);
		totalLogs++;
	}
}

void Dodmesg()
{
	int i;
	for(i=0;i<totalLogs;i++){
		printf("%d :  %s\n",kernelLogs[i].time, kernelLogs[i].Message);
	}
}

void printk(char *format, ...)
{
	uint32 *stackAddress = &format;
	int i=0;
	char buf[30];
	int pos = 0;
	char finalBuffer[100];
	while(format[i]!='\0'){
		if(format[i]=='%'){
			i++;
			switch(format[i]){
				case 'd': stackAddress++;
					itoa (buf, 10, *stackAddress);
					copy(finalBuffer+pos,buf,strlen(buf));
					pos = pos+strlen(buf);
					i++;
					break;
				case 'c': stackAddress++;
					finalBuffer[pos] = *stackAddress;
					pos++;
					i++;
					break;
				case 's': stackAddress++;
					copy(finalBuffer+pos, *stackAddress, strlen(*stackAddress));
					pos = pos+strlen(*stackAddress);
					i++;
					break;
				case 'x': stackAddress++;
					itoa (buf, 'x', *stackAddress);
					copy(finalBuffer+pos,buf,strlen(buf));
					pos = pos+strlen(buf);
					i++;
					break;
			}
		}
		else{
			finalBuffer[pos] = format[i];
			pos++;
			i++;
		}
	}
	finalBuffer[pos] = '\0';
    AddKernelMesg(finalBuffer);
}
