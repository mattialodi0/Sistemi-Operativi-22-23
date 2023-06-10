 #include <interrupts.h>


void interruptHandler() {
    //per trovare la linea di interrupt
    unsigned int line, cause, dev_num;
    cause = getCAUSE();
    cause = cause & 1111111100000000;   //maschera per  avere IP
    cause >>= 8;

    //per trovare anche il numero del device
    if(cause >= 1000000)
        line = 1;
    else if(cause >= 100000)
        line = 2;
    else if(cause >= 10000) {
        line = 3;
        dev_num = find_dev_num(0x10000040);
    }
    else if(cause >= 1000) {
        line = 4;
        dev_num = find_dev_num(0x10000040 + 0x04);
    }
    else if(cause >= 100) {
        line = 5;
        dev_num = find_dev_num(0x10000040 + 0x08);
    }
    else if(cause >= 10) {
        line = 6;
        dev_num = find_dev_num(0x10000040 + 0x0C);
    }
    else if(cause >= 1) {
        line = 7;
        dev_num = find_dev_num(0x10000040 + 0x10);
    }


    //in caso di più interrupt si risolve quello con priorità più alta (switch)
    switch (line)
    {
    case 0:     //ingnorato
        break;
    case 1:
        //PLT
        PLTInterrupt();
        break;
    case 2:
        //Interval Timer
        ITInterrupt();
        break;
    case 3:
        //disk devices
    case 4:
        //flash devices
    case 5:
        //network devices
    case 6:
        //printer devices
        nonTimerInterrupt(line, dev_num);
        break;
    case 7:     //prima in scrittura poi in lettura
        //terminal devices
        nonTimerInterrupt(line, dev_num);
        break;
    default:
        break;
    }

}

//è scaduto il quanto di tempo a disposizione del processo corrente e questo va messo da running a ready
void PLTInterrupt() {
    //caricare un nuovo valore nel PLT
    setTIMER(5);   //non è specificato quanto mettere ma credo 5

    //copiare lo stato del processore (all'inizio della BIOS data page) nel pcb del processo corrente
    active_process->p_s = *(state_t *)0x0FFFF000;  //non so se sia giusto il cast                                  **************************

    insertProcQ(&ready_queue, active_process);

    scheduler();

}

void ITInterrupt() {
    LDIT(100000/timescale); // carica nell'interval timer  T * la timescale del processore

    //sbloccare tutti i processi fermi al semaforo dello pseudo clock
    while(headBlocked(&IT_sem) != NULL) {
        if(IT_sem == 1) 
            Passeren(&IT_sem);
        else if(IT_sem == 0)
            Verhogen(&IT_sem);
    }

    //settare il semaforo a 0
    IT_sem = 0;
debug();
    //LDST per tornare il controllo al processo corrente
    state_t* state = (state_t*) BIOSDATAPAGE; //costante definita in umps
    LDST(state);
}

void nonTimerInterrupt(int int_line_no, int dev_num) {
    //calcolare l'ind. per il device register
    unsigned int dev_reg_addr =  0x10000054 + ((int_line_no - 3) * 0x80) + (dev_num * 0x10);

    //salvare lo status code del device register 
    unsigned int status_code = 0;       //????????????????????? da cambiare  

    //ack dell'interrupt: ACKN del device register
    dev_reg_addr = ACK;         //??????????????????????????????????? sicuramente sbagliato

    //V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    //se la V non ritorna il pcb salta le prossime due operazioni
    pcb_t* proc;

    //mettere lo status code nel reg. v0 del pcb del processo sbloccato
    proc->p_s.reg_v0 = status_code;

    //mettere il processo nella redy queue
    insertProcQ(&ready_queue, proc);

    //LDST per tornare il controllo al processo corrente
    state_t* state = (state_t*) BIOSDATAPAGE; //costante definita in umps
    LDST(state);
}



unsigned int find_dev_num(unsigned int  bit_map_word) {
    unsigned int num;
    if(bit_map_word >= 10000000)
        num = 7;
    else if(bit_map_word >= 1000000)
        num = 6;
    else if(bit_map_word >= 100000)
        num = 5;
    else if(bit_map_word >= 10000)
        num = 4;
    else if(bit_map_word >= 1000)
        num = 3;
    else if(bit_map_word >= 100)
        num = 2;
    else if(bit_map_word >= 10)
        num = 1;
    else if(bit_map_word >= 1)
        num = 0;

    return num;
}