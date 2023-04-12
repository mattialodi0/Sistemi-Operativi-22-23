#include ``/usr/include/umps3/umps/libumps.h''
#include <pandos_const.h>
#include <pandos_types.h>

#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <ash.h>
#include <ns.h>
#include "scheduler.h"

#define DISPNUM 49
#define timescale 1     //il valore può essere letto dal registro 0x1000.0024 ma per ora assumiamo sia 1

extern void test();


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
    //unsigned int timescale = 0x1000.0024;     //per leggere il valore della timescale
    LDIT(100/timescale); // carica nell'interval timer  T * la timescale del processore

    //creazione di un processo      cap. 2.2 della documentazione
    pcb_t *first_proc = allocPcb();

    first_proc->p_s.s_entryHI = 0;      //pid
    first_proc->p_s.s_cause;
    first_proc->p_s.s_status = setStatus(10001000000000001111111100000100);   //KUp = 0 per la kernel-mode, IEp = 1 e IM = 1 per abilitare gli interrupt, TE = 1 per l'interval timer
    first_proc->p_s.s_pc = (memaddr) test;
    first_proc->p_s.s_sp = RAMTOP;
    
    first_proc->p_parent = NULL;      
    first_proc->p_child = NULL;
    first_proc->p_sib = NULL;
    first_proc->p_time = 0;

    //chiamata allo scheduler
    scheduler();
}

//non so se sia corretto il pass upo vector, è un po strano il modo in cui sono usati gli ind. come puntatori
//non so se i bit di status di cui non si parla nelle specifiche siano corretti (tipo BEV)