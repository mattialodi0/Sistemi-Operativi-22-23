#include <interrupts.h>

extern int debug_var;

void interruptHandler()
{
    // per trovare la linea di interrupt
    unsigned int cause, dev_num;
    cause = getCAUSE();
    cause >>= 8;
    cause &= 11111111; // maschera per avere solo IP

    debug_var = cause;

    // per trovare anche il numero del device
    if (cause == 0) {
        return;
    }
    else if (cause & LOCALTIMERINT > 0)
    { // linea 1
        PLTInterrupt();
    }
    else if (cause & TIMERINTERRUPT > 0)
    { // linea 2
        ITInterrupt();
    }
    else if (cause & DISKINTERRUPT > 0)
    { // linea 3
        nonTimerInterrupt(3, dev_num);
        dev_num = find_dev_num(0x10000040);
    }
    else if (cause & FLASHINTERRUPT > 0)
    { // linea 4
        nonTimerInterrupt(4, dev_num);
        dev_num = find_dev_num(0x10000040 + 0x04);
    }
    else if (cause & 0x00002000 > 0)
    { // linea 5
        nonTimerInterrupt(5, dev_num);
        dev_num = find_dev_num(0x10000040 + 0x08);
    }
    else if (cause & PRINTINTERRUPT > 0)
    { // linea 6
        nonTimerInterrupt(6, dev_num);
        dev_num = find_dev_num(0x10000040 + 0x0C);
    }
    else if (cause & TERMINTERRUPT > 0)
    { // linea 7
        nonTimerInterrupt(7, dev_num);
        dev_num = find_dev_num(0x10000040 + 0x10);
    }
        debug();
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

void ITInterrupt()
{    
    debug1();
    LDIT(100000 / timescale); // carica nell'interval timer  T * la timescale del processore

    // sbloccare tutti i processi fermi al semaforo dello pseudo clock
    while (headBlocked(&IT_sem) != NULL)
    {
        if (IT_sem == 1)
            Passeren(&IT_sem);
        else if (IT_sem == 0)
            Verhogen(&IT_sem);
    }

    // settare il semaforo a 0
    IT_sem = 0;

    // LDST per tornare il controllo al processo corrente
    state_t *state = (state_t *)BIOSDATAPAGE; // costante definita in umps
    LDST(state);
}

void nonTimerInterrupt(int int_line_no, int dev_num)
{
    // calcolare l'ind. per il device register
    unsigned int dev_reg_addr = 0x10000054 + ((int_line_no - 3) * 0x80) + (dev_num * 0x10);

    // salvare lo status code del device register
    unsigned int status_code = 0; //????????????????????? da cambiare

    // ack dell'interrupt: ACKN del device register
    dev_reg_addr = ACK; //??????????????????????????????????? sicuramente sbagliato

    // V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    // se la V non ritorna il pcb salta le prossime due operazioni
    pcb_t *proc;

    // mettere lo status code nel reg. v0 del pcb del processo sbloccato
    proc->p_s.reg_v0 = status_code;

    // mettere il processo nella redy queue
    insertProcQ(&ready_queue, proc);

    // LDST per tornare il controllo al processo corrente
    state_t *state = (state_t *)BIOSDATAPAGE; // costante definita in umps
    LDST(state);
}

unsigned int find_dev_num(unsigned int bit_map_word)
{
    unsigned int num;
    if (bit_map_word >= 128)
        num = 7;
    else if (bit_map_word >= 64)
        num = 6;
    else if (bit_map_word >= 32)
        num = 5;
    else if (bit_map_word >= 16)
        num = 4;
    else if (bit_map_word >= 8)
        num = 3;
    else if (bit_map_word >= 4)
        num = 2;
    else if (bit_map_word >= 2)
        num = 1;
    else if (bit_map_word >= 1)
        num = 0;

    return num;
}