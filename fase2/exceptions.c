#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "syscalls.h"













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