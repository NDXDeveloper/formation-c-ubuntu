/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Mode SIG_BLOCK - ajouter des signaux au masque
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t mask;

    /* Bloquer SIGINT et SIGTERM */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);

    printf("Blocage de SIGINT et SIGTERM...\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    /* Maintenant, bloquer aussi SIGUSR1 (en plus des autres) */
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    printf("Ajout de SIGUSR1 au masque...\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    /* A ce stade, SIGINT, SIGTERM ET SIGUSR1 sont bloques */

    printf("3 signaux bloques: SIGINT, SIGTERM, SIGUSR1\n");
    sleep(5);

    return 0;
}
