#include "scheduler.h"

//scelta pcb 
//rimozione pcb dalla testa e tornare il puntatore al pcb nel campo dei processi correnti
//caricare 5 millisecondi come time slice
//creare un Load Processor State sullo stato del processore memorizzato nel pcb del processo corrente
//cambio stato (?)
//se il process count è 0 richiama l'HALT BIOS
//se il process count è > 0 e il Soft-Block Count > 0 va in Wait State 
//controllo se process count > 0 e Soft-Block Count == 0 => deadlock. ???. Richiamare PANIC BIOS.

extern struct list_head *ready_queue; 
extern pcb_t *active_process = NULL;   
extern int process_count;
extern int soft_blocked_count;

void scheduler() {
    active_process = removeProcQ(ready_queue);
    
    //load PLT

    //load state

    if(process_count == 0) {
        HALT();     //probabilmente anche qui servono gli interrupt
    }
    else if(process_count > 0) {
        if(soft_blocked_count > 0) {
            //abilita gli interrupt, disabilita PLT
            //WAIT
        }
        else if(soft_blocked_count == 0) {
            //deadlock detection
            PANIC();
        }
    }
}