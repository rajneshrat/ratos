#include "pci.h"
#include "rtl8139.h"
#include "screen.h"
#include "isr.h"

#define CONFIG1_REGISTER 0x52
#define COMMANDREGISTER  0x37 
	
char rx_buffer[8208]; 
 
static uint32 BaseAddress=0x0;

//   for RTL8139  network driver -----
#define BIT0    0x1
#define BIT1    0x2
#define BIT2    0x4
#define BIT3    0x8
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80
#define BIT8    0x100
#define BIT9    0x200
#define BIT10   0x400
#define BIT11   0x800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000
#define BIT16   0x10000
#define BIT17   0x20000
#define BIT18   0x40000
#define BIT19   0x80000
#define BIT20   0x100000
#define BIT21   0x200000
#define BIT22   0x400000
#define BIT23   0x800000
#define BIT24   0x1000000
#define BIT25   0x2000000
#define BIT26   0x4000000
#define BIT27   0x8000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

#define CR      0x37
#define CR_RST          BIT4
#define CR_RE           BIT3
#define CR_TE           BIT2
#define TCR     0x40
#define RCR     0x44
#define TCR_IFG1        BIT25
#define TCR_IFG0        BIT24
#define TCR_MXDMA2      BIT10
#define TCR_MXDMA1      BIT9
#define TCR_MXDMA0      BIT8
#define RCR_RBLEN0      BIT11
#define RCR_MXDMA2      BIT10
#define RCR_MXDMA1      BIT9
#define RCR_AB          BIT3
#define RCR_AM          BIT2
#define RCR_APM         BIT1
#define RCR_AAP         BIT0
#define RBSTART 0x30
#define IMR     0x3c
#define R39_INTERRUPT_MASK      0x7f
unsigned long   RxBufferPhysicalAddress;
#define PCI_INTERRUPT_PIN    0x3d  
char *Rx_Desc;
struct Tx_Desc_Com
{
	uint16 command : 16;
	uint16 length : 16;
	uint16 Rsvd : 16;
	uint16 vlan:16;
	uint32 TxBufLow;
	uint32 TxBufHigh;
};
uint32 rtl8139_rx_buffer;
struct Tx_Desc_Com TxDesc;

void DumpReceiveReg(char *buf)
{
	int i;
	for(i=0;i<40;i++)
	{
		printf("-%x",buf[i]);
		buf[i] = 0x0;
	}
}

void Rtl8139Interrupt(registers_t *regs)
{
   printf("Rtl interrupt called\n");
	outw(BaseAddress + 0x3c, 0);
   uint32 reg2 = inw(BaseAddress + 0x003E);
	printf("Status of Interrupt Status = 0x%x\n", reg2);
   outw(BaseAddress + 0x003E, reg2);              //reset
//	reg2 = inw(BaseAddress + 0x0037);
//	printf("Status of Interrupt Status = 0x%x\n", reg2);
  // outw(BaseAdress + 0x38,  - 16);
   int i;
//	char buf[1000];
//	for(i=0;i<100;i=i+2)
	{
//		buf[i] = 0xa;
//		buf[i] = 0xac;
//		buf[i+1] = 0xb;
   }
//   TestReceive();
		PushPacketToEth(rx_buffer);
      printf("\n\n");
	//	DumpReceiveReg(rx_buffer);
//   while ((inb(BaseAddress + 0x37) & 0x01) == 0){
//		TestReceive();
      outw((BaseAddress + 0x0038), 0);
//		printf("Wait\n");
  // DumpReceiveReg(buf);
//	outw(BaseAddress + 0x3c, 0xff);
    outw((BaseAddress + 0x3c), 0xe1ff);  // TOK + ROK
   printf("\n");
//	DumpReceiveReg(Rx_Desc);
 //  outb(0x70, 0x0c);   // select RTC status register C
//int val = inb(0x71);
}

void GetInterruptPin()
{
	uint32 InterruptReg  = ReadConfigurationDword(0, 3, 0x3c);
	uint8 InterruptPin = (InterruptReg > 8)&0xff;
	uint8 InterruptLine = (InterruptReg)&0xff;
	printf("Interrupt Details %x interrupt pin %x interrupt line %x\n",InterruptReg, InterruptPin, InterruptLine);
	attachirqhandler(Rtl8139Interrupt, InterruptLine + 32);	 // I have done remapping so have to add 32.
}

char TxBuffer[1300];

void Send()
{
	TxDesc.length = 100;
	TxDesc.TxBufLow = (uint32)TxBuffer;   	
	TxDesc.command = 0x800f; 
   outl((BaseAddress + 0x20),(uint32)&TxDesc);
   outl((BaseAddress + 0x10), (uint32)&TxDesc);
 //  DumpReceiveReg((char *)&TxDesc);
}

void TestReceive()
{
		printf("struck \n ");
   // rtl8139_rx_buffer = imalloc(1000);
    uint32 RTL8139_COMMAND = 0x37;
    outb(BaseAddress + CR, CR_RST);              //reset
	// Reset Packet
    //power on 
    outb( (uint16) (BaseAddress + 0x52), 0x00);
   //reset 
   outb( (uint16) (BaseAddress + RTL8139_COMMAND), 0x10);
    while ( (inb( (uint16) (BaseAddress + RTL8139_COMMAND)) & 0x10) != 0 ) {
    	printf("In Reset\n");
    }
   outw((BaseAddress + 0x03e), 0xe1ff);
   outw((BaseAddress + 0x0038), 0);
    // Setup receive buffer location
    outl((BaseAddress + 0x30), (uint32)rx_buffer );
    Rx_Desc = imalloc(10240);
	 Rx_Desc = (int)Rx_Desc & 0xffffff80;
    outl((BaseAddress + 0xe4), (uint32)Rx_Desc );
    outl((BaseAddress + 0x20),(uint32)&TxDesc );
    // Buffer type
    // IRQ
    outw((BaseAddress + 0x3c), 0xe1ff);  // TOK + ROK
    // Enable receive

   //outb( (BaseAddress + 0x37), 0x04 );
   //outl((BaseAddress + 0x30), 0xe1ff);  // TOK + ROK
   //outb( (BaseAddress + 0x37), 0x0c );
   
  
//   outl((BaseAddress + 0x44), 0xF);// | (1<<7) );

//   while(1) 
   {
      outl((BaseAddress + 0x44), 0xa);// | (1<<7) );
      //outw((BaseAddress + 0x43), 0x2FF);// | (1<<7) );
	   uint32 reg2 = inl(BaseAddress + 0x44);
      if ( reg2 ){
		//	break;
      }
		printf("struck %x\n", reg2);
    outb( (BaseAddress + RTL8139_COMMAND), 0x0C );
		//while(1);
//      outb( (uint16) (BaseAddress + RTL8139_COMMAND), 0x10);
   }
//   Send();
//   Send();
}


void CheckBuffer()
{
	int i;
//	return 0;
	while(1)
	{
	     static uint32 last = 122;
	     uint32 PciCommandReg = ReadConfigurationDword(0, 3, 0x04);
	     if(PciCommandReg != last)
		{
				printf("Found something 0x%x\n", PciCommandReg);
		}
		buzywait(10);	
		last = PciCommandReg;
		for(i=0;i<1000;i++)
		{
			if(rx_buffer[i]!=0)
			{
	//			printf("Found something\n");
			}
		}	
	}
}	

	
#define  PCI_PM_CTRL_STATE_MASK 0x0003 

void DetectRTL8139()
{
    uint16 vendorid = 0x10ec;
    uint16 deviceid = 0x8139;
    int present = checkpcitable(deviceid, vendorid);
    if(present)
    {
        printf(" Detected RTL8139 Network Driver\n");
	    BaseAddress = ReadConfigurationDword(0, 3, 0x10);
       WriteConfigurationDword(0,3,0x10,~0);
		 uint32 size = ReadConfigurationDword(0, 3, 0x10);
		 WriteConfigurationDword(0,3,0x10,BaseAddress);

      
	     uint32 PciCommandReg = ReadConfigurationDword(0, 3, 0x04);
		printf("Pci status and Command = %x  Register Base = %x size = %x\n", PciCommandReg, BaseAddress, size);	
	      //  uint32 state = PciCommandReg &  PCI_PM_CTRL_STATE_MASK;
//		printf(" State = %x", state);
	//	PciCommandReg = PciCommandReg & 0xffff;
	//	PciCommandReg = 0x01103;
	        //WriteConfigurationDword(0, 3, 0x04, 0x00080103);	
	     //   WriteConfigurationWord(0, 3, 0x14, 0x0b1);	
	     //   PciCommandReg = ReadConfigurationDword(0, 3, 0x37);//0000 0014h

//		printf("New Pci Command Register %x\n", PciCommandReg);	
		if( (BaseAddress & 0x1) == 1)
		{
			printf("Detected I/O type \n");
			BaseAddress = BaseAddress & 0xfffffffc ;
		}
		else
		{
			printf("Detected unknown will do dump\n");
		}
		GetMacAddress();
		GetInterruptPin();
		TestReceive();
  //              install_RTL8139();
//		NewTestReceive();
		CheckBuffer();
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

