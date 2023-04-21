#include "exceptions.h"


void exceptionHandler() {
    //disabilita gli interrupt
    unsigned int status = getSTATUS();
    status &= DISABLEINTS;    //disabilita anche gli interrupt
    LDCXT(active_process->p_s.reg_sp, status, active_process->p_s.pc_epc);    //il valore che ci interessa settare è il secondo

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
        TLBExceptionHandler();
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
        ProgramTrapExceptionHandler();
        break;
    case 8:
        syscallHandler();
        break;
    
    default:
        break;
    }

    //abilita gli interrupt
    unsigned int status = getSTATUS();
    status |= ~DISABLEINTS;    //abilita gli interrupt
    LDCXT(active_process->p_s.reg_sp, status, active_process->p_s.pc_epc);    //il valore che ci interessa settare è il secondo
}


void TLBExceptionHandler() {
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
    case CREATEPROCESS:
        CreateProcess((state_t *)v1, (support_t *)v2, (nsd_t *)v3);
        break;
    case TERMPROCESS:
        TerminateProcess(v1);
        break;
    case PASSEREN:
        Passeren((int *)v1);
        break;
    case VERHOGEN:
        Verhogen((int *)v1);
        break;
    case IOWAIT:
        DOIO((int *)v1, (int *)v2);
        break;
    case GETTIME:
        GetCPUTime();
        break;
    case CLOCKWAIT:
        WaitForClock();
        break;
    case GETSUPPORTPTR:
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