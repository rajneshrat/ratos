#include "common.h"

void outb(uint16 port, uint8 value)
{
    asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

void outw(uint16 port, uint16 val)
{
    asm __volatile__ ("outw %%ax, %%dx" : : "d" (port), "a" (val));
}

uint8 inb(uint16 port)
{
    uint8 ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


void outl(uint16 port, uint32 val)
{
    asm volatile("outl %0,%1" : : "a" (val), "dN" (port));
}

uint32 inl(uint16  port)
{
    uint32 val;
    asm volatile("inl %1,%0" : "=a" (val) : "dN" (port));
    return val;
}

uint16 inw(uint16 port)
{
    uint16 ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void buzywait(uint32 time)
{
    int i;
    for(i=0; i<10000000*time; i++)
        ;
}
void memset(uint32 *dest, int val, uint32 size)
{
    int i;
    uint32 *poi = dest;
    for(i=0; i<size; i++)
    {
        *poi = val;
        poi++;
    }
}
