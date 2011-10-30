#include "screen.h"
extern test();
int main(struct multiboot *mboot_ptr)
{
	int i, j;
	char ch = 'a';
//	clr();
	test();
	printstring("rajnesh");
	return 0xDEADBABA;
}
