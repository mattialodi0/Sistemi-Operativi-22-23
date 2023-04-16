#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "scheduler.h"


// ALLA FINE DELLE SYSCALL CHE NON BLOCCANO O TERMINANO IL PROCESSO:
// deve essere restituito il controllo al processo aumentando il suo pc di 4



int Create_Process(state_t *statep, support_t *supportp, nsd_t *ns){
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

void Terminate_Process(int pid){

}

//decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr){
    semaddr--;      //non so se sia un intero o proprio una struttura per un semaforo
    
    if(semaddr <= 0) {
        soft_blocked_count++;
        //blocco del processo: si salva lo stato, lo si rimuove dalla coda ready e lo si mette ad aspettare
    }
}

//incrementa il semaforo all'ind semaddr, se diventa > 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr){
    semaddr++;
    if(semaddr > 0) {
        soft_blocked_count--;
        //sblocco del processo, aggiunta alla coda ready
    }
}

int DO_IO(int *cmdAddr, int *cmdValues){

}

int Get_CPU_Time(){
    return active_process->p_time;
    //al momento torna 0, bisogna fare un calcolo tra inizio di quando viene chiamato 
    //per la prima volta il processo e quando viene chiamata la funzione

    //in realtà credo di no
}

int Wait_For_Clock(){

}

// ritorna un puntatore alla struttura di supporto del chiamante
support_t* Get_Support_Data(){
    return active_process->p_supportStruct;
}

// ritorna il pid del chiamante
int Get_Process_Id(int parent){
    return active_process->p_pid;
}

// cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
// e ritorna il numero di figli del processo
int Get_Children(int *children, int size){
    struct list_head *pos;
    struct list_head *member;
    int n = 0;
    list_for_each_entry(pos, active_process->p_child, member) {
        if(active_process->namespaces == container_of(member, pid_t, p_child)->namespaces)    
            n++;
    }
    int i = 0;
    list_for_each_entry(pos, active_process->p_child, member) {
        if(active_process->namespaces == container_of(member, pid_t, p_child)->namespaces) {
            children[i] = container_of(member, pid_t, p_child)->p_pid; i++;
        }            
        if(i >= size) break;
    }

    return n;
}   
//qualcosa non torna 
