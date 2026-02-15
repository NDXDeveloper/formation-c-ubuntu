/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGUSR1/SIGUSR2 pour statistiques et bascule de mode debug
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

/* Statistiques globales */
volatile sig_atomic_t request_count = 0;
volatile sig_atomic_t debug_mode = 0;

void sigusr_handler(int signum) {
    char msg[256];
    int len;

    if (signum == SIGUSR1) {
        /* SIGUSR1 : Afficher les statistiques */
        time_t now = time(NULL);
        len = snprintf(msg, sizeof(msg),
                      "\n=== Statistiques ===\n"
                      "Requetes traitees: %d\n"
                      "Mode debug: %s\n"
                      "Timestamp: %ld\n"
                      "====================\n\n",
                      request_count,
                      debug_mode ? "ON" : "OFF",
                      (long)now);
        write(STDOUT_FILENO, msg, (size_t)len);
    }
    else if (signum == SIGUSR2) {
        /* SIGUSR2 : Basculer le mode debug */
        debug_mode = !debug_mode;
        len = snprintf(msg, sizeof(msg),
                      "\n[Config] Mode debug: %s\n\n",
                      debug_mode ? "ACTIVE" : "DESACTIVE");
        write(STDOUT_FILENO, msg, (size_t)len);
    }
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigusr_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("Service demarre (PID: %d)\n\n", getpid());
    printf("Commandes disponibles:\n");
    printf("  kill -USR1 %d  : Afficher les statistiques\n", getpid());
    printf("  kill -USR2 %d  : Basculer le mode debug\n\n", getpid());

    for (int i = 0; i < 20; i++) {
        request_count++;

        if (debug_mode) {
            printf("[DEBUG] Traitement requete #%d\n", request_count);
        } else {
            printf("Requete #%d\n", request_count);
        }

        sleep(1);
    }

    return 0;
}
