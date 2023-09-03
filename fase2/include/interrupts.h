#include <lib_def.h>
#include <syscalls.h>

#define PLTLINE 1
#define ITLINE 2
#define DEVLINE 3 ... 6
#define TERMLINE 7

extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int IT_sem;
extern cpu_t timer_start;
extern cpu_t exc_timer_start;


/* Gestore degli interrupt */
void interruptHandler();

/* Mette il proc corrente da running a ready, perchè è scaduto il suo quanto di tempo*/
void PLTInterrupt();

/* Sblocca tutti i proc fermi sul semaforo dello pseudo-clock con una V, ogni 100 ms */
void ITInterrupt();

/* Sblocca il proc in attesa della fine dell'operazione di I/O che aveva iniziato */
void nonTimerInterrupt(unsigned int line, unsigned int dev_num);

/* Sblocca il proc in attesa della fine dell'operazione di I/O su un terminale che aveva iniziato */
void nonTimerInterruptTerm(unsigned int line, unsigned int dev_num);


/* Funzione ausiliaria per trovare il numero del device */
unsigned int find_dev_num(unsigned int  bitmap_ind);