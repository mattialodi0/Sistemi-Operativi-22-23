#include <lib_def.h>
#include <scheduler.h>

extern struct list_head *ready_queue; 
extern pcb_t *active_process;   

void interruptHandler();

//è scaduto il quanto di tempo a disposizione del processo corrente e questo va messo da running a ready
void PLTInterrupt();

void ITInterrupt();

void nonTimerInterrupt();

unsigned int find_dev_num(unsigned int  bit_map_word);