typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;

#define NULL (void *)0
void outb(uint16 port, uint8 value);
void buzywait(uint32 time);
uint16 inw(uint16 port);
uint8 inb(uint16 port);
void memset(uint32 *dest, int val, uint32 len);
void outl(uint16 port, uint32 v);
uint32 inl(uint16 port);
void sleep(uint32 time);
void copy(unsigned char *dest, unsigned char *src, int len);

