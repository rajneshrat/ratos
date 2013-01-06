#include "shell.h"
#include "common.h"
#include "screen.h"

void ShowShell()
{
	printf("root# ");

}

void DoPing(char *dstIP)
{
    unsigned char IP1[4] = {10,0,2,2};
    SendPingRequest(IP1, dstIP);
}

void GetDstFromStr(char *command, char *IP)
{
	int i=0;
	int l=0;
	int num=0;
	command = command + 4;
	while( command[i]!='\0'){
		if(command[i]==' '){
			i++;
			continue;
		}
		if(command[i]=='.'){
			IP[l++] = num;
			i++;
			num = 0;
			continue;
		}
		num=num*10+command[i]-48;
		i++;
	}
	IP[l++] = num;
	IP[l]="\0";
	return IP;
}

void DoShellProcess()
{
	char command[100];
	static int i = 0;
	char ch;
	int ret = 0;
	int lastret = 0;
	while(1){
		while(ret==lastret){
			ret = GetLastChar(&ch);
		}
		lastret = ret;
	if(ch==10){
		command[i]='\0';
	//	if(!strcmp(command, "ps")){
//		}
		if(!strncmp(command, "clear", 5)){
			clr();
		}
		if(!strncmp(command, "dmesg", 5)){
			Dodmesg();
		}
		if(!strncmp(command, "schedule", 8)){
		     PremptProcessQueue();
//			Dodmesg();
		}
		if(!strncmp(command, "check", 5)){
	 		check(6);
		}
		if(!strncmp(command, "fork", 4)){
		int old = 81;
	int a = fork();
	printf("fork return val = %d and old %d\n",a,old);
		//	clr();
		}
		if(!strncmp(command, "ping", 4)){
			char dstIP[8];
            GetDstFromStr(command, dstIP);
		    DoPing(dstIP);
		}
		ShowShell();
		i=0;
	}
	else{
		command[i]=ch;
		i++;
	}
	}

}
