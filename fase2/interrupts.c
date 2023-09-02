#include <interrupts.h>

/* Gestore degli interrupt */
void interruptHandler()
{
    // per trovare la linea dell interrupt
    unsigned int line = 0, cause = 0, dev_num = 0;
    cause = getCAUSE();
    cause &= 0x0000FF00; // maschera per  avere IP
    cause >>= 8;

    // per trovare il numero del device
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

    // in caso di più interrupt si risolve quello con priorità più alta
    switch (line)
    {
    case 0:
        // ingnorato
        break;
    case PLTLINE:
        PLTInterrupt();
        break;
    case ITLINE:
        ITInterrupt();
        break;
    case DEVLINE:
        // disk, flash, network, printer devices
        nonTimerInterrupt(line, dev_num);
        break;
    case TERMLINE:
        // terminal devices
        nonTimerInterruptTerm(line, dev_num);
        break;
    default:
        PANIC();
        break;
    }
}

/* Mette il proc corrente da running a ready, perchè è scaduto il suo quanto di tempo*/
void PLTInterrupt()
{
    // carica 5 ms nel PLT
    setTIMER(5000);

    // copia lo stato del processore nel pcb del processo corrente
    active_process->p_s = *((state_t *)0x0FFFF000);

    insertProcQ(&ready_queue, active_process);

    update_time();

    scheduler();
}

/* Sblocca tutti i proc fermi sul semaforo dello pseudo-clock con una V, ogni 100 ms */
void ITInterrupt()
{
    // carica 100 ms nell'interval timer
    LDIT(100000 / timescale);

    pcb_t *waked_proc;
    // sblocca tutti i processi fermi al semaforo dello pseudo clock
    while (headBlocked(&IT_sem) != NULL)
    {
        waked_proc = removeBlocked(&IT_sem);
        insertProcQ(&ready_queue, waked_proc);
        soft_blocked_count--;
    }

    // setta il semaforo a 0
    IT_sem = 0;

    remove_time();

    // LDST per tornare il controllo al processo corrente
    if (!on_wait)
    {
        state_t *state = (state_t *)BIOSDATAPAGE;
        LDST(state);
    }
    else
        scheduler();
}

/* Sblocca il proc in attesa della fine dell'operazione di I/O che aveva iniziato */
void nonTimerInterrupt(unsigned int int_line_no, unsigned int dev_num)
{
    // calcola l'ind. per il device register
    dtpreg_t *dev_reg = (dtpreg_t *)(0x10000054 + ((int_line_no - 3) * 0x80) + (dev_num * 0x10));

    // salva lo status code del device register
    unsigned int status_code = dev_reg->status;

    // ack dell'interrupt
    dev_reg->command = ACK;

    pcb_t *proc;
    int *semaddr;

    switch (int_line_no)
    {
    case 3:
        semaddr = &sem_dev_disk[dev_num];
        break;
    case 4:
        semaddr = &sem_dev_flash[dev_num];
        break;
    case 5:
        semaddr = &sem_dev_net[dev_num];
        break;
    case 6:
        semaddr = &sem_dev_printer[dev_num];
        break;
    default:
        break;
    }

    // V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    proc = removeBlocked(semaddr);
    if (proc != NULL)
    {
        // mette il valore di ritorno nel reg. v0 del pcb del processo sbloccato
        if ((status_code & 0x000000FF) == 5)
            proc->p_s.reg_v0 = 0;
        else
            proc->p_s.reg_v0 = -1;
        *proc->io_addr = status_code & 0x000000FF;

        insertProcQ(&ready_queue, proc);
        soft_blocked_count--;
    }

    if (on_wait)
    {
        update_time_proc(proc);
        remove_time();
        scheduler();
    }
    else
    {
        update_time_proc(proc);
        remove_time();
        state_t *state = (state_t *)BIOSDATAPAGE;
        LDST(state);
    }
}

/* Sblocca il proc in attesa della fine dell'operazione di I/O su un terminale che aveva iniziato */
void nonTimerInterruptTerm(unsigned int int_line_no, unsigned int dev_num)
{
    // calcola l'ind. per il device register
    termreg_t *dev_reg = (termreg_t *)(0x10000054 + ((int_line_no - 3) * 0x80) + (dev_num * 0x10));
    int *semaddr;

    // per distinguere read e write sul terminale
    if ((dev_reg->transm_status & 0x5) == 0x5)
    {
        semaddr = &sem_dev_terminal_w[dev_num];
    }
    else
    {
        semaddr = &sem_dev_terminal_r[dev_num];
    }

    // salva lo status code del device register
    unsigned int status_code = dev_reg->transm_status;

    // ack dell'interrupt
    dev_reg->transm_command = ACK;

    // V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    pcb_t *proc;
    proc = removeBlocked(semaddr);
    if (proc != NULL)
    {
        // mette il valore di ritorno nel reg. v0 del pcb del processo sbloccato
        if ((status_code & 0x000000FF) == 5)
            proc->p_s.reg_v0 = 0;
        else
            proc->p_s.reg_v0 = -1;
        *proc->io_addr = status_code & 0x000000FF;

        insertProcQ(&ready_queue, proc);
        soft_blocked_count--;
    }

    if (on_wait)
    {
        update_time_proc(proc);
        remove_time();
        scheduler();
    }
    else
    {
        update_time_proc(proc);
        remove_time();
        state_t *state = (state_t *)BIOSDATAPAGE;
        LDST(state);
    }
}

/* Funzione ausiliaria per trovare il numero del device */
unsigned int find_dev_num(unsigned int bitmap_ind)
{
    unsigned int num;
    unsigned int *bitmap = (unsigned int *)bitmap_ind;

    if ((*bitmap & 1) == 1)
        num = 0;
    else if ((*bitmap & 2) == 2)
        num = 1;
    else if ((*bitmap & 4) == 4)
        num = 2;
    else if ((*bitmap & 8) == 8)
        num = 3;
    else if ((*bitmap & 16) == 16)
        num = 4;
    else if ((*bitmap & 32) == 32)
        num = 5;
    else if ((*bitmap & 64) == 64)
        num = 6;
    else if ((*bitmap & 128) == 128)
        num = 7;

    return num;
}