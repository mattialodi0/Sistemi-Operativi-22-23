#include <lib_def.h>
#include <exceptions.h>

extern void test();
extern void uTLB_RefillHandler();
extern void exceptionHandler();

// processi vivi
int process_count;

// processi bloccati
int soft_blocked_count;

// processi ready
struct list_head ready_queue;

// puntatore al proc attivo
pcb_t *active_process;

// contatore di pid usato in exeptions.c
int pid_count;

// tempo di inizio del quanto di un proc
cpu_t timer_start;

// semaforo per lo pseudo clock
int IT_sem; // da verificare se vada inizializzato a 0

// array di semafori, uno per dispositivo
// int sem_disp[DISPNUM];  un unico array
int sem_dev_disk[8];
int sem_dev_flash[8];
int sem_dev_net[8];
int sem_dev_printer[8];
int sem_dev_terminal_r[8];
int sem_dev_terminal_w[8];

int debug_var = 0;
int debug_var1 = 0;
int debug_char = '0';

int main(void)
{
    process_count = 0;
    soft_blocked_count = 0;
    mkEmptyProcQ(&ready_queue);
    active_process = NULL;
    pid_count = 1;
    IT_sem = 0;

    // setta i semafori a 0
    for (int i = 0; i < 8; i++)
    {
        sem_dev_disk[i] = 0;
        sem_dev_flash[i] = 0;
        sem_dev_net[i] = 0;
        sem_dev_printer[i] = 0;
        sem_dev_terminal_r[i] = 0;
        sem_dev_terminal_w[i] = 0;
    }

    // pass up vector, for processor 0 at 0x0FFF.F900
    passupvector_t *puv = (passupvector_t *)PASSUPVECTOR;
    puv->tlb_refill_handler = (memaddr)uTLB_RefillHandler;
    puv->tlb_refill_stackPtr = 0x20001000;
    puv->exception_handler = (memaddr)exceptionHandler;
    puv->exception_stackPtr = 0x20001000;

    // iniz. strutture fase 1
    initPcbs();
    initASH();
    initNamespaces();

    // iniz. interval timer 100ms
    // unsigned int timescale = 0x1000.0024;     //per leggere il valore della timescale
    LDIT(100000 / timescale); // carica nell'interval timer  T * la timescale del processore

    // creazione di un processo      cap. 2.2 della documentazione
    pcb_t *first_proc = allocPcb();
    process_count++;
    insertProcQ(&ready_queue, first_proc);
    STST(&first_proc->p_s);
    first_proc->p_s.status |= IEPON | IMON | TEBITON;

    // first_proc->p_s.status |= 0x00400000; //cambia come vengono gestite le eccezioni

    first_proc->p_s.reg_t9 = first_proc->p_s.pc_epc = (memaddr)test;

    RAMTOP(first_proc->p_s.reg_sp);     // setta sp a RAMTOP
    first_proc->p_supportStruct = NULL; // settata la struttura di supporto a NULL
    first_proc->p_pid = pid_count++;    // setta il pid

    /*first_proc->p_parent = NULL;    //vanno cambiati
    INIT_LIST_HEAD(&first_proc->p_child);
    INIT_LIST_HEAD(&first_proc->p_sib);
    first_proc->p_semAdd = NULL;*/
    first_proc->p_time = 0;

    // namespace
    nsd_t *ns = allocNamespace(0);
    first_proc->namespaces[ns->n_type] = ns;

    // chiamata allo scheduler
    scheduler();

    return 0;
}

// non so se sia corretto il pass up vector, è un po strano il modo in cui sono usati gli ind. come puntatori
// non so se i bit di status di cui non si parla nelle specifiche siano corretti (tipo BEV)