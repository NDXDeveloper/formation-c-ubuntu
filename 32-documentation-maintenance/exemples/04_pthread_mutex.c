/*
 * Section  : 32.5.3 - Modernisation (concurrence pre-C11)
 * Description : Threads POSIX avec mutex pour proteger donnees partagees
 * Fichier source : 05.3-modernisation.md
 */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4
#define INCREMENTS 100000

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int shared_data = 0;

static void *worker(void *arg) {
    (void)arg;
    for (int i = 0; i < INCREMENTS; i++) {
        pthread_mutex_lock(&lock);
        shared_data++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void) {
    printf("=== Concurrence avec pthread + mutex ===\n");
    printf("Threads: %d, Increments/thread: %d\n\n", NUM_THREADS, INCREMENTS);

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    int expected = NUM_THREADS * INCREMENTS;
    printf("Resultat : %d (attendu: %d)\n", shared_data, expected);
    printf("Correct  : %s\n", (shared_data == expected) ? "Oui" : "Non");

    return 0;
}
