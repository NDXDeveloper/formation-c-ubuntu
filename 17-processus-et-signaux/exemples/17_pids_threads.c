/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : PIDs et threads - TID du thread principal via syscall
   Fichier source : 02-pids-hierarchie.md
   Note : Necessite _GNU_SOURCE, compiler sans -pedantic
   ============================================================================ */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

int main(void) {
    pid_t pid = getpid();
    pid_t tid = (pid_t)syscall(SYS_gettid);

    printf("PID du processus: %d\n", pid);
    printf("TID du thread principal: %d\n", tid);

    /* Pour le thread principal: PID == TID */
    if (pid == tid) {
        printf("Confirmation: je suis le thread principal.\n");
    }

    return 0;
}
