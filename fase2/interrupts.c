 #include "interrupts.h"



void interruptHandler() {
    //per sapere la causa si legge il reg. cause.IP
    int line;

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
        nonTimerInterrupt();
        break;
    case 4:
        //flash devices
        nonTimerInterrupt();
        break;
    case 5:
        //network devices
        nonTimerInterrupt();
        break;
    case 6:
        //printer devices
        nonTimerInterrupt();
        break;
    case 7:     //prima in scrittura poi in lettura
        //terminal devices
        nonTimerInterrupt();
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

    insertProcQ(ready_queue, active_process);

    scheduler();

}

void ITInterrupt() {
    LDIT(100000/timescale); // carica nell'interval timer  T * la timescale del processore

    //sbloccare tutti i processi fermi al semaforo dello pseudo clock

    //settare il semaforo a 0

    //LDST per tornare il controllo al processo corrente
}

void nonTimerInterrupt() {
    //calcolare l'ind. per il device register
    int dev_reg_addr;
    //...

    //salvare lo status code del device register 
    //int status_code = ;

    //ack dell'interrupt: ACKN del device register
    
    //V sul semaforo associato al device dell'interrupt per sbloccare il processo che sta aspettando la fine dell'I/O
    //se la V non ritorna il pcb salta le prossime due operazioni

    //mettere lo status code nel reg. v0 del pcb del processo sbloccato

    //mettere il processo nella resy queue

    //LDST per tornare il controllo al processo corrente
}