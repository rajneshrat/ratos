SOURCES= arp.o ethernet.o i825xx.o pcibiosservice.o biosservice.o gdt.o gdtasm.o rtl8139.o pci.o timer.o boot.o vm.o main.o screen.o common.o sample.o idt.o isr.o interrupt.o iheap.o debug.o
CFLAGS=-nostdlib -nostdinc -fno-builtin  
#-fno-stack-protector
LDFLAGS=-Tlink.ld -melf_i386 -g -nostdlib -X  
ASFLAGS=-felf

all:	$(SOURCES) link

clean: 
	-rm *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

#.c.o:	gcc $(CFLAGS) -c $<

.s.o:
	nasm $(ASFLAGS) $<
