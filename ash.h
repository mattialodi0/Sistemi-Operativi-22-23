#ifndef _LINUX_HASHTABLE_H
#define _LINUX_HASHTABLE_H

#include "list.h"
#include "hash.h"
#include "log2.h"
#include "hashtable.h"

struct list_head* semd_table[MAXPROC];

LIST_HEAD(s);
struct list_head* semdFree_h = &s;

DEFINE_HASHTABLE(semd_h);    

/**
 * initASH - initialize the list of semdFree so that it contains all of the elements of semdTable 
 * 
 * This method has to be called only once in the initialization of the data structure
 */
static void initASH() 
{
    //inizializza semdFree_h(lista) con tutti i semafori in semd_table(array)

        

    //ciclo sugli elementi di semd_table,
    //e per ogni elemento aggiunge un campo a semdFree

        struct list_head *t = semdFree_h;
        list_for_each(t, *semd_table);
        list_add_tail(t, semdFree_h);

}

/*
* insertBloked - insert the PCB pointed by p in the queue inside semd_table[&semAdd]
* return false unless it is impossible to allocate a new SEMD because the semdFree_h is empty 
*/
static int insertBlocked(int *semAdd,pcb_t *p) {
    //controlla se il SEMD con chiave semAdd esiste
    if(list_empty(semd_table[*semAdd])) {
        //se la lista dei semdFree è vuota ritorna true
        if(list_empty(semdFree_h))
            return true;
        //se no ne prende uno, lo setta e lo aggiunge a semd_table
        else {
            struct list_head *new_free = semdFree_h;
            semdFree_h->prev->next = semdFree_h->next;
            semdFree_h->next->prev = semdFree_h->prev;
            semdFree_h = semdFree_h->next;
            hash_add(semd_table, new_free, *semAdd);
        }
    }
    else {
        //aggiunge p a semd_table[semAdd]
        list_add_tail(p, semd_table[*semAdd]);
        return false;
    }
}

/*
* removeBlocked - return the first PCB blocked in the SEMD inside semd_table[&semAdd]
* return NULL if there is no SEMD in semd_table[&semAdd]
*/
static pcb_t* removeBlocked(int *semAdd) {
    //verifica che il SEMD esista altrimenti ritorna NULL
    if(list_empty(semd_table[*semAdd]))
        return NULL;
    else {
        //rimuove il primo PCB bloccato
        struct list_head *deletd = semd_table[*semAdd];
        deleted->prev->next = deleted->next;
        deleted->next->prev = deleted->prev;
        semd_table[*semAdd] = deleted->next;
        //verifica se ora il SEMD è vuoto, se lo è lo rimuove e lo mette in semdFree_h
        if(list_empty(semd_table[*semAdd]))
            hash_del(semAdd);
        //ritonra il PCB
        return deleted;
    }
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
    if()
    else if(list_empty(semd_table[&semAdd])) 
        return NULL;
    //ritorna il primo primo PCB in quella lista
    else 
        return semd_table[&semAdd];     //forse serve has_for_each_possible
}



#endif