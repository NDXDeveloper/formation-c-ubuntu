/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Gerer plusieurs signaux (SIGINT, SIGTERM, SIGUSR1)
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t usr1_received = 0;

void signal_handler(int signum) {
    char msg[64];
    int len;

    switch (signum) {
        case SIGINT:
            len = snprintf(msg, sizeof(msg),
                          "\n[HANDLER] SIGINT (Ctrl+C) recu\n");
            write(STDOUT_FILENO, msg, (size_t)len);
            keep_running = 0;
            break;

        case SIGTERM:
            len = snprintf(msg, sizeof(msg),
                          "\n[HANDLER] SIGTERM recu\n");
            write(STDOUT_FILENO, msg, (size_t)len);
            keep_running = 0;
            break;

        case SIGUSR1:
            len = snprintf(msg, sizeof(msg),
                          "\n[HANDLER] SIGUSR1 recu\n");
            write(STDOUT_FILENO, msg, (size_t)len);
            usr1_received++;
            break;
    }
}

int main(void) {
    struct sigaction sa;

    /* Configuration commune */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    /* Installer les handlers */
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    printf("Programme demarre (PID: %d)\n", getpid());
    printf("Signaux geres:\n");
    printf("  - Ctrl+C ou SIGINT  : Arret propre\n");
    printf("  - SIGTERM           : Arret propre\n");
    printf("  - SIGUSR1           : Incremente un compteur\n");
    printf("\nTestez avec: kill -USR1 %d\n\n", getpid());

    int count = 0;
    while (keep_running) {
        printf("Iteration %d (SIGUSR1 recu %d fois)\n",
               ++count, usr1_received);
        sleep(2);
    }

    printf("\n[MAIN] Arret demande, nettoyage...\n");
    printf("[MAIN] Total SIGUSR1 recus: %d\n", usr1_received);
    printf("[MAIN] Terminaison propre\n");

    return 0;
}
