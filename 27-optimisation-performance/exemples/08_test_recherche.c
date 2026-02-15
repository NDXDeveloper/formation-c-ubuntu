/* ============================================================================
   Section 27.6 : Optimisations algorithmiques
   Description : Benchmark recherche lineaire O(n) vs dichotomique O(log n)
                 sur un tableau de 1 million d'elements
   Fichier source : 06-optimisations-algorithmiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 1000000

/* Recherche lineaire : O(n) */
int recherche_lineaire(const int *tableau, int taille, int cible) {
    for (int i = 0; i < taille; i++) {
        if (tableau[i] == cible) {
            return i;
        }
    }
    return -1;
}

/* Recherche dichotomique : O(log n) - tableau trie requis */
int recherche_dichotomique(const int *tableau, int taille, int cible) {
    int gauche = 0;
    int droite = taille - 1;

    while (gauche <= droite) {
        int milieu = gauche + (droite - gauche) / 2;

        if (tableau[milieu] == cible) {
            return milieu;
        }

        if (tableau[milieu] < cible) {
            gauche = milieu + 1;
        } else {
            droite = milieu - 1;
        }
    }

    return -1;
}

int main(void) {
    int *tableau = malloc(TAILLE * sizeof(int));
    if (!tableau) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    /* Remplir avec valeurs triees */
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = i;
    }

    clock_t debut, fin;
    int resultat;

    /* Recherche lineaire (1000 recherches) */
    debut = clock();
    for (int i = 0; i < 1000; i++) {
        resultat = recherche_lineaire(tableau, TAILLE, TAILLE - 1);
    }
    fin = clock();
    printf("Recherche lineaire      : %.1f ms  (index = %d)\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC, resultat);

    /* Recherche dichotomique (1000 recherches) */
    debut = clock();
    for (int i = 0; i < 1000; i++) {
        resultat = recherche_dichotomique(tableau, TAILLE, TAILLE - 1);
    }
    fin = clock();
    printf("Recherche dichotomique  : %.1f ms  (index = %d)\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC, resultat);

    free(tableau);
    return 0;
}
