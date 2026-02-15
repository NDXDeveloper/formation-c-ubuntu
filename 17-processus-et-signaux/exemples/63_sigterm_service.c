/* ============================================================================
   Section 17.6 : Signaux courants
   Description : Gestion de SIGTERM pour arret propre d'un service
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t termination_requested = 0;

void sigterm_handler(int signum) {
    (void)signum;
    termination_requested = 1;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, NULL);

    printf("Service demarre (PID: %d)\n", getpid());
    printf("Utilisez 'kill %d' pour l'arreter proprement\n\n", getpid());

    /* Ouvrir une ressource */
    FILE *log = fopen("/tmp/service_test.log", "a");
    if (log == NULL) {
        perror("fopen");
        return 1;
    }

    fprintf(log, "Service demarre\n");
    fflush(log);

    int request_count = 0;

    while (!termination_requested) {
        printf("Traitement requete #%d\n", ++request_count);
        fprintf(log, "Requete #%d traitee\n", request_count);
        fflush(log);
        sleep(2);
    }

    /* Nettoyage propre */
    printf("\nSIGTERM recu, arret en cours...\n");
    fprintf(log, "Arret propre apres %d requetes\n", request_count);

    fclose(log);

    printf("Nettoyage termine, au revoir!\n");

    return 0;
}
