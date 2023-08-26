#include <lib_def.h>
#include <syscalls.h>
#include <interrupts.h>

extern volatile pcb_t *active_process;   
extern volatile cpu_t timer_start;

void exceptionHandler();

void TLBExceptionHandler();

void ProgramTrapExceptionHandler();

void syscallHandler(state_t state);
