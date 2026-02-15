/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Gestion des erreurs de fork()
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        // fork() a echoue
        fprintf(stderr, "Erreur fork: %s\n", strerror(errno));

        // Causes possibles :
        // EAGAIN : Nombre max de processus atteint
        // ENOMEM : Memoire insuffisante

        return 1;
    }

    if (pid == 0) {
        printf("Enfant cree avec succes - PID: %d\n", getpid());
    } else {
        printf("Parent - PID: %d, Enfant: %d\n", getpid(), pid);
    }

    return 0;
}
