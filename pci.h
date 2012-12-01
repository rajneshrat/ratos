#include "common.h"

void StrobePciDevices();

int checkpcitable(uint16 deviceid, uint16 vendorid);
uint32 PciReadRegister(uint32 BusNumber, uint32 DeviceNumber, uint32 Register);
uint32 PciStrobeRegister(uint32 BusNumber, uint32 DeviceNumber, uint32 Register);
