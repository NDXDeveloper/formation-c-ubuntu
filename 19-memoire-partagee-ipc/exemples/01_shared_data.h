/* ============================================================================
   Section 19.1 : Shared Memory (Memoire Partagee)
   Description : Header de structure partagee pour producteur-consommateur POSIX
   Fichier source : 01-shared-memory.md
   ============================================================================ */
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE sizeof(struct shared_data)

struct shared_data {
    int counter;
    char message[256];
};

#endif
