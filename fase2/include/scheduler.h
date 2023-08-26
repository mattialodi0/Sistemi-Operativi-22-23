#include <lib_def.h>

extern volatile struct list_head ready_queue; 
extern volatile pcb_t *active_process;   
extern volatile int soft_blocked_count;
extern volatile int process_count;
extern volatile cpu_t timer_start;

extern void debug();


void scheduler();

