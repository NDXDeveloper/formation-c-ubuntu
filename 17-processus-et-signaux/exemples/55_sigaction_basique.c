/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Utilisation de sigaction() - methode recommandee
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigint_handler(int signum) {
    (void)signum;
    const char msg[] = "\n[HANDLER] SIGINT recu\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void) {
    struct sigaction sa;

    /* Initialiser la structure */
    memset(&sa, 0, sizeof(sa));

    /* Definir le handler */
    sa.sa_handler = sigint_handler;

    /* Pas de signaux bloques supplementaires pendant le handler */
    sigemptyset(&sa.sa_mask);

    /* Options : redemarrer automatiquement les appels systeme interrompus */
    sa.sa_flags = SA_RESTART;

    /* Installer le handler */
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Handler installe pour SIGINT (PID: %d)\n", getpid());
    printf("Appuyez sur Ctrl+C (10 iterations)\n\n");

    for (int i = 0; i < 10; i++) {
        printf("En cours...\n");
        sleep(2);
    }

    return 0;
}
