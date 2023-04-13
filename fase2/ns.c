#include "ns.h"

#define MAXPROC 20
#define MAXTYPES 1

/*typedef struct list_head {
    struct list_head* next;
    struct list_head* prev;
} list_head;


//namespace descriptor data structure
typedef struct nsd_t {
    int n_type;
    struct list_head n_link;
} nsd_t;


typedef struct pcb_t {
    //process queue
    struct list_head p_next;

    //process tree fields
    struct pcb_t *p_parent; //ptr to parent
    struct list_head p_child; //children list
    struct list_head p_sib; //sibling list

    //process status information
    state_t p_s; //processor state
    cpu_t p_time; //cpu time used by proc
    int *p_semAdd; //ptr to semaphore on which proc is blocked
    nsd_t *p_namespaces[MAXSEM]; //ACTIVE namespace for each type
} pcb_t, *pcb_PTR;*/




static nsd_t *nsFree_h[MAXTYPES];
//^ogni cella di questo vettore contiene la testa della lista dei namespace di tipo i (0 <= i < MAXTYPES) liberi o inutilizzati

static nsd_t *nsList_h[MAXTYPES];
//^ogni cella di questo vettore contiene la testa della lista dei namespace di tipo i (0 <= i < MAXTYPES) utilizzati

static nsd_t pid_nsdTable[MAXPROC]; //vettore dei namespace di tipo PID


void initNamespaces() {
/*Inizializza tutte le liste dei namespace liberi.
Questo metodo viene invocato una volta sola durante l'inizializzazione della struttura dati.*/
// --> per la fase 1 abbiamo un solo tipo di namespace : PID

    // inizializzo la lista dei nsd liberi
    nsFree_h[0] = &pid_nsdTable[0];
    nsd_t *ele = nsFree_h[0];    

    for (int i = 1; i < MAXPROC; i++)
    {
        ele->n_link.next = &(pid_nsdTable[i].n_link);
        ele->n_link.prev = NULL;
        ele = container_of(ele->n_link.next, nsd_t, n_link);
    }

    // inizializzo la lista dei nsd attivi
    nsList_h[0] = NULL;

}


nsd_t *getNamespace(pcb_t *p,int type) {
/*Ritorna il namespace di tipo type associato al processo p (o NULL).*/
    if (p == NULL || (type < 0 || type >= MAXTYPES)) return NULL;

    return p->namespaces[type];
}


nsd_t *allocNamespace(int type) {
/*Alloca un namespace di tipo type dalla lista corretta.*/

    if (type >= 0 && type < MAXTYPES) {
        //rimuovo l'el in testa alla lista dei nsd di tipo type liberi
        nsd_t *p = nsFree_h[type];
        nsFree_h[type] = container_of(nsFree_h[type]->n_link.next, nsd_t, n_link);

        //e la inserisco in testa alla lista dei nsd di tipo type utilizzati
        
        nsd_t *t = nsList_h[type]; //testa attuale della lista nsList_h
        nsList_h[type] = p;
        nsList_h[type]->n_link.next = &t->n_link;
        
        return p;
    }
    
    return NULL;
}



int addNamespace(pcb_t *p,nsd_t *ns) {
/*Associa al processo p e a tutti i suoi figli il namespace ns. Ritorna FALSE in caso di errore, TRUE altrimenti.*/

    int res=1;
    if (p == NULL || ns == NULL) res=0;
    
    //associo il namespace ns al processo p
    p->namespaces[ns->n_type] = ns;
    //^in questa fase del prog. ns->n_type € {0}

    //associo ns anche agli eventuali figli di p
    if (emptyChild(p) == false)
    {
        struct list_head *ele;
        
        //scorro la lista dei figli (con list_for_each 'esplicito')
        for (ele=(p->p_child.next); ele!=&(p->p_child); ele=ele->next) {
            pcb_t *current = container_of(ele, pcb_t, p_sib);
            current->namespaces[ns->n_type] = ns;
        }
    }
    
    return res;

}



void freeNamespace(nsd_t *ns) {
/*Libera il namespace ns ri-inserendolo nella lista di namespace corretta.*/

    //rimuovo ns dalla lista dei namespace di tipo ns->n_type utilizzati
    nsd_t *p = nsList_h[ns->n_type];
    nsList_h[ns->n_type] = container_of(nsList_h[ns->n_type]->n_link.next, nsd_t, n_link);

    //lo inserisco quindi in testa alla lista dei namespace di tipo ns->n_type liberi
    nsd_t *t = nsFree_h[ns->n_type]; //testa attuale della lista nsFree_h
    nsFree_h[ns->n_type] = p;
    nsFree_h[ns->n_type]->n_link.next = &t->n_link;
    
}
