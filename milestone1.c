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

    FILE *f = fopen("results.csv", "w");
    if (!f) {
        perror("Erreur ouverture CSV");
        return 1;
    }

    fprintf(f, "N,Resultat_lineaire,Temps_lineaire,Resultat_disperse,Temps_disperse,Difference\n");


    int num[] = {10000, 100000, 1000000
    };
    for(int i = 0; i<sizeof(num)/sizeof(num[0]); i++) {
        RessourceManager *drm = create_dispersed_rm(num[i]);
        RessourceManager *lrm = create_linear_rm(num[i]);


        uint64_t dstart = ns();
        uint64_t ds = traverse_next(drm);
        uint64_t dend = ns();
        uint64_t lstart = ns();
        uint64_t ls = traverse_next(lrm);
        uint64_t lend = ns();

        uint64_t dtime = dend - dstart;
        uint64_t ltime = lend - lstart;

        fprintf(f, "%d,%lu,%lu,%lu,%lu,%lu\n",
        num[i], ls, ltime, ds, dtime, dtime - ltime);

        // Libération mémoire
        free_rm(drm);
        free_rm(lrm);

    }



    // Free
    fclose(f);

    return 0;
}

