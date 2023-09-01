#include <lib_def.h>

extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int soft_blocked_count;
extern int process_count;
extern cpu_t timer_start;

extern void debug();

// variabile per sapere se il processore è in stato di WAIT
int on_wait;

void scheduler();