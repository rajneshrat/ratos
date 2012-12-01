#ifndef __BIOSSERVICE__
#define __BIOSSERVICE__
#include "common.h"

#define KERNEL_CS  0x0
int DetectBios32Service();

struct bios32segment {
    unsigned long address;
    unsigned short segment;
};

uint32 GetBiosService(uint32 serviceType);

#endif
