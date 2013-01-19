
#include "screen.h"
#include "idt.h"
#include "iheap.h"
#include "vm.h"
#include "gdt.h"
#include "pci.h"
#include "debug.h"
#include "pcibiosservice.h"
uint32 firstProcessStartingStack;
int main(uint32 startingStackAddress)//struct multiboot *mboot_ptr)
{
    clr ();
    clr ();
	firstProcessStartingStack = startingStackAddress;
    asm volatile("cli");
    initializegdt();
    initializeidt();
    asm volatile("sti");
 //   InitializePaging();  // removing comment of this will enable virtual memory support
  
// to enable networking support following three functions are needed.
  DetectBios32Service();
  DetectPciBiosService();
  StrobePciDevices();

    ShowShell();
	DoShellProcess();
    while(1);
    return 0;//0xDEADBABA;
}
