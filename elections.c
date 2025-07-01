#include "elections.h"
#include "committee.h"
#include "district.h"
#include "quotient.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern bool sainte_lague;
extern int total_votes;

extern Committee** committees;
extern int committees_count;

extern District** districts;
extern int districts_count;

void free_resources() {
    if (committees != NULL) {
        for (int i = 0; i < committees_count; ++i) {
            delete_committee(committees[i]);
            committees[i] = NULL;
        }

        free(committees);
    }

    if (districts != NULL) {
        for (int i = 0; i < districts_count; ++i) {
            delete_district(districts[i]);
            districts[i] = NULL;
        }

        free(districts);
    }
}

int process_votes() {
    for (int i = 0; i < districts_count; ++i) {
        const int quotients_in_district = districts[i]->mandates * committees_count;
        Quotient* quotients = malloc(sizeof(Quotient) * quotients_in_district);

        if (!quotients) {
            fprintf(stderr, "BLAD! Blad alokacji pamieci!");
            return 1;
        }

        // Tworzymy ilorazy dla wszystkich partii w okręgu
        for (int j = 0; j < committees_count; ++j) {
            for (int k = 1; k <= districts[i]->mandates; ++k) {
                int divisor = sainte_lague ? 2 * k - 1 : k;
                quotients[j * districts[i]->mandates + k - 1].committee_index = j;
                quotients[j * districts[i]->mandates + k - 1].value = (double)(districts[i]->votes[j]) / divisor;
                quotients[j * districts[i]->mandates + k - 1].votes = districts[i]->votes[j];
            }
        }

        qsort(quotients, quotients_in_district, sizeof(Quotient), quotient_comparator);

        int counter = 0;
        int current_quotient = 0;
        while (counter < districts[i]->mandates) {
            const int current_committee = quotients[current_quotient].committee_index;
            const double threshold 
                = committees[current_committee]->alliance ? 8.0 : 5.0;

            if (!committees[current_committee]->exempt && 
                ((double)committees[current_committee]->votes / total_votes) * 100.0 < threshold) {
                current_quotient++;
                continue;
            }

            committees[current_committee]->mandates++;
            counter++;
            current_quotient++;
        }
    }

    return 0;
}

int read_input() {
    char line[200];

    if (fgets(line, sizeof(line), stdin) == NULL) {
        fprintf(stderr, "BLAD! Pusty plik wejsciowy!\n");
        return 1;
    }

    // ========================================================
    // ============      CZYTANIE NAGŁÓWKA       ==============
    // ========================================================
    char* token = strtok(line, ";");
    if (strcmp(token, "okreg") != 0) {
        fprintf(stderr, "BLAD! Nieprawidlowe formatowanie naglowka!\n");
        return 1;
    }

    token = strtok(NULL, ";");
    if (strcmp(token, "mandatow") != 0) {
        fprintf(stderr, "BLAD! Nieprawidlowe formatowanie naglowka!\n");
        return 1;
    }

    // Wczytujemy kolejne komitety
    while ((token = strtok(NULL, ";\n")) != NULL) {
        for (int i = 0; i < committees_count; ++i) {
            if (strcmp(token, committees[i]->name) == 0) {     
                fprintf(stderr, "BLAD! Powtorzony komitet!\n");
                return 1;
            }
        }
        
        Committee** temp = realloc(committees, sizeof(Committee*) * (committees_count + 1));
        if (!temp) {
            fprintf(stderr, "BLAD! Niepowodzenie alokacji pamieci!\n");
            return 1;
        }

        committees = temp;
        committees[committees_count] = create_committee(token);

        if (!committees[committees_count]) {
            fprintf(stderr, "BLAD! Nie udalo sie stworzyc komitetu!\n");
            return 1;
        }

        committees_count++;
    }

    // ========================================================
    // ============       CZYTANIE OKRĘGÓW       ==============
    // ========================================================
    while (fgets(line, sizeof(line), stdin) != NULL) {
        int line_index = 0;

        char curr_str[200];
        while (line[line_index] != ';') {
            curr_str[line_index] = line[line_index];
            line_index++;
        }
        curr_str[line_index] = '\0';
        

        District** temp = realloc(districts, sizeof(District*) * (districts_count+1));
        if (!temp) {
            fprintf(stderr, "BLAD! Niepowodzenie alokacji pamieci!\n");
            return 1;
        }

        districts = temp;
        districts[districts_count] = create_district(curr_str, 0, committees_count);
        if (!districts[districts_count]) {
            fprintf(stderr, "BLAD! Niepowodzenie alokacji pamieci!\n");
            return 1;
        }

        // Czytamy liczbę mandatów
        line_index++;
        int curr_str_index = 0;
        while (line[line_index] != ';') {
            curr_str[curr_str_index] = line[line_index];
            line_index++;
            curr_str_index++;
        }
        curr_str[curr_str_index] = '\0';

        int mandates = atol(curr_str);
        if (mandates == 0) {
            fprintf(stderr, "BLAD! Nieprawidlowy format liczby mandatow!\n");
            return 1;
        }

        districts[districts_count]->mandates = mandates;

        // Czytanie liczby głosów na kolejne komitety
        int current_committee = 0;
        while ((size_t)line_index < strlen(line) - 1) {
            line_index++;
            curr_str_index = 0;
            while (line[line_index] != ';' && line[line_index] != '\0') {
                curr_str[curr_str_index] = line[line_index];
                line_index++;
                curr_str_index++;
            }
            curr_str[curr_str_index] = '\0';

            int votes = (int)strlen(curr_str) == 0 ? 0 : atoi(curr_str);
            committees[current_committee]->votes += votes;
            districts[districts_count]->votes[current_committee] = votes;
            total_votes += votes;

            current_committee++;
            if (current_committee >= committees_count && (size_t)line_index != strlen(line)) {
                fprintf(stderr, "BLAD! Wiersz zawiera zbyt wiele komitetow!\n");
                return 1;
            }
        }

        if (current_committee < committees_count) {
            fprintf(stderr, "BLAD! Wiersz zawiera zbyt malo komitetow!\n");
            return 1;
        }
        
        districts_count++;
    }

    return 0;
}

int parse_arguments(const int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp("--sainte-lague", argv[i]) == 0) {
            if (sainte_lague == true) {
                fprintf(stderr, "BLAD! Flaga --sainte_lague moze wystapic tylko raz!\n");
                return 1;
            }

            sainte_lague = true;
        }

        else if (strcmp("--minority", argv[i]) == 0) {
            if (i == argc - 1 || argv[i+1][0] == '-') {
                fprintf(stderr, "BLAD! Nieprawidlowe uzyce flagi --minority!\n");
                return 1;
            }

            int j = 0;
            while (j < committees_count) {
                if (strcmp(committees[j]->name, argv[i+1]) == 0) {
                    if (committees[j]->exempt == true) {
                        fprintf(stderr, "BLAD! Zdublowane uzycie --minority!\n");
                        return 1;
                    }

                    committees[j]->exempt = true;
                    i++;
                    break;
                }
                j++;
            }

            if (j == committees_count) {
                fprintf(stderr, "BLAD! Flaga --minority uzyta dla nieistiniejacego stronnictwa!\n");
                return 1;
            }
            continue;
        }

        else if (strcmp("--alliance", argv[i]) == 0) {
            if (i == argc - 1) {
                fprintf(stderr, "BLAD! Nieprawidlowe uzyce flagi --alliance!\n");
                return 1;
            }

            // Pierwszy komitet staje się liderem
            int leader = -1;
            char* alliance_committees = strdup(argv[i+1]);
            char* committee = strtok(alliance_committees, ";");

            while (committee != NULL) {
                // Flaga przechowująca, czy dany komitet w ogóle istnieje
                bool found = false; 
                
                for (int j = 0; j < committees_count; j++) {
                    if (strcmp(committees[j]->name, committee) == 0) {
                        found = true;

                        // Powtórzenie komitetu w sojuszu
                        if(committees[j]->alliance == true) {
                            fprintf(stderr, "BLAD! Powtorzenie komitetu w --alliance!\n");
                            free(alliance_committees);
                            return 1;
                        }

                        if (leader == -1) {
                            leader = j;
                            committees[j]->alliance = true;
                        }

                        else {
                            committees[leader]->votes += committees[j]->votes;
                            committees[j]->votes = 0;
                            committees[j]->alliance = true;
                        }
                    }
                }

                if (found == false) {
                    fprintf(stderr, "BLAD! Nieistniejacy komitet w --alliance!\n");
                    free(alliance_committees);
                    return 1;
                }

                committee = strtok(NULL, ";");
            }

            free(committees[leader]->name);
            committees[leader]->name = strdup(argv[i + 1]);

            for (int j = 0; (size_t)j < strlen(committees[leader]->name); ++j) {
                if (committees[leader]->name[j] == ';') {
                    committees[leader]->name[j] = '+';
                }
            }

            free(alliance_committees);
            i++;
        }

        else {
            fprintf(stderr, "BLAD! Nieznany argument wywolania!\n");
            return 1;
        }
    }

    return 0;
}