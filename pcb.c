#include "include/pcb.h"

HIDDEN struct list_head pcbFree_h;      /*lista dei pcb liberi o inutilizzati*/
HIDDEN pcb_t pcbFree_table[MAXPROC];    /*array di pcb con dimensione massima MAXPROC*/


/*Inizializza la lista pcbFree_h in modo da contenere gli elementi della pcbFree_table*/
void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h);
    for(int i=0; i<MAXPROC ; i++){      /*Con un ciclo inserisco di iterazione in iterazione gli elementi della pcbFree_table in pcbFree_h*/
        INIT_LIST_HEAD(&pcbFree_table[i].p_list);
        list_add(&pcbFree_table[i].p_list, &pcbFree_h);
    }
}


/*Inserisce il pcb puntato da p nella lista pcbFree_h*/
void freePcb(pcb_t *p){
    list_add(&p->p_list, &pcbFree_h);
}


/*Rimuove un elemento dalla pcbFree_h e restituisce l'elemento rimosso*/
pcb_t *allocPcb(){
    if (list_empty(&pcbFree_h) == 1){       /*Se la pcbFree_h è vuota ritorno NULL*/
        return NULL;
    } 
    else{
        pcb_t *tmp = container_of(pcbFree_h.next, pcb_t, p_list);   /*In tmp salvo l'elemento che andrà rimosso*/
        list_del(pcbFree_h.next);       /*Rimuovo l'elemento dalla pcbFree_h*/
        tmp->p_parent = NULL;           /*Inizializzo tutti i campi dell'elemento rimosso a NULL / 0*/
        INIT_LIST_HEAD(&tmp->p_child);
        INIT_LIST_HEAD(&tmp->p_sib);
        tmp->p_time = 0;
        tmp->p_semAdd = NULL;
        for(int i=0; i < NS_TYPE_MAX; i++){
            tmp->namespaces[i] = NULL;
        }
        return tmp;     /*Restituisco l'elemento rimosso*/
    }
}


/*Crea una lista di pcb inizializzandola come lista vuota*/
void mkEmptyProcQ(struct list_head *head){
    INIT_LIST_HEAD(head);
}


/*Se la lista puntata da head è vuota restituisce TRUE, altrimenti FALSE*/
int emptyProcQ(struct list_head *head){
    return (list_empty(head));      /*restituisco list_empty(head) che ritorna TRUE se lista presa in input è vuota, FALSE altrimenti*/
}


/*Inserisce l'elemento puntato da p nella coda dei processi puntata da head*/
void insertProcQ(struct list_head *head, pcb_t *p){
    list_add_tail(&p->p_list, head);
}


/*Restituisce l'elemento di testa della coda dei processi puntata da head senza rimuoverlo*/
pcb_t *headProcQ(struct list_head *head){
    if(list_empty(head) == 1){      /*Se head è vuota restituisco NULL*/
        return NULL;
    }
    else{
        pcb_t *tmp = container_of(head->next, pcb_t, p_list);    /*Se head non è vuota ne estraggo l'elemento di testa e lo restituisco*/
        return tmp;
    } 
}

/*Rimuove il primo elemento della coda dei processi puntata da head*/
pcb_t *removeProcQ(struct list_head *head){
    if(list_empty(head) == 1){      /*Se head è vuota restituisco NULL*/
        return NULL;
    }
    else{
        pcb_t *tmp = container_of(head->next, pcb_t, p_list);       /*Salvo in tmp il primo elemento della coda puntata da head*/
        list_del(head->next);       /*Rimuovo il primo elemento della coda puntata da head*/
        return tmp;     /*Restituisco l'elemento rimosso salvato precedentemente*/
    } 
}


/*Rimuove il pcb puntato da p dalla coda dei processi puntata da head*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p){
    if (list_empty(head) == 1){     /*Se head è vuota restituisco NULL*/
        return NULL;
    }
    else{
        struct list_head *tmp = head->next;     /*Creo una lista temporanea da scorrere*/
        while(tmp != head){    /*Con il ciclo while scorro la lista temporanea creata precedentemente*/
            pcb_t *t = container_of(tmp, pcb_t, p_list);    /*Salvo in t l'elemento corrente della lista*/
            if(t == p){     /*Verifico se l'elemento corrente è p, ovvero l'elemento da rimuovere, se è così lo rimuovo*/
                list_del(tmp);
                return t;
            }
            tmp = tmp->next;  
        }
        return NULL;    /*Se p non è stato trovato precedentemente (e quindi non è presente nella coda) restituisco NULL*/
    }
}


/*Se il pcb puntato da p non ha figli restituisce TRUE, altrimenti FALSE*/
int emptyChild(pcb_t *p){
    return (list_empty(&p->p_child));       /*Verifico tramite list_empty se il pcb puntato da p ha figli o meno*/
}


/*Inserisce il pcb puntato da p come figlio del pcb puntato da prnt*/
void insertChild(pcb_t *prnt, pcb_t *p){
    p->p_parent = prnt; 
    list_add_tail(&p->p_sib, &prnt->p_child);       /*aggiungo il pcb puntato da p come figlio del pcb puntato da prnt*/
}


/*Rimuove il primo figlio del pcb puntato da p*/
pcb_t *removeChild(pcb_t *p){
    if (list_empty(&p->p_child) == 1){      /*Se p non ha figli restituisco NULL*/
        return NULL;
    }
    else{
        pcb_t *tmp = container_of(list_next(&p->p_child), pcb_t, p_sib);
        list_del(&tmp->p_sib);      /*Rimuovo il primo figlio del pcb puntato da p*/
        tmp->p_parent = NULL;
        return tmp;     /*Restituisco il figlio rimosso*/
    }
}



/*Rimuove il pcb puntato da p dalla lista dei figli del padre*/
pcb_t *outChild(pcb_t *p){
    if(p->p_parent == NULL){        /*Se il pcb puntato da p non ha un padre restituisco NULL*/
        return  NULL;
    }
    else{
        list_del(&p->p_sib);        /*RImuovo il pcb puntato da p*/
        p->p_parent = NULL;
        return p;       /*Restituisco l'elemento rimosso*/
    }
}

