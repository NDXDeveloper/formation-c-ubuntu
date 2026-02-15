/* ============================================================================
   Section 27.5 : Branch prediction
   Description : Condition previsible (tableau trie) vs imprevisible (aleatoire)
                 pour demontrer l'impact de la prediction de branchement
   Fichier source : 05-branch-prediction.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 10000000

int compare_int(const void *a, const void *b) {
    return (*(const int *)a - *(const int *)b);
}

int main(void) {
    int *tableau = malloc(TAILLE * sizeof(int));
    if (!tableau) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    /* Initialiser avec valeurs aleatoires */
    srand(42);  /* Seed fixe pour reproductibilite */
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand() % 256;
    }

    clock_t debut, fin;
    long long somme;

    /* Test 1 : Condition previsible (valeurs triees) */
    qsort(tableau, TAILLE, sizeof(int), compare_int);

    debut = clock();
    somme = 0;
    for (int i = 0; i < TAILLE; i++) {
        if (tableau[i] >= 128) {  /* Previsible car trie */
            somme += tableau[i];
        }
    }
    fin = clock();

    printf("Condition previsible (trie) :\n");
    printf("  Somme : %lld\n", somme);
    printf("  Temps : %.3f ms\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC);

    /* Test 2 : Condition imprevisible (valeurs aleatoires) */
    srand(42);
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand() % 256;
    }

    debut = clock();
    somme = 0;
    for (int i = 0; i < TAILLE; i++) {
        if (tableau[i] >= 128) {  /* Imprevisible ! */
            somme += tableau[i];
        }
    }
    fin = clock();

    printf("\nCondition imprevisible (aleatoire) :\n");
    printf("  Somme : %lld\n", somme);
    printf("  Temps : %.3f ms\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC);

    free(tableau);
    return 0;
}
