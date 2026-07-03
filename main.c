#include <bits/time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "ressourceManager.h"



static inline uint64_t ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main() {


    size_t num = 1000000;

    RessourceManager *drm = create_dispersed_rm(num);
    RessourceManager *lrm = create_linear_rm(num);

    uint64_t dstart = ns();
    uint64_t ds = traverse_next(drm);
    uint64_t dend = ns();
    uint64_t lstart = ns();
    uint64_t ls = traverse_next(lrm);
    uint64_t lend = ns();

    printf("Resultat Lineaire: %lu\n",ls);
    printf("Temps Lineaire: %lu\n\n", lend-lstart);


    printf("Resultat Disperse: %lu\n", ds);
    printf("Temps Disperse: %lu\n\n", dend-dstart);

    printf("Différence: %lu\n", (dend-dstart)-(lend-lstart));


    // Free
    free_rm(drm);
    free_rm(lrm);

    return 0;
}

