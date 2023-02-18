#ifndef ASH_H
#define ASH_H

#include "pcb.h"
#include "list.h"
#include "hash.h"
#include "hashtable.h"
#include <pandos_types.h>
#include <pandos_const.h>


/* insertBloked - insert the PCB pointed by p in the queue inside semd_table[&semAdd]
* return false unless it is impossible to allocate a new SEMD because the semdFree_h is empty */
 int insertBlocked(int *semAdd,pcb_t *p);

/* removeBlocked - return the first PCB blocked in the SEMD inside semd_table[&semAdd]
* return NULL if there is no SEMD in semd_table[&semAdd] */
 pcb_t* removeBlocked(int *semAdd);

/* outBlocked - return the PCB p from the queue of the SEMD inside semd_table[p->p_semAdd]
* return NULL if there is no such PCB in semd_table[p->p_semAdd] */
 pcb_t* outBlocked(pcb_t *p);

/* headBlocked - return the first PCB blocked in the SEMD inside semd_table[&semAdd] without removing it
* return NULL if there is no SEMD in semd_table[p->p_semAdd] or if it is empty */
 pcb_t* headBlocked(int *semAdd);

/* initASH - initialize the list of semdFree so that it contains all of the elements of semdTable 
 * This method has to be called only once in the initialization of the data structure */
 void initASH();


#endif