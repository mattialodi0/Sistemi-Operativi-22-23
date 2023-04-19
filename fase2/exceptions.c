#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "syscalls.h"


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
        break;
    case 2:
        //Interval Timer
        break;
    case 3:
        //disk devices
        break;
    case 4:
        //flash devices
        break;
    case 5:
        //network devices
        break;
    case 6:
        //printer devices
        break;
    case 7:     //prima in scrittura poi in lettura
        //terminal devices
        break;
    
    default:
        break;
    }

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
        CreateProcess(v1, v2, v3);
        break;
    case 2:
        TerminateProcess(v1);
        break;
    case 3:
        Passeren(v1);
        break;
    case 4:
        Verhogen(v1);
        break;
    case 5:
        DOIO(v1, v2);
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
        GetChildren(v1, v2);
        break;
    default:
        break;
    }
}