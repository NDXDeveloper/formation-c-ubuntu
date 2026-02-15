/* ============================================================================
   Section 18.10.4 : Compare-and-swap (CAS)
   Description : Incrementation avec CAS - operation lock-free
   Fichier source : 10.4-compare-and-swap.md
   ============================================================================ */

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

#define NUM_THREADS 4
#define ITERATIONS 100000

atomic_int compteur = 0;

void incrementer_cas(atomic_int *variable) {
    int ancienne_valeur = atomic_load(variable);
    int nouvelle_valeur;

    do {
        nouvelle_valeur = ancienne_valeur + 1;
    } while (!atomic_compare_exchange_weak(
        variable,
        &ancienne_valeur,
        nouvelle_valeur
    ));
}

void *worker(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        incrementer_cas(&compteur);
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Compteur final : %d\n", atomic_load(&compteur));
    printf("Attendu : %d\n", NUM_THREADS * ITERATIONS);

    return 0;
}
