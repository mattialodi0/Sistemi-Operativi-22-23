
#ifndef _LINUX_HASHTABLE_H
#define _LINUX_HASHTABLE_H

#include "list.h"
#include "hash.h"
#include "log2.h"
#include "hashtable.h"

/**
 * initASH - initialize the list of semdFree so that it contains all of the elements of semdTable 
 * 
 * This method has to be called only once in the initialization of the data structure
 */
static void initASH() 
{
    //inizializza semdFree_h(lista) con tutti i semafori in semdTable(array)

        LIST_HEAD(semdFree_h);

    //ciclo sugli elementi di semdTable,
    //e per ogni elemento aggiunge un campo a semdFree

        struct list_head *t;
        list_for_each(t, semdTable);
        list_add_tail(t, semdFree_h);

}

/*
* insertBloked - insert the PCB pointed by p in the queue inside semd_table[&semAdd]
* return false unless it is impossible to allocate a new SEMD because the semdFree_h is empty 
*/
static int insertBlocked(int *semAdd,pcb_t *p) {
    //controlla se il SEMD con chiave semAdd esiste
    //se esiste gli aggiunge il processo p
    //altrimenti:
    //se la lista dei semdFree è vuota ritorna true
    //se no ne prende uno, lo setta e lo aggiunge a semd_table

    //ritorna false
}

/*
* removeBlocked - return the first PCB blocked in the SEMD inside semd_table[&semAdd]
* return NULL if there is no SEMD in semd_table[&semAdd]
*/
static pcb_t* removeBlocked(int *semAdd) {
    //verifica che il SEMD esista altrimenti ritorna NULL
    //rimuove il primo PCB bloccato
    //verifica se ora il SEMD è vuoto, se lo è lo rimuove e lo mette in semdFree_h

    //ritonra il PCB
}

/*
* outBlocked - return the PCB p from the queue of the SEMD inside semd_table[p->p_semAdd]
* return NULL if there is no such PCB in semd_table[p->p_semAdd]
*/
static pcb_t* outBlocked(pcb_t *p) {
    //verifica che p compaia in semd_table[p->p_semAdd], altrimenti ritorna NULL
    //lo rimuove 
    //verifica se ora il SEMD è vuoto, se lo è lo rimuove e lo mette in semdFree_h

    //ritonra il PCB
}

/*
* headBlocked - return the first PCB blocked in the SEMD inside semd_table[&semAdd] without removing it
* return NULL if there is no SEMD in semd_table[p->p_semAdd] or if it is empty
*/
static pcb_t* headBlocked(int *semAdd) {
    //verifica che il SEMD in semd_table[&semAdd] esista e non si vuoto
    //altrimenti ritorna NULL
    
    //ritorna il primo PCB il semd_table[&semAdd]
}



#endif
