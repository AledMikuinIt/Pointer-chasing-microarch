#ifndef RESSOURCEMANAGER_H
#define RESSOURCEMANAGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct Object {
    struct Object *next;    // pointeur principal
    struct Object *alt;     // pointeur alternatif
    struct Object *meta;    // pointeur additionel
    int val;     // valeur

} Object;

typedef struct RessourceManager {
    Object **handles; // tableau de pointeurs
    size_t num_objects; 
} RessourceManager;

RessourceManager* create_linear_rm(size_t n);
RessourceManager* create_dispersed_rm(size_t n);
void free_rm(RessourceManager *rm);
uint64_t traverse_next(RessourceManager *rm);

#endif
