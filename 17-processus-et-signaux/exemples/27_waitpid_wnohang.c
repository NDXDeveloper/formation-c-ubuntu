/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Mode non-bloquant avec waitpid() et WNOHANG
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Enfant dort 5 secondes */
        printf("[Enfant] Je dors 5 secondes...\n");
        sleep(5);
        return 0;
    }

    /* Parent verifie periodiquement sans bloquer */
    printf("[Parent] Je verifie l'etat de mon enfant sans bloquer...\n");

    int status;
    pid_t result;
    int checks = 0;

    do {
        result = waitpid(pid, &status, WNOHANG);

        if (result == 0) {
            /* Enfant pas encore termine */
            printf("[Parent] Check %d: enfant encore actif\n", ++checks);
            sleep(1);
        }
        else if (result == pid) {
            /* Enfant termine */
            printf("[Parent] Enfant termine!\n");
            break;
        }
        else {
            perror("waitpid");
            break;
        }

    } while (result == 0);

    return 0;
}
