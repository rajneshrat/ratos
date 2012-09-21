#include "pci.h"
#include "network.h"
#include "screen.h"

#define CONFIG1_REGISTER 0x52
#define COMMANDREGISTER  0x37 
	
char rx_buffer[8192+16]; // declare the local buffer space (8k + header)
 
static uint32 BaseAddress=0x0;

//   for RTL8139  network driver -----


void GetInterruptPin()
{
	uint32 InterruptReg  = ReadConfigurationDword(0, 3, 0x3c);
	uint8 InterruptPin = (InterruptReg > 8)&0xff;
	uint8 InterruptLine = (InterruptReg)&0xff;
	printf("Interrupt Details %x %x imp = %x\n",InterruptReg, InterruptPin, InterruptLine);}

void TestReceive()
{
	outb( BaseAddress + CONFIG1_REGISTER, 0x00);
	outb( BaseAddress + COMMANDREGISTER, 0x10);
    outl( BaseAddress +  0x30, (unsigned long)rx_buffer); 
	outl(BaseAddress + 0x44, 0xf | (1 << 7));
	outb(BaseAddress + 0x37, 0x0C);
	outw(BaseAddress + 0x3C, 0x0005);
}

void CheckBuffer()
{
	int i;
	while(1)
	{
		for(i=0;i<1000;i++)
		{
			if(rx_buffer[i]!=0)
			{
				printf("Found something\n");
			}
		}	
	}
}	
	
void DetectRTL8139()
{
    uint16 vendorid = 0x10ec;
    uint16 deviceid = 0x8139;
    int present = checkpcitable(deviceid, vendorid);
    if(present)
    {
        printf(" Detected RTL8139 Network Driver\n");
	    BaseAddress = ReadConfigurationDword(0, 3, 0x10);
		BaseAddress = BaseAddress -1; // there is some bug need to decrease base by 1
		GetMacAddress();
		GetInterruptPin();
//		TestReceive();
//		CheckBuffer();
    }
}

void GetMacAddress()
{
//	printf("%x\n", RegNo10[0]);
	unsigned int i;
	uint8 Mac;
    for(i=0;i<6;i++)
	{
        Mac = inb(BaseAddress + i);
		printf("%x\n",Mac);
	}
}

void DetectNetworkDevices()
{
    DetectRTL8139();
}
