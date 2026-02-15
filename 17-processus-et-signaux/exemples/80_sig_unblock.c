/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Mode SIG_UNBLOCK - retirer des signaux du masque
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t mask;

    /* Bloquer plusieurs signaux */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGUSR1);

    printf("Blocage de SIGINT, SIGTERM et SIGUSR1...\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    sleep(3);

    /* Debloquer uniquement SIGINT */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    printf("Deblocage de SIGINT (SIGTERM et SIGUSR1 restent bloques)\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    sleep(3);

    return 0;
}
