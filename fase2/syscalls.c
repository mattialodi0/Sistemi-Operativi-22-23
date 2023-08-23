#include <syscalls.h>

extern cpu_t timer_start;
extern int pid_count;
extern int debug_var;
extern int debug_char;

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

        // return new_proc->p_pid;
        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = 0; // non ha senso ma è richiesto
        state->reg_v0 = new_proc->p_pid;
    }
    else
    {
        // return (-1);
        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = -1;
    }
    NonBlockingExceptEnd();
}

// termina il processo indicato da pid insieme ai suoi figli
void TerminateProcess(int pid)
{
    pcb_t *proc, *f_proc;

    if (pid == 0) // pid == 0, bisogna terminare active_process (processo invocante), e i suoi figli
    {
        proc = active_process;
        f_proc = active_process;
    }
    else
    { 
     //stessa cosa dell'if ma con il processo del pid preso in input
        proc = findProcess(pid);
        f_proc = findProcess(pid);
    }

    while (proc != NULL)
        {
            outChild(proc);         // outChild per eliminare il figlio dal padre
            if (proc->p_semAdd < 0) // if semaphor < 0 deve essere incrementato (controllare su 3.9 del libro)
            {                                   
                if (headBlocked(proc->p_semAdd) == NULL)        // !!!!! SOLO SE NON è DI UN DEVICE !!!!!
                {
                    proc->p_semAdd++;
                    soft_blocked_count--;
                }
            }
            proc = NULL;
            process_count--;
            proc = removeChild(f_proc);
        }

    scheduler();
}


pcb_PTR findProcess(int pid) {
    struct list_head* current_process;
    pcb_PTR pcb;

    // list_for_each(current_process, &ready_queue) {
    //     pcb = list_entry(current_process, pcb_PTR, p_list);
    //     if (pcb->p_pid == pid) {
    //         return pcb;
    //     }
    // }
    // Processo non trovato
    return NULL;
}


// decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr)
{
    // per semafori semplici
    // (*semaddr)--;
    // if (*semaddr < 0)
    // {
    //     soft_blocked_count++;
    //     if (insertBlocked(semaddr, active_process)) {
    //         PANIC(); // errore nei semafori
    //     }
    //     BlockingExceptEnd();
    // }
    // else
    //     NonBlockingExceptEnd();

    // per semafori binari
    if (*semaddr <= 0)
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
        debug_var = *semaddr;
        debug3();
        BlockingExceptEnd(semaddr);
    }
    else if (*semaddr >= 1)
    {
        pcb_t *waked_proc = removeBlocked(semaddr);
        if (waked_proc != NULL)
        {
            // wakeup proc
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
            debug_var = *semaddr;
            debug4();
        }
        else
        {
            (*semaddr)--;
            debug_var = *semaddr;
            debug5();
        }
        NonBlockingExceptEnd();
    }
    else
        PANIC();
}

// incrementa il semaforo all'ind semaddr, se diventa >= 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr)
{
    // per semafori semplici
    // (*semaddr)++;
    // pcb_t *waked_proc = removeBlocked(semaddr);
    // if (waked_proc != NULL)
    // {
    //     // wakeup proc
    //     insertProcQ(&ready_queue, waked_proc);
    //     soft_blocked_count--;
    // }
    // NonBlockingExceptEnd();

    // per semafori binari
    if (*semaddr >= 1)
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
        debug_var = *semaddr;
        debug3();
        BlockingExceptEnd();
    }
    else if (*semaddr <= 0)
    {
        pcb_t *waked_proc = removeBlocked(semaddr);
        if (waked_proc != NULL)
        {
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
            debug_var = *semaddr;
            debug4();
        }
        else
        {
            (*semaddr)++;
            debug_var = *semaddr;
            debug5();
        }
        NonBlockingExceptEnd();
    }
    else
        PANIC();
}

/*
 * Esegue un operazione di I/O sul dispositivo indicato, in modo sincrono
 * cmdAddr contiene l'ind del comando e cmdValues è un array di puntatori ai valori
 * del comando
 */
int *mem;
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
    int *semaddr = &sem_dev_terminal_w[0];
    (*semaddr)--;
    if (*semaddr < 0)
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
    }
    else
        PANIC();

    termreg_t *dev_reg = (termreg_t *)(cmdAddr - 2);
    dev_reg->transm_command = cmdValues[1]; // 2 | (((unsigned int)'O') << 8);

    // *cmdAddr = cmdValues[0];
    // *(cmdAddr++) = cmdValues[1];

    // *(cmdAddr + 0xC) = cmdValues[0];
    // debug_char = base->transm_command;  debug3();
    // debug_var = *(unsigned int *)(cmdAddr + 0x8); debug3();

    // copia dei valori in cmdValues,  probabilmente va fatto nel nonTimerInterruptT
    // cmdValues[0] = 5;
    mem = cmdValues;

    // unsigned int status = *(unsigned int *)(cmdAddr + 0x8);
    // if (status == 5)
    //     return 0;
    // else
    //     return -1;

    BlockingExceptEnd();
}

int GetCPUTime()
{
    cpu_t time;
    STCK(time);

    // return active_process->p_time + (time - timer_start);
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = (int) (active_process->p_time + (time - timer_start)); 

    NonBlockingExceptEnd();
}

int WaitForClock()
{
    Passeren(&IT_sem);
    BlockingExceptEnd();
}

// ritorna un puntatore alla struttura di supporto del chiamante
support_t *GetSupportData()
{
    // return active_process->p_supportStruct;
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = (int)active_process->p_supportStruct;

    NonBlockingExceptEnd();
}

// ritorna il pid del chiamante
int GetProcessId(int parent)
{
    // return active_process->p_pid;
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = active_process->p_pid;

    NonBlockingExceptEnd();
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
