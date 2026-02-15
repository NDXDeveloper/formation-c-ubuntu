/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Mode SIG_SETMASK - remplacer completement le masque
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t mask1, mask2;

    /* Premier masque : SIGINT */
    sigemptyset(&mask1);
    sigaddset(&mask1, SIGINT);

    printf("Masque 1: SIGINT bloque\n");
    sigprocmask(SIG_SETMASK, &mask1, NULL);
    sleep(2);

    /* Deuxieme masque : SIGTERM (remplace SIGINT) */
    sigemptyset(&mask2);
    sigaddset(&mask2, SIGTERM);

    printf("Masque 2: SIGTERM bloque (SIGINT debloque)\n");
    sigprocmask(SIG_SETMASK, &mask2, NULL);
    sleep(2);

    /* Vider le masque (debloquer tout) */
    sigemptyset(&mask1);
    printf("Masque vide: tous les signaux debloques\n");
    sigprocmask(SIG_SETMASK, &mask1, NULL);

    sleep(2);

    return 0;
}
