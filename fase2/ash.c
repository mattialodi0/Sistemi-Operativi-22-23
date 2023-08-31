#include "include/ash.h"

semd_t semd_table[MAXPROC];

static struct list_head *semdFree_h;

DEFINE_HASHTABLE(semd_h, 8);

int insertBlocked(int *semAdd, pcb_t *p)
{
    int key = hash_min((int)semAdd, HASH_BITS(semd_h));
    int empty = hlist_empty(&semd_h[key]);
    // se il SEMD esiste nella ash
    if (!empty)
    {
        semd_t *t = container_of(semd_h[key].first, semd_t, s_link);
        p->p_semAdd = semAdd;
        insertProcQ(&(t->s_procq), p);
        return false;
    }
    // se il SEMD non è presente ma se ne può aggiungere uno libero
    else if (semdFree_h != NULL)
    {
        // lo prende
        struct semd_t *t = container_of(semdFree_h, semd_t, s_freelink);
        semdFree_h = semdFree_h->next;
        // lo setta
        t->s_freelink.next = NULL;
        t->s_key = semAdd;
        mkEmptyProcQ(&(t->s_procq));
        p->p_semAdd = semAdd;
        insertProcQ(&(t->s_procq), p);
        // lo aggiunge
        hlist_add_head(&t->s_link, &semd_h[key]);
        return false;
    }
    // se non ci sono SEMD liberi
    else
        return true;
}

pcb_t *removeBlocked(int *semAdd)
{
    semd_t *t;
    // cerca il SEMD con chiave semAdd
    hash_for_each_possible(semd_h, t, s_link, (int)semAdd)
    {
        // verifica che esista
        if (t != NULL)
        {
            pcb_t *p = removeProcQ(&(t->s_procq));
            p->p_semAdd = NULL;
            // se il SEMD ora è vuoto lo rimuove e lo riaggiunge alla lista dei liberi
            if (emptyProcQ(&(t->s_procq)))
            {
                hash_del(&(t->s_link));
                t->s_freelink.next = semdFree_h;
                // semdFree_h->prev = &t->s_freelink;
                semdFree_h = &t->s_freelink;
            }
            return p;
        }
    }
    // se non esiste:
    return NULL;
}

pcb_t *outBlocked(pcb_t *p)
{
    semd_t *t;
    // cerca il SEMD puntato da p
    hash_for_each_possible(semd_h, t, s_link, (int)p->p_semAdd)
    {
        pcb_t *out = outProcQ(&t->s_procq, p);
        out->p_semAdd = NULL;
        // se il SEMD ora è vuoto lo rimuove e lo riaggiunge alla lista dei liberi
        if (emptyProcQ(&(t->s_procq)))
        {
            hash_del(&(t->s_link));
            t->s_freelink.next = semdFree_h;
            // semdFree_h->prev = &t->s_freelink;
            semdFree_h = &t->s_freelink;
        }
        return out;
    }
    // se non esiste:
    return NULL;
}

pcb_t *headBlocked(int *semAdd)
{
    semd_t *t;
    // cerca il SEMD con chiave semAdd
    hash_for_each_possible(semd_h, t, s_link, (int)semAdd)
    {
        return headProcQ(&t->s_procq);
    }
    // se non esiste:
    return NULL;
}

void initASH()
{
    semdFree_h = &semd_table[0].s_freelink;
    struct list_head *t = semdFree_h;
    t->next = NULL;
    t->prev = NULL;
    for (int i = 1; i < MAXPROC; i++)
    {
        t->next = &semd_table[i].s_freelink;
        t = t->next;
        t->next = NULL;
    }
}
