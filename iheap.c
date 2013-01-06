
//   linked list is used as the base for heap, whenever a malloc request comes, I just break the hole and give one part from it to malloc request.
// The current implementation of kmalloc is as follows-
// We just keep a big buffer in ram for our malloc and are always limited to that, duw to which we also have the upper limit on max maloc we can do.
// Even we are allocating the frame at starting, but we will still see the page fault in malloc this is because we are not giving the actual frames to our malloc.

#include "iheap.h"
#include "debug.h"

extern uint32 end;
uint32 placement_address = (uint32)&end + 0x1000;
uint32 KMallocStartingAddress = 0;//3*1024*1024*1024; // reinitialized in vm.c;
uint32 KMallocSize = 500 * 1024 * 1024;  // giving 500 MB for malloc
#define MallocConsitencyValue 123
struct holes
{
    uint8 cc; 		//to check if our data structure is not get corrupted.Default to 123
    uint32 length;
    struct holes *next;
    uint8 IsHole;
};

static struct holes *head = NULL;

extern int VmEnabled;

uint32 kmalloc( uint32 size )
{
	if(VmEnabled == 0){
		return imalloc(size);
	}
    uint32 returnaddress=0;
    if( head == NULL )
    {
        if( KMallocStartingAddress )
        {
            head = KMallocStartingAddress;
            head->IsHole = 0;
            head->cc = MallocConsitencyValue;
            head->length = size;
            struct holes *temp = (uint32) head + size + sizeof(struct holes);
            temp->IsHole = 1;
            temp->cc = MallocConsitencyValue;
            temp->length = KMallocSize - size - 2*sizeof(struct holes);
            head->next = temp;
            temp->next = NULL;
            returnaddress = (uint32 )head + sizeof(struct holes);
            puts("Initiating the malloc\n");
        }
        else
        {
            puts(" malloc failed at step 1 \n");  //DoCoreDump
        }

    }
    else
    {
        struct holes *ptr = head;
        while(ptr && (ptr->length < size + sizeof(struct holes) ) || (ptr->IsHole == 0))
        {
            if( ptr->next == NULL )
            {
                puts("malloc failed at step 2\n"); //DoCoreDump
                putint(ptr->length);
                puts(" ");
                putint(ptr->IsHole);
                puts(" ");
                putint(ptr->next);
                puts("next\n");
                DoCoreDump();
                return 0;
            }
            ptr = ptr->next;
        }
//		  buzywait(10);
//		  DoCoreDump();
        struct holes *temp = (uint32) ptr + size + sizeof(struct holes);
        temp->length = ptr->length - size - sizeof(struct holes);
        temp->next = ptr->next;
        temp->cc = MallocConsitencyValue;
        temp->IsHole = 1;
        ptr->next = temp;
        ptr->IsHole = 0;
        ptr->length = size;
        returnaddress = (uint32) ptr + sizeof(struct holes);
        puts("Allocating memory from malloc \n");
    }
    return returnaddress;
}

//just looping through all to find the address to free and then just increasing the size of holes next to it.
void kfree(uint32 address)
{
    struct holes *ptr = head;
    struct holes *last = NULL;
    while(( ptr != NULL) && ( ((uint32) ptr + sizeof(struct holes)) != address) )
    {
        last = ptr;
        puts("in\n");
        last = ptr;
        ptr = ptr->next;
    }
    if( (ptr == NULL) || (ptr->cc != MallocConsitencyValue) )
    {
        puts("Malloc get corrupted\n");
        if( ptr == NULL )
        {
            puts("freeing address not found\n");
        }
        DoCoreDump();
    }
    if( (last!=NULL) && ( last->IsHole == 1))
    {
        last->next = ptr->next;
        last->length = last->length + ptr->length + sizeof(struct holes);
    }
    else
    {
        ptr->IsHole = 1;
        last = ptr;
        puts("in\n");
    }
    if((last->next != NULL) && (last->next->IsHole == 1))
    {
        last->length = last->length + last->next->length + sizeof(struct holes);
        last->next = last->next->next;
        puts("in\n");
    }
}


uint32 imalloc(int size)
{
    if (placement_address & 0xFFFFF000) // If the address is not already page-aligned
    {
        // Align it.
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    uint32 tmp = placement_address;
    placement_address += size;
    return tmp;
}

