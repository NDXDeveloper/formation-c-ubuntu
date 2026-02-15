/* ============================================================================
   Section 27.3 : Profiling (gprof)
   Description : Programme de test pour profiling avec gprof - fonctions lentes,
                 rapides et intermediaires pour identifier les goulots
   Fichier source : 03-profiling.md
   ============================================================================ */

#include <stdio.h>

void fonction_lente(void) {
    /* Simule un calcul lourd */
    for (volatile int i = 0; i < 100000000; i++) {
        /* Calcul inutile */
    }
}

void fonction_rapide(void) {
    for (volatile int i = 0; i < 1000; i++) {
        /* Peu d'iterations */
    }
}

void fonction_intermediaire(void) {
    fonction_lente();
    fonction_rapide();
}

int main(void) {
    printf("Debut du programme\n");

    /* Appelle plusieurs fois les fonctions */
    for (int i = 0; i < 5; i++) {
        fonction_intermediaire();
    }

    fonction_rapide();

    printf("Fin du programme\n");
    return 0;
}
