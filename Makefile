SOURCES= log.o shell.o icmp.o ipv4.o arp.o ethernet.o pcibiosservice.o biosservice.o gdt.o gdtasm.o rtl8139.o pci.o timer.o boot.o vm.o main.o screen.o common.o idt.o isr.o interrupt.o iheap.o debug.o
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
