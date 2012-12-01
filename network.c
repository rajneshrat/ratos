#include "pci.h"
#include "network.h"
#include "screen.h"

#define CONFIG1_REGISTER 0x52
#define COMMANDREGISTER  0x37 
	
char rx_buffer[8192+16]; // declare the local buffer space (8k + header)
 
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

void GetInterruptPin()
{
	uint32 InterruptReg  = ReadConfigurationDword(0, 3, 0x3c);
	uint8 InterruptPin = (InterruptReg > 8)&0xff;
	uint8 InterruptLine = (InterruptReg)&0xff;
	printf("Interrupt Details %x interrupt pin %x interrupt line %x\n",InterruptReg, InterruptPin, InterruptLine);
}

/*
void NewTestReceive()
{
    outb(BaseAddress + CR, CR_RE | CR_TE);       //enable Tx/Rx
    outw(BaseAddress + IMR, 0x1 |  0x8000 | 0x4000 | R39_INTERRUPT_MASK);//enable interrupt
    int rx_mode = 0x08 | 0x04 | 0x02 | 0x01;
	uint32 tmp = rx_mode;
      //  if (cp->rx_config != tmp) {
          outl(BaseAddress + RCR, tmp);
        //ratchanges    cp->rx_config = tmp;
        //}    
    
}
*/

void TestReceive()
{
       RxBufferPhysicalAddress = rx_buffer;//imalloc(1000);
       outb(BaseAddress + CR, CR_RST);              //reset
    //  printf("Before wait\n");
//	buzywait(100);
  //    printf("After wait\n");

/*	 // Check that the chip has finished the reset
  int i;
	for (i = 1000; i > 0; i--) {
	printf("Waiting\n");
    if ((inb(BaseAddress + 0x37) & 0x10) == 0) break;
  }
	  outb(BaseAddress + CR, CR_RE | CR_TE);       //enable Tx/Rx
//        outw(BaseAddress + TCR,    TCR_IFG0   |
  //                              TCR_IFG1   |
    //                            TCR_MXDMA2 |
      //                          TCR_MXDMA1);

        outw(BaseAddress + RCR,    RCR_RBLEN0 |
                                RCR_MXDMA2 |
                                RCR_MXDMA1 |
                                RCR_AB     |
                                RCR_AM     |
                                RCR_APM |
				RCR_AAP
				);
        outw(BaseAddress + RBSTART , RxBufferPhysicalAddress);
        outw(BaseAddress + IMR, 0x8000 | 0x4000 | 0x08 | 0x04 | 0x02 | 0x01 | R39_INTERRUPT_MASK);//enable interrupt

*/
//start 
  
        uint32       IntrMask        = 0x3C;
        uint32 RxOK            = (1 << 0);
	uint32 RxOn            = (1 << 3);
	uint32 TxOn            = (1 << 2);
        uint32 Cmd             = 0x37;
        uint32 RxConfig        = 0x44;	
        outw(BaseAddress + IntrMask, RxOK);
        outb(BaseAddress + Cmd, RxOn | TxOn);
        uint32 tmp = 0x08 | 0x04 | 0x02 | 0x01;
        outl (BaseAddress + RxConfig, tmp);

/*  
    RxBufferPhysicalAddress = imalloc(1000);
    outb( (uint16) (BaseAddress + CR), 0x10);
    while ( (inb( (uint16) (BaseAddress + CR)) & 0x10) != 0 ) {}
    // Setup receive buffer location
    outb( (uint16) (BaseAddress + CR), 1<<3 );
    outl( (uint16) (BaseAddress + 0x30), (uint32) RxBufferPhysicalAddress );
    // Buffer typ
    outl( (uint16) (BaseAddress + 0x44), 0xF | (1<<7) );
    // IRQ
    outw( (uint16) (BaseAddress + 0x3C), 5);  // TOK + ROK
    // Enable receive
*/
// end
//	outb( BaseAddress + CONFIG1_REGISTER, 0x00);
//	outb( BaseAddress + COMMANDREGISTER, 0x10);
//    	outl( BaseAddress +  0x30, (unsigned long)rx_buffer); 
//	outl(BaseAddress + 0x44, 0xf | (1 << 7));
//	outb(BaseAddress + 0x37, 0x0C);
//	outw(BaseAddress + 0x3C, 0x0005);

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
				printf("Found something\n");
		}
//		buzywait(10);	
		last = PciCommandReg;
/*		for(i=0;i<1000;i++)
		{
			if(rx_buffer[i]!=0)
			{
				printf("Found something\n");
			}
		}	
*/	}
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
	     uint32 PciCommandReg = ReadConfigurationDword(0, 3, 0x04);
		printf("Pci status and Command  Register %x\n", PciCommandReg);	
	      //  uint32 state = PciCommandReg &  PCI_PM_CTRL_STATE_MASK;
//		printf(" State = %x", state);
	//	PciCommandReg = PciCommandReg & 0xffff;
	//	PciCommandReg = 0x01103;
	        //WriteConfigurationDword(0, 3, 0x04, 0x00080103);	
	        WriteConfigurationWord(0, 3, 0x03, 0x0008);	
	        PciCommandReg = ReadConfigurationDword(0, 3, 0x0);
		printf("Pci Command Register %x\n", PciCommandReg);	
		BaseAddress = BaseAddress & 0xfffffff8 ;
		GetMacAddress();
		GetInterruptPin();
		TestReceive();
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
