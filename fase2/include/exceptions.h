#include <lib_def.h>
#include <syscalls.h>
#include <interrupts.h>

extern pcb_t *active_process;   


void exceptionHandler();

void TLBExceptionHandler();

void ProgramTrapExceptionHandler();

void syscallHandler(state_t state);
