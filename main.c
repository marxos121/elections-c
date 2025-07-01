#include "committee.h"
#include "district.h"
#include "elections.h"

#include <stdio.h>
#include <stdlib.h>

bool sainte_lague = false;
int total_votes = 0;

Committee** committees = NULL;
int committees_count = 0;

District** districts = NULL;
int districts_count = 0;




int main (int argc, char* argv[]) {
   if (read_input() == 1) {
        free_resources();
        return 1;
    }

   if(parse_arguments(argc, (const char**) argv) == 1) {
        free_resources();
        return 1;
   }

    if (process_votes() == 1) {
        free_resources();
        return 1;
    }

   qsort(committees, committees_count, sizeof(Committee*), committee_comparator);

   for (int i = 0; i < committees_count; ++i) {
        if (committees[i]->mandates > 0)
            printf("%s: %d\n", committees[i]->name, committees[i]->mandates);
   } 

   free_resources();
   return 0;
}