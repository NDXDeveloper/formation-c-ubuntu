/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Creation de 1000 zombies (exemple pedagogique dangereux)
   Fichier source : 03-orphelins-zombies.md
   Note : NE PAS executer sur un systeme de production !
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    printf("Creation de 1000 processus zombies...\n");

    for (int i = 0; i < 1000; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            /* Enfant se termine immediatement */
            return 0;
        }

        /* Parent ne fait JAMAIS de wait() */
    }

    printf("1000 zombies crees! Verifiez avec 'ps aux | grep Z'\n");
    sleep(30);  /* Garde les zombies actifs */

    return 0;
}
