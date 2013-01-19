// This shows shell for ratos.
#include "shell.h"
#include "common.h"
#include "screen.h"

void ShowShell()
{
	printf("root# ");

}
void ShowShell2()
{
	printf("child# ");

}

void DoPing(char *dstIP)
{
    unsigned char IP1[4] = {10,0,2,2};
    SendPingRequest(IP1, dstIP);
}

void Help()
{
	printf("This is the minimum os I am writing ( rajnesh raturi ) . Right now it has following feature -\n");
	printf("Networking -\n");
	printf("To test run followng command on fresh line\n");
	printf("ping 10.0.2.21");
	printf("To clear screen run clear\n");
	printf("To see all process run ps, but it is broken.\n");
	printf("To test fork run fork\n");
	printf("to reschedule process run schedule\n");
	printf("to see kernel message run dmesg\n");
	printf("It has vm support to but multitsking is not working when it is enabled\n");
	printf("For more queries mail me at rajneshraturi25@gmail.com\n");
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
		if(!strncmp(command, "ps", 2)){
			DoPs();
		}
		if(!strncmp(command, "help", 2)){
			Help();
		}
		if(!strncmp(command, "malloc", 6)){
			int n = 100000;
			char  *a = kmalloc(n);
			int i;
			for(i=0;i<n;i++){
				*a = 12;
				a++;
			}
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
	if(a == 0){
		printf("In child\n");
//		ShowShell2();
	}
	else{	
//		ShowShell();
	}
	printf("fork return val = %d and old %d\n",a,old);
		//	clr();
		}
		ShowShell();
		if(!strncmp(command, "ping", 4)){
			char dstIP[8];
            GetDstFromStr(command, dstIP);
		    DoPing(dstIP);
		}
		i=0;
	}
	else{
		command[i]=ch;
		i++;
	}
	}

}
