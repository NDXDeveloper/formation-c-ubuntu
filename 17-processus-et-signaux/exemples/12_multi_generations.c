/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Hierarchie multi-generationnelle (4 generations de processus)
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    printf("[Generation 0] PID: %d (Arriere-grand-parent)\n", getpid());

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        printf("  [Generation 1] PID: %d, Parent: %d (Grand-parent)\n",
               getpid(), getppid());

        pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            printf("    [Generation 2] PID: %d, Parent: %d (Parent)\n",
                   getpid(), getppid());

            pid = fork();
            if (pid == -1) {
                perror("fork");
                return 1;
            }
            if (pid == 0) {
                printf("      [Generation 3] PID: %d, Parent: %d (Enfant)\n",
                       getpid(), getppid());
                sleep(5);
                return 0;
            }

            sleep(5);
            return 0;
        }

        sleep(5);
        return 0;
    }

    printf("\nArbre cree! Utilisez 'pstree -p %d' pour visualiser.\n", getpid());
    sleep(5);

    return 0;
}
