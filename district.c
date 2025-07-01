#include "district.h"

#include <stdlib.h>
#include <string.h>

District* create_district (const char* name, const int mandates, const int committees) {
    District* ret = malloc(sizeof(District));
    
    if (!ret) return NULL;
    ret->name = strdup(name);

    if (!ret->name) {
        free(ret);
        return NULL;
    }

    ret->votes = malloc(sizeof(int) * committees);
    if (!ret->votes) {
        free(ret);
        return NULL;
    }

    ret->mandates = mandates;

    return ret;
}

void delete_district(District* target) {
    if (!target) return;

    free(target->votes);
    free(target->name);
    free(target);
}
