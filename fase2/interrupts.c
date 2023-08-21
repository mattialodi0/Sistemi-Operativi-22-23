#include <interrupts.h>

extern int debug_var;

void interruptHandler()
{
    // per trovare la linea di interrupt
    unsigned int line = 0, cause = 0, dev_num = 0;
    cause = getCAUSE();
    cause &= 0x0000FF00; // maschera per  avere IP
    cause >>= 8;

    // int *addr3 = (int*) 0x10000040;
    // int *addr4 = (int*) (0x10000040 + 0x04);
    // int *addr5 = (int*) (0x10000040 + 0x08);
    // int *addr6 = (int*) (0x10000040 + 0x0C);
    // int *addr7 = (int*) (0x10000040 + 0x10);
    // if((*addr3) > 0) debug_var = 3;
    // else if((*addr4) > 0) debug_var = 4;
    // else if((*addr5) > 0) debug_var = 6;
    // else if((*addr6) > 0) debug_var = 6;
    // else if((*addr7) > 0) debug_var = 7;
    debug_var = cause;
    debugInt();

    // per trovare anche il numero del device
    if ((cause & 1) == 1)
    {
        line = 0;
    }
    else if ((cause & 2) == 2)
    {
        line = 1;
    }
    else if ((cause & 4) == 4)
    {
        line = 2;
    }
    else if ((cause & 8) == 8)
    {
        line = 3;
        dev_num = find_dev_num(0x10000040);
    }
    else if ((cause & 16) == 16)
    {
        line = 4;
        dev_num = find_dev_num(0x10000040 + 0x04);
    }
    else if ((cause & 32) == 32)
    {
        line = 5;
        dev_num = find_dev_num(0x10000040 + 0x08);
    }
    else if ((cause & 64) == 64)
    {
        line = 6;
        dev_num = find_dev_num(0x10000040 + 0x0C);
    }
    else if ((cause & 128) == 128)
    {
        line = 7;
        dev_num = find_dev_num(0x10000040 + 0x10);
    }

    // in caso di più interrupt si risolve quello con priorità più alta (switch)
    switch (line)
    {
    case 0: // ingnorato
        break;
    case 1:
        // PLT
        PLTInterrupt();
        break;
    case 2:
        // Interval Timer
        ITInterrupt();
        break;
    case 3:
        // disk devices
    case 4:
        // flash devices
    case 5:
        // network devices
    case 6:
        // printer devices
        nonTimerInterrupt(line, dev_num);
        break;
    case 7: // prima in scrittura poi in lettura
        // terminal devices
        nonTimerInterruptT(line, dev_num);
        break;
    default:
        break;
    }
}

// è scaduto il quanto di tempo a disposizione del processo corrente e questo va messo da running a ready
void PLTInterrupt()
{
    // caricare un nuovo valore nel PLT
    setTIMER(5); // non è specificato quanto mettere ma credo 5

    // copiare lo stato del processore (all'inizio della BIOS data page) nel pcb del processo corrente
    active_process->p_s = *(state_t *)0x0FFFF000; // non so se sia giusto il cast                                  **************************

    insertProcQ(&ready_queue, active_process);

    scheduler();
}

extern int on_wait;
void ITInterrupt()
{
    LDIT(100000/timescale); // carica nell'interval timer  T * la timescale del processore

    pcb_t *waked_proc;
    // sbloccare tutti i processi fermi al semaforo dello pseudo clock
    while((waked_proc = removeBlocked(&IT_sem)) != NULL) {
        insertProcQ(&ready_queue, waked_proc);
        soft_blocked_count--;
    }

    // settare il semaforo a 0
    IT_sem = 0;
    // // debug 
    // termreg_t *base = (termreg_t *)(0x10000254);
    // debug_var = base->transm_status;  debug3();

    // LDST per tornare il controllo al processo corrente
    if(!on_wait) 
    {
        state_t *state = (state_t *)BIOSDATAPAGE; // costante definita in umps
        LDST(state);
    }
}

void nonTimerInterrupt(unsigned int int_line_no, unsigned int dev_num)
{
    // calcolare l'ind. per il device register
    dtpreg_t *dev_reg = (dtpreg_t *)(0x10000054 + ((7 - 3) * 0x80) + (0 * 0x10)); //int_line_no, dev_num

    // salvare lo status code del device register
    unsigned int status_code = dev_reg->status;

    // ack dell'interrupt: ACKN del device register
    dev_reg->command = ACK;
    
    pcb_t *proc;
    int *ind;

    switch (int_line_no)
    {
    case 3:
        ind = &sem_dev_disk[dev_num];
        break;
    case 4:
        ind = &sem_dev_flash[dev_num];
        break;
    case 5:
        ind = &sem_dev_net[dev_num];
        break;
    case 6:
        ind = &sem_dev_printer[dev_num];
        break;
    default:
        break;
    }

    // V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    // se la V non ritorna il pcb salta le prossime due operazioni
    (*ind)++;
    proc = removeBlocked(ind);

    if (proc != NULL)
    {
        // mettere lo status code nel reg. v0 del pcb del processo sbloccato
        proc->p_s.reg_v0 = status_code;
        // wakeup proc
        insertProcQ(&ready_queue, proc);
        soft_blocked_count--;
    }

    // LDST per tornare il controllo al processo corrente
    state_t *state = (state_t *)BIOSDATAPAGE;
    LDST(state);

}

void nonTimerInterruptT(unsigned int int_line_no, unsigned int dev_num)
{
    // da distinguere R e W 

    // calcolare l'ind. per il device register
    // dtpreg_t *dev_reg = (dtpreg_t *)(0x10000054 + ((int_line_no - 3) * 0x80) + (dev_num * 0x10));
    termreg_t *dev_reg = (termreg_t *)(0x10000254);

    // salvare lo status code del device register
    unsigned int status_code = dev_reg->transm_status; 

    // ack dell'interrupt: ACKN del device register
    dev_reg->transm_command = ACK;

    pcb_t *proc;
    int *semaddr = &sem_dev_terminal_w[dev_num];  // non va bene  

    // V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    // se la V non ritorna il pcb salta le prossime due operazioni
    (*semaddr)++;
    proc = removeBlocked(semaddr);
    if (proc != NULL)
    {
        // mettere lo status code nel reg. v0 del pcb del processo sbloccato
        proc->p_s.reg_v0 = status_code;
        // wakeup proc
        insertProcQ(&ready_queue, proc);
        soft_blocked_count--;
    }

    // LDST per tornare il controllo al processo corrente
    state_t *state = (state_t *)BIOSDATAPAGE;
    LDST(state);
}

unsigned int find_dev_num(unsigned int bitmap_ind)
{
    unsigned int num;
    unsigned int *bitmap = (unsigned int*) bitmap_ind;
    
    // debug_var = *bitmap;
    // debug4();
    
    if ((*bitmap & 128) == 128)
        num = 7;
    else if ((*bitmap & 64) == 64)
        num = 6;
    else if ((*bitmap & 32) == 32)
        num = 5;
    else if ((*bitmap & 16) == 16)
        num = 4;
    else if ((*bitmap & 8) == 8)
        num = 3;
    else if ((*bitmap & 4) == 4)
        num = 2;
    else if ((*bitmap & 2) == 2)
        num = 1;
    else if ((*bitmap & 1) == 1)
        num = 0;

    return num;
}