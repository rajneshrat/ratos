#ifndef PCI_H
#define PCI_H

#define PCI_CONFIG_ADDRESS		0xCF8
#define PCI_CONFIG_DATA			0xCFC

#define PCI_VENDOR		0x00
#define PCI_DEVICE		0x02
#define PCI_CLASS			0x0B
#define PCI_SUBCLASS		0x0A
#define PCI_PROGIF		0x09
#define PCI_REVISION		0x08

void pci_scan(void);

void pci_scan();
#endif
