//#include ``/usr/include/umps3/umps/libumps.h''
#include <pandos_const.h>
#include <pandos_types.h>

#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <ash.h>
#include <ns.h>
#include <scheduler.h>

#define DISPNUM 49
#define timescale 1     //il valore può essere letto dal registro 0x1000.0024 ma per ora assumiamo sia 1

extern void test();
extern void uTLB_RefillHandler();
extern struct list_head *ready_queue; 
extern pcb_t *active_process;   
extern int process_count;
extern int soft_blocked_count;

int main(void) {
    //processi vivi
    process_count = 0;

    //processi bloccati
    soft_blocked_count = 0;

    //processi ready
    ready_queue; 
    mkEmptyProcQ(ready_queue);

    //puntatore al proc attivo
    active_process = NULL;   

    //array di semafori, uno per dispositivo
    //int sem_disp[DISPNUM];  un unico array
    int sem_dev_disk[8];
    int sem_dev_flash[8];
    int sem_dev_net[8];
    int sem_dev_printer[8];
    int sem_dev_terminal_r[8];
    int sem_dev_terminal_w[8];

    //setta i semafori a 0
    for(int i=0; i<8; i++) {
        sem_dev_disk[i] = 0;
        sem_dev_flash[i] = 0;
        sem_dev_net[i] = 0;
        sem_dev_printer[i] = 0;
        sem_dev_terminal_r[i] = 0;
        sem_dev_terminal_w[i] = 0;
    }

    //iniz. strutture fase 1
    initPcbs();
    initASH();
    initNamespaces();

    //pass up vector, for processor 0 at 0x0FFF.F900
    passupvector_t* puv = (passupvector_t*) PASSUPVECTOR;
    puv->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    puv->tlb_refill_stackPtr = 0x20001000;
    puv->exception_handler = (memaddr) 0x20001000;  //fooBar ancora da implementare; per ora ci mettiamo un ind a caso
    puv->exception_stackPtr = 0x20001000;


    //iniz. interval timer 100ms
    //unsigned int timescale = 0x1000.0024;     //per leggere il valore della timescale
    LDIT(100/timescale); // carica nell'interval timer  T * la timescale del processore


    //creazione di un processo      cap. 2.2 della documentazione
    pcb_t *first_proc = allocPcb();
    process_count++;
    insertProcQ(ready_queue, first_proc);

    first_proc->p_s.entry_hi = 0;      //pid
    first_proc->p_s.cause;
    first_proc->p_s.status = first_proc->p_s.status | 17826302;   //KUp = 0 per la kernel-mode, IEp = 1 e IM = 1 per abilitare gli interrupt, TE = 1 per l'interval timer
    first_proc->p_s.pc_epc = (memaddr) test;
    RAMTOP(first_proc->p_s.reg_sp);
    first_proc->p_supportStruct = NULL;     //struttura di supporto settata a NULL

    first_proc->p_parent = NULL;    //vanno cambiati  
    INIT_LIST_HEAD(&first_proc->p_child);
    INIT_LIST_HEAD(&first_proc->p_sib);
    first_proc->p_time = 0;
    first_proc->p_semAdd = NULL;

    //chiamata allo scheduler
    scheduler();

    return 0;
}

//non so se sia corretto il pass up vector, è un po strano il modo in cui sono usati gli ind. come puntatori
//non so se i bit di status di cui non si parla nelle specifiche siano corretti (tipo BEV)