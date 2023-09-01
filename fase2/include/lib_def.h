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

#define timescale  (* ((cpu_t *) TIMESCALEADDR))

//funzione usata per mettere un breakpoint
void debug();
void debug1();
void debug2();
void debug3();
void debug4();
void debugE();
void debugInt();
void debugX();

#endif