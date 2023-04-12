#ifndef NS_H
#define NS_H

#include <pandos_types.h>
#include <pandos_const.h>

void initNamespaces();
nsd_t *getNamespace(pcb_t *p, int type);
int addNamespace(pcb_t *p, nsd_t *ns);
nsd_t *allocNamespace(int type);
void freeNamespace(nsd_t *ns);

#endif
