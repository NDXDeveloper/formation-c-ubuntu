/* ============================================================================
   Section 18.10.2 : Types atomiques
   Description : Histogramme concurrent avec tableau d'atomiques
   Fichier source : 10.2-types-atomiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>

#define NB_BUCKETS 10
#define NB_VALEURS 100000

atomic_int histogramme[NB_BUCKETS];

void *generer_valeurs(void *arg) {
    (void)arg;
    unsigned int seed = (unsigned int)pthread_self();

    for (int i = 0; i < NB_VALEURS; i++) {
        int bucket = (int)(rand_r(&seed) % NB_BUCKETS);
        atomic_fetch_add(&histogramme[bucket], 1);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[4];

    for (int i = 0; i < NB_BUCKETS; i++) {
        atomic_store(&histogramme[i], 0);
    }

    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, generer_valeurs, NULL);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Histogramme des valeurs generees :\n");
    int total = 0;
    for (int i = 0; i < NB_BUCKETS; i++) {
        int count = atomic_load(&histogramme[i]);
        printf("Bucket %d : %d\n", i, count);
        total += count;
    }
    printf("Total : %d (attendu : %d)\n", total, 4 * NB_VALEURS);

    return 0;
}
