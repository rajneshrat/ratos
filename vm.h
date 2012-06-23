#include "common.h"
typedef struct vpage
{
   uint32 present    : 1;   // Page present in memory
   uint32 rw         : 1;   // Read-only if clear, readwrite if set
   uint32 user       : 1;   // Supervisor level only if clear
   uint32 accessed   : 1;   // Has the page been accessed since last refresh?
   uint32 dirty      : 1;   // Has the page been written to since last refresh?
   uint32 unused     : 7;   // Amalgamation of unused and reserved bits
   uint32 frame      : 20;  // Frame address (shifted right 12 bits)
}vpage_t;

typedef struct vpagetable
{
	vpage_t pages[1024];
}vpagetable_t;

typedef struct pagedir
{
	vpagetable_t *tables[1024];
        uint32 physical[1024];
}vpagedir_t;	

vpagedir_t *working_page_directory;

void InitializePaging();
void AllocFrame(uint32 address, vpagedir_t *dir); 

void changePagedir(vpagedir_t *dir);
