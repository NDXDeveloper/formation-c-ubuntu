/* ============================================================================
   Section 18.13 : Barrieres de threads
   Description : Barriere simple - synchronisation de 5 threads entre deux phases
   Fichier source : 13-barrieres-threads.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

pthread_barrier_t barriere;

void *worker(void *arg) {
    int id = *(int *)arg;

    printf("[Thread %d] Phase 1 : Preparation...\n", id);
    sleep(id);
    printf("[Thread %d] Phase 1 terminee\n", id);

    printf("[Thread %d] Attente a la barriere...\n", id);
    pthread_barrier_wait(&barriere);

    printf("[Thread %d] Phase 2 : Traitement\n", id);

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    pthread_barrier_init(&barriere, NULL, NUM_THREADS);

    printf("=== Demarrage de %d threads ===\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barriere);

    printf("=== Programme termine ===\n");

    return 0;
}
