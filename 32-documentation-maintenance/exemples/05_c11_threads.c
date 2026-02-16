/*
 * Section  : 32.5.3 - Modernisation (concurrence C11)
 * Description : Threads C11 standards avec mutex (remplace pthread)
 * Fichier source : 05.3-modernisation.md
 */

#include <stdio.h>
#include <threads.h>

#define NUM_THREADS 4
#define INCREMENTS 100000

static mtx_t lock;
static int shared_data = 0;

static int worker(void *arg) {
    (void)arg;
    for (int i = 0; i < INCREMENTS; i++) {
        mtx_lock(&lock);
        shared_data++;
        mtx_unlock(&lock);
    }
    return 0;
}

int main(void) {
    printf("=== Concurrence avec C11 threads + mutex ===\n");
    printf("Threads: %d, Increments/thread: %d\n\n", NUM_THREADS, INCREMENTS);

    mtx_init(&lock, mtx_plain);

    thrd_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    int expected = NUM_THREADS * INCREMENTS;
    printf("Resultat : %d (attendu: %d)\n", shared_data, expected);
    printf("Correct  : %s\n", (shared_data == expected) ? "Oui" : "Non");

    mtx_destroy(&lock);
    return 0;
}
