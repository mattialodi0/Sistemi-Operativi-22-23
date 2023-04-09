#include ``/usr/include/umps3/umps/libumps.h''
#include <pandos_const.h>
#include <pandos_types.h>

#include <umps3/umps/libumps.h>
#include "pcb.h"
#include "ash.h"
#include "ns.h"

#define DISPNUM 49

int main(void) {
    //processi vivi
    int process_count = 0;

    //processi bloccati
    int soft_blocked_count = 0;
    
    //processi ready
    struct list_head *ready_queue; 
    mkEmptyProcQ(ready_queue);

    //puntatore al proc attivo
    pcb_t *active_process = NULL;

    //array di semafori, uno per dispositivo
    int sem_disp[DISPNUM];
    for(int i=0; i<49; i++) 
        sem_disp[i] = 0;

    //iniz. strutture fase 1
    initPcbs();
    initSemd();
    initNS();

    //pass up vector

    //iniz. interval timer 100ms
    //registro:  0x1000.0020
    //LDIT(T) aggiorna l'interval timer con T per la timescale del processore

    //creazione di un processo

    //chiamata allo scheduler
}