/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Premier exemple de fork() - parent et enfant
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    printf("Avant fork() - PID: %d\n", getpid());

    pid = fork();

    // A partir d'ici, le code est execute par 2 processus !

    if (pid == -1) {
        // Erreur lors du fork
        perror("Erreur fork");
        return 1;
    }
    else if (pid == 0) {
        // Code execute par l'ENFANT
        printf("Je suis le processus ENFANT - PID: %d, Parent PID: %d\n",
               getpid(), getppid());
    }
    else {
        // Code execute par le PARENT
        printf("Je suis le processus PARENT - PID: %d, Enfant PID: %d\n",
               getpid(), pid);
    }

    printf("Fin du processus PID: %d\n", getpid());

    return 0;
}
