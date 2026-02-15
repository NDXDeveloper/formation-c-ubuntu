/* ============================================================================
   Section 27.4 : Cache awareness (cas d'etude)
   Description : Comparaison de 3 versions de somme de tableau : naive,
                 deroulage de boucle (loop unrolling) et multi-accumulateurs
                 (instruction-level parallelism)
   Fichier source : 04-cache-awareness.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 10000000  /* 10 millions */
#define ITERATIONS 10

/* Version 1 : Naive */
long long somme_naive(const int *tableau) {
    long long somme = 0;
    for (int i = 0; i < TAILLE; i++) {
        somme += tableau[i];
    }
    return somme;
}

/* Version 2 : Deroulage de boucle (4 elements a la fois) */
long long somme_unroll(const int *tableau) {
    long long somme = 0;
    int i;

    /* Traiter 4 elements a la fois */
    for (i = 0; i < TAILLE - 3; i += 4) {
        somme += tableau[i];
        somme += tableau[i + 1];
        somme += tableau[i + 2];
        somme += tableau[i + 3];
    }

    /* Reste */
    for (; i < TAILLE; i++) {
        somme += tableau[i];
    }
    return somme;
}

/* Version 3 : Multi-accumulateurs (parallisme ILP) */
long long somme_multi_acc(const int *tableau) {
    long long somme1 = 0, somme2 = 0, somme3 = 0, somme4 = 0;
    int i;

    /* 4 accumulateurs independants */
    for (i = 0; i < TAILLE - 3; i += 4) {
        somme1 += tableau[i];
        somme2 += tableau[i + 1];
        somme3 += tableau[i + 2];
        somme4 += tableau[i + 3];
    }

    long long somme = somme1 + somme2 + somme3 + somme4;

    /* Reste */
    for (; i < TAILLE; i++) {
        somme += tableau[i];
    }
    return somme;
}

int main(void) {
    int *tableau = malloc(TAILLE * sizeof(int));
    if (!tableau) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = i;
    }

    clock_t debut, fin;
    long long resultat;

    /* Version naive */
    debut = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        resultat = somme_naive(tableau);
    }
    fin = clock();
    printf("Naive          : %.1f ms  (somme = %lld)\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS,
           resultat);

    /* Version unroll */
    debut = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        resultat = somme_unroll(tableau);
    }
    fin = clock();
    printf("Loop unrolling : %.1f ms  (somme = %lld)\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS,
           resultat);

    /* Version multi-accumulateurs */
    debut = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        resultat = somme_multi_acc(tableau);
    }
    fin = clock();
    printf("Multi-accum.   : %.1f ms  (somme = %lld)\n",
           (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS,
           resultat);

    free(tableau);
    return 0;
}
