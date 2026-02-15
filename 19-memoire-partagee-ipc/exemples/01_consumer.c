/* ============================================================================
   Section 19.1 : Shared Memory (Memoire Partagee)
   Description : Processus consommateur (lecteur) POSIX shared memory
   Fichier source : 01-shared-memory.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "01_shared_data.h"

int main(void) {
    /* 1. Ouvrir le segment existant */
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open (le segment existe-t-il ?)");
        exit(1);
    }

    /* 2. Mapper le segment en memoire */
    struct shared_data *data = mmap(NULL, SHM_SIZE,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* 3. Lire les donnees */
    printf("Consommateur : Lecture des donnees...\n");
    printf("Consommateur : counter = %d\n", data->counter);
    printf("Consommateur : message = %s\n", data->message);

    /* Modifier les donnees */
    data->counter += 10;
    printf("Consommateur : nouveau counter = %d\n", data->counter);

    /* 4. Detacher le mapping */
    if (munmap(data, SHM_SIZE) == -1) {
        perror("munmap");
    }

    /* 5. Fermer le descripteur */
    close(shm_fd);

    /* 6. Supprimer le segment */
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }

    return 0;
}
