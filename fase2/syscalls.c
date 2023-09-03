#include <syscalls.h>


/* Crea un nuovo processo come figlio del chiamante */
void CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns)
{
    pcb_t *new_proc = allocPcb();
    if (new_proc != NULL)
    {
        INIT_LIST_HEAD(&new_proc->p_child);
        INIT_LIST_HEAD(&new_proc->p_sib);
        new_proc->p_pid = pid_count++;        // assegno il pid successivo al processo
        new_proc->p_s = *statep;              // assegno allo stato il parametro in input
        new_proc->p_supportStruct = supportp; // assegno alla struttura di supporto il parametro in input
        process_count++;
        insertProcQ(&ready_queue, new_proc); // inseriamo in coda il processo

        insertChild(active_process, new_proc); // inseriamo new_proc come figlio di active_process
        new_proc->p_time = 0;
        new_proc->p_semAdd = NULL;

        if (ns != NULL)
        {
            new_proc->namespaces[ns->n_type] = ns;
        }
        else
        {
            for (int i = 0; i < NS_TYPE_MAX; i++)
            {
                new_proc->namespaces[i] = active_process->namespaces[i];
            }
        }

        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = new_proc->p_pid;
    }
    else
    {
        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = -1;
    }

    NonBlockingExceptEnd();
}

/* Termina il processo indicato da pid insieme ai suoi figli */
void TerminateProcess(int pid)
{
    pcb_t *f_proc;

    if (pid == 0) // pid == 0, bisogna terminare active_process (processo invocante), e i suoi figli
    {
        f_proc = active_process;

        kill(f_proc);

        BlockingExceptEnd();
    }
    else // stessa cosa dell'if ma con il processo del pid preso in input
    {
        f_proc = findProcess(pid);

        if (kill(f_proc))
            BlockingExceptEnd();
        else
            NonBlockingExceptEnd(); // in caso il processo corrente non sia nell'albero di quelli terminati gli si ritorna il controllo
    }
}

/* Decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler */
void Passeren(int *semaddr)
{
    if (*semaddr == 0) // blocca il proc
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
        BlockingExceptEnd();
    }
    else
    {
        pcb_t *waked_proc = removeBlocked(semaddr);
        if (waked_proc != NULL) // sblocca un proc
        {
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
        }
        else // decrementa il valore del semaforo
        {
            (*semaddr)--;
        }
        NonBlockingExceptEnd();
    }
}

/* Incrementa il semaforo all'ind semaddr, se diventa >= 0 il processo viene messo nella coda ready */
void Verhogen(int *semaddr)
{

    if (*semaddr == 1) // blocca il proc
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
        BlockingExceptEnd();
    }
    else
    {
        pcb_t *waked_proc = removeBlocked(semaddr);
        if (waked_proc != NULL) // sblocca un proc
        {
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
        }
        else // incrementa il valore del semaforo
        {
            (*semaddr)++;
        }
        NonBlockingExceptEnd();
    }
}

/* 
 * Esegue un operazione di I/O sul dispositivo indicato, in modo sincrono cmdAddr contiene l'ind del 
 * comando e cmdValues è un array di puntatori ai valori del comando
 */
void DoIO(int cmdAddr, unsigned int *cmdValues)
{
    int line, no, *semaddr;

    // trova linea e numero del device
    switch (cmdAddr)
    {
    case DISK:
        line = 0;
        no = (cmdAddr - 0x10000054) / 0x10;
        semaddr = &sem_dev_disk[no];
        break;
    case FLASH:
        line = 1;
        no = (cmdAddr - 0x100000D4) / 0x10;
        semaddr = &sem_dev_flash[no];
        break;
    case NETWORK:
        line = 2;
        no = (cmdAddr - 0x10000154) / 0x10;
        semaddr = &sem_dev_net[no];
        break;
    case PRINTER:
        line = 3;
        no = (cmdAddr - 0x100001D4) / 0x10;
        semaddr = &sem_dev_printer[no];
        break;
    case TERM:
        line = 4;
        no = (cmdAddr - 0x10000254) / 0x10;
        if (cmdAddr == (0x10000254 + 0x10 * no) + 0x8)
        {
            semaddr = &sem_dev_terminal_w[no];
        }
        else if (cmdAddr == (0x10000254 + 0x10 * no))
        {
            semaddr = &sem_dev_terminal_r[no];
        }
        else
            PANIC();
        break;
    default:
        PANIC();
        break;
    }

    // verifica che il device sia installato
    switch (no)
    {
    case 0:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV0ON) == 0)
            PANIC();
        break;
    case 1:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV1ON) == 0)
            PANIC();
        break;
    case 2:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV2ON) == 0)
            PANIC();
        break;
    case 3:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV3ON) == 0)
            PANIC();
        break;
    case 4:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV4ON) == 0)
            PANIC();
        break;
    case 5:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV5ON) == 0)
            PANIC();
        break;
    case 6:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV6ON) == 0)
            PANIC();
        break;
    case 7:
        if ((*(unsigned int *)(0x1000002C + (0x4 * line)) & DEV7ON) == 0)
            PANIC();
        break;
    default:
        break;
    }

    // inserimento degli operandi nel device register
    if (line == 4)
    {
        int *p = (int *)cmdAddr;
        *p = cmdValues[0];
        *(p + 1) = cmdValues[1];
    }
    else
    {
        int *p = (int *)cmdAddr;
        *p = cmdValues[0];
        *(p + 1) = cmdValues[1];
        *(p + 2) = cmdValues[2];
        *(p + 3) = cmdValues[3];
    }

    // P
    if (*semaddr <= 0)
    {
        soft_blocked_count++;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
    }
    else
        PANIC();

    active_process->io_addr = cmdValues;

    BlockingExceptEnd();
}

/* Ritorna il tempo di uso della CPU del proc corrente */
void GetCPUTime()
{
    cpu_t time;
    STCK(time);

    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = (int)(active_process->p_time + (time - timer_start));

    NonBlockingExceptEnd();
}

/* Blocca il proc corrente sul semaforo dello pseudo-clock con una P */
void WaitForClock()
{
    int *semaddr = &IT_sem;
    if (*semaddr <= 0)
    {
        soft_blocked_count++;
        (*semaddr)--;
        if (insertBlocked(semaddr, active_process))
        {
            PANIC(); // errore nei semafori
        }
    }
    else
    {
        PANIC();
    }

    BlockingExceptEnd();
}

/* Ritorna un puntatore alla struttura di supporto del chiamante */
void GetSupportData()
{
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = (int)active_process->p_supportStruct;

    NonBlockingExceptEnd();
}

/* Ritorna il pid del chiamante o quello del padre */
void GetProcessId(int parent)
{
    if (parent)
    {
        if (eqNS(active_process, active_process->p_parent))
        {
            state_t *state = (state_t *)BIOSDATAPAGE;
            state->reg_v0 = active_process->p_parent->p_pid;
        }
        else
        {
            state_t *state = (state_t *)BIOSDATAPAGE;
            state->reg_v0 = 0;
        }
    }
    else
    {
        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = active_process->p_pid;
    }

    NonBlockingExceptEnd();
}

/*
 * Cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
 * e ritorna il numero di figli del processo
 */
void GetChildren(int *children, int size)
{
    struct list_head *pos;
    pcb_t *p;
    int n = 0;

    list_for_each(pos, &active_process->p_child)
    {
        p = list_entry(pos, pcb_t, p_sib);
        if (eqNS(p, active_process))
        {
            if (n < size)
                children[n] = p->p_pid;
            n++;
        }
    }

    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = n;

    NonBlockingExceptEnd();
}


/* Confronta i namespaces, un campo alla volta, da implementare */
bool eqNS(pcb_t *a, pcb_t *b)
{
    for (int i = 0; i < NS_TYPE_MAX; i++)
    {
        if (getNamespace(a, i) != getNamespace(b, i))
        {
            return false;
        }
    }
    return true;
}

/* Termina un proc e la sua progenie */
int kill(pcb_t *f_proc)
{
    int term_active = 0;

    if (f_proc == NULL)
    {
        PANIC();
    }

    if (f_proc->p_pid == active_process->p_pid)
        term_active = 1;

    outChild(f_proc);

    if (headBlocked(f_proc->p_semAdd) != NULL)
    {
        if (notDevice(f_proc->p_semAdd)) // se non è il sem di un device
        {
            if (outBlocked(f_proc) == NULL)
                PANIC();
            soft_blocked_count--;
        }
    }
    else
    {
        outProcQ(&ready_queue, f_proc);
    }

    // chiamata ricorsiva su tutti i figli
    pcb_t *proc = removeChild(f_proc);
    while (proc != NULL)
    {
        term_active |= kill(proc);
        proc = removeChild(f_proc);
    }

    freePcb(f_proc);
    process_count--;

    return term_active;
}

/* Restituisce un proc dato il pid */
pcb_PTR findProcess(int pid)
{
    // è nella ready queue
    struct list_head *pos;
    list_for_each(pos, &ready_queue)
    {
        pcb_t *p = list_entry(pos, pcb_t, p_list);
        if (p->p_pid == pid)
        {
            return p;
        }
    }

    // è in attesa su un semaforo
    for (int i = 0; i < MAXPROC; i++)
    {
        semd_t *semd = &semd_table[i];
        if (!emptyProcQ(&semd->s_procq))
        {
            list_for_each(pos, &semd->s_procq)
            {
                pcb_t *p = list_entry(pos, pcb_t, p_list);
                if (p->p_pid == pid)
                {
                    return p;
                }
            }
        }
    }

    // non esiste
    return NULL;
}

/* Funzione ausiliaria per sapere se un proc è bloccato su un semaforo di un device o no*/
int notDevice(int *semaddr)
{
    for (int i = 0; i < 8; i++)
    {
        if (&sem_dev_disk[i] == semaddr)
            return 0;
        else if (&sem_dev_flash[i] == semaddr)
            return 0;
        else if (&sem_dev_net[i] == semaddr)
            return 0;
        else if (&sem_dev_printer[i] == semaddr)
            return 0;
        else if (&sem_dev_terminal_r[i] == semaddr)
            return 0;
        else if (&sem_dev_terminal_w[i] == semaddr)
            return 0;
    }
    return 1;
}

/* Aggiorna il tempo usato dal proc corrente */
void update_time()
{
    cpu_t time;
    STCK(time);
    active_process->p_time += (time - timer_start);
}

/* Aggiorna il tempo usato da un proc */
void update_time_proc(pcb_t *proc)
{
    cpu_t time;
    STCK(time);
    proc->p_time += (time - exc_timer_start);
}

/* Diminuisce il tempo usato da un proc */
void remove_time()
{
    cpu_t time;
    STCK(time);
    active_process->p_time -= (time - exc_timer_start);
}

/* Terminazione non bloccante di una syscall */
void NonBlockingExceptEnd()
{
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->pc_epc += 4;

    LDST(state);
}

/* Terminazione bloccante di una syscall */
void BlockingExceptEnd()
{
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->pc_epc += 4;
    active_process->p_s = *state;

    // aggiornamento del tempo di uso della CPU
    update_time();

    scheduler();
}
