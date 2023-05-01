#ifndef PANDOS_TYPES_H_INCLUDED
#define PANDOS_TYPES_H_INCLUDED

/****************************************************************************
 *
 * This header file contains utility types definitions.
 *
 ****************************************************************************/

#include <umps3/umps/types.h>
#include "pandos_const.h"
#include <list.h>

/* PID namespace */
#define NS_PID 0
#define NS_TYPE_LAST NS_PID
#define NS_TYPE_MAX (NS_TYPE_LAST + 1)

typedef signed int cpu_t;
typedef unsigned int memaddr;

/* Page Table Entry descriptor */
typedef struct pteEntry_t
{
    unsigned int pte_entryHI;
    unsigned int pte_entryLO;
} pteEntry_t;

/* process context */
typedef struct context_t
{
    /* process context fields */
    unsigned int stackPtr, /* stack pointer value */
        status,            /* status reg value */
        pc;                /* PC address */
} context_t;

typedef struct support_t
{
    int sup_asid;                   /* Process Id (asid) */
    state_t sup_exceptState[2];     /* stored excpt states */
    context_t sup_exceptContext[2]; /* pass up contexts */
    pteEntry_t sup_privatePgTbl[USERPGTBLSIZE]; /* user page table */
} support_t;

/* Exceptions related constants */
#define PGFAULTEXCEPT 0
#define GENERALEXCEPT 1

typedef struct nsd_t
{
    /* Namespace type */
    int n_type;

    /* Namespace list */
    struct list_head n_link;
} nsd_t, *nsd_PTR;

/* process table entry type */
typedef struct pcb_t
{
    /* process queue  */
    struct list_head p_list;

    /* process tree fields */
    struct pcb_t *p_parent;   /* ptr to parent	*/
    struct list_head p_child; /* children list */
    struct list_head p_sib;   /* sibling list  */

    /* process status information */
    state_t p_s;  /* processor state */
    cpu_t p_time; /* cpu time used by proc */

    /* Pointer to the semaphore the process is currently blocked on */
    int *p_semAdd;

    /* Pointer to the support struct */
    support_t *p_supportStruct;

    /* process id */
    int p_pid;

    /* Namespace list */
    nsd_t *namespaces[NS_TYPE_MAX];
} pcb_t, *pcb_PTR;

/* semaphore descriptor (SEMD) data structure */
typedef struct semd_t
{
    /* Semaphore key */
    int *s_key;
    /* Queue of PCBs blocked on the semaphore */
    struct list_head s_procq;

    /* Semaphore hash table */
    struct hlist_node s_link;
    /* Free Semaphores list */
    struct list_head s_freelink;
} semd_t, *semd_PTR;

#endif
