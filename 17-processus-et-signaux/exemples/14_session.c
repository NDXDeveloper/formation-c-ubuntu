/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Session de processus (SID, PGID, PID)
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = getpid();
    pid_t sid = getsid(0);   /* 0 = processus courant */
    pid_t pgid = getpgrp();

    printf("Mon PID:  %d\n", pid);
    printf("Mon PGID: %d\n", pgid);
    printf("Mon SID:  %d\n", sid);

    return 0;
}
