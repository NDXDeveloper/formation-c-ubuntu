/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Daemon cree volontairement comme orphelin
   Fichier source : 03-orphelins-zombies.md
   Note : Le daemon tourne en boucle infinie (Ctrl+C ou kill pour arreter)
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void become_daemon(void) {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        /* Parent se termine volontairement */
        exit(EXIT_SUCCESS);
    }

    /* L'enfant continue et devient orphelin (adopte par systemd) */
    /* C'est maintenant un daemon */

    printf("Je suis un daemon, PID: %d, Parent: %d\n",
           getpid(), getppid());
}

int main(void) {
    printf("Processus initial: %d\n", getpid());
    become_daemon();

    /* Code du daemon ici */
    while (1) {
        sleep(10);
        /* Travail en arriere-plan... */
    }

    return 0;
}
