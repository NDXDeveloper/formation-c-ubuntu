/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Envoyer un signal avec kill() - s'envoyer SIGINT a soi-meme
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    printf("Je m'envoie SIGINT...\n");

    /* Envoyer SIGINT a soi-meme */
    kill(getpid(), SIGINT);

    /* Ou utiliser raise() */
    /* raise(SIGINT); */

    printf("Cette ligne ne sera probablement jamais affichee\n");

    return 0;
}
