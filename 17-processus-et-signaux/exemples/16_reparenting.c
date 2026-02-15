/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Reparenting - adoption d'un orphelin par systemd (PID 1)
   Fichier source : 02-pids-hierarchie.md
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
        /* ENFANT */
        printf("[Enfant] PID: %d, Parent initial: %d\n",
               getpid(), getppid());

        sleep(3);  /* Attendre que le parent se termine */

        printf("[Enfant] Apres sommeil, nouveau parent: %d\n",
               getppid());
        /* Le PPID sera probablement 1 (systemd) */

        sleep(2);
        return 0;
    }
    else {
        /* PARENT */
        printf("[Parent] PID: %d, enfant cree: %d\n",
               getpid(), pid);
        printf("[Parent] Je me termine immediatement!\n");

        return 0;  /* Parent se termine */
    }
}
