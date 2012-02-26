#include "vm.h"
#include "common.h"
void InitializePaging()
{
   vpagedir_t *dir = (vpagedir_t *)imalloc(sizeof(vpagedir_t));
   memset(dir, 0, sizeof(vpagedir_t));
   changePagedir(dir);
}

void changePagedir(vpagedir_t *dir)
{
   working_page_directory = dir;
   asm volatile("mov %0, %%cr3":: "b"(working_page_directory));
   uint32 cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging!
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}
