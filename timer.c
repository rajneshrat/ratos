#include "common.h"
#include "timer.h"
#include "screen.h"
void DoTimer()
{

//	static uint32 count = 0;
//	putint(count);
//	puts("\n");
//   count++;
    BlinkCursor();
    AddTicks(1);
//	PremptProcessQueue();
}
