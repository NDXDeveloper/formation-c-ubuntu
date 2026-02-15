/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Daemonisation d'un processus (fork + setsid)
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void daemonize(void) {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);  /* Parent se termine */
    }

    /* Enfant continue et devient leader de session */
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    /* Maintenant le processus est un daemon */
    printf("Daemon PID: %d, SID: %d\n", getpid(), getsid(0));
}

int main(void) {
    printf("Avant daemonisation - PID: %d\n", getpid());

    daemonize();

    /* Seul le daemon arrive ici */
    printf("Le daemon tourne (PID: %d)...\n", getpid());
    sleep(2);
    printf("Le daemon se termine.\n");

    return 0;
}
