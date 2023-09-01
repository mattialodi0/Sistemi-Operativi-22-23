#include <lib_def.h>
#include <syscalls.h>
#include <interrupts.h>

#define INTCODE 0
#define TLBEXCODE 1 ... 3
#define PRGTRAPCODE 4 ... 7
#define PRGTRAPCODE1 9 ... 12
#define SYSCALLCODE 8


extern pcb_t *active_process;   
extern cpu_t timer_start;

// per misurare la durata delle eccezioni
cpu_t exc_timer_start;

/* Gestore delle eccezioni */
void exceptionHandler();

/* Standard Pass Up or Die operation con GENERALEXCEPT come index value */
void TLBExceptionHandler();

/* Standard Pass Up or Die operation con GENERALEXCEPT come index value */
void ProgramTrapExceptionHandler();

/* Gestore delle syscall */
void syscallHandler(state_t state);
