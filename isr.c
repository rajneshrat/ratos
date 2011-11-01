#include "common.h"
#include "isr.h"
#include "screen.h"

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t *regs)
{
   puts("recieved interrupt: ");
   putint(regs->int_no);
   putch('\n');
   putint(0);
} 
