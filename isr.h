/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */
} ;

typedef struct regs registers_t;
//structure defination taken from site http://www.osdever.net/bkerndev/Docs/isrs.htm

typedef  void (*irq_handler)(registers_t *r) ;
void attachirqhandler(irq_handler fun, int irq_number);
void initializeidt();

