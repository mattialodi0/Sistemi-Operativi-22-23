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

// variabile per sapere se il processore è in stato di WAIT
int on_wait;

#endif