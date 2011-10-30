#include"gdt.h"

gdt_entry_s gdt_entries[3];

void fillgdttable( int32 i, int32 base, uint32 limit, int8 access, uint8 gran )
{
	gdt_entries[i].baselow    	 = (base & 0xFFFF);
   	gdt_entries[i].basemiddle	 = (base >> 16) & 0xFF;
   	gdt_entries[i].basehigh    	 = (base >> 24) & 0xFF;
   	gdt_entries[i].limitlow   	 = (limit & 0xFFFF);
   	gdt_entries[i].limithighandflag  = (limit >> 16) & 0x0F;
   	gdt_entries[i].limithighandflag  |= gran & 0xF0;
   	gdt_entries[i].accessbyte   	 = access;
}


gdt_desc_s gdt_pointer;

void initializegdt()
{
	
	gdt_pointer.limit = (5*(sizeof(gdt_entry_s)))-1;
	fillgdttable(0,0,0,0,0);
}
