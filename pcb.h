#include "pandos_types.h"
#include "list.h"

int pcbFree_table[MAXPROC];

//Funzione 1
void initPcbs(){
    for(int i=0; i<MAXPROC; i++){
        //list_add(pcbFree_table[i], pcbFree_h);
    }
}


//Funzione 2
void freePcb(pcb_t *p){
    //list_add(p, pcbFree_h);
}


//Funzione 3
pcb_t *allocPcb(){
/*  new pcblist per salvare l'elemento rimosso per poi restituirlo
    if(list_empty(pcbFree_h) == 1){
        return NULL;
    }
    else{
        salvare l'elemento nella new pcblist
        list_del(pcbFree_h);
        ???
        ritornare pcblist
    } 
*/
}


//Funzione 4

void mkEmptyProcQ(struct list_head *head){
    
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
    
}

//Funzione 8
pcb_t *removeProcQ(struct list_head *head){

}


//Funzione 9
pcb_t *outProcQ(struct list_head *head, pcb_t *p){

}


//Funzione 10
int emptyChild(pcb_t *p){
/*    if(p->p_child == NULL){
        return true;
    }
    else{
        return false;
    }
*/
}


//Funzione 11
void insertChild(pcb_t *prnt, pcb_t *p){

}


//Funzione 12
pcb_t *removeChild(pcb_t *p){

}


//Funzione 13
pcb_t *outChild(pcb_t *p){

}

