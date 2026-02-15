/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Creer et manipuler un ensemble sigset_t
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>

int main(void) {
    sigset_t myset;

    /* 1. Initialiser l'ensemble (vide) */
    sigemptyset(&myset);

    /* 2. Ajouter des signaux */
    sigaddset(&myset, SIGINT);
    sigaddset(&myset, SIGTERM);
    sigaddset(&myset, SIGUSR1);

    /* 3. Tester l'appartenance */
    if (sigismember(&myset, SIGINT)) {
        printf("SIGINT est dans l'ensemble\n");
    }

    if (!sigismember(&myset, SIGQUIT)) {
        printf("SIGQUIT n'est PAS dans l'ensemble\n");
    }

    /* 4. Retirer un signal */
    sigdelset(&myset, SIGTERM);

    if (!sigismember(&myset, SIGTERM)) {
        printf("SIGTERM a ete retire\n");
    }

    return 0;
}
