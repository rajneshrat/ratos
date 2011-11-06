#include "common.h"
struct IDTDescr{
   uint16 offset_1; // offset bits 0..15
   uint16 selector; // a code segment selector in GDT or LDT
   uint8 zero;      // unused, set to 0
   uint8 type_attr; // type and attributes 
   uint16 offset_2; // offset bits 16..31
}__attribute__((packed));
// struct taken from wiki site http://wiki.osdev.org/Interrupt_Descriptor_Table

struct idt_ptr
{
    uint16 limit;
    uint32 base;
} __attribute__((packed));
// struct taken from wiki site http://wiki.osdev.org/Interrupt_Descriptor_Table

typedef struct IDTDescr IDTDescr_t;
typedef struct idt_ptr idt_ptr_t;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr8();
extern void isr32();
extern void isr33();

