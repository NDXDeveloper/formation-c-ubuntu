/* ============================================================================
   Section 29.5 : Tests d'integration
   Description : Test d'integration multi-threads (compteur thread-safe)
   Fichier source : 05-tests-integration.md
   ============================================================================ */
#include "unity.h"
#include <stdlib.h>
#include <pthread.h>

/* --- Compteur thread-safe --- */
typedef struct {
    int count;
    pthread_mutex_t mutex;
} Counter;

Counter *counter_create(void) {
    Counter *c = malloc(sizeof(Counter));
    if (c) {
        c->count = 0;
        pthread_mutex_init(&c->mutex, NULL);
    }
    return c;
}

void counter_increment(Counter *c) {
    pthread_mutex_lock(&c->mutex);
    c->count++;
    pthread_mutex_unlock(&c->mutex);
}

int counter_get(Counter *c) {
    pthread_mutex_lock(&c->mutex);
    int value = c->count;
    pthread_mutex_unlock(&c->mutex);
    return value;
}

void counter_destroy(Counter *c) {
    if (c) {
        pthread_mutex_destroy(&c->mutex);
        free(c);
    }
}

/* --- Thread worker --- */
typedef struct {
    Counter *counter;
    int iterations;
} ThreadArg;

void *increment_thread(void *arg) {
    ThreadArg *targ = (ThreadArg *)arg;

    for (int i = 0; i < targ->iterations; i++) {
        counter_increment(targ->counter);
    }

    return NULL;
}

/* --- Tests --- */
void setUp(void) {}
void tearDown(void) {}

void test_counter_create(void) {
    Counter *c = counter_create();
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(0, counter_get(c));
    counter_destroy(c);
}

void test_counter_single_thread(void) {
    Counter *c = counter_create();

    for (int i = 0; i < 100; i++) {
        counter_increment(c);
    }

    TEST_ASSERT_EQUAL_INT(100, counter_get(c));
    counter_destroy(c);
}

void test_counter_thread_safety(void) {
    Counter *counter = counter_create();

    const int NUM_THREADS = 10;
    const int ITERATIONS = 1000;

    pthread_t threads[10];
    ThreadArg arg = {.counter = counter, .iterations = ITERATIONS};

    /* Lancer les threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_thread, &arg);
    }

    /* Attendre la fin */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Verifier le resultat : 10 threads x 1000 = 10000 */
    int final_count = counter_get(counter);
    TEST_ASSERT_EQUAL_INT(NUM_THREADS * ITERATIONS, final_count);

    counter_destroy(counter);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_counter_create);
    RUN_TEST(test_counter_single_thread);
    RUN_TEST(test_counter_thread_safety);

    return UNITY_END();
}
