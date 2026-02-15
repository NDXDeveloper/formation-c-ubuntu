/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Obtenir le PID du processus courant et de son parent
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t mon_pid = getpid();
    pid_t parent_pid = getppid();

    printf("Mon PID est: %d\n", mon_pid);
    printf("Le PID de mon parent est: %d\n", parent_pid);

    return 0;
}
