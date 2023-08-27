#ifndef PCB_H
#define PCB_H

#include <pandos_types.h>
#include <pandos_const.h>


/*Inizializza la lista pcbFree_h in modo da contenere gli elementi della pcbFree_table*/
void initPcbs();

/*Inserisce il pcb puntato da p nella lista pcbFree_h*/
void freePcb(pcb_t *p);

/*Rimuove un elemento dalla pcbFree_h e restituisce l'elemento rimosso*/
pcb_t *allocPcb();

/*Crea una lista di pcb inizializzandola come lista vuota*/
void mkEmptyProcQ(struct list_head *head);

/*Se la lista puntata da head è vuota restituisce TRUE, altrimenti FALSE*/
int emptyProcQ(struct list_head *head);

/*Inserisce l'elemento puntato da p nella coda dei processi puntata da head*/
void insertProcQ(struct list_head *head, pcb_t *p);

/*Restituisce l'elemento di testa della coda dei processi puntata da head senza rimuoverlo*/
pcb_t *headProcQ(struct list_head *head);

/*Rimuove il primo elemento della coda dei processi puntata da head*/
pcb_t *removeProcQ(struct list_head *head);

/*Rimuove il pcb puntato da p dalla coda dei processi puntata da head*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

/*Se il pcb puntato da p non ha figli restituisce TRUE, altrimenti FALSE*/
int emptyChild(pcb_t *p);

/*Inserisce il pcb puntato da p come figlio del pcb puntato da prnt*/
void insertChild(pcb_t *prnt, pcb_t *p);

/*Rimuove il primo figlio del pcb puntato da p*/
pcb_t *removeChild(pcb_t *p);

/*Rimuove il pcb puntato da p dalla lista dei figli del padre*/
pcb_t *outChild(pcb_t *p);

#endif