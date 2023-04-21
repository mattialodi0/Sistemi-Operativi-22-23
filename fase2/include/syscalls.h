#include <lib_def.h>
#include <scheduler.h>

extern struct list_head *ready_queue; 
extern pcb_t *active_process;   
extern int process_count;
extern int soft_blocked_count;
extern int IT_sem;


// crea un nuovo processo come figlio del chiamante
int CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns);

// termina il processo indicato dal pid insieme ai figli
void TerminateProcess(int pid);

//decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr);

//incrementa il semaforo all'ind semaddr, se diventa > 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr);

// effettua un'operazione di I/O
int DOIO(int *cmdAddr, int *cmdValues);

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

