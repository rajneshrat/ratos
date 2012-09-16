#include "debug.h"
#include "screen.h"
#include "common.h"

void DoCoreDump()
{
	puts("Doing Core Dump\n");
	int a = 1;
	int ret=1;
	while( ret != NULL ){
		asm(" pop (%0)\n\t"
			:"=r" (ret)
		);
   	printf("%d\n",ret);
   }
	while (1) { a++ ;}
	putint(a);
}
