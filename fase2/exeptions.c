#include </usr/include/umps3/umps/libumps.h>
#include "include/types.h"
#include "scheduler.h"

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

void Passeren(int *semaddr){

}

void Verhogen(int *semaddr){

}

int DO_IO(int *cmdAddr, int *cmdValues){

}

int Get_CPU_Time(){
    return active_process->p_time;
    //al momento torna 0, bisogna fare un calcolo tra inizio di quando viene chiamato 
    //per la prima volta il processo e quando viene chiamata la funzione
}

int Wait_For_Clock(){

}

support_t* Get_Support_Data(){

}

int Get_Process_Id(int parent){

}

int Get_Children(int *children, int size){

}
