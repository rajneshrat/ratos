#include "pcibiosservice.h"
#include "biosservice.h"

#define PCISERVICEIDENTIFIER 0x49435024
#define PCI_FUNCTION_ID 0xb1
#define PCI_BIOS_PRESENT 0x01
#define READ_CONFIG_BYTE 0x08
#define READ_CONFIG_WORD 0x09
#define READ_CONFIG_DWORD 0x0a
#define WRITE_CONFIG_BYTE 0x0b
#define WRITE_CONFIG_WORD 0x0c
#define WRITE_CONFIG_DWORD 0x0d

void PciBiosError(uint32 errorCode);

void FillPciIndirectAddress(uint32 address)
{
    pci_indirect.address = address;
    uint32 kernelcodesegment = 0;
    asm ("movl %%cs, %0;"
         : "=r" (kernelcodesegment)
         :);
    pci_indirect.segment = kernelcodesegment;
}

int DetectPciBiosService()
{
    uint32 PciserviceAddress = GetBiosService(PCISERVICEIDENTIFIER);
	FillPciIndirectAddress(PciserviceAddress);	 
	IsPciBiosPresent();
}

int IsPciBiosPresent()
{
	uint32 ReturnStatus, Version, NumOfBus;
	uint32 InputComb = PCI_FUNCTION_ID << 8 | PCI_BIOS_PRESENT ;
//	InputComb = InputComb << 8 | ;
  //  InputComb |= PCI_BIOS_PRESENT;
	asm( "lcall (%%edi)"
		 :"=a"(ReturnStatus),
		  "=b"(Version),
		  "=c"(NumOfBus)
		 :"0"(InputComb),
		  "D"(&pci_indirect)
	   );
	uint16 PresentStatus = (ReturnStatus >> 16) & 0xffff;
	if( PresentStatus == 0 ){
		printf("Pci Bios Service final test passed\n");
	}
	else{
		printf("Pci Bios Service not detected\n");
	}
}

uint8 ReadConfigurationByte(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber)
{
	uint16 ReturnValue;
	uint16 ReturnStatus;
	uint16 InputComb = PCI_FUNCTION_ID << 8;
	InputComb |= READ_CONFIG_BYTE;
    uint16 BusAndReg = (busNumber << 8 | deviceNumber << 3 ) & 0xfff8; 
	asm( "lcall (%%esi)"
		 :"=c" (ReturnValue),
		  "=a" (ReturnStatus)
		 :"1" (InputComb),
		  "b" (BusAndReg),
		  "D" (registerNumber),
		  "S" (&pci_indirect));
	if( (ReturnStatus & 0xff00) ==0 ){
		return ReturnValue & 0xff;
	}
	else{
		printf("Failed to read Byte from Pci Bios\n");
		uint8 errorCode = ReturnStatus >> 8;
		PciBiosError(errorCode);
		return 0;
	}
}

uint16 ReadConfigurationWord(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber)
{
	uint16 ReturnValue;
	uint16 ReturnStatus;
	uint16 InputComb = PCI_FUNCTION_ID << 8;
	InputComb |= READ_CONFIG_WORD;
    uint16 BusAndReg = (busNumber << 8 | deviceNumber << 3 ) & 0xfff8; 
	asm( "lcall (%%esi)"
		 :"=c" (ReturnValue),
		  "=a" (ReturnStatus)
		 :"1" (InputComb),
		  "b" (BusAndReg),
		  "D" (registerNumber),
		  "S" (&pci_indirect)
		);
	if( (ReturnStatus & 0xff00) ==0 ){
		return ReturnValue;
	}
	else{
		printf("Failed to read Word from Pci Bios\n");
		uint8 errorCode = ReturnStatus >> 8;
        PciBiosError(errorCode);
        return 0;
	}
}

uint32 ReadConfigurationDword(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber)
{
	uint32 ReturnValue;
	uint16 ReturnStatus;
	uint16 InputComb = PCI_FUNCTION_ID << 8;
	InputComb |= READ_CONFIG_DWORD;
    uint16 BusAndReg = (busNumber << 8 | deviceNumber << 3 ) & 0xfff8;
	asm( "lcall (%%esi)"
		 :"=c" (ReturnValue),
		  "=a" (ReturnStatus)
		 :"1" (InputComb),
		  "b" (BusAndReg),
		  "D" (registerNumber),
		  "S" (&pci_indirect)
	);
	if( (ReturnStatus & 0xff00) ==0 ){
		return ReturnValue;
	}
	else{
		printf("Failed to read Dword from Pci Bios with status = %x\n", ReturnStatus);
	    uint8 errorCode = ReturnStatus >> 8;
        PciBiosError(errorCode);
        return 0;
	}
}

uint8 WriteConfigurationByte(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber, uint8 value)
{
	uint16 ReturnStatus;
	uint16 InputComb = PCI_FUNCTION_ID << 8;
	InputComb |= WRITE_CONFIG_BYTE;
    uint16 BusAndReg = (busNumber << 8 | deviceNumber << 3 ) & 0xfff8; 
	asm( "lcall (%%esi)"
		 :"=a" (ReturnStatus)
		 :"0" (InputComb),
		  "b" (BusAndReg),
		  "D" (registerNumber),
		  "c" (value),
		  "S" (&pci_indirect));
	if( (ReturnStatus & 0xff00) ==0 ){
		return 1;
	}
	else{
		printf("Failed to write Byte in Pci Register\n");
		uint8 errorCode = ReturnStatus >> 8;
        PciBiosError(errorCode);
        return 0;
	}
}

uint8 WriteConfigurationWord(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber, uint16 value)
{
	uint16 ReturnStatus;
	uint16 InputComb = PCI_FUNCTION_ID << 8;
	InputComb |= WRITE_CONFIG_WORD;
    uint16 BusAndReg = (busNumber << 8 | deviceNumber << 3 ) & 0xfff8; 
	asm( "lcall (%%esi)"
		 :"=a" (ReturnStatus)
		 :"0" (InputComb),
		  "b" (BusAndReg),
		  "D" (registerNumber),
		  "c" (value),
		  "S" (&pci_indirect));
	if( (ReturnStatus & 0xff00) ==0 ){
		return 1;
	}
	else{
		printf("Failed to write word in Pci Register\n");
		uint8 errorCode = ReturnStatus >> 8;
        PciBiosError(errorCode);
        return 0;
	}
}

uint8 WriteConfigurationDword(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber, uint32 value)
{
	uint16 ReturnStatus;
	uint16 InputComb = PCI_FUNCTION_ID << 8;
	InputComb |= WRITE_CONFIG_DWORD;
    uint16 BusAndReg = (busNumber << 8 | deviceNumber << 3 ) & 0xfff8; 
	asm( "lcall (%%esi)"
		 :"=a" (ReturnStatus)
		 :"0" (InputComb),
		  "b" (BusAndReg),
		  "D" (registerNumber),
		  "c" (value),
		  "S" (&pci_indirect));
	if( (ReturnStatus & 0xff00) ==0 ){
		return 1;
	}
	else{
		printf("Failed to write dword in Pci Register\n");
		uint8 errorCode = ReturnStatus >> 8;
        PciBiosError(errorCode);
        return 0;
	}
}

void PciBiosError(uint32 errorCode)
{
	switch(errorCode){
		case 0x00 : printf("Sussessful operation\n"); break;	
		case 0x81 : printf("FUNC_NOT_SUPPORTED\n"); break;	
		case 0x83 : printf("BAD_VENDOR_ID\n"); break;	
		case 0x86 : printf("DEVICE_NOT_FOUND\n"); break;	
		case 0x87 : printf("BAD_REGISTER_NUMBER\n"); break;	
		case 0x88 : printf("SET_FAILED\n"); break;	
		case 0x89 : printf("BUFFER_TOO_SMALL\n"); break;
	}
}	
