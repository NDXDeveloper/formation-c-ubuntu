/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Relations parent-enfant apres fork()
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    printf("=== AVANT fork() ===\n");
    printf("PID du processus: %d\n", getpid());
    printf("PID du parent: %d\n\n", getppid());

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // PROCESSUS ENFANT
        printf("=== PROCESSUS ENFANT ===\n");
        printf("Mon PID: %d\n", getpid());
        printf("PID de mon parent: %d\n", getppid());
        printf("Je suis l'enfant cree par fork()\n\n");
    }
    else {
        // PROCESSUS PARENT
        printf("=== PROCESSUS PARENT ===\n");
        printf("Mon PID: %d\n", getpid());
        printf("PID de mon parent: %d\n", getppid());
        printf("J'ai cree un enfant avec le PID: %d\n\n", pid);
    }

    return 0;
}
