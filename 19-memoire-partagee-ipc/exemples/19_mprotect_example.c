/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : Protection memoire avec mprotect() - provoque un SEGFAULT
   Fichier source : 05-mmap.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

void segfault_handler(int sig) {
    (void)sig;
    printf("SEGFAULT attrape ! Tentative d'ecriture interdite.\n");
    exit(1);
}

int main(void) {
    signal(SIGSEGV, segfault_handler);

    size_t size = 4096;
    char *data = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Ecrire des donnees */
    strcpy(data, "Configuration importante");
    printf("Donnees : %s\n", data);

    /* Rendre read-only */
    if (mprotect(data, size, PROT_READ) == -1) {
        perror("mprotect");
        return 1;
    }

    printf("Zone maintenant en lecture seule\n");

    /* Tentative d'ecriture -> SEGFAULT */
    data[0] = 'X';  /* Crash ici ! */

    munmap(data, size);
    return 0;
}
