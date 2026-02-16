/*
 * Section  : 32.5.3 - Modernisation (atomics C11)
 * Description : Operations atomiques C11 sans mutex (lock-free)
 * Fichier source : 05.3-modernisation.md
 */

#include <stdio.h>
#include <stdatomic.h>
#include <threads.h>

#define NUM_THREADS 4
#define INCREMENTS 100000

static atomic_int shared_data = 0;

static int worker(void *arg) {
    (void)arg;
    for (int i = 0; i < INCREMENTS; i++) {
        atomic_fetch_add(&shared_data, 1);
    }
    return 0;
}

int main(void) {
    printf("=== Concurrence avec C11 atomics (lock-free) ===\n");
    printf("Threads: %d, Increments/thread: %d\n\n", NUM_THREADS, INCREMENTS);

    thrd_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    int result = atomic_load(&shared_data);
    int expected = NUM_THREADS * INCREMENTS;
    printf("Resultat : %d (attendu: %d)\n", result, expected);
    printf("Correct  : %s\n", (result == expected) ? "Oui" : "Non");
    printf("Lock-free: %s\n", atomic_is_lock_free(&shared_data) ? "Oui" : "Non");

    return 0;
}
