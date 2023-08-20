#include <syscalls.h>

extern int pid_count;
extern cpu_t timer_start;
extern int debug_var;

// ALLA FINE DELLE SYSCALL CHE NON BLOCCANO O TERMINANO IL PROCESSO:
// deve essere restituito il controllo al processo aumentando il suo pc di 4
int CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns)
{
    pcb_t *new_proc = allocPcb(); // inizializza new_proc, allocPcb la mette == NULL se vuota, quindi va direttamente nell'else?
    if (new_proc != NULL)
    {
        new_proc->p_parent = NULL; // Inizializzo tutti i campi del processo attivo a NULL/0
        INIT_LIST_HEAD(&new_proc->p_child);
        INIT_LIST_HEAD(&new_proc->p_sib);
        new_proc->p_pid = pid_count++;        // assegno il pid successivo al processo
        new_proc->p_s = *statep;              // assegno allo stato il parametro in input
        new_proc->p_supportStruct = supportp; // assegno alla struttura di supporto il parametro in input
        process_count++;
        insertProcQ(&ready_queue, new_proc);   // inseriamo in coda il processo
        insertChild(active_process, new_proc); // inseriamo new_proc come figlio di active_process
        new_proc->p_semAdd = NULL;
        new_proc->p_time = 0; // inizializzo il tempo a 0
        if (ns != NULL)
        {
            for (int i = 0; i < NS_TYPE_MAX; i++)
            {
                new_proc->namespaces[i] = &ns[i];
            }
        }
        else
        {
            for (int i = 0; i < NS_TYPE_MAX; i++)
            {
                new_proc->namespaces[i] = active_process->namespaces[i];
            }
        }
        return new_proc->p_pid;
    }
    else
    {
        return (-1);
    }
}

// termina il processo indicato da pid insieme ai suoi figli
void TerminateProcess(int pid)
{
    if (pid == 0)
    {                             // pid == 0, bisogna terminare active_process (processo invocante), e i suoi figli
        outChild(active_process); // outChild per eliminare i figli dal padre
        if (active_process->p_semAdd < 0)
        { // if semaphor < 0 deve essere incrementato (controllare su 3.9 del libro)
            if (headBlocked(active_process->p_semAdd) == NULL)
            {
                active_process->p_semAdd++;
            }
        }
        active_process = NULL;
    }
    else
    { /*
     //cercare processo con stesso pid
     pcb_PTR target_process = findProcess(pid);
     //stessa cosa dell'if ma con il processo del pid preso in input
     if(target_process != NULL){
         outChild(target_process);
         target_process->p_semAdd++;
         target_process = NULL;
     }*/
    }
    process_count--;
    soft_blocked_count--; // ?
    scheduler();
}

/*
pcb_PTR findProcess(int pid) {
    struct list_head* current_process;
    pcb_PTR pcb;

    list_for_each(current_process, &ready_queue) {
        pcb = list_entry(current_process, pcb_PTR, p_list);
        if (pcb->p_pid == pid) {
            return pcb;
        }
    }
    // Processo non trovato
    return NULL;
}
*/


// decrementa il semaforo all'ind semaddr, se diventa <= 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr)
{
    (*semaddr)--;
    if (*semaddr < 0)
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process)) {
            PANIC(); // errore nei semafori
        }
        BlockingExceptEnd();
    }
    else
        NonBlockingExceptEnd();

    // per semafori binari
    // if (*semaddr == 0)
    // {
    //     BlockingExceptEnd(semaddr);
    // }
    // else
    // {
    //     *semaddr--;
    //     pcb_t *waked_proc = removeBlocked(semaddr);
    //     if (waked_proc != NULL)
    //     {
    //         // wakeup proc
    //         insertProcQ(&ready_queue, waked_proc);
    //         soft_blocked_count--;
    //     }
    //     NonBlockingExceptEnd();
    // }
}

// incrementa il semaforo all'ind semaddr, se diventa >= 1 il processo viene messo nella coda ready
void Verhogen(int *semaddr)
{
    (*semaddr)++;
    pcb_t *waked_proc = removeBlocked(semaddr);
    if (waked_proc != NULL)
    {
        // wakeup proc
        insertProcQ(&ready_queue, waked_proc);
        soft_blocked_count--;
    }
    NonBlockingExceptEnd();

    // per semafori binari
    // if (*semaddr == 1)
    // {
    //     BlockingExceptEnd();
    // }
    // else
    // {
    //     *semaddr++;
    //     pcb_t *waked_proc = removeBlocked(semaddr);
    //     if (waked_proc != NULL)
    //     {
    //         insertProcQ(&ready_queue, waked_proc);
    //         soft_blocked_count--;
    //     }
    //     NonBlockingExceptEnd();
    // }
}

/*
 * Esegue un operazione di I/O sul dispositivo indicato, in modo sincrono
 * cmdAddr contiene l'ind del comando e cmdValues è un array di puntatori ai valori
 * del comando
 */
int DoIO(unsigned int *cmdAddr, unsigned int *cmdValues)
{
    // Installed Devices Bit Map 0x1000002C
    // Interrupting Devices Bit Map 0x10000040
    // devAddrBase = 0x10000054 + ((IntlineNo - 3) * 0x80) + (DevNo * 0x10)

    // (verifica che il disp sia installato)
    // P sul sem indicato in a1 e a2
    // ...
    // ritorna 0 o -1

    // solo per print:

    // P 
    Passeren(&sem_dev_terminal_w[0]);
    // unsigned int *semaddr = &sem_dev_terminal_w[0];
    // (*semaddr) --;
    // if (*semaddr <= 0)
    // {
    //     if (insertBlocked(semaddr, active_process)) {
    //         PANIC(); // errore nei semafori
    //     }
    //     else soft_blocked_count++;

    // } 
    // else PANIC();

    *(cmdAddr + 0xc) = cmdValues[0];

    // copia dei valori dei registri in cmdValues

    unsigned int status = *(unsigned int *)(cmdAddr + 0x8);
    if (status == 5)
        return 0;
    else
        return -1;

    BlockingExceptEnd(); //  a quale ind ?
}

int GetCPUTime()
{
    cpu_t time;
    STCK(time);
    return active_process->p_time + (time - timer_start);
    // STCK(); //potrebbe servire il timer TOD e questa macro serve per leggerlo
    // bisogna sommargli il tempo accumulato nel quanto corrente
}

int WaitForClock()
{
    Passeren(&IT_sem);
    scheduler();
}

// ritorna un puntatore alla struttura di supporto del chiamante
support_t *GetSupportData()
{
    return active_process->p_supportStruct;
}

// ritorna il pid del chiamante
int GetProcessId(int parent)
{
    return active_process->p_pid;
}

// cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
// e ritorna il numero di figli del processo
int GetChildren(int *children, int size)
{
    struct list_head *pos;
    int n = 0;
    /*pcb_t *child = container_of(active_process->p_child, pcb_t, p_child);
    pcb_t *child_sib = container_of(child->p_sib, pcb_t, p_sib);

    if(eqNS(child->namespaces, active_process->namespaces)) n++;
    list_for_each_entry(pos, child_sib, p_sib) {
        if(eqNS(container_of(pos, pcb_t, p_sib)->namespaces, active_process->namespaces))
            n++;
    }

    int i = 0;
    if(eqNS(child->namespaces, active_process->namespaces)) {
        children[i] = child->p_pid;
        i++;
    }
    list_for_each_entry(pos, child_sib, p_sib) {
        if(i >= size) break;
        if(eqNS(container_of(pos, pcb_t, p_sib)->namespaces, active_process->namespaces))
            children[i] = container_of(pos, pcb_t, p_sib)->p_pid;
            i++;
    }*/

    return n;
}
// qualcosa non torna

// confronta i namespaces, un campo alla volta, da implementare
bool eqNS(nsd_t *a[], nsd_t *b[])
{
    bool res = true;
    for (int i = 0; i < MAXPROC; i++)
    {
        if (a[i] != b[i])
            res = false;
    }

    return res;
}
