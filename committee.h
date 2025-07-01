#pragma once

#include <stdbool.h>

typedef struct {
    char* name;
    int votes;
    int mandates;

    bool exempt;
    bool alliance;
} Committee;

Committee* create_committee(const char* name);
void delete_committee(Committee* target);
int committee_comparator(const void* q1, const void* q2);