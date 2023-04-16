#include <pandos_const.h>
#include <pandos_types.h>

#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <ash.h>
#include <ns.h>

void scheduler();


//per ora ho messo le def qua per non avere casini con include e extern poi le mettiamo in un .h separato

//processi vivi
int process_count;
//processi bloccati
int soft_blocked_count;
//processi ready
struct list_head *ready_queue; 
//puntatore al proc attivo
pcb_t *active_process;
//contatore di pid usato in exeptions.c
int pid_count;