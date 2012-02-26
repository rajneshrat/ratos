typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;

void outb(uint16 port, uint8 value);
uint16 inw(uint16 port);
uint8 inb(uint16 port);
void memset(uint32 *dest, int val, uint32 len);

