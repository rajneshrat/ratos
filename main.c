#include "screen.h"
#include "idt.h"
#include "iheap.h"
//#include "gdt.h"

int main(struct multiboot *mboot_ptr)
{
	int i, j;
	char ch = 'a';
//	initializegdt();
	putch('\n');
	initializeidt();
	asm volatile ("int $0x1");
	asm volatile ("int $0x0");
	asm volatile ("int $0x2");
	asm volatile("sti");
        int *pointer = (int *)imalloc(12);
        *pointer = 201;
//        putint(*pointer);
//	clr();
//	test();
	puts("rajnesh\n");
//	putint(12323);
	return 0xDEADBABA;
}
