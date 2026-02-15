/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Groupe de processus (PGID)
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid, pgid;

    pid = getpid();
    pgid = getpgrp();  /* ou getpgid(0) */

    printf("Mon PID: %d\n", pid);
    printf("Mon PGID (Process Group ID): %d\n", pgid);

    /* Generalement, PID == PGID pour le leader du groupe */
    if (pid == pgid) {
        printf("Je suis le LEADER de mon groupe de processus.\n");
    }

    return 0;
}
