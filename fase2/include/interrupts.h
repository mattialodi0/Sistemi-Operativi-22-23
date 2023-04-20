#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"



void interruptHandler();

//è scaduto il quanto di tempo a disposizione del processo corrente e questo va messo da running a ready
void PLTInterrupt();

void ITInterrupt();

void nonTimerInterrupt();