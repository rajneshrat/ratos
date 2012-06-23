#include "common.h"
#include "screen.h"
int CurrentDebugLevel = 0;
uint16 *video = (uint16 *) 0xB8000;
static uint32 screenx = 0;
static uint32 screeny = 0;
#define SCREENWIDTH 80
#define SCREENLENGTH 25
#define CursorBlinkFrequency 5

void BlinkCursor()
{
	static uint32 counter = 0;
	if( counter == CursorBlinkFrequency)
		DeleteCursor();
	if( counter >= 2*CursorBlinkFrequency)
	{
		ShowCursor();
		counter = 0;
	}
	counter ++;
}

void SetCurrentDebugLevel(int newlevel)
{
	CurrentDebugLevel = newlevel;
}


void scroll(int len)
{
   int i,j;
	int screenpos = 0;
	uint16 screenvalue = 0;
	uint8 background = 0;
	uint8 foreground = 7;
	for(i=0;i<SCREENLENGTH - len;i++){
		for(j=0;j<SCREENWIDTH;j++){
	       video[screenpos] = video[screenpos + len*SCREENWIDTH];
			 screenpos++;
		}
	}
	for(i=SCREENLENGTH -len;i<SCREENLENGTH;i++){
		for(j=0;j<SCREENWIDTH;j++){
			screenvalue = background<<12;
			screenvalue = screenvalue | foreground<<8;
			screenvalue = screenvalue | 32;
	      video[screenpos] = screenvalue;
			screenpos++;
		}
	}
	screenx = 0;
	screeny = SCREENLENGTH - len;
}


void ShowCursor()
{
	int screenpos = screenx + screeny*SCREENWIDTH;
	uint16 screenvalue = 0;
	uint8 background = 0;
	uint8 foreground = 7;
	screenvalue = background<<12;
	screenvalue = screenvalue | foreground<<8;
	screenvalue = screenvalue | '_';
	video[screenpos] = screenvalue;//ch | attribute;//screenvalue;
}

void DeleteCursor()
{
	int screenpos = screenx + screeny*SCREENWIDTH;
	uint16 screenvalue = 0;
	uint8 background = 0;
	uint8 foreground = 7;
	screenvalue = background<<12;
	screenvalue = screenvalue | foreground<<8;
	screenvalue = screenvalue | 32;
	video[screenpos] = screenvalue;//ch | attribute;//screenvalue;
}
	
void putch(char ch)
{
	if( screeny == SCREENLENGTH )
        	scroll(1);
	int screenpos = screenx + screeny*SCREENWIDTH;
	uint16 screenvalue = 0;
	uint8 background = 0;
	uint8 foreground = 7;
	screenvalue = background<<12;
	screenvalue = screenvalue | foreground<<8;
   if(ch == 40 )
	{
	   screenvalue = screenvalue | 32;
	   video[screenpos] = screenvalue;//ch | attribute;//screenvalue;
		screenx = 0;
		screeny++;
		return;
	}	
   if(ch == 8 )
	{
	   screenvalue = screenvalue | 32;
	   video[screenpos] = screenvalue;//ch | attribute;//screenvalue;
		if( screenx > 1)
			screenx--;
		else
		{
		   screeny--;
		   screenx = SCREENWIDTH;
		}
		return;
	}	
   if(ch == 10 )
	{
	   screenvalue = screenvalue | 32;
	   video[screenpos] = screenvalue;//ch | attribute;//screenvalue;
		screenx = 0;
		screeny++;
		return;
	}	
	screenvalue = screenvalue | ch;
	video[screenpos] = screenvalue;//ch | attribute;//screenvalue;
	screenx++;
	if(screenx > SCREENWIDTH ){
		screenx = 1;
		screeny++;
	}
}

void clr()
{
	int i,j;
	screenx = 0;
	screeny = 0;
	for(i=0;i<SCREENLENGTH;i++){
		for(j=0;j<SCREENWIDTH;j++){
			putch(32);
		}
	}
	screenx = 0;
	screeny = 0;
}

void puts(char *str)
{
	int i = 0;
	while(str[i]!='\0'){
		putch(str[i]);
		i++;
	}
}

int reverseint(int num)
{
	int temp = 0 ;
	while(num)
	{
		temp = temp + num % 10;
		temp = temp * 10;
		num = num / 10;
	}
	return temp/10;
}
	
void putint(int num)
{
	int i = 0,j;
        char arr[30];
        if( num == 0 )
	{
		putch('0');
	}
	while( num )
	{
		arr[i] = num%10 + 48;
                i++;
                num = num/10;
        }
        for(j=i-1;j>=0;j--) 
        {
                putch(arr[j]);
	}
}

void puthex(uint32 num)
{
   int a, i = 0, j;
   char str[32];
   char ch;
   while(num)
   {
       a = num%16;
       if(a>=10)
       {
           ch = 'a' + a-10;
           str[i] = ch;
       }
       else
       {
           ch = '0' + a ;
           str[i] = ch;
       } 
       num = num/16;
       i++;
   }
   for(j=0;j<i;j++)
   {
      putch(str[i-j-1]);
   }
}

int printk(int level, char * message)
{
	if( level >= CurrentDebugLevel )
	{
		puts(message);
	}
}
