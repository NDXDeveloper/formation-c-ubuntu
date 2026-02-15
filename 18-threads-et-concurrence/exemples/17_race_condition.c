/* ============================================================================
   Section 18.5 : Race conditions
   Description : Demonstration d'une race condition sur un compteur partage
   Fichier source : 05-race-conditions.md
   NOTE : Programme volontairement bugue (pedagogique)
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define ITERATIONS 1000000

int compteur = 0;

void *incrementer(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        compteur++;  /* RACE CONDITION ! */
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementer, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Valeur attendue : %d\n", NUM_THREADS * ITERATIONS);
    printf("Valeur obtenue  : %d\n", compteur);

    return 0;
}
