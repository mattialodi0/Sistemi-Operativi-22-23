#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "syscalls.h"

#define timescale 1     //il valore può essere letto dal registro 0x1000.0024 ma per ora assumiamo sia 1



void exeptionHandler() {
    int cause_reg, exc_code;

    cause_reg = getCAUSE();
    exc_code = cause_reg & 124;    //in binario: 1111100, la maschera per excCode
    //bisogna trasformare il binario in intero

    switch (exc_code)
    {
    case 0:
        interruptHandler();
        break;
    case 1:
    case 2:
    case 3:
        //TLB exception handler
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
        //Program Trap exception handler
        break;
    case 8:
        syscallHandler();
        break;
    
    default:
        break;
    }
}


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

void TLBExceprionHandler() {
    //bisogna realizzare una standard Pass Up or Die operation con PGFAULTEXCEPT come index value:

    if(active_process->p_supportStruct == NULL)
        TerminateProcess(0);    //elimina il processo correte e la sua progenie
    else {
        /*
            copy the saved exception state into a location accessible to the Support Level,
             and pass control to a routine specified by the Support Level
        */
    }
}

void ProgramTrapExceptionHandler() {
    //bisogna realizzare una standard Pass Up or Die operation con GENERALEXCEPT come index value:

    if(active_process->p_supportStruct == NULL)
        TerminateProcess(0);    //elimina il processo correte e la sua progenie
    else {
        /*
            copy the saved exception state into a location accessible to the Support Level,
             and pass control to a routine specified by the Support Level
        */
    }
}


void syscallHandler() {
    int v0, v1, v2, v3;
    //vanno presi dai registri e castati

    switch (v0)
    {
    case 1:
        CreateProcess((state_t *)v1, (support_t *)v2, (nsd_t *)v3);
        break;
    case 2:
        TerminateProcess(v1);
        break;
    case 3:
        Passeren((int *)v1);
        break;
    case 4:
        Verhogen((int *)v1);
        break;
    case 5:
        DOIO((int *)v1, (int *)v2);
        break;
    case 6:
        GetCPUTime();
        break;
    case 7:
        WaitForClock();
        break;
    case 8:
        GetSupportData();
        break;    
    case 9:
        GetProcessId(v1);
        break;
    case 10:
        GetChildren((int *)v1, v2);
        break;
    default:
        break;
    }
}