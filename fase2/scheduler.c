#include "scheduler.h"


void scheduler()
{  
    on_wait = false;
    if (!emptyProcQ(&ready_queue))
    {
        active_process = removeProcQ(&ready_queue);
        // carica il PLT con 5 ms
        setTIMER(5000);
        // salva il valore corrente del timer
        STCK(timer_start);

        LDST((STATE_PTR)(&(active_process->p_s))); 
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
                on_wait = true;
                unsigned int mask = 0;
                unsigned int status = getSTATUS();
                mask = ~TEBITON;
                status &= mask;         // disabilita il PLT
                status |= ~DISABLEINTS; // abilita gli interrupt
                LDCXT(active_process->p_s.reg_sp, status, active_process->p_s.pc_epc);

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


#include <stddef.h>
void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char *)dest)[i] = ((char *)src)[i];
    }
}
