/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGKILL et SIGSTOP sont impossibles a intercepter
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signum) {
    printf("Signal %d recu\n", signum);
}

int main(void) {
    printf("PID: %d\n", getpid());

    /* Installer un handler pour plusieurs signaux */
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGUSR1, signal_handler);

    /* Tenter d'installer un handler pour SIGKILL (IMPOSSIBLE) */
    if (signal(SIGKILL, signal_handler) == SIG_ERR) {
        printf("Impossible d'intercepter SIGKILL (normal)\n");
    }

    /* Tenter d'installer un handler pour SIGSTOP (IMPOSSIBLE) */
    if (signal(SIGSTOP, signal_handler) == SIG_ERR) {
        printf("Impossible d'intercepter SIGSTOP (normal)\n");
    }

    printf("\nTestez les signaux:\n");
    printf("  kill -TERM %d  (sera intercepte)\n", getpid());
    printf("  kill -KILL %d  (tuera immediatement)\n\n", getpid());

    for (int i = 0; i < 10; i++) {
        printf("En cours...\n");
        sleep(2);
    }

    return 0;
}
