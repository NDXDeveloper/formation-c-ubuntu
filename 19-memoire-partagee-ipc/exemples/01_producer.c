/* ============================================================================
   Section 19.1 : Shared Memory (Memoire Partagee)
   Description : Processus producteur (ecrivain) POSIX shared memory
   Fichier source : 01-shared-memory.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "01_shared_data.h"

int main(void) {
    /* 1. Creer le segment de memoire partagee */
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    /* 2. Definir la taille du segment */
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    /* 3. Mapper le segment en memoire */
    struct shared_data *data = mmap(NULL, SHM_SIZE,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* 4. Utiliser la memoire partagee */
    printf("Producteur : Ecriture des donnees...\n");
    data->counter = 42;
    strcpy(data->message, "Hello from producer!");

    printf("Producteur : counter = %d\n", data->counter);
    printf("Producteur : message = %s\n", data->message);

    /* Attendre que l'utilisateur appuie sur Entree */
    printf("Appuyez sur Entree pour terminer...\n");
    getchar();

    /* 5. Detacher le mapping */
    if (munmap(data, SHM_SIZE) == -1) {
        perror("munmap");
    }

    /* 6. Fermer le descripteur */
    close(shm_fd);

    return 0;
}
