#include <lib_def.h>
#include <exceptions.h>


extern void test();
extern void uTLB_RefillHandler();
extern void exceptionHandler();
/*
extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int process_count;
extern int soft_blocked_count;
extern int IT_sem;
*/
//processi vivi
int process_count;
//processi bloccati
int soft_blocked_count;
//processi ready
struct list_head ready_queue; 
//puntatore al proc attivo
pcb_t *active_process;
//contatore di pid usato in exeptions.c
int pid_count;

//semaforo per lo pseudo clock
int IT_sem;     // da verificare se vada inizializzato a 0


int main(void) {  
    //processi vivi
    process_count = 0;

    //processi bloccati
    soft_blocked_count = 0;

    //processi ready
    mkEmptyProcQ(&ready_queue);

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
    
    //setta a 0 il semaforo dello pseudo clock
    IT_sem = 0;

    //iniz. strutture fase 1
    initPcbs();
    initASH();
    initNamespaces();

    //pass up vector, for processor 0 at 0x0FFF.F900
    passupvector_t* puv = (passupvector_t*) PASSUPVECTOR;
    puv->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    puv->tlb_refill_stackPtr = 0x20001000;
    puv->exception_handler = (memaddr) exceptionHandler;
    puv->exception_stackPtr = 0x20001000;


    //iniz. interval timer 100ms
    //unsigned int timescale = 0x1000.0024;     //per leggere il valore della timescale
    LDIT(100000/timescale); // carica nell'interval timer  T * la timescale del processore


    //creazione di un processo      cap. 2.2 della documentazione
    pcb_t *first_proc = allocPcb();
    process_count++;
    insertProcQ(&ready_queue, first_proc);

    //set dello stato 
    first_proc->p_s.status = getSTATUS();
    //interrupt abilitati 
    first_proc->p_s.status |= IEPON;
    first_proc->p_s.status |= IMON;
    //PLT abilitato
    first_proc->p_s.status |= TEBITON;
    //kernel mode abilitata
    //first_proc->p_s.status &= ~USERPON; //lo è già
    
    //first_proc->p_s.status |= 0x00400000; //cambia come vengono gestite le eccezioni

    first_proc->p_s.pc_epc = (memaddr) test;

//    first_proc->p_s.entry_hi = 0;      //pid forse

    RAMTOP(first_proc->p_s.reg_sp);     //setta sp a RAMTOP
    first_proc->p_supportStruct = NULL;     //settata la struttura di supporto a NULL
    first_proc->p_pid = 1;                  //setta il pid

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