#include "pci.h"
#include "common.h"
#include "screen.h"
#include "iheap.h"
#define PCI_DEV_MAX         0x1F        // Max. value for dev parameter
#define PCI_FUNC_MAX        0x07        // Max. value for func parameter

struct PciTable
{
	uint16 DeviceId;
	uint16 VendorId;
	struct PciTable *next;
};

static uint16  PciReadConfigWord( uint8  bus, uint8  dev, uint8  func, uint16  reg )
{   
    uint16    _cx     = 0;
    uint8     devfunc = 0;

    // Mask the input values
    dev  &= PCI_DEV_MAX;
    func &= PCI_FUNC_MAX;

    // Make the conbined dev/func parameter
    devfunc = dev;
    devfunc <<= 3;
    devfunc |= func;

    // PCI BIOS v2.0c+ - READ CONFIGURATION WORD
    // AX = B109h
    // BL = device/function number (bits 7-3 device, bits 2-0 function)
    // BH = bus number
                        asm ("
        mov    ax, PCI_BIOS_CFG_RD16
        mov    bh, bus
        mov    bl, devfunc
        mov    di, reg
        int    0x1A
        mov    pciLastError, ah
        mov    _cx, cx"
    );

    return  _cx;
}

typedef struct PciTable * PciDevices;

static PciDevices head;
// for device and vendors information http://www.pcidatabase.com/search.php?device_search_str=8139&device_search.x=0&device_search.y=0

int checkpcitable(uint16 deviceid, uint16 vendorid)
{
   PciDevices temp = head;
	while ( temp != NULL )
	{
		if( temp->DeviceId == deviceid && temp->VendorId == vendorid )
		{
			return 1;//PciTable[deviceid][vendorid];
		}
		temp = temp->next;
	}
	return 0;
}

// tables taken from http://wiki.osdev.org/PCI   
void PrintClassCode(uint8 classcode)
{
	switch ( classcode )
	{
		case 0x00 :   printf("Device was built prior definition of the class code field\n");
						  break;
		case 0x01 :   printf("Mass Storage Controller\n"); break;
		case 0x02 :	  printf("Network Controller \n"); break;
		case 0x03 :	  printf("Display Controller \n"); break;
		case 0x04 :   printf("Multimedia Controller \n"); break;
		case 0x05 :	  printf("Memory Controller\n"); break;
		case 0x06 :	  printf("Bridge Device \n"); break;
		case 0x07 :   printf("Simple Communication Controllers"); break;
		case 0x08 :   printf("Base System Peripherals\n"); break;
		case 0x09 :	  printf("Input Devices \n"); break;
		case 0x0A :   printf("Docking Stations \n"); break;
		case 0x0B :	  printf("Processors\n"); break;
		case 0x0C :	  printf("Serial Bus Controllers \n"); break;
		case 0x0D :	  printf("Wireless Controllers \n"); break;
		case 0x0E :	  printf("Intelligent I/O Controllers \n"); break;
		case 0x0F :   printf("Satellite Communication Controllers \n"); break;
		case 0x10 :	  printf("Encryption/Decryption Controllers \n"); break;
		case 0x11 :	  printf("Data Acquisition and Signal Processing Controllers\n"); 
						  break;
		case 0xFF :   printf("Device does not fit any defined class \n"); break;
	}
}

void StrobePciDevices()
{
   int i,j;
   head = NULL;
   PciDevices last = NULL;
	for(i=0;i<256;i++)
	{
		for(j=0;j<32;j++)
		{
			uint32 ReadWord = 0;
			ReadWord = ReadWord | i << 16;
			ReadWord = ReadWord | j << 11 | ((uint32)0x80000000);
			outl(ReadWord, 0xCF8);
			uint32 ret = inl(0xCFC);
			if( (ret & 0xffff) != 0xFFFF )
			{
				PciDevices temp = kmalloc(sizeof(struct PciTable));
				temp->VendorId = ret & 0xffff;
				temp->DeviceId = ret >> 16 & 0xffff;
				temp->next = NULL;
				if( head == NULL )
				{
					head = temp;
				}
				else
				{
					last->next = temp;
				}
				last = temp;	
				printf("Detected Device with BusNumber = %d and DeviceNumber %d\n",i,j);
				printf("Vendor ID = 0x%x\n", ret & 0xffff);
				printf("Device ID = 0x%x\n", ret >> 16 & 0xffff);
/*			uint32 ReadWord2 =0;
         ReadWord2 = ReadWord2 | i << 16;
			ReadWord2 = ReadWord2 | j << 11 | ((uint32)0x80000000);
	      ReadWord2 = (0x0b & 0xfc0) | ReadWord2;
			outl(ReadWord2, 0xCF8);
*/
			uint32 ret = inl(0xCFC);
				ret = PciReadRegister(i,j,0x08);
				printf("print %x\n", ret);
				uint8 classcode = ret >> 24;
				PrintClassCode(classcode);
            // writing all 1
				uint32 val = 0xffffffff;
				//uint32 ret = inl(0xCFC);
        //    printf("ret value %x\n", val);
//				PciStrobeRegister(i,j,0x10);
//            outl(val, 0xCFC);
            uint32 ret2 = PciReadRegister(i,j,0x8);
				//outl(val, 0xCFC);
            //ret = PciReadRegister(i,j,0x10);
				//outl(val, 0xCF8);
				//uint32 ret = inl(0xCFC);
           // printf("ret value %x\n", ret);
            //ret = inl(val);
            printf("ret value %d\n", ret2);
			}
		}
	}
}				
	
uint32 PciStrobeRegister(uint32 BusNumber, uint32 DeviceNumber, uint32 Register)
{
	uint32 ReadWord = 0;
	ReadWord = ReadWord | BusNumber << 16;
	ReadWord = ReadWord | DeviceNumber << 11 | ((uint32)0x80000000);
	ReadWord = Register << 2 | ReadWord;
	outl(ReadWord, 0xCF8);
}
uint32 PciReadRegister(uint32 BusNumber, uint32 DeviceNumber, uint32 Register)
{
	uint32 ReadWord = 0;
	ReadWord = BusNumber << 16 | Register <<2 | DeviceNumber << 11 | ((uint32)0x80000000);
	outl(ReadWord, 0xCF8);
	uint32 ret = inl(0xCFC);
	return ret;
}


	
