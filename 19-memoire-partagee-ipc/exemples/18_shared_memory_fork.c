/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : Memoire partagee entre processus avec MAP_SHARED + MAP_ANONYMOUS
   Fichier source : 05-mmap.md
   ============================================================================ */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    /* Creer une zone partagee AVANT fork() */
    size_t size = sizeof(int);
    int *shared_counter = mmap(NULL, size, PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_counter == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    *shared_counter = 0;  /* Initialiser */

    fflush(stdout);

    /* Fork */
    pid_t pid = fork();

    if (pid == 0) {
        /* ===== PROCESSUS FILS ===== */
        for (int i = 0; i < 5; i++) {
            (*shared_counter)++;
            printf("[FILS] Compteur : %d\n", *shared_counter);
            usleep(100000);  /* 100ms */
        }
        exit(0);

    } else {
        /* ===== PROCESSUS PARENT ===== */
        for (int i = 0; i < 5; i++) {
            (*shared_counter)++;
            printf("[PARENT] Compteur : %d\n", *shared_counter);
            usleep(100000);
        }

        wait(NULL);  /* Attendre le fils */

        printf("\nValeur finale : %d\n", *shared_counter);

        munmap(shared_counter, size);
    }

    return 0;
}
