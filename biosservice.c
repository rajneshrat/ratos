/*
Bios provides many services even from PM.

First thing is, you have to locate the BIOS32 service directory entry point. This is done by scanning for the 4 bytes of "magic" that is "_32_" (0x5F32335F). The BIOS32 SD can lie in memory from 0xE0000 to 0x100000 and it always lies on a paragraph alignment.

Once we have the starting address of BIOS32 service directory we can check for all the services it is providing, for example PCI bios32 service.

This file will only check if bios32 service directory exits and where it is.

*/

#include "biosservice.h"
#include "common.h"
#include "screen.h"

#define BIOS32_SIGNATURE   (('_' << 0) | ('3' << 8) | ('2' << 16) | ('_' << 24))


static unsigned long bios32_entry = 0;


struct bios32segment bios32_indirect = { 0, KERNEL_CS };

struct bios32service {
    uint32 signature;
    uint32 entrypoint;
    char revision;
    char length;
    char checksum;
    char reserved[5];
};

typedef struct bios32service * biosservicepointer;

int DetectBios32Service()
{
    biosservicepointer i;
    biosservicepointer pointer;
    for( i = 0x0e0000 ; i< 0x0fffff ; i++ )
    {
        pointer = (biosservicepointer)i;
        if(BIOS32_SIGNATURE == pointer->signature) {
//pointer->signature[0] == '_' && pointer->signature[1] == '3' && pointer->signature[2] == '2' && pointer->signature[3] == '_' ){
            if (pointer->entrypoint >= 0x100000) {
                printf("pcibios_init: entry in high memory, unable to access\n");
            }
            if (pointer->revision != 0) {
                printf("pcibios_init : unsupported revision %d\n", pointer->revision);
                continue;
            }
            printf("Detected Bios32 Directory Services\n");
            printf("Bios Revision is %d \n", pointer->revision);
            bios32_indirect.address = pointer->entrypoint;
            uint32 kernelcodesegment = 0;
            asm ("movl %%cs, %0;"
                 : "=r" (kernelcodesegment)
                 :);
            bios32_indirect.segment = kernelcodesegment;
            break;
        }
    }
}

/*
The BIOS32 Service Directory provides a single function to determine whether a particulari 32-bit BIOS service is supported by the platform. All parameters to the function are passed in registers. Parameter descriptionsare provided below. If a prticular service is implemented in the platform BIOS, three values are returned. The first value is the base physical address of the BIOS service. The second value is the length of the BIOS service. These two values can be used to built the code segment selector and data segment selector for accessing the service.The third value provides the entry point to the BIOS service encoded as an offset from the base.

ENTRY
[EAX]
Service Identifier. This is a four character string used to specifically
identify which 32-bit BIOS Service is being sought.
[EBX]
The low order byte ([BL]) is the BIOS32 Service Directory function
selector. Currently only one function is defined (with the encoding of
zero) which returns the values provided below.
EXIT:
[AL]
Return code.
00h = Service corresponding to Service Identifier is present.
80h = Service corresponding to Service Identifier is not present
81h = Unimplemented function for BIOS Service Directory
(i.e. BL has an unrecognized value)
[EBX]
Physical address of the base of the BIOS service
[ECX]
Length of the BIOS service
[EDX]
Entry point into BIOS service. This is an offset from the base provided
in EBX
*/

//#define PCI_SERVICE  (('$' << 0) | ('P' << 8) | ('C' << 16) |  ('I' << 24))
uint32 GetBiosService(uint32 serviceType)
{
    uint8 returnStatus;
    uint32 baseAddress;
    uint32 length;
    uint32 offset;

    asm( "lcall (%%edi)"  // DI/EDI/RDI: Destination index for string operations
         :"=a"(returnStatus),
         "=b"(baseAddress),
         "=c"(length),
         "=d"(offset)
         :"0"(serviceType),
         "1"(0),
         "D"(&bios32_indirect));
    if( returnStatus == 0 ) {
        return baseAddress + offset;
    }
    else {
        printf("Failed to get service for service type = %d", serviceType);
        return 0;
    }

}

/*manual

to make a asm call use the below format

asm(  command1 %0
      command2
      :output operands and registers with there mapping to variable
      :input operands
   );

example
    asm ("movl %1, %%ebx;"
         "movl %%ebx, %0;"
         : "=r" ( val )
         : "r" ( no )
         : "%ebx"         // clobbered register
        );

    here %1 is input variable %0 is output.
*/
