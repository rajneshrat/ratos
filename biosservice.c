#include "biosservice.h"
#include "common.h"
#include "screen.h"

#define PCIBIOS_PCI_BIOS_PRESENT    0xb101
#define PCI_SERVICE     (('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))

#define BIOS32_SIGNATURE   (('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24))
#define READCONFIGREG32 0xb10a

#define KERNEL_CS  0x10

static unsigned long bios32_entry = 0;
static struct {
   unsigned long address;
   unsigned short segment;
} bios32_indirect = { 0, KERNEL_CS };

static struct {
   unsigned long address;
   unsigned short segment;
} pci_indirect = { 0, KERNEL_CS };


static unsigned long  PCIBIOSENTRY;
struct bios32service{
	//uint32 signature ;
	uint32 signature;
	uint32 entrypoint;
	char revision;
	char length;
	char checksum;
	char reserved[5];
};

typedef struct bios32service * biosservicepointer;


int ReadConfigReg32(uint8 functionid, uint8 busNumber, uint32 deviceNumber, uint8 registerNumber, uint32* value);

static void (*test)();

static void test1()
{
	printf("Ok in test\n");
}

int searchbiossignature()
{
	biosservicepointer i;
	biosservicepointer pointer;
	for( i = 0x0e0000 ; i< 0x0fffff ; i++ )
	{
		pointer = (biosservicepointer)i;
		if(BIOS32_SIGNATURE == pointer->signature){
//pointer->signature[0] == '_' && pointer->signature[1] == '3' && pointer->signature[2] == '2' && pointer->signature[3] == '_' ){
			printf("Detected Bios32 Directory Services\n");
			printf("Bios Revision is %d \n", pointer->revision);
         test = test1;
			printf("test = %u, test1 = %u\n",test,test1);
			PCIBIOSENTRY = pointer->entrypoint;
			bios32_indirect.address = pointer->entrypoint;	
		//break;
		}
	}
	uint32 value;
   printf(" bios entry = %u\n", PCIBIOSENTRY);
  // DoCoreDump();
   ReadConfigReg32(0,0,1,0,&value);
//	getpciclasscode();
}

int ReadConfigReg32(uint8 functionid, uint8 busNumber, uint32 deviceNumber, uint8 registerNumber, uint32* value)
{

   unsigned char return_code; /* %al */
   unsigned long address;     /* %ebx */
   unsigned long length;      /* %ecx */
   unsigned long entry;    /* %edx */

   __asm__("lcall (%%edi)"
      : "=a" (return_code),
        "=b" (address),
        "=c" (length),
        "=d" (entry)
      : "0" (PCI_SERVICE),
        "1" (0),
        "D" (&bios32_indirect));

printf(" address = %d, return code = %d \n",address,return_code);
/*
	   unsigned long signature;
     unsigned char present_status;
   unsigned char major_revision;
   unsigned char minor_revision;
   int pack;

   //if ((pcibios_entry = bios32_service(PCI_SERVICE))) {
      pci_indirect.address = pcibios_entry;

      __asm__("lcall (%%edi)\n\t"
         "jc 1f\n\t"
         "xor %%ah, %%ah\n"
         "1:\tshl $8, %%eax\n\t"
         "movw %%bx, %%ax"
         : "=d" (signature),
           "=a" (pack)
         : "1" (PCIBIOS_PCI_BIOS_PRESENT),
           "D" (&pci_indirect)
         : "bx", "cx");




	uint32 res=0;
   printf("address = %u", PCIBIOSENTRY);
			printf("test = %u, test1 = %u\n",test,test1);
	uint32 input = (busNumber << 8)|(deviceNumber);// << 3)|(functionid);
    __asm__("lcall (%%esi)\n\t"
      "jc 1f\n\t"
      "xor %%ah, %%ah\n"
      "1:"
      : "=c" (*value),
        "=a" (res)
      : "1" (READCONFIGREG32),
        "b" (input),
        "D" ((long) registerNumber),
        "S" (&pci_indirect));

	__asm__ volatile ("call (%0)\n\t"
				:"=c" (*value),
				 "=a" (res)
				:"r" (&test),
				 "1" (READCONFIGREG32),
				 "b" (input),
				 "D" (registerNumber));
  */
// printf(" ret value = %d\n",res);
}

int getpciclasscode(uint32 bus, uint32 device)
{
 __asm__ ( "movl $10, %eax;"
                "movl $20, %ebx;"
                "addl %ebx, %eax;"
    );
        int no = 0xb001, val ;
		  asm ("movl %1, %%eax;"
             "movl %%ebx, %0;"
             : "=r" ( val )        /* output */
             : "r" ( no )         /* input */
             : "%ebx"         /* clobbered register */
         );
		  asm ("movl %1, %%ebx;"
             "movl %%ebx, %0;"
             : "=r" ( val )        /* output */
             : "r" ( no )         /* input */
             : "%ebx"         /* clobbered register */
         );
    printf("\n%d\n", val);

}
