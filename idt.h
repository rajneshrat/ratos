#include "common.h"
struct IDTDescr {
    uint16 offset_1; // offset bits 0..15
    uint16 selector; // a code segment selector in GDT or LDT
    uint8 zero;      // unused, set to 0
    uint8 type_attr; // type and attributes
    uint16 offset_2; // offset bits 16..31
} __attribute__((packed));
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
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

//isr starts after this
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();
extern void isr48();
extern void isr49();
extern void isr50();
extern void isr51();
extern void isr52();
extern void isr53();
extern void isr54();
extern void isr55();
extern void isr56();
extern void isr57();
extern void isr58();
extern void isr59();
extern void isr60();
extern void isr255();

