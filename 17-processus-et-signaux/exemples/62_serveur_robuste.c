/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Squelette de serveur robuste avec gestion complete des signaux
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

volatile sig_atomic_t shutdown_requested = 0;
volatile sig_atomic_t reload_config = 0;

void signal_handler(int signum) {
    char msg[64];
    int len;

    switch (signum) {
        case SIGINT:
        case SIGTERM:
            len = snprintf(msg, sizeof(msg),
                          "\n[Signal] Arret demande (signal %d)\n", signum);
            write(STDOUT_FILENO, msg, (size_t)len);
            shutdown_requested = 1;
            break;

        case SIGHUP:
            len = snprintf(msg, sizeof(msg),
                          "\n[Signal] Rechargement config (signal %d)\n", signum);
            write(STDOUT_FILENO, msg, (size_t)len);
            reload_config = 1;
            break;

        case SIGUSR1:
            len = snprintf(msg, sizeof(msg),
                          "\n[Signal] SIGUSR1 - Affichage des stats (signal %d)\n",
                          signum);
            write(STDOUT_FILENO, msg, (size_t)len);
            break;
    }
}

void sigchld_handler(int signum) {
    (void)signum;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char msg[64];
        int len = snprintf(msg, sizeof(msg),
                          "[Signal] Worker %d termine\n", pid);
        write(STDOUT_FILENO, msg, (size_t)len);
    }
}

void setup_signals(void) {
    struct sigaction sa;

    /* Signaux d'arret */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    /* SIGCHLD */
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    /* Ignorer SIGPIPE */
    signal(SIGPIPE, SIG_IGN);
}

void load_config(void) {
    printf("[Config] Chargement de la configuration...\n");
    sleep(1);
    printf("[Config] Configuration chargee\n");
}

int main(void) {
    printf("=== Serveur Robuste ===\n");
    printf("PID: %d\n\n", getpid());

    setup_signals();
    load_config();

    printf("Commandes:\n");
    printf("  kill -TERM %d  : Arret propre\n", getpid());
    printf("  kill -HUP %d   : Recharger config\n", getpid());
    printf("  kill -USR1 %d  : Afficher stats\n", getpid());
    printf("\nServeur demarre...\n\n");

    int request_count = 0;

    while (!shutdown_requested) {
        /* Verifier si rechargement demande */
        if (reload_config) {
            reload_config = 0;
            load_config();
        }

        /* Simuler le traitement de requetes */
        printf("[Serveur] Traitement requete #%d\n", ++request_count);
        sleep(2);
    }

    printf("\n[Serveur] Arret en cours...\n");
    printf("[Serveur] Nettoyage des ressources...\n");
    sleep(1);
    printf("[Serveur] Total requetes traitees: %d\n", request_count);
    printf("[Serveur] Arret propre termine\n");

    return 0;
}
