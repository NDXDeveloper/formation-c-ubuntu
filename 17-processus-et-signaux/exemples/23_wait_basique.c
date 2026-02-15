/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Utilisation basique de wait() pour eviter les zombies
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* ENFANT */
        printf("[Enfant] PID: %d, je travaille...\n", getpid());
        sleep(2);
        printf("[Enfant] Travail termine, je me termine.\n");
        return 42;  /* Code de retour */
    }
    else {
        /* PARENT */
        int status;

        printf("[Parent] J'attends mon enfant %d...\n", pid);

        pid_t terminated_pid = wait(&status);

        printf("[Parent] L'enfant %d s'est termine.\n", terminated_pid);

        /* Pas de zombie! L'enfant a ete correctement nettoye */

        return 0;
    }
}
