/* ============================================================================
   Section 27.6 : Optimisations algorithmiques
   Description : Benchmark tri a bulles O(n^2) vs quicksort O(n log n)
                 sur un tableau de 10000 elements aleatoires
   Fichier source : 06-optimisations-algorithmiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAILLE 10000

/* Tri a bulles : O(n^2) */
void tri_bulles(int *tableau, int taille) {
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (tableau[j] > tableau[j + 1]) {
                int temp = tableau[j];
                tableau[j] = tableau[j + 1];
                tableau[j + 1] = temp;
            }
        }
    }
}

/* Quicksort : O(n log n) en moyenne */
int partition(int *tableau, int bas, int haut) {
    int pivot = tableau[haut];
    int i = bas - 1;

    for (int j = bas; j < haut; j++) {
        if (tableau[j] < pivot) {
            i++;
            int temp = tableau[i];
            tableau[i] = tableau[j];
            tableau[j] = temp;
        }
    }

    int temp = tableau[i + 1];
    tableau[i + 1] = tableau[haut];
    tableau[haut] = temp;

    return i + 1;
}

void quicksort(int *tableau, int bas, int haut) {
    if (bas < haut) {
        int pi = partition(tableau, bas, haut);
        quicksort(tableau, bas, pi - 1);
        quicksort(tableau, pi + 1, haut);
    }
}

void tri_rapide(int *tableau, int taille) {
    quicksort(tableau, 0, taille - 1);
}

void generer_aleatoire(int *tableau, int taille) {
    for (int i = 0; i < taille; i++) {
        tableau[i] = rand();
    }
}

int main(void) {
    int *tableau1 = malloc(TAILLE * sizeof(int));
    int *tableau2 = malloc(TAILLE * sizeof(int));
    if (!tableau1 || !tableau2) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    generer_aleatoire(tableau1, TAILLE);
    memcpy(tableau2, tableau1, TAILLE * sizeof(int));

    clock_t debut, fin;

    /* Tri a bulles */
    debut = clock();
    tri_bulles(tableau1, TAILLE);
    fin = clock();
    printf("Tri a bulles : %.1f ms\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC);

    /* Quicksort */
    debut = clock();
    tri_rapide(tableau2, TAILLE);
    fin = clock();
    printf("Quicksort    : %.1f ms\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC);

    /* Verification */
    int ok = 1;
    for (int i = 0; i < TAILLE; i++) {
        if (tableau1[i] != tableau2[i]) {
            ok = 0;
            break;
        }
    }
    printf("Resultats identiques : %s\n", ok ? "oui" : "non");

    free(tableau1);
    free(tableau2);
    return 0;
}
