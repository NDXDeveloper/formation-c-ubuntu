/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Attendre plusieurs signaux avec sigsuspend
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t which_signal = 0;

void signal_handler(int signum) {
    signal_received = 1;
    which_signal = signum;
}

int main(void) {
    sigset_t blockmask, waitmask;
    struct sigaction sa;

    /* Installer le handler pour plusieurs signaux */
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    /* Bloquer ces signaux */
    sigemptyset(&blockmask);
    sigaddset(&blockmask, SIGUSR1);
    sigaddset(&blockmask, SIGUSR2);
    sigaddset(&blockmask, SIGINT);
    sigprocmask(SIG_BLOCK, &blockmask, NULL);

    printf("PID: %d\n", getpid());
    printf("Attente de SIGUSR1, SIGUSR2 ou SIGINT...\n");

    /* Masque pour sigsuspend : tout sauf les signaux attendus */
    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);
    sigdelset(&waitmask, SIGUSR2);
    sigdelset(&waitmask, SIGINT);

    sigsuspend(&waitmask);

    /* Identifier quel signal a ete recu */
    if (signal_received) {
        switch (which_signal) {
            case SIGUSR1:
                printf("SIGUSR1 recu\n");
                break;
            case SIGUSR2:
                printf("SIGUSR2 recu\n");
                break;
            case SIGINT:
                printf("SIGINT recu\n");
                break;
            default:
                printf("Signal inconnu: %d\n", which_signal);
                break;
        }
    }

    return 0;
}
