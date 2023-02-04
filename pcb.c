#include "pcb.h"

HIDDEN pcb_t *pcbFree_h;
HIDDEN pcbFree_table[MAXPROC];


//Funzione 1
void initPcbs(){
    for(int i=0; i<MAXPROC; i++){
        list_add(pcbFree_table[i], pcbFree_h);
    }
}


//Funzione 2
void freePcb(pcb_t *p){
    list_add(p, pcbFree_h);
}


//Funzione 3
pcb_t *allocPcb(){
    if(list_empty(pcbFree_h) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = pcbFree_h;
        pcbFree_h = pcbFree_h->next;
        tmp->next = NULL;
        tmp->prev = NULL;
        tmp->p_child = NULL;
        tmp->p_parent = NULL;
        tmp->p_sib = NULL;
        return tmp;
    } 
}


//Funzione 4

void mkEmptyProcQ(struct list_head *head){
    head->next = NULL;
    head->prev = NULL;
}


//Funzione 5

int emptyProcQ(struct list_head *head){
    return list_empty(head);
}


//Funzione 6

void insertProcQ(struct list_head *head, pcb_t *p){
    list_add_tail(p, head);
}


//Funzione 7

pcb_t *headProcQ(struct list_head *head){
    if(list_empty(head) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = head;
        return tmp;
    } 
}

//Funzione 8
pcb_t *removeProcQ(struct list_head *head){
    if(list_empty(head) == 1){
        return NULL;
    }
    else{
        pcb_t *tmp = head;
        head = head->next;
        return tmp;
        //??
    } 
}


//Funzione 9
pcb_t *outProcQ(struct list_head *head, pcb_t *p){

}


//Funzione 10
int emptyChild(pcb_t *p){
    if((p != NULL) && (p->p_child == NULL)){
        return TRUE;
    }
    else{
        return FALSE;
    }
}


//Funzione 11
void insertChild(pcb_t *prnt, pcb_t *p){
    prnt->p_child = p;
}


//Funzione 12
pcb_t *removeChild(pcb_t *p){

}


//Funzione 13
pcb_t *outChild(pcb_t *p){

}
