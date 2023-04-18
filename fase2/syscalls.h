#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "scheduler.h"


// ALLA FINE DELLE SYSCALL CHE NON BLOCCANO O TERMINANO IL PROCESSO:
// deve essere restituito il controllo al processo aumentando il suo pc di 4
 


int CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns){
    pcb_t *new_proc = allocPcb();
    if(new_proc != NULL){
        process_count++;
        insertProcQ(ready_queue, new_proc);     //inseriamo in coda il processo
        insertChild(active_process, new_proc);  //inseriamo new_proc come figlio di active_process
        new_proc->p_time = 0;                   //inizializzo il tempo a 0
        new_proc->p_pid = pid_count++;          //assegno il pid successivo al processo
        for(int i=0; i<NS_TYPE_MAX; i++){      
            new_proc->namespaces[i] = ns;       //controllare inizializzazione namespace
        }
        return new_proc->p_pid;
    }
    else{
        return (-1);
    }
    //rimane da gestire state_t e support_t e controllare inizializzazione
}

void TerminateProcess(int pid){

}

//decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr){
    if(*semaddr == 0) {
        soft_blocked_count++;
        //salvataggio dello stato
        //...
        insertBlocked(semaddr, active_process);
    }
    else {
        pcb_t *waked_proc = removeBlocked(semaddr);
        if(waked_proc != NULL) {
            //wakeup proc
            //...
            soft_blocked_count++;
        }
        else 
            *semaddr--;
    }
}

//incrementa il semaforo all'ind semaddr, se diventa > 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr){
    if(*semaddr == 1) {
        soft_blocked_count++;
        //salvataggio dello stato
        //...
        insertBlocked(semaddr, active_process);
    }
    else {
        pcb_t *waked_proc = removeBlocked(semaddr);
        if(waked_proc != NULL) {
            //wakeup proc
            //...
            soft_blocked_count++;
        }
        else 
            *semaddr++;
    }
}

int DOIO(int *cmdAddr, int *cmdValues){

}

int GetCPUTime(){
    return active_process->p_time;
    //al momento torna 0, bisogna fare un calcolo tra inizio di quando viene chiamato 
    //per la prima volta il processo e quando viene chiamata la funzione

    //in realtà credo di no
}

int WaitForClock(){

}

// ritorna un puntatore alla struttura di supporto del chiamante
support_t* GetSupportData(){
    return active_process->p_supportStruct;
}

// ritorna il pid del chiamante
int GetProcessId(int parent){
    return active_process->p_pid;
}

// cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
// e ritorna il numero di figli del processo
int GetChildren(int *children, int size){
    struct list_head *pos;
    int n = 0;
    pcb_t *child = container_of(active_process->p_child, pid_t, p_child);
    pcb_t *child_sib = container_of(child->p_sib, pid_t, p_sib);

    if(eqNS(child->namespaces, active_process->namespaces)) n++;
    list_for_each_entry(pos, child_sib, p_sib) {
        if(eqNS(container_of(pos, pid_t, p_sib)->namespaces, active_process->namespaces))    
            n++;
    }

    int i = 0;
    if(eqNS(child->namespaces, active_process->namespaces)) {
        children[i] = child->p_pid;
        i++;
    }
    list_for_each_entry(pos, child_sib, p_sib) {
        if(i >= size) break;
        if(eqNS(container_of(pos, pid_t, p_sib)->namespaces, active_process->namespaces))    
            children[i] = container_of(pos, pid_t, p_sib)->p_pid;
            i++;
    }

    return n;
}   
//qualcosa non torna 

//confronta i namespaces, un campo alla volta, da implementare
bool eqNS(nsd_t *a[], nsd_t *b[]) {
    return false;
}

