#include <syscalls.h>

extern int debug_var;
extern int debug_var1;
extern int debug_char;

int CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns)
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
void Passeren(int *semaddr, int i)
{

    if (*semaddr == 0)      // blocca il proc
    {
    if(i) {debug_var = *semaddr; debug_var1 = 0x0; debug();}
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
        if (waked_proc != NULL)     // sblocca un proc
        {
    if(i) {debug_var = *semaddr; debug_var1 = 0x0; debug1();}
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
        }
        else                // decrementa il valore del semaforo
        {
            (*semaddr)--;
    if(i) {debug_var = *semaddr; debug_var1 = 0x0; debug2();}
        }
        NonBlockingExceptEnd();
    }
}

// incrementa il semaforo all'ind semaddr, se diventa >= 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr, int i)
{

    if (*semaddr == 1)      // blocca il proc
    {
    if(i) {debug_var = *semaddr; debug_var1 = 0xFFFFFFFF; debug3();}
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
        if (waked_proc != NULL)     // sblocca un proc
        {
    if(i) {debug_var = *semaddr; debug_var1 = 0xFFFFFFFF; debug4();}
            insertProcQ(&ready_queue, waked_proc);
            soft_blocked_count--;
        }
        else                // incrementa il valore del semaforo
        {
            (*semaddr)++;
    if(i) {debug_var = *semaddr; debug_var1 = 0xFFFFFFFF; debug5();}
        }
        NonBlockingExceptEnd();
    }
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

    // (verifica che il disp sia installato)
    // P sul sem indicato in a1 e a2
    // ...
    // ritorna 0 o -1

    /* solo per print: */
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
    state->reg_v0 = (int)(active_process->p_time + (time - timer_start));

    NonBlockingExceptEnd();
}

int WaitForClock()
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
    if (parent)
    {
        if (eqNS(active_process->namespaces, active_process->p_parent->namespaces))
        {
            state_t *state = (state_t *)BIOSDATAPAGE;
            state->reg_v0 = active_process->p_parent->p_pid;
        }
        else {
            state_t *state = (state_t *)BIOSDATAPAGE;
            state->reg_v0 = 0;
        }
    }
    else {
        state_t *state = (state_t *)BIOSDATAPAGE;
        state->reg_v0 = active_process->p_pid;
    }

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

    // return n;
    NonBlockingExceptEnd();
}
// qualcosa non torna

// confronta i namespaces, un campo alla volta, da implementare
bool eqNS(nsd_t *a[], nsd_t *b[])
{
    // bool res = true;
    // for (int i = 0; i < MAXPROC; i++)
    // {
    //     if (a[i] != b[i])
    //         res = false;
    // }

    // return res;
    return 1;
}

void kill(pcb_t *f_proc)
{
    outChild(f_proc);
    if (f_proc->p_semAdd < 0) // if semaphor < 0 deve essere incrementato (controllare su 3.9 del libro)
    {
        if (headBlocked(f_proc->p_semAdd) == NULL && notDevice(f_proc->p_semAdd)) // !!!!! SOLO SE NON è DI UN DEVICE !!!!!
        {
            f_proc->p_semAdd++;
            soft_blocked_count--;
        }
    }
    process_count--;

    pcb_t *proc = removeChild(f_proc);
    // chiamata ricorsiva su tutti i figli
    while (proc != NULL)
    {
        kill(proc);
        proc = removeChild(f_proc);
    }
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

    // non esiste
    return NULL;
}

int notDevice(int *semaddr)
{
    for (int i = 0; i < 8; i++)
    {
        if (&sem_dev_disk[i] == semaddr)
            return 1;
        else if (&sem_dev_flash[i] == semaddr)
            return 1;
        else if (&sem_dev_net[i] == semaddr)
            return 1;
        else if (&sem_dev_printer[i] == semaddr)
            return 1;
        else if (&sem_dev_terminal_r[i] == semaddr)
            return 1;
        else if (&sem_dev_terminal_w[i] == semaddr)
            return 1;
    }
    return 0;
}

void NonBlockingExceptEnd() {
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->pc_epc += 4;

    LDST(state);
}

void BlockingExceptEnd() {
    state_t *state = (state_t *)BIOSDATAPAGE;
    state->pc_epc += 4;
    active_process->p_s = *state;

    // aggiornamento del tempo di uso della CPU
    update_time();

    scheduler();
}