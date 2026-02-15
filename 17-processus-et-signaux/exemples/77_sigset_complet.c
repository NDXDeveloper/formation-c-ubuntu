/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Ensemble complet avec sigfillset vs ensemble vide
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>

int main(void) {
    sigset_t fullset, emptyset;

    /* Ensemble avec tous les signaux */
    sigfillset(&fullset);

    /* Ensemble vide */
    sigemptyset(&emptyset);

    printf("Ensemble complet contient SIGINT: %s\n",
           sigismember(&fullset, SIGINT) ? "Oui" : "Non");

    printf("Ensemble vide contient SIGINT: %s\n",
           sigismember(&emptyset, SIGINT) ? "Oui" : "Non");

    return 0;
}
