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

extern semd_t semd_table[MAXPROC];


#define DISK 0x10000054 ... 0x100000D3
#define FLASH 0x100000D4 ... 0x10000153
#define NETWORK 0x10000154 ... 0x100001D3
#define PRINTER 0x100001D4 ... 0x10000253
#define TERM 0x10000254 ... 0x100002D3

/* Crea un nuovo processo come figlio del chiamante */
void CreateProcess(state_t *statep, support_t *supportp, nsd_t *ns);

/* Termina il processo indicato da pid insieme ai suoi figli */
void TerminateProcess(int pid);

/* Decrementa il semaforo all'ind semaddr, se diventa < 0 il processo viene bloccato e si chiama lo scheduler */
void Passeren(int *semaddr);

/* Incrementa il semaforo all'ind semaddr, se diventa >= 0 il processo viene messo nella coda ready */
void Verhogen(int *semaddr);

/* 
 * Esegue un operazione di I/O sul dispositivo indicato, in modo sincrono cmdAddr contiene l'ind del 
 * comando e cmdValues è un array di puntatori ai valori del comando
 */
void DoIO(int cmdAddr, unsigned int *cmdValues);

/* Ritorna il tempo di uso della CPU del proc corrente */
void GetCPUTime();

/* Blocca il proc corrente sul semaforo dello pseudo-clock con una P */
void WaitForClock();

/* Ritorna un puntatore alla struttura di supporto del chiamante */
void GetSupportData();

/* Ritorna il pid del chiamante o quello del padre */
void GetProcessId(int parent);

/* 
 * cerca i primi size figli con lo stesso NS del chiamante e li ritorna nell'array children
 *  e ritorna il numero di figli del processo 
 */
void GetChildren(int *children, int size);


/* Confronta i namespaces, un campo alla volta, da implementare */
bool eqNS(pcb_t *a, pcb_t*b);

/* Termina un proc e la sua progenie */
int kill(pcb_t *f_proc);

/* Restituisce un proc dato il pid */
pcb_PTR findProcess(int pid);

/* Funzione ausiliaria per sapere se un proc è bloccato su un semaforo di un device o no*/
int notDevice(int *semaddr);

/* aggoirna il tempo usato dal proc corrente */
void update_time();

/* aggoirna il tempo usato da un proc */
void update_time_proc(pcb_t * proc);

/* diminuisce il tempo usato da un proc */
void remove_time();

/* Terminazione non bloccante di una syscall */
void NonBlockingExceptEnd();

/* Terminazione bloccante di una syscall */
void BlockingExceptEnd();