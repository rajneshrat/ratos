#include "vm.h"
#include "common.h"
extern int end;
extern uint32 placement_address;// = (uint32)&end;
static int TOTAL_PHYSICAL_MEMORY = 0x10000000;     //512 MB
unsigned char Frames[0x10000];
int GetFreePage(int *index, int *bitpos)
{
   int MaximumIndex = TOTAL_PHYSICAL_MEMORY/0x1000;
   int i = 0, j;
   while(i<MaximumIndex)
   {
      if(Frames[i]^0xff)
      {
         int a = Frames[i];
         for(j=0;j<8;j++)
         {
            if((a&0x01) == 0)
            {
               *index = i;
               *bitpos = j;
               return 1;
            }
            a = a/2;
         }
      }
      i++;
   }  
   return 0; 
}

void SetFrame(int index, int bitpos)
{
   int n = 1;
   int i;
   for(i=0;i<bitpos;i++)
   {
      n = n*2;
   }
   Frames[index] = Frames[index] | n;
}
 
void InitializePaging()
{
   vpagedir_t *kernel_directory = (vpagedir_t *)imalloc(sizeof(vpagedir_t));
   memset(kernel_directory, 0, sizeof(vpagedir_t));
   int x = 0;
   int last = -1;
   while(x<placement_address)   // this is one to one mapping of virtual to real memory for kernel code.
   {
      int add = x/0x1000;
      int indj = add/1024;
      vpagetable_t *ptable;
      if( last != indj )
      {
         ptable = (vpagetable_t *)imalloc(sizeof(vpagetable_t));
         memset(ptable, 0, sizeof(vpagetable_t));
         kernel_directory->tables[indj] = ptable;
         kernel_directory->physical[indj] = (vpagedir_t *)((uint32) ptable | 0x7);
         last = indj;
      }
      int indi = add%1024;
      vpage_t *page = (vpage_t*)&ptable->pages[indi];
      static uint32 num = 0;
      page->frame = num; 
      SetFrame(num/8, num%8);
      num++;
      page->present = 1;
      page->rw = 0;
      page->user = 1;
      x = x + 0x1000;
   }
   changePagedir(kernel_directory);
}

void changePagedir(vpagedir_t *dir)
{
   working_page_directory = dir;
  // asm volatile("mov %0, %%cr3":: "r"(working_page_directory));
   asm volatile("mov %0, %%cr3":: "r"(&working_page_directory->physical));
   uint32 cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging!
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}
