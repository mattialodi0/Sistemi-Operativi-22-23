#include "include/ash.h"


static struct semd_t semd_table[MAXPROC];

struct list_head semdFree_h;
//static struct semd_t *semdFree_h;

DEFINE_HASHTABLE(semd_h,4);    



 int insertBlocked(int *semAdd,pcb_t *p) {
 /* //controlla se il SEMD con chiave semAdd esiste
    if(hlist_empty(&semd_h[*semAdd])) {
        //se la lista dei semdFree è vuota ritorna true
        if(list_empty(semdFree_h))
            return true;
        //se no ne prende uno, lo setta e lo aggiunge a semd_table
        else {
            struct list_head *new_free = semdFree_h;
            semdFree_h->prev->next = semdFree_h->next;
            semdFree_h->next->prev = semdFree_h->prev;
            semdFree_h = semdFree_h->next;
            hash_add(semd_table, new_free, *semAdd);        //manca la rimozione da quelli liberi
            return false;
        }
    }
    else {
        //aggiunge p a semd_h[semAdd]
        semd_t *sem = container_of(&semd_h[*semAdd].first, semd_t, s_link);
        p->p_semAdd = semAdd;
        insertProcQ(&sem->s_procq, p);
        return false;    
    }*/return 1;
}

 pcb_t* removeBlocked(int *semAdd) {
/*    //verifica che il SEMD esista altrimenti ritorna NULL
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
    }*/   return NULL;
}

 pcb_t* outBlocked(pcb_t *p) {
    //verifica che p compaia in semd_table[p->p_semAdd], altrimenti ritorna NULL
/*  semd_t item;
    pcb_t *out;
    int found = 0;
    int field = 4;
    //ciclo sugli elementi idella lista
    hash_for_each_possible(semd_h, item, field, p->p_semAdd);
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

 pcb_t* headBlocked(int *semAdd) {
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

    //return semd_table[*semAdd]->s_procq.next;
    return container_of(&semd_table[*semAdd]->s_procq, pcb_t, p_list);
    */
   return NULL;
}

 void initASH() 
{
    INIT_LIST_HEAD(&semdFree_h);
    for(int i=0; i<MAXPROC; i++) {
        INIT_LIST_HEAD(&semd_table[i].s_freelink);
        list_add(&semd_table[i].s_freelink, &semdFree_h);
    }
}