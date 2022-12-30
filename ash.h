
#ifndef _LINUX_HASHTABLE_H
#define _LINUX_HASHTABLE_H

#include <list.h>
#include <hash.h>
#include <log2.h>
#include "hashtable.h"

/**
 * initASH - initialize the list of semdFree so that it contains all of the elements of semdTable 
 * 
 * This method has to be called only once in the initialization of the data structure
 */
void initASH() 
{
    //inizializza semdFree(lista) con tutti i semafori in semdTable(array)
    //ciclo sugli elementi di semdTable,
    //e per ogni elemento aggiunge un campo a semdFree

}

int insertBlocked(int *semAdd,pcb_t *p)
pcb_t* removeBlocked(int *semAdd)
pcb_t* outBlocked(pcb_t *p)
pcb_t* headBlocked(int *semAdd)



#endif
