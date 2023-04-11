#include ``/usr/include/umps3/umps/libumps.h''
#include <pandos_const.h>
#include <pandos_types.h>

#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <ash.h>
#include <ns.h>
#include "scheduler.h"

#define DISPNUM 49

int main(void) {
    //processi vivi
    int process_count = 0;

    //processi bloccati
    int soft_blocked_count = 0;
    
    //processi ready
    struct list_head *ready_queue; 
    mkEmptyProcQ(ready_queue);

    //puntatore al proc attivo
    pcb_t *active_process = NULL;   

    //array di semafori, uno per dispositivo
    int sem_disp[DISPNUM];
    for(int i=0; i<49; i++) 
        sem_disp[i] = 0;

    //iniz. strutture fase 1
    initPcbs();
    initSemd();
    initNS();

    //pass up vector, for processor 0 at 0x0FFF.F900
    0x0FFF.F900->tlb_refll_handler = (memaddr) uTLB_RefillHandler;    // set the Nucleus TLB-Refill event handler address
    (0x0FFF.F900 + 0x04)* = 0x2000.1000;    // set the Stack Pointer for the Nucleus TLB-Refill event handler to the top of the Nucleus stack page: 0x2000.1000
    (0x0FFF.F900 + 0x08)->exception_handler = (memaddr) fooBar;   // set the Nucleus exception handler address to the address of your Level 3 Nucleus function that is to be the entry point for exception handling 
    (0x0FFF.F900 + 0x0c)* = 0x2000.1000;    // set the Stack pointer for the Nucleus exception handler to the top of the Nucleus stack page: 0x2000.1000


    //iniz. interval timer 100ms
    //registro:  0x1000.0020
    //LDIT(T) aggiorna l'interval timer con T per la timescale del processore

    //creazione di un processo

    //chiamata allo scheduler
    scheduler();
}

//non so se sia corretto il pass upo vector, è un po strano il modo in cui sono usati gli ind. come puntatori