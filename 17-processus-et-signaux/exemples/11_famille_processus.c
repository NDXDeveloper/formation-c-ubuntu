/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Parent avec plusieurs enfants - visualisation avec pstree
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid1, pid2, pid3;

    printf("Processus parent initial - PID: %d\n\n", getpid());

    /* Creation du premier enfant */
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        return 1;
    }
    if (pid1 == 0) {
        printf("  [Enfant 1] PID: %d, Parent: %d\n", getpid(), getppid());
        sleep(10);  /* Reste actif pour visualiser */
        return 0;
    }

    /* Creation du deuxieme enfant */
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        return 1;
    }
    if (pid2 == 0) {
        printf("  [Enfant 2] PID: %d, Parent: %d\n", getpid(), getppid());
        sleep(10);
        return 0;
    }

    /* Creation du troisieme enfant */
    pid3 = fork();
    if (pid3 == -1) {
        perror("fork");
        return 1;
    }
    if (pid3 == 0) {
        printf("  [Enfant 3] PID: %d, Parent: %d\n", getpid(), getppid());
        sleep(10);
        return 0;
    }

    /* Code du parent */
    printf("\n[Parent] Mes enfants sont:\n");
    printf("  - Enfant 1: PID %d\n", pid1);
    printf("  - Enfant 2: PID %d\n", pid2);
    printf("  - Enfant 3: PID %d\n", pid3);

    printf("\nExecutez 'pstree -p %d' dans un autre terminal pour voir l'arbre!\n",
           getpid());

    sleep(10);  /* Laisser le temps de visualiser */

    return 0;
}
