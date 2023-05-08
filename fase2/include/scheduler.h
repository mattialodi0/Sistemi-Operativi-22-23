#include <lib_def.h>

extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int soft_blocked_count;

void scheduler();

