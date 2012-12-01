#include "common.h"
struct gdt_entry
{
    uint16 limitlow;
    uint16 baselow;
    uint8  basemiddle;
    uint8  accessbyte;
    uint8  limithighandflag;//4 bits for limithigh 4 higher bits  for flags
    uint8  basehigh;
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_s;

struct gdt_desc
{
    uint16 size;
    uint32 gdt_address;
}
__attribute__((packed));
typedef struct gdt_desc gdt_desc_s;
void initializegdt();
