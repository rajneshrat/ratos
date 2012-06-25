#include "debug.h"
#include "screen.h"

void DoCoreDump()
{
	puts("Doing Core Dump\n");
	int a = 1;
   while (1) { a++ ;}
	putint(a);
}
