#include "include/pcb.h"

HIDDEN struct list_head pcbFree_h;
HIDDEN pcb_t pcbFree_table[MAXPROC];


/*Funzione 1        funziona*/
void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h);
    for(int i=0; i<MAXPROC ; i++){
        INIT_LIST_HEAD(&pcbFree_table[i].p_list);
        list_add(&pcbFree_table[i].p_list, &pcbFree_h);
    }
}


/*Funzione 2        funziona*/
void freePcb(pcb_t *p){
    list_add(&p->p_list, &pcbFree_h);
}


/*Funzione 3        funziona*/
pcb_t *allocPcb(){
    if (list_empty(&pcbFree_h) == 1){
        return NULL;
    } 
    else{
        pcb_t *tmp = container_of(pcbFree_h.next, pcb_t, p_list);
        list_del(pcbFree_h.next);
        tmp->p_parent = NULL;
        INIT_LIST_HEAD(&tmp->p_child);
        INIT_LIST_HEAD(&tmp->p_sib);
        tmp->p_time = 0;
        tmp->p_semAdd = NULL;
        for(int i=0; i < NS_TYPE_MAX; i++){
            tmp->namespaces[i] = NULL;
        }
        return tmp;
    }
}


/*Funzione 4        funziona*/
void mkEmptyProcQ(struct list_head *head){
    INIT_LIST_HEAD(head);
}


/*Funzione 5        funziona*/
int emptyProcQ(struct list_head *head){
    return (list_empty(head));
}


/*Funzione 6        funziona*/
void insertProcQ(struct list_head *head, pcb_t *p){
    list_add_tail(&p->p_list, head);
}


/*Funzione 7        funziona*/
pcb_t *headProcQ(struct list_head *head){
    if(list_empty(head) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = container_of(head->next, pcb_t, p_list);
        return tmp;
    } 
}

/*Funzione 8        funziona*/
pcb_t *removeProcQ(struct list_head *head){
    if(list_empty(head) == 1){
        return NULL;
    }/*
    else if(head->next == NULL){
        pcb_t *tmp = container_of(head->next, pcb_t, p_list);
        list_del(head->next);
        return tmp;
    }
    */
    else{
        pcb_t *tmp = container_of(head->next, pcb_t, p_list);
        list_del(head->next);
        return tmp;
    } 
}


/*Funzione 9        funziona*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p){
    if (list_empty(head) == 1){
        return NULL;
    }
    else{
        struct list_head *tmp = head->next;
        while(tmp != head){    
            pcb_t *t = container_of(tmp, pcb_t, p_list);
            if(t == p){
                list_del(tmp);
                return t;
            }
            tmp = tmp->next;  
        }
        return NULL;
    }
}


/*Funzione 10       funziona*/
int emptyChild(pcb_t *p){
    return (list_empty(&p->p_child));
}


/*Funzione 11       funziona*/
void insertChild(pcb_t *prnt, pcb_t *p){
    list_add_tail(&p->p_list, &prnt->p_child);
}


/*Funzione 12       aggiustare*/
pcb_t *removeChild(pcb_t *p){
    if(list_empty(&p->p_child) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = list_first_entry(p->p_child.next, pcb_t, p_sib);
        list_del(&p->p_sib);
        p->p_parent = NULL;
        return tmp;
    }
}



/*Funzione 13      aggiustare*/
pcb_t *outChild(pcb_t *p){
    if (p == NULL || p->p_parent == NULL || list_empty(&p->p_parent->p_child)){
        return NULL;
    }
    return p;
}

