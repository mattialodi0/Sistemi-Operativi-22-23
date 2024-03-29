#include <lib_def.h>
#include <exceptions.h>

extern void test();
extern void uTLB_RefillHandler();


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
int IT_sem;

// per misurare la durata delle eccezioni
cpu_t exc_timer_start;

// variabile per sapere se il processore è in stato di WAIT
int on_wait;

// array di semafori, uno per dispositivo
int sem_dev_disk[8];
int sem_dev_flash[8];
int sem_dev_net[8];
int sem_dev_printer[8];
int sem_dev_terminal_r[8];
int sem_dev_terminal_w[8];

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

    // popola il pass up vector
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
    LDIT(100000 / timescale);

    // creazione di un processo
    pcb_t *first_proc = allocPcb();
    process_count++;
    insertProcQ(&ready_queue, first_proc);
    STST(&first_proc->p_s);

    first_proc->p_s.status |= IEPON | IMON | TEBITON;
    first_proc->p_s.reg_t9 = first_proc->p_s.pc_epc = (memaddr)test;

    RAMTOP(first_proc->p_s.reg_sp);     // setta sp
    first_proc->p_semAdd = NULL;        // settata l'indirizzo del sem.
    first_proc->p_supportStruct = NULL; // settata la struttura di supporto
    first_proc->p_pid = pid_count++;    // setta il pid
    first_proc->p_time = 0;             // setta il tempo

    // namespace
    nsd_t *ns = allocNamespace(0);
    first_proc->namespaces[ns->n_type] = ns;

    // chiamata allo scheduler
    scheduler();

    return 0;
}