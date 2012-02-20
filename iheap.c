#include "iheap.h"

extern uint32 end;
static uint32 placement_address = (uint32)&end;
uint32 imalloc(int size)
{
	 if (placement_address & 0xFFFFF000) // If the address is not already page-aligned
  {
    // Align it.
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }
  uint32 tmp = placement_address;
  placement_address += size;
  return tmp;
}

