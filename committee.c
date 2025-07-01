#include "committee.h"

#include <stdlib.h>
#include <string.h>

Committee* create_committee (const char* name) {
    Committee* ret = malloc(sizeof(Committee));
    
    if (!ret) return NULL;

    ret->name = strdup(name);
    if (!ret->name) {
        free(ret);
        return NULL;
    }

    ret->votes = ret->mandates = 0;
    ret->exempt = ret->alliance = false;
    return ret;
}

void delete_committee (Committee* target) {
    if (!target) return;
    free(target->name);
    free(target);
}

int committee_comparator(const void *q1, const void *q2) {
    const Committee* com1 = *(const Committee**) q1;
    const Committee* com2 = *(const Committee**) q2;

    if (com1->mandates > com2->mandates) 
        return -1;
    
    return 1;
}