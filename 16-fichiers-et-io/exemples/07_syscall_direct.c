/* ============================================================================
   Section 16.2 : Appels systeme
   Description : Appels systeme directs avec syscall() sans passer par les wrappers
   Fichier source : 02-appels-systeme.md
   ============================================================================ */

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

int main(void) {
    /* Appeler gettid() directement */
    pid_t tid = (pid_t)syscall(SYS_gettid);
    printf("Thread ID : %d\n", tid);

    /* Appeler write() directement */
    const char *msg = "Direct syscall\n";
    syscall(SYS_write, STDOUT_FILENO, msg, 15);

    return 0;
}
