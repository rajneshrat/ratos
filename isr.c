#include "common.h"
#include "isr.h"
#include "screen.h"
#include "keyboard.h"
#include "vm.h"
#include "timer.h"
irq_handler irq_array[252];

void attachirqhandler(irq_handler fun, int irq_number)
{
    irq_array[irq_number] = fun;
}

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t *regs)
{
    if(irq_array[regs->int_no] )
    {
        irq_handler fun = irq_array[regs->int_no];
        fun(regs);
        outb(0xa0, 0x20); // for pic1
        outb(0x20, 0x20);  // for pic 
        return;
    }
    puts("recieved unregistered interrupt: ");  // only for unregistered interrupts.
    putint(regs->int_no);
//    while(1);
    putch('\n');
}

//function body taken from http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html.
static void page_fault(registers_t *regs)
{
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    uint32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    // The error code gives us details of what happened.
    int present   = !(regs->err_code & 0x1); // Page not present
    int rw = regs->err_code & 0x2;           // Write operation?
    int us = regs->err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

    // Output an error message.
    // puts("Page fault! ( ");
    puts("Page fault! ( ");
    if (present) {
        puts("page not present ");
    }
    if (rw) {
        puts("read-only ");
    }
    if (us) {
        puts("user-mode ");
    }
    if (reserved) {
        puts("reserved ");
    }
    puts(") at 0x");

    puthex(faulting_address);
    putch('\n');
//   putint(faulting_address);
//   putint(faulting_address);
    puts("\n");
    AllocFrame(faulting_address, NULL);
//    outb(0x20, 0x20);
//   while(1);
//   PANIC("Page fault");
}

static void pittimer( registers_t *r)
{
//        puts("in timer function\n");
    DoTimer();
//    outb(0x20, 0x20);
}

static void GenProtectionFault( registers_t *reg)
{
//        puts("in timer function\n");
    printf("Interrupt 13 raised by cpu; Someone caused General Protection Fault\n");
    printf("Error Code =0x%x\n", reg->err_code); 
    printf("Error Flag = 0x%x\n", reg->eflags);
	 if( 0x15b == reg->err_code ){
		printf("Reason May be an unhandled interrupt\n"); 
    }
   //  outb(0x20, 0x20);
//    outb(0x20, 0x20);

}

static void doublefault( registers_t *r)
{
    puts("Interrupt 8 raised by cpu, double fault function\n");
//    outb(0x20, 0x20);
    DoCoreDump();
}
//function body taken as it is from http://www.osdever.net/bkerndev/Docs/keyboard.htm

static void keyboardisr( registers_t *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        putch(kbdus[scancode]);
    }
//    outb(0x20, 0x20);
}


void initializeisr()
{
    attachirqhandler(&pittimer, 32);
    attachirqhandler(&doublefault, 8);
    attachirqhandler(&keyboardisr, 33);
    attachirqhandler(&page_fault, 14);
    attachirqhandler(&GenProtectionFault, 13);
}

