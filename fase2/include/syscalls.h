#include <lib_def.h>
#include <scheduler.h>

extern struct list_head ready_queue; 
extern pcb_t *active_process;   
extern int process_count;
extern int soft_blocked_count;
extern int IT_sem;
extern cpu_t timer_start;
extern int pid_count;
extern int sem_dev_disk[8];
extern int sem_dev_flash[8];
extern int sem_dev_net[8];
extern int sem_dev_printer[8];
extern int sem_dev_terminal_r[8];
extern int sem_dev_terminal_w[8];
extern cpu_t exc_timer_start;


#define DISK 0x10000054 ... 0x100000D3
#define FLASH 0x100000D4 ... 0x10000153
#define NETWORK 0x10000154 ... 0x100001D3
#define PRINTER 0x100001D4 ... 0x10000253
#define TERM 0x10000254 ... 0x100002D3


// crea un nuovo processo come figlio del chiamante
void CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns);

// termina il processo indicato dal pid insieme ai figli
void TerminateProcess(int pid);

//decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler
void Passeren(int *semaddr);

//incrementa il semaforo all'ind semaddr, se diventa > 0 il processo viene messo nella coda ready
void Verhogen(int *semaddr);

// effettua un'operazione di I/O
void DoIO(int cmdAddr, unsigned int *cmdValues);

// restituisce il tempo di esecuzione del processo che l'ha chiamata fino a quel momento
void GetCPUTime();

// blocca il processo invocante fino al prossimo tick del dispositivo
void WaitForClock();

// ritorna un puntatore alla struttura di supporto del chiamante
void GetSupportData();

// ritorna il pid del chiamante
void GetProcessId(int parent);

/* cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
*  e ritorna il numero di figli del processo */
void GetChildren(int *children, int size);


//confronta i namespaces, un campo alla volta
bool eqNS(pcb_t *a, pcb_t*b);

// elimina un proc
int kill(pcb_t *f_proc);

// ritorna un ptr al processo di cui si ha il pid
pcb_PTR findProcess(int pid);

// ritorna 1 se il proc non è bloccato su un semaforo di un device
int notDevice(int *semaddr);

// aggiornamento del tempo di uso della CPU
void update_time();
void update_time_proc(pcb_t * proc);
// elimina il tempo usato nella gestione dell'eccezione da quello del proc
void remove_time();

void NonBlockingExceptEnd();

void BlockingExceptEnd();