#include <lib_def.h>
#include <syscalls.h>

extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int IT_sem;
extern cpu_t timer_start;
extern cpu_t exc_timer_start;

void interruptHandler();

//è scaduto il quanto di tempo a disposizione del processo corrente e questo va messo da running a ready
void PLTInterrupt();

void ITInterrupt();

void nonTimerInterrupt(unsigned int line, unsigned int dev_num);

void nonTimerInterruptT(unsigned int line, unsigned int dev_num);


unsigned int find_dev_num(unsigned int  bitmap_ind);

// aggiornamento del tempo di uso della CPU
void update_time();
void update_time_proc(pcb_t * proc);
// elimina il tempo usato nella gestione dell'eccezione da quello del proc
void remove_time();