#include "include/ns.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXSEM  MAXPROC
#define MAXNS   MAXPROC


nsd_t pid_nsFree_h; //lista dei NSD di tipo pid liberi o inutilizzati
nsd_t pid_nsList_h; //lista dei namespace di tipo type attivi


static nsd_t pid_nsTable[MAXPROC];


bool empty(nsd_t ns) {
    bool res = false;

    if ((ns.n_link.prev == NULL) && (ns.n_link.next == NULL)) {
        res = true;
    }

    return res;
}


bool last_el(nsd_t ns) {
    bool res = false;

    if ( ns.n_link.prev == &ns.n_link && ns.n_link.next == &ns.n_link) {
        res = true;
    }

    return res;
}



void initNamespaces() {
/*Inizializza tutte le liste dei namespace liberi.
Questo metodo viene invocato una volta sola durante l’inizializzazione della struttura dati.*/

    int i;
    pid_nsList_h.n_link.prev=NULL;
    pid_nsList_h.n_link.next=NULL;
    nsd_t *current = &pid_nsFree_h;
    nsd_t *past = current;

    for (i=0; i<MAXPROC; i++) {
        pid_nsTable[i].n_type = 0;
        pid_nsTable[i].n_link.prev = &pid_nsFree_h.n_link;
        pid_nsTable[i].n_link.next = &pid_nsFree_h.n_link;
        
        current->n_type = pid_nsTable[i].n_type;

        current->n_link.prev = &past->n_link;

        if (i<MAXPROC-1) {
            nsd_t succ;
            current->n_link.next = &succ.n_link;

            past = current;
            current = &succ;
        }

        else {
            current->n_link.next = &pid_nsFree_h.n_link;
            pid_nsFree_h.n_link.prev = &(current->n_link);
        }        

    }


        /*for (int i=0; i<MAXPROC; i++) {
            pid_nsTable[i] = new nsd_t;
            pid_nsTable[i].n_type = 0;

            pid_nsFree_h = new nsd_t;
            nsd_t *current = pid_nsFree_h;
            nsd_t *past = current;
            for (int j=0; j<MAXPROC; j++) {
                
                current.n_type = pid_nsTable[i].n_type;
                current.n_link = new list_head;
                current.n_link->prev = past.n_link;

                if (j<MAXPROC-1) {
                    nsd_t succ = new nsd_t;
                    succ.n_link = new list_head;
                    current.n_link->next = succ.n_link;

                    past = current;
                    current = succ;
                }

                else {
                    current.n_link->next = pid_nsFree_h.n_link;
                    pid_nsFree_h.n_link->prev = current.n_link;
                }
                
            }
        
        pid_nsTable[i].n_link = pid_nsFree_h.n_link;
        }*/

}


nsd_t *getNamespace(pcb_t *p,int type) {
/*Ritorna il namespace di tipo type associato al processo p (o NULL).*/
    int i;
    nsd_t *res = NULL;
    
    for (i=0; i<MAXSEM; i++) {
        if (p->p_namespaces[i]->n_type == type) {
            res = p->p_namespaces[i];
        }
    }

    return res;
}


nsd_t *allocNamespace(int type) {
/*Alloca un namespace di tipo type dalla lista corretta.*/

    //Rimuovo un namespace dalla lista dei namespace di tipo type liberi
    nsd_t *res=NULL;
    if (empty(pid_nsFree_h) == false) {

        if (last_el(pid_nsFree_h) == true) {
            res = &pid_nsFree_h;

            pid_nsFree_h.n_link.prev = NULL;
            pid_nsFree_h.n_link.next = NULL;
        }

        else {
            res = &pid_nsFree_h;
            list_head *h = &pid_nsFree_h.n_link;

            h->next->prev = h->prev;
            h->prev->next = h->next;

            pid_nsFree_h.n_link = *pid_nsFree_h.n_link.next;
        }
    }


    //Aggiungo (in testa) il nuovo namespace alla lista dei namespace di tipo type utilizzati
    list_head *h = &pid_nsList_h.n_link;

    h->next->prev = &res->n_link;
    res->n_link.next = h->next;
    res->n_link.prev = h;
    h->next = &res->n_link;

    return res;

}



int addNamespace(pcb_t *p,nsd_t *ns) {
/*Associa al processo p e a tutti i suoi figli il namespace ns. Ritorna FALSE in caso di errore, TRUE altrimenti.*/
/* Insert the namespace ns as the namespace for the correct type of p to ns. If the namespace is currently the base (i.e. there are
no descriptor for the namespace), allocate a new descriptor from the type nsFree list, insert it in the correct list, initialize all of
the fields, and proceed as above. If a new namespace descriptor needs to be allocated and the type nsFree list is empty, return
TRUE. In all other cases return FALSE. */

    int i,res=0;
    for (i=0; i<MAXSEM; i++) {
        if (p->p_namespaces[i]->n_type == ns->n_type) {
                                      if (p->p_namespaces[i] == NULL) {
                                                     if (empty(pid_nsFree_h) == true) { res=1; break; }
                                                     
                                                     nsd_t *t=allocNamespace(ns->n_type);
                                                     *t=*ns;
                                                     p->p_namespaces[i]=t;
                                                     //^ da associare anche ai figli
                                      }
        }
        
    }

    return res;
}



void freeNamespace(nsd_t *ns) {
/*Libera il namespace ns ri-inserendolo nella lista di namespace corretta.*/

    //Rimuovo il namespace ns dalla lista dei namespace di tipo ns.n_type utilizzati
    int i;
    list_head *t = &pid_nsList_h.n_link;

    //if (empty(pid_nsList_h) == false) {                   <-------------------------------------------------------------------------------

    for (i=0; i<MAXPROC ; i++) {
        if (t == &ns->n_link) {
            list_head *p = t->prev;
            p->next = t->next;

            list_head *n = t->next;
            n->prev = t->prev;            

            break;
        }

        t = t->next;
    }

    //Aggiungo (in testa) il namespace ns alla lista dei namespace di tipo ns.n_type liberi
    list_head *h = &pid_nsFree_h.n_link;

    h->next->prev = &ns->n_link;
    ns->n_link.next = h->next;
    ns->n_link.prev = h;
    h->next = &ns->n_link;

/* Free a namespace, adding its list pointer (n link) to the correct type nsFree list. */
}
