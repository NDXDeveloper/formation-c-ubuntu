/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Ignorer SIGCHLD pour eviter les zombies (methode simple)
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(void) {
    /* Dire au noyau de nettoyer automatiquement les enfants */
    signal(SIGCHLD, SIG_IGN);

    printf("[Parent] SIGCHLD ignore, pas de zombies possibles\n");

    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            printf("[Enfant %d] PID: %d\n", i, getpid());
            sleep(1);
            return 0;
        }
    }

    sleep(5);
    printf("[Parent] Aucun zombie cree!\n");

    return 0;
}
