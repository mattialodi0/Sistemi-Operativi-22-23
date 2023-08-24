#include <lib_def.h>
#include <scheduler.h>

extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int process_count;
extern int soft_blocked_count;
extern int IT_sem;
extern int sem_dev_disk[8];
extern int sem_dev_flash[8];
extern int sem_dev_net[8];
extern int sem_dev_printer[8];
extern int sem_dev_terminal_r[8];
extern int sem_dev_terminal_w[8];


// crea un nuovo processo come figlio del chiamante
int CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns);

// termina il processo indicato dal pid insieme ai figli
void TerminateProcess(int pid);

//decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr);

//incrementa il semaforo all'ind semaddr, se diventa > 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr);

// effettua un'operazione di I/O
int DoIO(unsigned *cmdAddr, unsigned int *cmdValues);

// restituisce il tempo di esecuzione del processo che l'ha chiamata fino a quel momento
int GetCPUTime();

// blocca il processo invocante fino al prossimo tick del dispositivo
int WaitForClock();

// ritorna un puntatore alla struttura di supporto del chiamante
support_t* GetSupportData();

// ritorna il pid del chiamante
int GetProcessId(int parent);

/* cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
*  e ritorna il numero di figli del processo */
int GetChildren(int *children, int size);



//confronta i namespaces, un campo alla volta, da implementare
bool eqNS(nsd_t *a[], nsd_t *b[]);

// trova un processo bloccato in un semaforo
pcb_PTR findProcess(int pid);

// terminazione di un'eccezione non bloccante
void NonBlockingExceptEnd();
// terminazione di un'eccezione bloccante
void BlockingExceptEnd();

// aggiornamento del tempo di uso della CPU
void update_time();
void update_time_exc(pcb_t * proc);
// elimina il tempo usato nella gestione dell'eccezione da quello del proc
void remove_time();