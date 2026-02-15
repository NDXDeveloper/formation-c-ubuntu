/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Comment un processus devient orphelin (adopte par systemd)
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* PROCESSUS ENFANT */
        printf("[Enfant] Mon PID: %d\n", getpid());
        printf("[Enfant] Mon parent initial: %d\n", getppid());

        sleep(3);  /* Parent va se terminer pendant ce temps */

        printf("[Enfant] Mon parent maintenant: %d\n", getppid());
        printf("[Enfant] Je suis devenu orphelin, adopte par systemd!\n");

        sleep(2);
        return 0;
    }
    else {
        /* PROCESSUS PARENT */
        printf("[Parent] Mon PID: %d\n", getpid());
        printf("[Parent] J'ai cree l'enfant: %d\n", pid);
        printf("[Parent] Je me termine immediatement!\n");

        /* Parent se termine sans attendre l'enfant */
        return 0;
    }
}
