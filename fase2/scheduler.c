#include "scheduler.h"

// scelta pcb
// rimozione pcb dalla testa e tornare il puntatore al pcb nel campo dei processi correnti
// caricare 5 millisecondi come time slice
// creare un Load Processor State sullo stato del processore memorizzato nel pcb del processo corrente
// cambio stato (?)
// se il process count è 0 richiama l'HALT BIOS
// se il process count è > 0 e il Soft-Block Count > 0 va in Wait State
// controllo se process count > 0 e Soft-Block Count == 0 => deadlock. ???. Richiamare PANIC BIOS.
extern int debug_var;

void scheduler()
{
    if (!emptyProcQ(&ready_queue))
    {
        // remove proc from ready queue
        active_process = removeProcQ(&ready_queue);
        // load 5 ms in PLT
        setTIMER(5);
        // load state
        debug_var = LDST((STATE_PTR)(&(active_process->p_s))); // causa un B5 che è sbagliato
        // LDST(&active_process->p_s);  //causa un B5 che è sbagliato
    }
    else
    {
        if (process_count == 0)
        {
            HALT();
        }
        else if (process_count > 0)
        {
            if (soft_blocked_count > 0)
            {
                unsigned int mask = 0;
                unsigned int status = getSTATUS();
                mask = ~TEBITON;
                status &= mask;         // disabilita il PLT
                status |= ~DISABLEINTS; // abilita gli interrupt
                // abilita gli interrupt, disabilita PLT
                LDCXT(active_process->p_s.reg_sp, status, active_process->p_s.pc_epc); // il valore che ci interessa settare è il secondo
                WAIT();
            }
            else if (soft_blocked_count == 0)
            {
                // deadlock detection
                PANIC();
            }
        }
    }
}

// questa è una magia, non preoccupatevi
#include <stddef.h>
void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char *)dest)[i] = ((char *)src)[i];
    }
}
