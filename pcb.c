#include "include/pcb.h"

HIDDEN struct list_head pcbFree_h;
HIDDEN pcb_t pcbFree_table[MAXPROC];


/*Funzione 1        funziona*/
void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h);
    for(int i=1; i<MAXPROC ; i++){
        INIT_LIST_HEAD(&pcbFree_table[i].p_list);
        list_add(&pcbFree_table[i].p_list, &pcbFree_h);
    }
}


/*Funzione 2        funziona*/
void freePcb(pcb_t *p){
    list_add(&p->p_list, &pcbFree_h);
}


/*Funzione 3        aggiustare*/
pcb_t *allocPcb(){
    if (list_empty(&pcbFree_h) == 1){
        return NULL;
    } 
    else{
        pcb_t *tmp = list_first_entry(&pcbFree_h, struct pcb_t, p_list);
        list_del(&tmp->p_list);
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


/*Funzione 7        fare*/
pcb_t *headProcQ(struct list_head *head){
    /*
    if(list_empty(head) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = head;
        return tmp;
    } 
    */
}

/*Funzione 8        fare*/
pcb_t *removeProcQ(struct list_head *head){
    /*
    if(list_empty(head) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = head;
        head = head->next;
        return tmp;
        
    } 
    */
}


/*Funzione 9        fare*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p){

}


/*Funzione 10       testare*/
int emptyChild(pcb_t *p){
    return (list_empty(&p->p_child));
}


/*Funzione 11       testare*/
void insertChild(pcb_t *prnt, pcb_t *p){
    list_add_tail(&p->p_list, &prnt->p_child);
}


/*Funzione 12       fare*/
pcb_t *removeChild(pcb_t *p){
    /*
    if(p->p_child == NULL){
        return NULL;
    }
    else{
        pcb_t *tmp = p->p_child;
        p->p_child = p->p_child->next;
        return tmp;
    }
    */
}



/*Funzione 13      fare*/
pcb_t *outChild(pcb_t *p){

}

