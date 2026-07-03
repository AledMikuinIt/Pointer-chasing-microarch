#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ressourceManager.h"


RessourceManager* create_linear_rm(size_t n) {
    RessourceManager *rm = malloc(sizeof(RessourceManager));    // on crée un tableau de la taille de la struct
    if(!rm) return NULL;
    rm->num_objects = n;                                              // le nombre d'objet souhaité
    rm->handles = malloc(n * sizeof(Object*));                  // les handles ont la taille du nombre de pointeurs de taille des pointeurs Objet
    if(!rm->handles) {
        free_rm(rm);
        return NULL;
    }
    

    for(size_t i = 0; i < n; i++) {
        rm->handles[i] = malloc(sizeof(Object));                // handle prends taille struct objet
        if(!rm->handles[i]) {
            free_rm(rm);
            return NULL;
        }
        rm->handles[i]->val = i;                                        // val = i comme ca c'est un peu comme l'id
        rm->handles[i]->alt = NULL;                                     // pointe vers NULL
        rm->handles[i]->meta = NULL;                                    // pointe vers NULL
    }

    for (size_t i = 0; i < n - 1; i++) {
        rm->handles[i]->next = rm->handles[i+1];
    }
    rm->handles[n-1]->next = NULL;
    return rm;
}



RessourceManager* create_dispersed_rm(size_t n) {
    RessourceManager *rm = malloc(sizeof(RessourceManager));
    if(!rm) return NULL;

    rm->num_objects = n;
    rm->handles = malloc(n * sizeof(Object*));
    if(!rm->handles) {
        free_rm(rm);
        return NULL;
    }

    for(size_t i = 0; i < n; i++) {
        Object *obj;                        // Crée un pointeur obj de type Object
        if(posix_memalign((void**)&obj, 4096, sizeof(Object)) != 0) {   // on alloue de la mémoire aligné sur 4096, donc la taille de sizeof Object alloué a une adresse multiple de 4096
            free_rm(rm);
            return NULL;
        }
        rm->handles[i] = obj;   // obj ici est l'adresse aligné sur 4096 par exemple 0x12000;



        rm->handles[i]->val = i;
        rm->handles[i]->alt = NULL;
        rm->handles[i]->meta = NULL;
    }

    if(n<1) return NULL;
    for (size_t i = n - 1; i > 0; i--) {        // on parcours la liste a l'envers pour assurer l'uniformité & chaque i soit lu une seule fois
        size_t j = rand() % (i + 1);    // algo Fisher-Yates, modulo i+1 obligé : par exemple il reste 10 éléments : i = 9 (car 0-9) donc j rand % 10
        Object *tmp = rm->handles[i];   // pour faire un swap
        rm->handles[i] = rm->handles[j];
        rm->handles[j] = tmp;
    }

    for (size_t i = 0; i < n - 1; i++) {
        rm->handles[i]->next = rm->handles[i+1];
    }
    rm->handles[n-1]->next = NULL;


    return rm;
}

void free_rm(RessourceManager *rm) {

    // Libération de mémoire 
    for(size_t i = 0; i < rm->num_objects; i++) {
        free(rm->handles[i]);
        rm->handles[i] = NULL;  // mettre le pointeur a NULL pour éviter les UAF
    }
    free(rm->handles);
    free(rm);

}

uint64_t traverse_next(RessourceManager *rm) {  // en gros c'est pour traverser tout les pointeurs jusqu'a la fin
    Object *current = rm->handles[0];
    uint64_t sum = 0;

    while (current != NULL) {
        sum += current->val;    // somme de toutes les valeurs 
        current = current->next;    // aller a l'objet suivant
    }
    return sum;

}