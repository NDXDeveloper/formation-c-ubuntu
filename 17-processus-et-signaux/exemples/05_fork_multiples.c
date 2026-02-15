/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Creation de multiples processus enfants
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid1, pid2;

    printf("Processus principal - PID: %d\n", getpid());

    // Premier enfant
    pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        return 1;
    }

    if (pid1 == 0) {
        printf("Premier enfant - PID: %d\n", getpid());
        return 0;
    }

    // Deuxieme enfant (cree par le parent uniquement)
    pid2 = fork();

    if (pid2 == -1) {
        perror("fork");
        return 1;
    }

    if (pid2 == 0) {
        printf("Deuxieme enfant - PID: %d\n", getpid());
        return 0;
    }

    // Code du parent
    printf("Parent - Mes enfants: %d et %d\n", pid1, pid2);

    return 0;
}
