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
	char   pName[20];
	uint32 state;
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
} direct, stackAdd, basePoi ;

void DoPs()
{
	printf("Pid		Process Name	 State\n");
	printf("%d		%s				 %d\n", currentTask->pid, currentTask->pName, currentTask->state);
	printf("\n");
}

void DumpStack(Tasks *srcTask)
{
	printf("Printing Stack for %d\n",srcTask->pid);
	int sign = 4;
	uint32 srcStack = (uint32 )srcTask->stackStartingAddress;
	uint32 srcStackCurrent = (uint32 )srcTask->stackPointer;
	printf("printing from %u to %u\n",srcStack, srcStackCurrent);
	
	if(srcStack > srcStackCurrent){
		sign = -4;
	}
// return;
	 int count = 0;
	while(srcStack != srcStackCurrent){
		if(
	//		 *(uint32*)srcStack 
//		&& 
		 	srcStackCurrent < srcStack + 10 )
		{
			printf("%u, %u   ",srcStack, *(uint32*)srcStack);
		    count ++;
			//printf("\n");	
		}
		if(count ==4 ){
			printf("\n");	
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
	uint32 dstStack;
//	if(srcTask == dstTask){
//		dstStack = (uint32 )imalloc(4000);
//	}
//	else{
		dstStack = (char *)kmalloc(4000);
			printf("Doing Check for %d\n",currentTask->pid);
		//	int n = 100000;
		//	char  *a = kmalloc(n);
		//	int i;
		//	for(i=0;i<n;i++){
		//		*a = 12;
		//		*(char *)dstStack = 12;
		//		a++;
		//		dstStack++;
		//	}
		//	printf("Done");
 //	}
	printf("Malloc address = %u\n",dstStack);
//	dstStack = (uint32 )imalloc(4000);
 	printf("Malloc address = %u\n",dstStack);
    uint32 diff =  dstStack - srcStackStart;
	printf("CopyStack called\n");
	uint32 ebp;
	asm volatile("mov %%ebp, %0" : "=r"(ebp) :);
	dstTask->basePointer = ebp + diff;
	dstTask->stackPointer = dstStack;//srcStackCurrent + diff;
//	dstStack = dstTask->stackPointer;
//	dstTask->stackPointer = dstStack;
	if(srcStack > srcStackStart){
		sign = -1;
	}
	srcStack = srcStackCurrent;
	printf("Will copt from %d to %d\n",srcStack,srcStackStart);
	while(srcStack != srcStackStart){
		if(
		(*(char *)srcStack <= srcTask->stackStartingAddress && *(char *)srcStack >= srcTask->stackPointer)
	//		||
	//		(*(char *)srcStack > srcTask->stackStartingAddress && *(char *)srcStack < srcTask->stackPointer) 
		) {
			*(char *)dstStack = *(char *)srcStack + diff ;
		}
		else{
		*(char *)dstStack =  *(char *)srcStack ;
	    }		
		//	printf("Value copying for %u with %u\n",dstStack, *(uint32 *)(srcStack));
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
	dstTask->stackStartingAddress = dstStack;
//	EIP = GetEIP();
//	int a = 5;
	dstTask->iPointer = GetEIP();
//	printf("Camed from %d and %d\n",currentTask->pid,a);
//	DumpStack(srcTask);
//	DumpStack(dstTask);
}
int loop = 0;
void ScheduleTask(Tasks *task)
{
	int times=0;
	asm volatile("cli");
	printf("Scheduler Called ");
    printf("current task id = %d ",currentTask->pid);
	printf("and next task id = %d\n",task->pid);
    loop = 0;
	currentTask->iPointer = GetEIP();
	loop++;
	if( loop >=2 ){
	    printf("Exiting from schedule loop %d\n", loop);
		asm volatile("sti");  //the process which was interrrupted will return from here.
		return;
	}
	printf("Address = %x and %x\n",task->iPointer, currentTask->iPointer);
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
/*	asm( "jmp (%%edi)"
         :
         :
		 "D"(&direct)
       );
  */
	currentTask = task;  //making new process as current
    uint32 kernelcodesegment = 0;
    asm ("movl %%cs, %0;"
         : "=r" (kernelcodesegment)
         :);
    direct.segment = kernelcodesegment;
    stackAdd.segment = kernelcodesegment;
    direct.address = currentTask->iPointer;
    stackAdd.address = currentTask->stackPointer;
    basePoi.segment = kernelcodesegment;
    basePoi.address = currentTask->basePointer;
	//asm volatile("mov %0, %%ebp" : :"r"(currentTask->basePointer));
	asm volatile("mov %0, %%ebp" : :"r"(basePoi));
    asm (
         "movl %0, %%esp;"
		 :
         //: "r" (currentTask->stackPointer)
         : "r" (stackAdd)
        );
	printf("Changing Process %d current id\n", currentTask->pid);
	asm volatile("sti");
 	asm( "ljmp (%%edi)"
         :
         :
		 "D"(&direct)
       );

	//asm volatile("pop %eax");
/*	asm( "jmp (%%edi)"
         :
         :
		 "D"(&currentTask->iPointer)  // it is necessary to have this as static as after changing basepointer values will change.
       );
*/
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
		printf("%d \n",temp->pid);
		temp = temp->next;
	}
}

void DumpProcess(Tasks *task)
{
	
	printf("pid %d ",task->pid);
	printf("Stack Pointer %d", task->stackPointer);
	printf("Stack Starting %d", task->stackStartingAddress);  // this we need to copy stack while doing fork.
	printf("Ipointer %d", task->iPointer);
	printf("Base %d\n",task->basePointer);
}

int fork()
{
	asm volatile("cli");
	if(currentTask==NULL){  //this is the first time we are called
		printf("Fork called first time will do initialization\n");
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
		//currentTask->iPointer = GetEIP();
		currentTask->next = currentTask;
		newPid++;
//		CopyStack(currentTask, currentTask);
	}
	uint32 parentId = currentTask->pid;
	Tasks *newTask = kmalloc(sizeof(Tasks));
	newTask->pid = newPid;
	uint32 childPid ;
	childPid = newPid;
	newPid++;
	printf("fork called by process %d\n",currentTask->pid);
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
