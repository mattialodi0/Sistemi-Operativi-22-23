#include <lib_def.h>
#include <syscalls.h>
#include <interrupts.h>

extern pcb_t *active_process;   
extern cpu_t timer_start;

void exceptionHandler();

void TLBExceptionHandler();

void ProgramTrapExceptionHandler();

void syscallHandler(state_t state);
