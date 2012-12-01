#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "rtl8139.h"
#include "demo.h"

void interrupt (*OldFunction)(void);
void interrupt NewFunction(void);
int	INTR;

extern far void outpdw(unsigned int,unsigned long);
extern far unsigned long inpdw(unsigned int);


void InitHardware();
void InitSoftware();
void IssueCMD();
void TxInterruptHandler();
BOOLEAN RxInterruptHandler();

//Transmit variables
#define TX_SW_BUFFER_NUM	4
TX_DESCRIPTOR	TxDesc[TX_SW_BUFFER_NUM];
unsigned char	TxHwSetupPtr;
unsigned char	TxHwFinishPtr;
unsigned char	TxHwFreeDesc;

//Receive variables
unsigned char	*RxBuffer,*RxBufferOriginal;
unsigned long	RxBufferPhysicalAddress;
unsigned int	RxReadPtrOffset;
PPACKET		pLeadingReadPacket;	//should be a link list
unsigned long	PacketReceivedGood = 0;
unsigned long	ByteReceived = 0;

#define	RX_BUFFER_SIZE		16*1024
#define RX_MAX_PACKET_LENGTH	1600
#define RX_MIN_PACKET_LENGTH	64
#define	RX_READ_POINTER_MASK	0x3FFC

ULONG	IOBase,Irq;
unsigned char *Buffer;
ULONG	PhysicalAddrBuffer;


void ShowStatistics(
)
{
    gotoxy(2,22); printf("Packet Received:: %u",PacketReceivedGood);
    gotoxy(2,23); printf("Byte   Received:: %9lu",ByteReceived);
}

BOOLEAN
FindIOIRQ(ULONG *IOBase,ULONG *IRQ)
{
    ULONG i,j,PciData,BaseAddr,Interrupt;
    for(i=0;i<32;i++)
    {
	j=0x80000000+(i<<11);
	outpdw(0xcf8,j);
	PciData=inpdw(0xcfc);
	if(PciData==0x813910ec)
	{
	   outpdw(0xcf8,j+0x10);
	   *IOBase=inpdw(0xcfc);
	   *IOBase &= 0xfffffff0;
	   outpdw(0xcf8,j+0x3c);
	   *IRQ=inpdw(0xcfc);
	   *IRQ &= 0xff;
	   return TRUE;
	}
    }
    return FALSE;
}


int
ComputeInterrupt(
	int	IrqNumber
)
{
	if(IrqNumber <=8) return IrqNumber+8;
	else		  return IrqNumber+0x68;
}
/////////////////////////////////////////////////////////////////////////
//Our Interrupt Service Routine (ISR)
/////////////////////////////////////////////////////////////////////////
void interrupt NewFunction(void)
{
    unsigned int curISR;
    disable();	// == _asm cli
    curISR = inport(IOBase + ISR);
    if( (curISR & R39_INTERRUPT_MASK) != 0)
    {
	do
	{
	    if(curISR & ISR_PUN)
	    {
//     	        ProcessLingChange();	//should write this code someday
		outport(IOBase + ISR , ISR_PUN);
	    }
	    if(curISR & ISR_TOK)
	    {
		TxInterruptHandler();
		outport(IOBase + ISR, ISR_TOK);
	    }
	    if(curISR & ISR_TER)
	    {
		outportb(IOBase + TCR , TCR_CLRABT);
		outport(IOBase + ISR , ISR_TER);
	    }
	    if( curISR & (ISR_ROK|ISR_RER|ISR_RXOVW|ISR_FIFOOVW) )
	    {
		if(curISR & ISR_ROK)
		{
		    RxInterruptHandler();
		}
		outport(IOBase + ISR, ISR_ROK | ISR_RER |ISR_RXOVW | ISR_FIFOOVW);
	    }
	    curISR = inport(IOBase + ISR);
	}while( (curISR & R39_INTERRUPT_MASK) != 0);
//	_asm int 3;
_asm    mov     al,020h
_asm    out     0a0h,al         //;issue EOI to 2nd 8259
_asm    out     20h,al          //;Issue EOI to 1nd 8259
    }
    else
    {//not our interrupt, should call original interrupt service routine.
     // OldFunction();
    }
    enable();	// == _asm sti
}

//////////////////////////////////////////////////////////////////////////
//Initialization part
//////////////////////////////////////////////////////////////////////////
void
InitHardware()
{
	outportb(IOBase + CR, CR_RST);              //reset
	outportb(IOBase + CR, CR_RE + CR_TE);       //enable Tx/Rx
	outpdw(IOBase + TCR, 	TCR_IFG0   |
				TCR_IFG1   |
				TCR_MXDMA2 |
				TCR_MXDMA1);

	outpdw(IOBase + RCR, 	RCR_RBLEN0 |
				RCR_MXDMA2 |
				RCR_MXDMA1 |
				RCR_AB	   |
				RCR_AM     |
				RCR_APM );
	outpdw(IOBase + RBSTART , RxBufferPhysicalAddress);
	outport(IOBase + IMR, R39_INTERRUPT_MASK);//enable interrupt
}

void
InitSoftware()
{
	ULONG	Offset,Segment,Delta,i;
	unsigned char *tmpBuffer;
//Init Tx Variables
	TxHwSetupPtr = 0;
	TxHwFinishPtr    = 0;
	TxHwFreeDesc = TX_SW_BUFFER_NUM;
//initialize TX descriptor
	for(i=0;i<TX_SW_BUFFER_NUM;i++)
	{	//allocate memory
		Buffer=(unsigned char *)malloc( 1600 / sizeof(int) );
		TxDesc[i].OriginalBufferAddress = Buffer;
		Offset=FP_OFF(Buffer);
		//align to DWORD
		if( Offset & 0x3 )
		{
			Delta = 4 - (Offset & 0x3);
			Offset = Offset + Delta;
			Buffer = Buffer + Delta;
		}
		TxDesc[i].buffer = Buffer;
		Segment=FP_SEG(Buffer);
		PhysicalAddrBuffer = (Segment << 4) + Offset;
		TxDesc[i].PhysicalAddress = PhysicalAddrBuffer;
//		TxDesc[i].DescriptorStatus = TXDESC_INIT;
	}
//Init Rx Buffer
	RxBufferOriginal =
	tmpBuffer	 = (unsigned char *)malloc( RX_BUFFER_SIZE + 2000 );
	Offset=FP_OFF(tmpBuffer);
	//align to DWORD
	if( Offset & 0x3 )
	{
		Delta = 4 - (Offset & 0x3);
		Offset = Offset + Delta;
		tmpBuffer = tmpBuffer + Delta;
	}
	RxBuffer = tmpBuffer;
	Segment=FP_SEG(tmpBuffer);
	RxBufferPhysicalAddress = (Segment << 4) + Offset;
//Init Rx Variable
	RxReadPtrOffset = 0;
}

//////////////////////////////////////////////////////////////////////////
//Transmit part
//////////////////////////////////////////////////////////////////////////
unsigned char
NextDesc(
	unsigned char CurrentDescriptor
    )
{
//    (CurrentDescriptor == TX_SW_BUFFER_NUM-1) ? 0 : (1 + CurrentDescriptor);
    if(CurrentDescriptor == TX_SW_BUFFER_NUM-1)
    {
	return  0;
    }
    else
    {
	return ( 1 + CurrentDescriptor);
    }
}

unsigned char
CheckTSDStatus(
    unsigned char            Desc
    )
{
    ULONG       Offset = Desc << 2;
    ULONG       tmpTSD;

    tmpTSD=inpdw(IOBase + TSD0 + Offset);
    switch ( tmpTSD & (TSD_OWN | TSD_TOK) )
    {
	case (TSD_OWN | TSD_TOK):      	return 	TSDSTATUS_BOTH;
	case (TSD_TOK) 		:       return  TSDSTATUS_TOK;
	case (TSD_OWN) 		:       return  TSDSTATUS_OWN;
	case 0 			:	return  TSDSTATUS_0;
    }
    return 0;
}



void
IssueCMD(unsigned char descriptor)
{
	unsigned long offset = descriptor << 2;
	outpdw(IOBase + TSAD0 + offset, TxDesc[TxHwSetupPtr].PhysicalAddress);
	outpdw(IOBase + TSD0 + offset , TxDesc[TxHwSetupPtr].PacketLength);
}

int
SendPacket(
	PPACKET pPacket
)
{
    disable();
    if( TxHwFreeDesc>0  )
    {
	TxDesc[TxHwSetupPtr].PacketLength=
		CopyFromPacketToBuffer( pPacket , TxDesc[TxHwSetupPtr].buffer);
	IssueCMD(TxHwSetupPtr);
	TxHwSetupPtr = NextDesc(TxHwSetupPtr);
	TxHwFreeDesc--;
	enable();
	return TRUE;//success
    }
    else
    {
	enable();
	return FALSE;//out of resource
    }
}

void
TxInterruptHandler()
{
    while( (CheckTSDStatus(TxHwFinishPtr) == TSDSTATUS_BOTH	) &&
	   (TxHwFreeDesc < 4 				)   )
    {
	//can Release this buffer now

	TxHwFinishPtr = NextDesc(TxHwFinishPtr);
	TxHwFreeDesc++;
    }
}
////////////////////////////////////////////////////////////////////////
// Start of Rx Path
////////////////////////////////////////////////////////////////////////
void
ReadPacket(
	PPACKET	RxPacket
)
{
    pLeadingReadPacket = RxPacket;
}

void
CopyPacket(
    unsigned char 	*pIncomePacket,
    unsigned int        PktLength
)
{
    if( (pLeadingReadPacket != NULL)           &&
	(pLeadingReadPacket->PacketLength == 0)  )
    {
	memcpy(pLeadingReadPacket->Buffers.Buffer , pIncomePacket , PktLength);
	pLeadingReadPacket->PacketLength = PktLength;
    }

}

BOOLEAN
PacketOK(
	PPACKETHEADER pPktHdr
)
{
    BOOLEAN BadPacket = pPktHdr->RUNT ||
			pPktHdr->LONG ||
			pPktHdr->CRC  ||
			pPktHdr->FAE;
    if( ( !BadPacket )   &&
	( pPktHdr->ROK )   )
    {
	if ( (pPktHdr->PacketLength > RX_MAX_PACKET_LENGTH ) ||
	     (pPktHdr->PacketLength < RX_MIN_PACKET_LENGTH )    )
	{
	    return(FALSE);
	}
	PacketReceivedGood++;
	ByteReceived += pPktHdr->PacketLength;
	return TRUE ;
    }
    else
    {
	return FALSE;
    }
}

BOOLEAN
RxInterruptHandler(
    )
{
    unsigned char  TmpCMD;
    unsigned int   PktLength;
    unsigned char  *pIncomePacket, *RxReadPtr;
    PPACKETHEADER  pPacketHeader;

    while (TRUE)
    {
	TmpCMD = inportb(IOBase + CR);
	if (TmpCMD & CR_BUFE)
	{
	    break;
	}

	do
	{
	    RxReadPtr	  = RxBuffer + RxReadPtrOffset;
	    pPacketHeader = (PPACKETHEADER)  RxReadPtr;
	    pIncomePacket = RxReadPtr + 4;
	    PktLength	  = pPacketHeader->PacketLength;	//this length include CRC
	    if ( PacketOK( pPacketHeader ) )
	    {
		if ( (RxReadPtrOffset + PktLength) > RX_BUFFER_SIZE )
		{      //wrap around to end of RxBuffer
//_asm int 3;
		    memcpy( RxBuffer + RX_BUFFER_SIZE ,	RxBuffer,
				(RxReadPtrOffset + PktLength - RX_BUFFER_SIZE)  );
		}
		//copy the packet out here
		CopyPacket(pIncomePacket,PktLength - 4);//don't copy 4 bytes CRC

		//update Read Pointer
		RxReadPtrOffset = (RxReadPtrOffset + PktLength + 4 + 3) & RX_READ_POINTER_MASK;
			//4:for header length(PktLength include 4 bytes CRC)
			//3:for dword alignment
		outport( IOBase + CAPR, RxReadPtrOffset - 0x10);	//-4:avoid overflow
	    }
	    else
	    {
//		ResetRx();
		break;
	    }
	    TmpCMD = inportb(IOBase + CR);
	} while (!(TmpCMD & CR_BUFE));
   }
    return (TRUE);              //Done
}

/////////////////////////////////////////////////////////////////////////
//Load / Unload
/////////////////////////////////////////////////////////////////////////
BOOLEAN
LoadDriver()
{
	int	INTR;
	FindIOIRQ( &IOBase , &Irq );
	INTR = ComputeInterrupt( Irq );
	OldFunction = getvect( INTR );
//hook interrupt vector
	disable();
	setvect(INTR,NewFunction);
	enable();
//initialize chip
	InitSoftware();
	InitHardware();

	return TRUE;
}

BOOLEAN
UnloadDriver()
{
	disable();
	setvect(INTR,OldFunction);
	enable();
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
// End Of Demo driver
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	Start of Test Program (only for demo)
///////////////////////////////////////////////////////////////////////////
PPACKET
BuildPacket(
)
{
	int i;
	PPACKET tmpPacket = (PPACKET) malloc(sizeof(PACKET));
	tmpPacket->BufferCount  = 1;
	tmpPacket->PacketLength = 64;
	tmpPacket->Buffers.Buffer = (unsigned char *) malloc(2000);
	tmpPacket->Buffers.Next   = NULL;
	tmpPacket->Buffers.BufferLength = 64;
	for(i=0;i<12;i++)
	{
		tmpPacket->Buffers.Buffer[i] = 0xff;
	}
	for(i=12;i<2000;i++)
	{
		tmpPacket->Buffers.Buffer[i] = i & 0xff;
	}
	return	tmpPacket;
}

PPACKET
PreparePacket(
)
{
	PPACKET tmpPacket = (PPACKET) malloc(sizeof(PACKET));
	tmpPacket->BufferCount  = 1;
	tmpPacket->PacketLength = 0;
	tmpPacket->Buffers.Buffer = (unsigned char *) malloc(2000);
	tmpPacket->Buffers.Next   = NULL;
	tmpPacket->Buffers.BufferLength = 0;
	return	tmpPacket;
}

void
ShowPacket(
    PPACKET pPacket
)
{
    unsigned char *PktBuf   = pPacket->Buffers.Buffer;
    unsigned int  i,PktLength = pPacket->PacketLength;
    clrscr();
    ShowStatistics();
    printf("Packet Length = %d\n",PktLength);
    for(i=0;i<PktLength;i++)
    {
	printf("%02X ",PktBuf[i]);
	if( (i&0xf) == 0xf )
	{
	    printf("  %4X\n", (i&0xfffffff0) );
	}
	if( (i&0xff) == 0xff )
	{
	    getchar();
	    clrscr();
	}
    }

}

BOOLEAN
Send(
)
{
	PPACKET	pTestPacket;
	int	i;

// Send Test
	pTestPacket = BuildPacket();
	for(i=0;i<2000;i++)
	{
		pTestPacket->PacketLength=pTestPacket->Buffers.BufferLength = 64 + (i%1430);
		while( !SendPacket(pTestPacket) )
		{
		};
//		delay(5);
	}
	return TRUE;
}

void
Receive()
{
	PPACKET	pTestPacket;
	int	i;

//Read Test
	pTestPacket = PreparePacket();
	ReadPacket(pTestPacket);
    do
    {
/*	while(pTestPacket->PacketLength == 0)
	{
	}
//	ShowPacket(pTestPacket);*/
	ShowStatistics();
	pTestPacket->PacketLength = 0;
    }while(!kbhit());
}

main()
{
	clrscr();
        directvideo = 1;
	LoadDriver();
	Send();
//	Receive();
	UnloadDriver();
	return 0;
}