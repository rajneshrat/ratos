#include "idt.h"

IDTDescr_t idtentries[256];

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
	idt_flush((uint32)&idtpointer);	
}	
