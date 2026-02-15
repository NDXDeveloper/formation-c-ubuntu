/* ============================================================================
   Section 27.4 : Cache awareness
   Description : Comparaison parcours par ligne vs par colonne d'une matrice
                 pour demontrer l'impact du cache (acces sequentiel vs stride)
   Fichier source : 04-cache-awareness.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>

#define TAILLE 1000
#define ITERATIONS 100

int matrice[TAILLE][TAILLE];

void parcours_ligne(void) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            matrice[i][j] = 0;
        }
    }
}

void parcours_colonne(void) {
    for (int j = 0; j < TAILLE; j++) {
        for (int i = 0; i < TAILLE; i++) {
            matrice[i][j] = 0;
        }
    }
}

int main(void) {
    clock_t debut, fin;

    /* Test parcours par ligne */
    debut = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        parcours_ligne();
    }
    fin = clock();
    double temps_ligne = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    printf("Parcours ligne   (%dx) : %.1f ms\n", ITERATIONS, temps_ligne);

    /* Test parcours par colonne */
    debut = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        parcours_colonne();
    }
    fin = clock();
    double temps_colonne = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    printf("Parcours colonne (%dx) : %.1f ms\n", ITERATIONS, temps_colonne);

    if (temps_ligne > 0.0) {
        printf("Ratio : colonne %.1fx plus lent\n",
               temps_colonne / temps_ligne);
    }

    return 0;
}
