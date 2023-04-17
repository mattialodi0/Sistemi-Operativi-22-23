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
        }
        else 
            *semaddr++;
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


void syscallHandler() {
    int v0, v1, v2, v3;
    //vanno presi dai registri e castati

    switch (v0)
    {
    case 1:
        Create_Process(v1, v2, v3);
        break;
    case 2:
        Terminate_Process(v1);
        break;
    case 3:
        Passeren(v1);
        break;
    case 4:
        Verhogen(v1);
        break;
    case 5:
        DO_IO(v1, v2);
        break;
    case 6:
        Get_CPU_Time();
        break;
    case 7:
        Wait_For_Clock();
        break;
    case 8:
        Get_Support_Data();
        break;    
    case 9:
        Get_Process_Id(v1);
        break;
    case 10:
        Get_Children(v1, v2);
        break;
    default:
        break;
    }
}