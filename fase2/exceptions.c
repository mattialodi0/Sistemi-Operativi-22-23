#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "syscalls.h"


void exeptionHandler() {
    int cause_reg, exc_code;

    cause_reg = getCAUSE();
    exc_code = cause_reg & 124;    //in binario: 1111100, la maschera per excCode

    switch (exc_code)
    {
    case 0:
        //interrupt handler
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