#include "common.h"

#define KERNEL_CS 0x0

static struct {
    unsigned long address;
    unsigned short segment;
} pci_indirect = { 0, KERNEL_CS };

int DetectPciBiosService();

uint8 ReadConfigurationByte(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber);
uint16 ReadConfigurationWord(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber);
uint32 ReadConfigurationDword(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber);
uint8 WriteConfigurationByte(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber, uint8 value);
uint8 WriteConfigurationWord(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber, uint16 value);
uint8 WriteConfigurationDword(uint8 busNumber, uint8 deviceNumber, uint8 registerNumber, uint32 value);
