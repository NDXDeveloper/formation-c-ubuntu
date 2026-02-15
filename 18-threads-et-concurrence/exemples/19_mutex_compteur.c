/* ============================================================================
   Section 18.6 : Mutex et sections critiques
   Description : Compteur partage protege par mutex (corrige la race condition)
   Fichier source : 06-mutex.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define ITERATIONS 1000000

int compteur = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incrementer(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        compteur++;
        pthread_mutex_unlock(&mutex);
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

    printf("Attendu : %d\n", NUM_THREADS * ITERATIONS);
    printf("Obtenu  : %d\n", compteur);

    return 0;
}
