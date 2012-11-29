#include "vm.h"
#include "common.h"
#define MallocHeapSize 1024*1024*50
extern int end;
extern uint32 placement_address;// = (uint32)&end;
static int TOTAL_PHYSICAL_MEMORY = 0x10000000;     //512 MB
extern uint32 KMallocStartingAddress;
extern uint32 KMallocSize;
unsigned char Frames[0x10000];
int GetFreeFrame(int *index, int *bitpos)  //get the index and bit pos of first free frame.
{
    int MaximumIndex = TOTAL_PHYSICAL_MEMORY/0x1000;
    int i = 0, j;
    while(i<MaximumIndex)
    {
        if(Frames[i]^0xff)
        {
            int a = Frames[i];
            for(j=0; j<8; j++)
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

void SetFrame(int index, int bitpos)  // set the bit pos of that frame to mark it as used.
{
    int n = 1;
    int i;
    for(i=0; i<bitpos; i++)
    {
        n = n*2;
    }
    Frames[index] = Frames[index] | n;
}

int GetFrame( uint32 *frameaddress) // get the addres of first free frame
{
    int index, pos;
    GetFreeFrame(&index, &pos);
    SetFrame(index, pos);
    frameaddress = index*8 + pos;
}

int FreeFrame(uint32 frame)  // given the frame addres , will set its position as free
{
    int index = frame/(0x1000 * 8);
    int pos = frame % ( 0x1000 * 8);
    Frames[index] = Frames[index] & (~ (1<<pos));
}


// written for the malloc implementation. Earlier it was the part of AllocFrame. as in malloc we create the page table from imalloc however for malloc implemetation we need to freeze placement pointer and start the vm.
vpage_t *GetPage(uint32 address, vpagedir_t *dir)  // just create the pagetable but still need to get frame.
{
    vpage_t *returnaddress = NULL;
    if(dir == NULL )
        dir = working_page_directory;
    address = address / 0x1000;
    uint32 tableindex = address / 1024;
    uint32 pageindex = address % 1024;
//   putint(tableindex);
    // puts(" ");
//   putint(pageindex);
    //puts("\n");
    //putint( dir->tables[754] );
//   puts("\n");
    if( NULL !=  dir->tables[tableindex] )
    {
        returnaddress = &(dir->tables[tableindex]->pages[pageindex]);  // tableios already allocated nothingto do;
    }
    else
    {
        printk("creating new page table\n");
        vpagetable_t *ptable = (vpagetable_t *)imalloc(sizeof(vpage_t));
        memset(ptable, 0, sizeof(vpagetable_t));
        dir->tables[tableindex] = ptable;
        dir->physical[tableindex] = (vpagedir_t *)((uint32) ptable | 0x7);
        returnaddress = &(ptable->pages[pageindex]);
    }
    vpage_t *page = (vpage_t*) returnaddress;
    page->present = 0;
    page->rw = 0;
    page->user = 0;
    return returnaddress;
}

// given the address allocate the frame to it. Called at pagefault
void AllocFrame(uint32 address, vpagedir_t *dir)
{

    printk("In AllocFrame\n");
    if( dir == NULL)
    {
        dir = working_page_directory;
    }
    vpage_t *page = (vpage_t*) GetPage(address, dir);
//   puts("printing\n");
//   putint(page->present);
//   puts("\n");
    buzywait(10);
    uint32 Frame;
    GetFrame(&Frame);
    page->frame = Frame;
    page->present = 1;
    page->rw = 1;
    page->user = 1;
}

void InitializePaging() //malloc the space for page directory and initializing it as 1 to 1 mappping.
{
    vpagedir_t *kernel_directory = (vpagedir_t *)imalloc(sizeof(vpagedir_t));
    memset(kernel_directory, 0, sizeof(vpagedir_t));
    int x = 0;
    int last = -1;
    while(x<placement_address + MallocHeapSize)   // this is one to one mapping of virtual to real memory for kernel code.
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
        page->user = 0;
        x = x + 0x1000;
    }
    int i;
    KMallocStartingAddress = placement_address + 0x3000;
    for(i=KMallocStartingAddress; i<KMallocSize; i = i + 0x1000)
    {
        // putint(i);
        //	puts("  ");
        GetPage(i, NULL);
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
