#include "include/ash.h"


static void initASH() 
{
    struct list_head *sentinel;
    sentinel->next = semd_table[0];
    struct list_head *t;
    //inizializza semdFree_h(lista) con tutti i semafori in semd_table(array)
    for(int i=0; i<MAXPROC; i++) {      //non so se va bene così
        t = semdFree_h;
        semd_table[i];
        list_add_tail(t, semdFree_h);
    }
    t->next = sentinel;
    sentinel->prev = t;
}

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
            return false;
        }
    }
    else {
        //aggiunge p a semd_table[semAdd]
        list_add_tail(p, semd_table[*semAdd]);
        return false;
    }
}

static pcb_t* removeBlocked(int *semAdd) {
    //verifica che il SEMD esista altrimenti ritorna NULL
    if(list_empty(semd_table[*semAdd]))
        return NULL;
    else {
        //rimuove il primo PCB bloccato
        struct list_head *deleted = semd_table[*semAdd];
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

static pcb_t* outBlocked(pcb_t *p) {
    //verifica che p compaia in semd_table[p->p_semAdd], altrimenti ritorna NULL
    semd_t item;
    pcb_t *out;
    int found = 0;
    int field = 4;
    //ciclo sugli elementi idella lista
/*    hash_for_each_possible(semd_h, item, field, p->p_semAdd);
    out = container_of(&item, pcb_t, p_list);   //probabilmente non va bene
    struct list_head item_list = item.s_procq;
    if(out == p) {
        out = p;
        //lo rimuove 
        item_list.prev->next = item_list.next;
        item_list.next->prev = item_list.prev;
        list_del(&item_list);
    }

    if(found) {
        //verifica se ora il SEMD è vuoto, se lo è lo rimuove e lo mette in semdFree_h
        if(list_empty(&semd_table[*p->p_semAdd]->s_procq))
            hash_del(*p->p_semAdd);
        //ritonra il PCB
        return out;
    }
    else */
        return NULL;
}

/*
* headBlocked - return the first PCB blocked in the SEMD inside semd_table[&semAdd] without removing it
* return NULL if there is no SEMD in semd_table[p->p_semAdd] or if it is empty
*/
static pcb_t* headBlocked(int *semAdd) {
/*    //verifica che il SEMD in semd_table[&semAdd] esista e non si vuoto
    int f = 0;
    int index;
    hash_for_each(semd_h, index, item, field);
    if(index = &semAdd)
        f = 1;
    
    if(f)
        return NULL;
    else if(list_empty(semd_table[&semAdd])) 
        return NULL;
    //ritorna il primo primo PCB in quella lista
    else 
        return semd_table[semAdd];     //forse serve hash_for_each_possible
*/
    //return semd_table[*semAdd]->s_procq.next;
    return container_of(&semd_table[*semAdd]->s_procq, pcb_t, p_list);
}

