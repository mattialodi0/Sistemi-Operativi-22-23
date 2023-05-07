#ifndef LIB_DEF_H
#define LIB_DEF_H

#include <pandos_const.h>
#include <pandos_types.h>
#include "types.h"
#include </usr/include/umps3/umps/libumps.h>
#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <ash.h>
#include <ns.h>

#define timescale  (* ((cpu_t *) TIMESCALEADDR))     //il valore può essere letto dal registro 0x1000.0024 ma per ora assumiamo sia 1


//processi vivi
int process_count;
//processi bloccati
int soft_blocked_count;
//processi ready
struct list_head *ready_queue; 
//puntatore al proc attivo
pcb_t *active_process;
//contatore di pid usato in exeptions.c
int pid_count;

//semaforo per lo pseudo clock
int IT_sem;     // da verificare se vada inizializzato a 0




//funzione usata per mettere un breakpoint
void debug();

#endif