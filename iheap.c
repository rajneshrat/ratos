#include "iheap.h"
#include "debug.h"
extern uint32 end;
uint32 placement_address = (uint32)&end + 0x1000;
uint32 KMallocStartingAddress = 0;//  = placement_address + 0x1000;
uint32 KMallocSize = 500 * 1024 * 1024;  // giving 500 MB for malloc

struct holes
{
	uint32 length;
	uint32 next;
	uint8 IsHole;
//   uint32 address;
};
    
static struct holes *head = NULL;

uint32 kmalloc( uint32 size )
{
	 uint32 returnaddress=0;
    if( head == NULL )
	 {
	     if( KMallocStartingAddress )
    	  {
			  head = KMallocStartingAddress;
			  head->IsHole = 0;
		     head->length = size;
			  struct holes *temp = KMallocStartingAddress + size + sizeof(struct holes);
        	  temp->IsHole = 1;
			  temp->length = KMallocSize - size - 2*sizeof(struct holes);
			  head->next = temp;
			  temp->next = NULL;
			  returnaddress = head + sizeof(struct holes);
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
		  while( (ptr->length < size + sizeof(struct holes) ) || (ptr->IsHole == 0))
		  {
			  if( ptr->next == NULL )
			  {
				   puts("malloc failed at step 2\n"); //DoCoreDumpi
				   putint(ptr->length); puts(" "); putint(ptr->IsHole);
				   DoCoreDump();
				   return 0;
			  }
			  ptr = ptr->next;
		  }
		  struct holes *temp = ptr + size + sizeof(struct holes);
		  temp->length = ptr->length - size - sizeof(struct holes);
		  temp->next = ptr->next;
		  temp->IsHole = 1;
		  ptr->next = temp;
		  ptr->IsHole = 0;
		  ptr->length = size;
		  returnaddress = ptr + sizeof(struct holes);
		  puts("Allocating memory from malloc \n");
	 }
    return returnaddress;
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

