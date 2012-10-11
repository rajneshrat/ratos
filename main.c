
#include "screen.h"
#include "idt.h"
#include "iheap.h"
#include "vm.h"
#include "pci.h"
#include "debug.h"
#include "pcibiosservice.h"
int main(struct multiboot *mboot_ptr)
{
	int i, j;
	char ch = 'a';
//	initializegdt();
    clr ();
    // int a = 204800000;
    //asm volatile("mov %0, %%esp" : : "r" (a));
    // asm volatile("cli");
    initializegdt();
    initializeidt();
    DetectBios32Service();
    DetectPciBiosService();
   // InitializePaging();

    StrobePciDevices();
    //asm volatile("sti");
    // pci_scan();
    // StrobePciDevices();
    DetectNetworkDevices();
//    asm volatile("sti");
    
	putch('\n');
//	asm volatile ("int $0x10");
//	asm volatile ("int $0x0");
//	asm volatile ("int $0x2");
//	asm volatile("sti");
//        while(1)
        {
       //    int  *pointer = (uint32 *)kmalloc(12000000);
  //      }
//        int *pointer = (uint32 *) 0x10000000;
     //      if( pointer )
	//				*pointer = 201;
	//		  else
	//			   puts("Failed to allocate");
  //         putint(*pointer);
//			 puts("unknown\n");
//          kfree(pointer);
        }
//        while(1);
//	clr();
//	test();
//	puts("rajnesh\n");
//	putint(12323);
	return 0xDEADBABA;
}
