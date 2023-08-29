#include <syscalls.h>

extern int debug_var;
extern int debug_var1;
extern int debug_char;

void CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns)
{
    pcb_t *new_proc = allocPcb(); // inizializza new_proc, allocPcb la mette == NULL se vuota, quindi va direttamente nell'else?
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
        new_proc->p_semAdd = NULL;
        new_proc->p_time = 0; // inizializzo il tempo a 0

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
        state->reg_v0 = 0; // non ha senso ma è richiesto
        state->reg_v0 = new_proc->p_pid;
    }
    else
    {
        // return (-1);
        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = -1;
    }
    // debug_var = new_proc->p_pid; debug4();
    NonBlockingExceptEnd();
}

// termina il processo indicato da pid insieme ai suoi figli
void TerminateProcess(int pid)
{
    pcb_t *proc, *f_proc;

    // if (pid == 0)
    //     debug_var = active_process->p_pid;
    // else
    //     debug_char = pid;
    // debug1();

    if (pid == 0) // pid == 0, bisogna terminare active_process (processo invocante), e i suoi figli
    {
        f_proc = active_process;
    }
    else // stessa cosa dell'if ma con il processo del pid preso in input
    {
        f_proc = findProcess(pid);
    }

    // terminazione ricorsiva
    kill(f_proc);

    update_time();

    scheduler();
}

// decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
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

// incrementa il semaforo all'ind semaddr, se diventa >= 0 il processo viene messo nella coda ready
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
 * Esegue un operazione di I/O sul dispositivo indicato, in modo sincrono
 * cmdAddr contiene l'ind del comando e cmdValues è un array di puntatori ai valori
 * del comando
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
        if (cmdAddr == (0x10000254 + 0x10 * no))
        {
            semaddr = &sem_dev_terminal_r[no];
        }
        else if (cmdAddr == (0x10000254 + 0x10 * no) + 0x8)
        {
            semaddr = &sem_dev_terminal_w[no];
        }
        else
            debugE();
        break;
    default:
        debugE();
        break;
    }

    // verifica che il device sia installato
    switch (no)
    {
    case 0:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV0ON == 0)
            PANIC();
        break;
    case 1:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV1ON == 0)
            PANIC();
        break;
    case 2:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV2ON == 0)
            PANIC();
        break;
    case 3:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV3ON == 0)
            PANIC();
        break;
    case 4:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV4ON == 0)
            PANIC();
        break;
    case 5:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV5ON == 0)
            PANIC();
        break;
    case 6:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV6ON == 0)
            PANIC();
        break;
    case 7:
        if(*(unsigned int*)(0x1000002C + (0x4 * line)) & DEV7ON == 0)
            PANIC();
        break;
    default:
        break;
    }
    
    // inserimento degli operandi nel devce register
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

void GetCPUTime()
{
    cpu_t time;
    STCK(time);

    // return active_process->p_time + (time - timer_start);
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = (int)(active_process->p_time + (time - timer_start));

    NonBlockingExceptEnd();
}

void WaitForClock()
{
    // Passeren(&IT_sem);
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

// ritorna un puntatore alla struttura di supporto del chiamante
void GetSupportData()
{
    // return active_process->p_supportStruct;
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = (int)active_process->p_supportStruct;

    NonBlockingExceptEnd();
}

// ritorna il pid del chiamante
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

// cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
// e ritorna il numero di figli del processo
void GetChildren(int *children, int size)
{
    struct list_head *pos;
    pcb_t *p;
    int n = 0;

    list_for_each(pos, &active_process->p_child)
    {
        pcb_t *p = list_entry(pos, pcb_t, p_sib);
        if (eqNS(p, active_process))
        {
            if (n < size)
                children[n] = p->p_pid;
            n++;
        }
    }

    // pcb_t *arr[MAXPROC];            // altro modo
    // int i = 0;
    // for(i=0; i<MAXPROC; i++) {
    //     arr[i] = removeChild(active_process);
    //     if(arr[i] == NULL) {
    //         i--;
    //         break;
    //     }
    // }
    // for(int j=0; j<i; j++) {
    //     if(eqNS(arr[j], active_process)) {
    //         if(n < size)
    //             children[n] = arr[j]->p_pid;
    //         n++;
    //     }
    // }
    // for(int j=0; j<i; j++) {
    //     insertChild(active_process, arr[j]);
    // }

    state_t *state = (state_t *)BIOSDATAPAGE;
    state->reg_v0 = n;

    NonBlockingExceptEnd();
}

// confronta i namespaces, un campo alla volta, da implementare
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

void kill(pcb_t *f_proc)
{
    outChild(f_proc);
    if (*(f_proc->p_semAdd) == 0 && headBlocked(f_proc->p_semAdd) != NULL) // se *semaddr < 0 deve essere incrementato
    {
        if (notDevice(f_proc->p_semAdd)) // !!!!! SOLO SE NON è DI UN DEVICE !!!!!
        {
            outBlocked(f_proc);
            // (*f_proc->p_semAdd)++;
            soft_blocked_count--;
        }
    }
    if (*(f_proc->p_semAdd) == 1 && headBlocked(f_proc->p_semAdd) != NULL) // se *semaddr  > 1 deve essere incrementato
    {
        if (notDevice(f_proc->p_semAdd)) // !!!!! SOLO SE NON è DI UN DEVICE !!!!!
        {
            outBlocked(f_proc);
            // (*f_proc->p_semAdd)--;
            soft_blocked_count--;
        }
    }

    // chiamata ricorsiva su tutti i figli
    pcb_t *proc = removeChild(f_proc);
    while (proc != NULL)
    {
        kill(proc);
        proc = removeChild(f_proc);
    }

    freePcb(f_proc);
    process_count--;
}

pcb_PTR findProcess(int pid)
{
    pcb_t *p, *first;

    // è il proc attivo
    if (active_process->p_pid == pid)
        return active_process;

    // è nella ready queue
    p = first = removeProcQ(&ready_queue);
    insertProcQ(&ready_queue, p);
    if (p->p_pid == pid)
        return p;
    while ((p = removeProcQ(&ready_queue)) != NULL)
    {
        insertProcQ(&ready_queue, p);
        if (p->p_pid == pid)
            return p;
        else if (p == first)
            break;
    }

    // è in un semaforo
    // è un problema
    // ????????????????????????????????????????????????????????????????????????????????????????????????????????

    if (p->p_semAdd != NULL)
    {
        /*Da capire la gestione e se p va bene da usare*/
        outBlocked(p);
    }

    // non esiste
    return NULL;
}

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

void NonBlockingExceptEnd()
{
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->pc_epc += 4;

    LDST(state);
}

void BlockingExceptEnd()
{
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->pc_epc += 4;
    active_process->p_s = *state;

    // aggiornamento del tempo di uso della CPU
    update_time();

    scheduler();
}
