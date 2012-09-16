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

extern void gdt_flush(uint32);

void initializegdt()
{
    gdt_pointer.gdt_address = gdt_entries;
    gdt_pointer.size = (3*(sizeof(gdt_entry_s)))-1;
    fillgdttable(0,0,0,0,0);
    fillgdttable(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    fillgdttable(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_flush((uint32)&gdt_pointer);
}
