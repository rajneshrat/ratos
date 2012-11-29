// this is the driver for rtl8139. It talks directly to the card. Written for ratos as the network card driver.

#include "pci.h"
#include "rtl8139.h"
#include "screen.h"
#include "isr.h"

#define CONFIG1_REGISTER 0x52
#define COMMANDREGISTER  0x37
#define INT_MASK_REG	0x3c
#define INT_STATUS_REG 0x3e
#define PACKET_READ_ADDRESS 0x38
#define RECEIVE_CONFIG_REG 0x44
#define TRSMIT_START_DESC 0x20
#define TRSMIT_STATUS_DESC 0x10
#define RECEIVE_BUFFER_ADDRESS 0x30

//Limitation
//1 - Right now there is no ring buffer yet to be implemented. I am just resetting the current buffer pointer to start after receiving every packet.
//2 - Doing everything in interrupt context. This will make system slow.

unsigned char rxBuffer[1500];
unsigned char TxBuffer[1300];

static uint32 BaseAddress=0x0;

void DumpReceiveReg(unsigned char *buf)
{
    int i;
    for(i=0; i<40; i++)
    {
        printf(" %x",buf[i]);
        buf[i] = 0x0;
    }
}

void Rtl8139Interrupt(registers_t *regs)
{
    outw(BaseAddress + INT_MASK_REG, 0);
    uint32 interruptStatus = inw(BaseAddress + INT_STATUS_REG);
//    printf("Status of Interrupt Status = 0x%x\n", interruptStatus);
    outw(BaseAddress + INT_STATUS_REG, interruptStatus); //reset
    if( interruptStatus & 0x01 ) {
        ReceivePacketFromWire(rxBuffer);
    }
    if( interruptStatus & 0x04 ) {
        //      printf("Transmitted a packet sussessfully\n");
    }
    outw((BaseAddress + PACKET_READ_ADDRESS), 0);
    outl((BaseAddress + RECEIVE_BUFFER_ADDRESS), (uint32)rxBuffer );
    outw((BaseAddress + INT_MASK_REG), 0xe1ff); // TOK + ROK
    outl((BaseAddress + RECEIVE_CONFIG_REG), 0xa);//
}

void GetInterruptPin()
{
    uint32 InterruptReg  = ReadConfigurationDword(0, 3, INT_MASK_REG);
    uint8 InterruptPin = (InterruptReg > 8)&0xff;
    uint8 InterruptLine = (InterruptReg)&0xff;
    printk("Interrupt Details %x interrupt pin %x interrupt line %x\n",InterruptReg, InterruptPin, InterruptLine);
    attachirqhandler(Rtl8139Interrupt, InterruptLine + 32);	 // I have done remapping so have to add 32.
}

void SendPacketOut(unsigned char *packetData, uint32 len)
{
    static uint32 currentStartDesc = TRSMIT_START_DESC;
    static uint32 currentStatusDesc = TRSMIT_STATUS_DESC;
    static int offset = 0;
    offset = offset + 4;
    uint32 value = (uint32)(&TxBuffer[0]) - (uint32)(&TxBuffer[0]) % 32 + 32;
    sleep(2);
    uint32 status = 0x003f0000 | len;
    outl((BaseAddress + currentStartDesc), packetData);
    outl((BaseAddress + currentStatusDesc),status);
    currentStartDesc = TRSMIT_START_DESC+(offset%20);
    currentStatusDesc = TRSMIT_STATUS_DESC+(offset%20);
}

void ResetCard()
{
    //power on
    outb( (uint16) (BaseAddress + 0x52), 0x00);
    //reset
    outb( (uint16) (BaseAddress + COMMANDREGISTER), 0x10);
    while ( (inb( (uint16) (BaseAddress + COMMANDREGISTER)) & 0x10) != 0 ) {
        printk("Waiting for resetting rtl8139 card\n");
        sleep(2);
    }
    outw((BaseAddress + INT_STATUS_REG), 0xe1ff);
    outw((BaseAddress + PACKET_READ_ADDRESS), 0);
    // Setup receive buffer location
    outl((BaseAddress + RECEIVE_BUFFER_ADDRESS), (uint32)rxBuffer );
    outw((BaseAddress + INT_MASK_REG), 0xe1ff);  // TOK + ROK
    outl((BaseAddress + RECEIVE_CONFIG_REG), 0xa);// | (1<<7) );
    outb( (BaseAddress + COMMANDREGISTER), 0x0C );
}

void InitializeRtl8139(uint8 busNumber, uint8 deviceNumber)
{
    uint16 vendorid = 0x10ec;
    uint16 deviceid = 0x8139;
    BaseAddress = ReadConfigurationDword(busNumber, deviceNumber, 0x10);
    WriteConfigurationDword(busNumber, deviceNumber, 0x10,~0);
    uint32 size = ReadConfigurationDword(busNumber, deviceNumber, 0x10);
    WriteConfigurationDword(busNumber, deviceNumber,0x10,BaseAddress);
    if( (BaseAddress & 0x1) == 1)
    {
        printk("Detected I/O type \n");
        BaseAddress = BaseAddress & 0xfffffffc ;
    }
    else
    {
        printk("Detected unknown mapped type will do dump\n");
        return;
    }
    GetMacAddress();
    GetInterruptPin();
    ResetCard();
}

void GetMacAddress()
{
    unsigned int i;
    uint8 Mac;
    printk("Card Mac Address = ");
    for(i=0; i<6; i++)
    {
        Mac = inb(BaseAddress + i);
        CardHwAdd[i] = Mac;
        printk("%x ",Mac);
    }
    printk("\n");
}
