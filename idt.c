#include "idt.h"
#include "isr.h"

IDTDescr_t idtentries[256];

void irq_remap();
void fillidttable(uint32 offset, int index, uint8 type )
{
	idtentries[index].offset_2 = (offset >> 16 ) & 0xffff ;
	idtentries[index].selector =  0x08;
	idtentries[index].zero = 0 ;
	idtentries[index].type_attr =  type; 
	// present(1bit)Ring(2bit)interrupttype(1bit)gatetype(4bits)
	idtentries[index].offset_1 = (offset) & 0xffff;
}	  

idt_ptr_t idtpointer;

void initializeidt()
{
	idtpointer.limit = sizeof(IDTDescr_t) * 256 -1;
	idtpointer.base = (uint32 ) &idtentries[0];
	fillidttable((uint32)isr0, 0, 0x8e);	
	fillidttable((uint32)isr1, 1, 0x8e);	
	fillidttable((uint32)isr2, 2, 0x8e);	
	fillidttable((uint32)isr8, 8, 0x8e);	
	fillidttable((uint32)isr14, 14, 0x8e);	
	fillidttable((uint32)isr32, 32, 0x8e);	
	fillidttable((uint32)isr33, 33, 0x8e);	
	idt_flush((uint32)&idtpointer);	
	irq_remap();
	initializeisr();
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
// taken from the site http://www.osdever.net/bkerndev/Docs/irqs.htm
void irq_remap(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}	
