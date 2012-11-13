#include "pci.h"
#include "common.h"
#include "screen.h"
#include "iheap.h"
#include "pcibiosservice.h"
#include "i825xx.h"
#define PCI_DEV_MAX         0x1F        // Max. value for dev parameter
#define PCI_FUNC_MAX        0x07        // Max. value for func parameter

struct PciTable
{
    uint16 DeviceId;
    uint16 VendorId;
    struct PciTable *next;
};

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
    case 0x00 :
        printf("Device was built prior definition of the class code field");
        break;
    case 0x01 :
        printf("Mass Storage Controller");
        break;
    case 0x02 :
        printf("Network Controller");
        break;
    case 0x03 :
        printf("Display Controller");
        break;
    case 0x04 :
        printf("Multimedia Controller");
        break;
    case 0x05 :
        printf("Memory Controller");
        break;
    case 0x06 :
        printf("Bridge Device");
        break;
    case 0x07 :
        printf("Simple Communication Controllers");
        break;
    case 0x08 :
        printf("Base System Peripherals");
        break;
    case 0x09 :
        printf("Input Devices");
        break;
    case 0x0A :
        printf("Docking Stations");
        break;
    case 0x0B :
        printf("Processors");
        break;
    case 0x0C :
        printf("Serial Bus Controllers");
        break;
    case 0x0D :
        printf("Wireless Controllers");
        break;
    case 0x0E :
        printf("Intelligent I/O Controllers");
        break;
    case 0x0F :
        printf("Satellite Communication Controllers");
        break;
    case 0x10 :
        printf("Encryption/Decryption Controllers");
        break;
    case 0x11 :
        printf("Data Acquisition and Signal Processing Controllers");
        break;
    case 0xFF :
        printf("Device does not fit any defined class");
        break;
    }
}


static char *subClassCodeString[13][8] =
{
    { "Other", "VGA", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "SCSI", "IDE", "Floppy", "IPI", "RAID", "Unknown", "Unknown", "Unknown" },
    { "Ethernet", "Token ring", "FDDI Controller", "ATM Controller", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "VGA", "XGA", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "Video", "Audio", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "RAM Controller", "Flash Memory Controller", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "Host Bridge", "ISA Bridge", "EISA Bridge", "MCA Channel Bridge", "PCI-PCI Bridge", "PCMCIA Bridge", "NuBus Bridge", "CardBus Bridge" },
    { "Serial Controller", "Parallel Port", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "PIC", "DMA", "Timers", "RTC", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "Keyboard", "Digitizer", "Mouse", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "Docking System", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "386 processor", "486 processor", "Pentium", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" },
    { "Firewire", "ACCESS", "SSA", "USB", "Unknown", "Unknown", "Unknown", "Unknown" }
};

void StrobePciDevices()
{
    int i,j;
    head = NULL;
    PciDevices last = NULL;
    for(i=0; i<256; i++)
    {
        for(j=0; j<32; j++)
        {
            uint32 ReadWord = 0;
            ReadWord = ReadWord | i << 16;
            ReadWord = ReadWord | j << 11 | ((uint32)0x80000000);
            outl(0xCF8, ReadWord);
            uint32 ret = inl(0xCFC);
            if( (ret & 0xffff) != 0xFFFF )
            {
                PciDevices temp = imalloc(sizeof(struct PciTable));
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
             
//                printf("Detected Device with BusNumber = %d and DeviceNumber %d\n",i,j);
  //              printf("Detected Pci Device ");
                printf("Vendor ID = 0x%x ", ret & 0xffff);
                printf("Device ID = 0x%x \n", ret >> 16 & 0xffff);
					 uint32 vendorId = ret & 0xffff;
					 uint32 deviceId = ret >> 16 & 0xffff;
				uint32 RegNo8 = ReadConfigurationDword(i,j, 0x8);
				uint8 classCode = RegNo8 >> 24;
				printf("device type = ");
                PrintClassCode(classCode);
				uint8 subClassCode =  (RegNo8 >> 16 ) & 0xff;
				printf(" : subtype = %s device = %d\n", subClassCodeString[classCode][subClassCode], j);
				uint32 RegNo4 = ReadConfigurationDword(i,j, 0x4);
            if( vendorId == 0x8086 && (deviceId == 0x100e || deviceId == 0x1209))
            {
					initialize825x(i,j);
//				printf(" register 4 status and command  = %x\n\n", RegNo4);
            }
            if( vendorId == 0x10ec && (deviceId == 0x8139))
            {
					InitializeRtl8139(i,j);
//				printf(" register 4 status and command  = %x\n\n", RegNo4);
            }
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
    outl(0xCF8, ReadWord);
}
uint32 PciReadRegister(uint32 BusNumber, uint32 DeviceNumber, uint32 Register)
{
    uint32 ReadWord = 0;
    ReadWord = BusNumber << 16 | Register <<2 | DeviceNumber << 11 | ((uint32)0x80000000);
    outl(0xCF8, ReadWord);
    uint32 ret = inl(0xCFC);
    return ret;
}



