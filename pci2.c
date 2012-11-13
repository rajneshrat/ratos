#include "common.h"
#include "screen.h"
#include "pci2.h"

uint8 pci_read_config_uint8(uint16 bus, uint16 slot, uint16 func, uint16 reg);
uint16 pci_read_config_uint16(uint16 bus, uint16 slot, uint16 func, uint16 reg);
uint32 pci_read_config_uint32(uint16 bus, uint16 slot, uint16 func, uint16 reg);

void pci_scan()
{
    printf("PCI: Begin scan...\n\n");
    uint16 bus = 0;
    uint16 slot = 0;
    uint16 vendor = 0;
    uint16 device = 0;

//	for(bus = 0; bus < 4; bus++)
    {
//		for(slot = 0; slot < 16; slot++)
        {
            vendor = pci_read_config_uint16(bus, slot, 0, PCI_VENDOR);
            device = pci_read_config_uint16(bus, slot, 0, PCI_DEVICE);
            if(vendor != 0xFFFF)
                if(device != 0xFFFF)
                {
                    uint8 class 	= pci_read_config_uint8(bus, slot, 0, PCI_CLASS);
                    uint8 subclass = pci_read_config_uint8(bus, slot, 0, PCI_SUBCLASS);
                    uint8 prog_if 	= pci_read_config_uint8(bus, slot, 0, PCI_PROGIF);
                    uint8 revision = pci_read_config_uint8(bus, slot, 0, PCI_REVISION);
                    printf(" vendor = %x device = %x class = %x subclass = %x\n", vendor, device, class, subclass);
                    //hw_add_device(HW_BUS_TYPE_PCI, device, vendor, class, subclass, prog_if, revision, 0, 0);
                }
        }
    }

}

uint8 pci_read_config_uint8(uint16 bus, uint16 slot, uint16 func, uint16 reg)
{
    return (uint8)( (pci_read_config_uint32(bus, slot, func, reg) >> (8 * (reg & 0x03) ) ) & 0x000000FF );
}

uint16 pci_read_config_uint16(uint16 bus, uint16 slot, uint16 func, uint16 reg)
{
    return (uint16)( (pci_read_config_uint32(bus, slot, func, reg) >> (8 * (reg & 0x02) ) ) & 0x0000FFFF );
}

uint32 pci_read_config_uint32(uint16 bus, uint16 slot, uint16 func, uint16 reg)
{
    uint32 address;
    uint32 lbus = (uint32)bus;
    uint32 lslot = (uint32)slot;
    uint32 lfunc = (uint32)func;

    address = (uint32)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (reg & 0xfc) | ((uint32)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

