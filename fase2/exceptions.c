#include <exceptions.h>
extern int debug_var;

void exceptionHandler()
{
    int cause_reg, exc_code, cause;
    cause_reg = getCAUSE();
    exc_code = cause_reg & GETEXECCODE;

    exc_code >>= 2;

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
        debug();
        ProgramTrapExceptionHandler();
        break;
    case 8:
        syscallHandler();
        break;

    default:
        // ProgramTrapExceptionHandler();  //se ci possono essere exc_code > 12
        break;
    }
}

// standard Pass Up or Die operation con PGFAULTEXCEPT come index value:
void TLBExceptionHandler()
{
    if (active_process->p_supportStruct == NULL)
        TerminateProcess(0); // elimina il processo correte e la sua progenie
    else
    {
        /*  copy the saved exception state into a location accessible to the Support Level,
            and pass control to a routine specified by the Support Level */
        // Copy the saved exception state from the BIOS Data Page to the correct sup_exceptState field of the Current Process.
        state_t state = *(state_t *)BIOSDATAPAGE;
        active_process->p_supportStruct->sup_exceptState[PGFAULTEXCEPT] = state;
        // forse serve salvare anche le altre info

        // Perform a LDCXT using the fields from the correct sup_exceptContext field of the Current Process.
        unsigned int sp = state.reg_sp;
        unsigned int status = state.status;
        unsigned int pc = state.pc_epc;
        LDCXT(sp, status, pc);
    }
}

// standard Pass Up or Die operation con GENERALEXCEPT come index value
void ProgramTrapExceptionHandler()
{

    if (active_process->p_supportStruct == NULL)
        TerminateProcess(0); // elimina il processo correte e la sua progenie
    else
    {
        /*  copy the saved exception state into a location accessible to the Support Level,
            and pass control to a routine specified by the Support Level */
        // Copy the saved exception state from the BIOS Data Page to the correct sup_exceptState field of the Current Process.
        state_t state = *(state_t *)BIOSDATAPAGE;
        active_process->p_supportStruct->sup_exceptState[GENERALEXCEPT] = state;

        // Perform a LDCXT using the fields from the correct sup_exceptContext field of the Current Process.
        unsigned int sp = state.reg_sp;
        unsigned int status = state.status;
        unsigned int pc = state.pc_epc;
        LDCXT(sp, status, pc);
    }
}

void syscallHandler()
{
    // controllo se si è in user mode, altrimenti Trap

    int v0, v1, v2, v3;
    // vanno presi dai registri e castati

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