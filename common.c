#include "common.h"
void outb(uint16 port, uint8 value)
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}
