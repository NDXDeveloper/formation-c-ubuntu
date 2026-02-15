/* ============================================================================
   Section 18.13 : Barrieres de threads
   Description : Implementation manuelle d'une barriere avec mutex et cond variable
   Fichier source : 13-barrieres-threads.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int arrived;
    int generation;
} Barrier;

void barrier_init(Barrier *b, int count) {
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cond, NULL);
    b->count = count;
    b->arrived = 0;
    b->generation = 0;
}

void barrier_wait(Barrier *b) {
    pthread_mutex_lock(&b->mutex);

    int my_generation = b->generation;

    b->arrived++;

    if (b->arrived == b->count) {
        b->arrived = 0;
        b->generation++;
        pthread_cond_broadcast(&b->cond);
    } else {
        while (my_generation == b->generation) {
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    }

    pthread_mutex_unlock(&b->mutex);
}

void barrier_destroy(Barrier *b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cond);
}

Barrier barriere;

void *test_worker(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        printf("[Thread %d] Iteration %d, avant barriere\n", id, i);

        barrier_wait(&barriere);

        printf("[Thread %d] Iteration %d, apres barriere\n", id, i);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4];

    barrier_init(&barriere, 4);

    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, test_worker, &ids[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&barriere);

    return 0;
}
