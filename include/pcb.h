#ifndef PCB_H
#define PCB_H

#include <pandos_types.h>
#include <pandos_const.h>


/*Funzione 1*/
void initPcbs();

/*Funzione 2*/
void freePcb(pcb_t *p);

/*Funzione 3*/
pcb_t *allocPcb();

/*Funzione 4*/
void mkEmptyProcQ(struct list_head *head);

/*Funzione 5*/
int emptyProcQ(struct list_head *head);

/*Funzione 6*/
void insertProcQ(struct list_head *head, pcb_t *p);

/*Funzione 7*/
pcb_t *headProcQ(struct list_head *head);

/*Funzione 8*/
pcb_t *removeProcQ(struct list_head *head);

/*Funzione 9*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

/*Funzione 10*/
int emptyChild(pcb_t *p);

/*Funzione 11*/
void insertChild(pcb_t *prnt, pcb_t *p);

/*Funzione 12*/
pcb_t *removeChild(pcb_t *p);

/*Funzione 13*/
pcb_t *outChild(pcb_t *p);

#endif