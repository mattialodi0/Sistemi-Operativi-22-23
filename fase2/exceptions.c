#include <exceptions.h>

/* Gestore delle eccezioni */
void exceptionHandler()
{
    STCK(exc_timer_start);
    state_t state = *((state_t *)BIOSDATAPAGE);
    
    int cause_reg, exc_code;
    cause_reg = getCAUSE();
    exc_code = cause_reg & GETEXECCODE;
    exc_code >>= 2;

    switch (exc_code)
    {          
    case INTCODE:
        interruptHandler();
        break;
    case TLBEXCODE:
        TLBExceptionHandler();
        break;
    case PRGTRAPCODE:
    case PRGTRAPCODE1:
        ProgramTrapExceptionHandler();
        break;
    case SYSCALLCODE:
        syscallHandler(state);
        break;
    default:
        PANIC();
        break;
    }
}

/* Standard Pass Up or Die operation con PGFAULTEXCEPT come index value: */
void TLBExceptionHandler()
{
    if (active_process->p_supportStruct == NULL) {
        TerminateProcess(0);
    }
    else
    {
        // Copy the saved exception state from the BIOS Data Page to the correct sup_exceptState field of the Current Process.
        state_t state = *(state_t *)BIOSDATAPAGE;
        active_process->p_supportStruct->sup_exceptState[PGFAULTEXCEPT] = state;
        
        // Perform a LDCXT using the fields from the correct sup_exceptContext field of the Current Process.
        context_t *context = &active_process->p_supportStruct->sup_exceptContext[PGFAULTEXCEPT];
        LDCXT(context->stackPtr, context->status, context->pc);
    }
}

/* Standard Pass Up or Die operation con GENERALEXCEPT come index value */ 
void ProgramTrapExceptionHandler()
{
    if (active_process->p_supportStruct == NULL){
        TerminateProcess(0);
    }
    else
    {
        // Copy the saved exception state from the BIOS Data Page to the correct sup_exceptState field of the Current Process.
        state_t state = *(state_t *)BIOSDATAPAGE;
        active_process->p_supportStruct->sup_exceptState[GENERALEXCEPT] = state;

        // Perform a LDCXT using the fields from the correct sup_exceptContext field of the Current Process.
        context_t *context = &active_process->p_supportStruct->sup_exceptContext[GENERALEXCEPT];
        LDCXT(context->stackPtr, context->status, context->pc);
    }
}

/* Gestore delle syscall  */
void syscallHandler(state_t state) 
{
    // controllo se si è in kernel mode, altrimenti Trap
    if((state.status & 2) == 2) {
        state_t *s = (state_t *)BIOSDATAPAGE;
        s->cause = (PRIVINSTR << 2);
        ProgramTrapExceptionHandler(); // lancia una program trap 
    }

    // i parametri sono presi dai registri e castati
    unsigned int v0=0;
    int v1=0, v2=0, v3=0;
    v0 = state.reg_a0;
    v1 = state.reg_a1;
    v2 = state.reg_a2;
    v3 = state.reg_a3;

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
        DoIO(v1, (unsigned int*) v2);
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
        ProgramTrapExceptionHandler();
        break;
    }
}
