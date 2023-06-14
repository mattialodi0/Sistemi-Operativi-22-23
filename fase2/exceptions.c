#include <exceptions.h>


void exceptionHandler() {

    int cause_reg, exc_code, cause;
    cause_reg = getCAUSE();
    exc_code = cause_reg & GETEXECCODE;//get cause execode
    //trasformazione da binario a intero
    exc_code = exc_code >> 2;
    if(exc_code == 1100)
        cause = 12;
    else if(exc_code == 1011)
        cause = 11;
    else if(exc_code == 1010)
        cause = 10;
    else if(exc_code == 1001)
        cause = 9;
    else if(exc_code == 1000)
        cause = 8;
    else if(exc_code == 111)
        cause = 7;
    else if(exc_code == 110)
        cause = 6;
    else if(exc_code == 101)
        cause = 5;
    else if(exc_code == 100)
        cause = 4;
    else if(exc_code == 11)
        cause = 3;
    else if(exc_code == 10)
        cause = 2;
    else if(exc_code == 1)
        cause = 1;
    else if(exc_code == 0) 
        cause = 0;

    switch (cause)
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

   //carica lo stato del processore come era prima dell'eccezione
   //state_t* s = (state_t*) 0x0FFFF000;  //ind del BIOS data page
   //LDST((STATE_PTR) s);
}


//standard Pass Up or Die operation con PGFAULTEXCEPT come index value:
void TLBExceptionHandler() {
    
    if(active_process->p_supportStruct == NULL)
        TerminateProcess(0);    //elimina il processo correte e la sua progenie
    else {
        
        /*  copy the saved exception state into a location accessible to the Support Level,
            and pass control to a routine specified by the Support Level */
        //Copy the saved exception state from the BIOS Data Page to the correct sup_exceptState field of the Current Process. 
        state_t state = *(state_t*) 0x0FFFF000;
        active_process->p_supportStruct->sup_exceptState[PGFAULTEXCEPT] = state; 
        //forse serve salvare anche le altre info

        //Perform a LDCXT using the fields from the correct sup_exceptContext field of the Current Process.
        unsigned int sp = state.reg_sp;
        unsigned int status = state.status;
        unsigned int pc = state.pc_epc;
        LDCXT(sp, status, pc);
    }
}

//standard Pass Up or Die operation con GENERALEXCEPT come index value
void ProgramTrapExceptionHandler() {

   if(active_process->p_supportStruct == NULL)
        TerminateProcess(0);    //elimina il processo correte e la sua progenie
    else {
        /*  copy the saved exception state into a location accessible to the Support Level,
            and pass control to a routine specified by the Support Level */
        //Copy the saved exception state from the BIOS Data Page to the correct sup_exceptState field of the Current Process. 
        state_t state = *(state_t*) 0x0FFFF000;
        active_process->p_supportStruct->sup_exceptState[GENERALEXCEPT] = state; 
        //forse serve salvare anche le altre info

        //Perform a LDCXT using the fields from the correct sup_exceptContext field of the Current Process.
        unsigned int sp = state.reg_sp;
        unsigned int status = state.status;
        unsigned int pc = state.pc_epc;
        LDCXT(sp, status, pc);
    }
}


void syscallHandler() {
    //controllo se si è in user mode, altrimenti Trap

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
    case DOIO:
        DoIO((int *)v1, (int *)v2);
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