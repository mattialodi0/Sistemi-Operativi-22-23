#include <syscalls.h>

extern int pid_count;

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
        insertProcQ(&ready_queue, new_proc);    // inseriamo in coda il processo
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
void TerminateProcess(int pid){
    if (pid == 0){                              //pid == 0, bisogna terminare active_process (processo invocante), e i suoi figli
        outChild(active_process);               //outChild per eliminare i figli dal padre
        //Da controllare da riga 36 a 40
        if(active_process->p_semAdd < 0){       //if semaphor < 0 deve essere incrementato (controllare su 3.9 del libro)
            if(headBlocked(active_process->p_semAdd) == NULL){
                active_process->p_semAdd++;
            }
        }
        active_process = NULL;
    }
    else{
        //cercare processo con stesso pid
        //stessa cosa dell'if ma con il processo del pid preso in input
    }
    process_count--;
    //soft_blocked_count--;   // ?
    scheduler();
}

// decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr)
{
    if (*semaddr == 0)
    {
        soft_blocked_count++;
        // salvataggio dello stato
        STST(&active_process->p_s); // MA setta il PC = 0

        insertBlocked(semaddr, active_process);
        //forse va richiamato lo scheduler per selezionare un nuovo processo
        //scheduler();
    }
    else
    {
        *semaddr--;
        pcb_t *waked_proc = removeBlocked(semaddr);
        if (waked_proc != NULL)
        {
            // wakeup proc
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
        }
        else
            *semaddr--;
    }
}

// incrementa il semaforo all'ind semaddr, se diventa > 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr)
{
    if (*semaddr == 1)
    {
        soft_blocked_count++;
        // salvataggio dello stato
        STST(&active_process->p_s); // MA setta il PC = 0

        insertBlocked(semaddr, active_process);
    }
    else
    {
        *semaddr++;
        pcb_t *waked_proc = removeBlocked(semaddr);
        if (waked_proc != NULL)
        {
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
        }
        else
            *semaddr++;
    }
}

int DoIO(int *cmdAddr, int *cmdValues)
{
    // capitolo 5 della documentazione
}

int GetCPUTime()
{
    return active_process->p_time;
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
