/* ============================================================================
   Section 19.4 : POSIX IPC vs System V IPC
   Description : Compteur partage avec POSIX shared memory
   Fichier source : 04-posix-vs-system-v.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    /* Creer et dimensionner */
    int fd = shm_open("/my_counter", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    ftruncate(fd, sizeof(int));

    /* Mapper */
    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    close(fd);  /* Peut fermer immediatement */

    /* Utiliser */
    *counter = 42;
    printf("Compteur (POSIX) : %d\n", *counter);

    /* Detacher */
    munmap(counter, sizeof(int));

    /* Supprimer */
    shm_unlink("/my_counter");

    return 0;
}
