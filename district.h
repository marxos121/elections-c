#pragma once

typedef struct {
    char* name;
    int mandates;
    int* votes;
} District;

District* create_district(const char* name, const int mandates, const int committees);
void delete_district(District* target);