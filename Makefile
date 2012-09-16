SOURCES=pcibiosservice.o gdt.o gdtasm.o biosservice.o network.o pci.o timer.o boot.o vm.o main.o screen.o common.o sample.o idt.o isr.o interrupt.o iheap.o debug.o
CFLAGS=-nostdlib -o0 -nostdinc -fno-builtin  
#-fno-stack-protector
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all:	$(SOURCES) link

clean: 
	-rm *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
