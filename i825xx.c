#include "pci.h"
#include "screen.h"
uint32 BaseAddress;

#define RCTL_EN				(1 << 1)
#define RCTL_SBP			(1 << 2)
#define RCTL_UPE			(1 << 3)
#define RCTL_MPE			(1 << 4)
#define RCTL_LPE			(1 << 5)
#define RDMTS_HALF			(0 << 8)
#define RDMTS_QUARTER		(1 << 8)
#define RDMTS_EIGHTH		(2 << 8)
#define RCTL_BAM			(1 << 15)
#define RCTL_BSIZE_256		(3 << 16)
#define RCTL_BSIZE_512		(2 << 16)
#define RCTL_BSIZE_1024		(1 << 16)
#define RCTL_BSIZE_2048		(0 << 16)
#define RCTL_BSIZE_4096		((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192		((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384	((1 << 16) | (1 << 25))
#define RCTL_SECRC			(1 << 26)

void WriteValue(uint8 reg, uint32 val)
{
	outl(BaseAddress + 0x00, reg);
	outl(BaseAddress + 0x04, val);
}

uint32 ReadValue(uint8 reg)
{
	outl(BaseAddress + 0x00, reg); 
	return inl(BaseAddress + 0x04); 
}
#define i825xx_REG_EERD 0x14
static uint16 eeprom_read(uint8 ADDR )
{
	uint16 DATA = 0;
	uint32 tmp = 0;
	WriteValue( i825xx_REG_EERD, (1) | ((uint32)(ADDR) << 8) );
	
	while( !((tmp = ReadValue(i825xx_REG_EERD)) & (1 << 4)) )
		buzywait(4);
		
	DATA = (uint16)((tmp >> 16) & 0xFFFF);
	return DATA;
}
#define i825xx_REG_RCTL  0x100
int initialize825x(uint8 busNumber, uint8 deviceNumber)
{
	BaseAddress  = ReadConfigurationDword(busNumber, deviceNumber, 0x10);
	if( BaseAddress & 0x1 )	{
		printf("Memory Mapped I/O\n");
		printf("Memory Mapped is not supported exiting \n");
		return 0;
	   BaseAddress = BaseAddress & 0xfffffff8;
   }
	else{
		printf("Port Mapped I/O\n");
	   BaseAddress = BaseAddress & 0xfffffff0 ;
   }
   uint16 mac16 = eeprom_read(0);
   printf(" mac = %x \n",mac16);
   WriteValue(i825xx_REG_RCTL, (RCTL_SBP | RCTL_UPE | RCTL_MPE | RDMTS_HALF | RCTL_SECRC | RCTL_LPE | RCTL_BAM | RCTL_BSIZE_8192) );
}
