// process.c is mainly for tasks scheduling and fork management written for ratos.

#include "process.h"
#include "common.h"


static uint32 EIP = 0;

struct Process{
	uint32 pid;
	uint32 stackPointer;
	uint32 stackStartingAddress;  // this we need to copy stack while doing fork.
	uint32 iPointer;
	uint32 basePointer;
	struct Process *next;
};

typedef struct Process Tasks;

Tasks ProcessQueue;
Tasks *newTask;// = imalloc(sizeof(Tasks)+100);

static uint32 newPid;

static Tasks *currentTask = NULL;

extern uint32 GetEIPofInterruptedTask();
extern uint32 GetEIP();
extern uint32 firstProcessStartingStack;
void SaveTask(Tasks *task)
{
	task->iPointer = GetEIP();
}

static struct {
    unsigned long address;
    unsigned short segment;
} direct ;


void DumpStack(Tasks *srcTask)
{
	printk("Printing Stack for %d\n",srcTask->pid);
	int sign = 4;
	uint32 srcStack = (uint32 )srcTask->stackStartingAddress;
	uint32 srcStackCurrent = (uint32 )srcTask->stackPointer;
	printk("printing from %u to %u\n",srcStack, srcStackCurrent);
	
	if(srcStack > srcStackCurrent){
		sign = -4;
	}

	 int count = 0;
	while(srcStack != srcStackCurrent){
		if(
	//		 *(uint32*)srcStack 
//		&& 
		 	srcStackCurrent < srcStack + 10 )
		{
			printk("%u, %u   ",srcStack, *(uint32*)srcStack);
		    count ++;
			//printk("\n");	
		}
		if(count ==4 ){
			printk("\n");	
			count =0;
		}
   		srcStack = srcStack + sign;
	}
}

// this not only copies stack but also set base pointer and instruction pointer.	
void CopyStack(Tasks *srcTask, Tasks *dstTask)
{
	int val = 23;
	int sign = 1;
	uint32 srcStack = (uint32 )srcTask->stackStartingAddress;
	uint32 srcStackStart = srcStack;
	uint32 srcStackCurrent = (uint32 )srcTask->stackPointer;
    asm (
         "movl %%esp, %0;"
         : "=r" (srcStackCurrent)
         : 
        );
	uint32 dstStack = (uint32 )kmalloc(4000);
	dstTask->stackStartingAddress = dstStack;
    uint32 diff =  dstStack - srcStackStart;
	printk("CopyStack called\n");
	uint32 ebp;
	asm volatile("mov %%ebp, %0" : "=r"(ebp) :);
	dstTask->basePointer = ebp + diff;
	dstTask->stackPointer = srcStackCurrent + diff;
	dstStack = dstTask->stackPointer;
	if(srcStack > srcStackStart){
		sign = -1;
	}
	srcStack = srcStackCurrent;
	printk("Will copt from %d to %d\n",srcStack,srcStackStart);
	while(srcStack != srcStackStart){
		if(
		(*(char *)srcStack <= srcTask->stackStartingAddress && *(char *)srcStack >= srcTask->stackPointer)
	//		||
	//		(*(char *)srcStack > srcTask->stackStartingAddress && *(char *)srcStack < srcTask->stackPointer) 
		) {
			*(char *)dstStack = *(char *)srcStack + diff;
		}
		else{
			*(char *)dstStack =  *(char *)srcStack ;
	    }		
		srcStack = srcStack + sign;
		dstStack = dstStack + sign;
	}
	if( 
		(*(char *)srcStack <= srcTask->stackStartingAddress && *(char *)srcStack >= srcTask->stackPointer)
	){
	 *(char *)dstStack = *(char *)srcStack + diff;
	}
	else{
		*(char *)dstStack = *(char *)srcStack ;
	}
	EIP = GetEIP();
	dstTask->iPointer = EIP;
//	DumpStack(dstTask);
//	DumpStack(srcTask);
}

void ScheduleTask(Tasks *task)
{
	int times=0;
	asm volatile("cli");
	printk("Scheduler Called ");
    printk("current task id = %d ",currentTask->pid);
	printk("and next task id = %d\n",task->pid);
    currentTask->iPointer = GetEIP();
	uint32 ebp;
	asm volatile("movl %%ebp, %0" : "=r"(ebp):);
	currentTask->basePointer = ebp;
	uint32 esp;
    asm (
         "movl %%esp, %0;"
         : "=r" (esp)
         : 
        );
	currentTask->stackPointer = esp;
	char *stackCurrent;
	stackCurrent = task->stackPointer;
	times++;
    sleep(5);
	if(times>=2){
	    asm volatile("sti");  //the process which was interrrupted will return from here.
		return ;
	}
    /*direct.address = eip;
    uint32 kernelcodesegment = 0;
    asm ("movl %%cs, %0;"
         : "=r" (kernelcodesegment)
         :);
    direct.segment = kernelcodesegment;
	asm( "jmp (%%edi)"
         :
         :
		 "D"(&eip)
       );
  */
	currentTask = task;  //making new process as current
    asm (
         "movl %0, %%esp;"
		 :
         : "r" (currentTask->stackPointer)
        );
	asm volatile("mov %0, %%ebp" : :"r"(currentTask->basePointer));
	asm volatile("sti");
/*   asm( "ljmp (%%edi)"
         :
         :
		 "D"(&direct)
       );
*/
	//asm volatile("pop %eax");
	printk("Changing Process\n");
	asm( "jmp (%%edi)"
         :
         :
		 "D"(&currentTask->iPointer)  // it is necessary to have this as static as after changing basepointer values will change.
       );
}

void PremptProcessQueue()
{
	Tasks *nextTask = currentTask->next;
	ScheduleTask(nextTask);
}

void check(int n)
{
	Tasks *temp = currentTask;
	int i;
	for(i=0;i<n;i++){
		printk("%d \n",temp->pid);
		sleep(5);
		temp = temp->next;
	}
}

void DumpProcess(Tasks *task)
{
	
	printk("pid %d ",task->pid);
	printk("Stack Pointer %d", task->stackPointer);
	printk("Stack Starting %d", task->stackStartingAddress);  // this we need to copy stack while doing fork.
	printk("Ipointer %d", task->iPointer);
	printk("Base %d\n",task->basePointer);
}

int fork()
{
	asm volatile("cli");
	if(currentTask==NULL){  //this is the first time we are called
		printk("Fork called first time will do initialization\n");
	    currentTask = kmalloc(sizeof(Tasks));
		currentTask->pid = newPid;
		char *stackCurrent;
    	asm( "movl %%esp, %0;" 
			: "=r" (stackCurrent)
      	    : 
       	 );
		currentTask->stackPointer = stackCurrent;
		currentTask->stackStartingAddress =  firstProcessStartingStack;
		uint32 ebp; 
		asm volatile("movl %%ebp, %0" : "=r"(ebp) :);
		currentTask->basePointer = ebp;
		currentTask->iPointer = GetEIP();
		currentTask->next = currentTask;
		newPid++;
	}
	uint32 parentId = currentTask->pid;
	Tasks *newTask = kmalloc(sizeof(Tasks));
	newTask->pid = newPid;
	uint32 childPid ;
	childPid = newPid;
	newPid++;
	printk("fork called by process %d\n",currentTask->pid);
	Tasks *temp = currentTask->next;
	currentTask->next = newTask;
	newTask->next = temp;
	CopyStack(currentTask, newTask);
	asm volatile("sti");
	if(currentTask->pid == parentId){
		return childPid;
	}
	else{
		return 0;
	}
}
