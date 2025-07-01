#pragma once

typedef struct {
    int committee_index; 
    int votes;
    int value;
} Quotient;


int quotient_comparator(const void* q1, const void* q2);