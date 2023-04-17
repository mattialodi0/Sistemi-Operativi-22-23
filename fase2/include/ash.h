#ifndef ASH_H
#define ASH_H

#include "pcb.h"
#include "list.h"
#include "hash.h"
#include "hashtable.h"
#include <pandos_types.h>
#include <pandos_const.h>


/* insertBloked - inserisce il PCB puntato da p nella coda dei processi bloccati del semafori con chiave semAdd
*   ritorna vero solo se il semaforo cercato non esiste e non è possibile allocarne uno nuovo */
 int insertBlocked(int *semAdd, pcb_t *p);

/* removeBlocked - ritorna il primo PCB bloccato nella coda del semaforo con chiave semAdd rimuovendolo
*   ritorna NULL se non esiste un SEMD con quella chiave nella ASH */
 pcb_t* removeBlocked(int *semAdd);

/* outBlocked - ritorna il PCB p rimuovendolo dalla coda sel SEMD su cui è bloccato
*   ritorna NULL se non esiste un PCB p in tale coda */
 pcb_t* outBlocked(pcb_t *p);

/* headBlocked - ritorna il primo PCB bloccato sul SEMD con chiave semAdd senza rimuoverlo
*   ritorna NULL se non è presente un SEMD con quella chiave o se la sua coda dei processi bloccati è vuota */
 pcb_t* headBlocked(int *semAdd);

/* initASH - inizializza la lista dei semdFree in modi che contengano tutti gli elementi di semdTable
*            questo metodo viene chiamato una volta sola per inizalizzare questa struttura */
 void initASH();


#endif