#include <lib_def.h>
#include <exceptions.h>

extern void test();
extern void uTLB_RefillHandler();
extern void exceptionHandler();
extern void scheduler();

    int process_count = 0;
    int soft_blocked_count = 0;
    struct list_head ready_queue;
    pcb_t *current_proc = NULL;
    int sem_dev_disk[8];
    int sem_dev_flash[8];
    int sem_dev_net[8];
    int sem_dev_printer[8];
    int sem_dev_terminal_r[8];
    int sem_dev_terminal_w[8];
    int IT_sem; 

int main() {

    passupvector_t* puv = (passupvector_t*) PASSUPVECTOR;
    puv->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    puv->tlb_refill_stackPtr = 0x20001000;
    puv->exception_handler = (memaddr) exceptionHandler;
    puv->exception_stackPtr = 0x20001000;

    initPcbs();
    initASH();
    initNamespaces();

    mkEmptyProcQ(&ready_queue);
    for(int i=0; i<8; i++) {
        sem_dev_disk[i] = 0;
        sem_dev_flash[i] = 0;
        sem_dev_net[i] = 0;
        sem_dev_printer[i] = 0;
        sem_dev_terminal_r[i] = 0;
        sem_dev_terminal_w[i] = 0;
    }

    LDIT(100000);
    pcb_t *first_proc = allocPcb();
    process_count++;
    insertProcQ(&ready_queue, first_proc);

    unsigned int s = 0;
    s |= IEPON; s |= IECON; s |= TEBITON; s |= IMON; 
    first_proc->p_s.status = s;
    RAMTOP(first_proc->p_s.reg_sp);
    first_proc->p_s.pc_epc = (memaddr) test;
    first_proc->p_s.reg_t9 = (memaddr) test;

    first_proc->p_parent = NULL;
    INIT_LIST_HEAD(&first_proc->p_child);
    INIT_LIST_HEAD(&first_proc->p_sib);
    first_proc->p_time = 0;
    first_proc->p_semAdd = NULL;
    first_proc->p_supportStruct = NULL;
    first_proc->p_pid = 1;

    scheduler();
}
