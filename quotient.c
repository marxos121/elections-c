#include "quotient.h"

int quotient_comparator(const void *q1, const void *q2) {
    const Quotient val1 = *(Quotient*) q1;
    const Quotient val2 = *(Quotient*) q2;

    if (val1.value > val2.value)
        return -1;
    if (val2.value > val1.value)
        return 1;

    if (val1.votes > val2.votes)
        return -1;
    if (val2.votes > val1.votes)
        return 1;

    
    return 0;
}
