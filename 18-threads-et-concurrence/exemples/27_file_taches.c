/* ============================================================================
   Section 18.8 : Variables de condition
   Description : File de taches avec arret propre (shutdown pattern)
   Fichier source : 08-variables-condition.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_TASKS 100
#define NUM_WORKERS 3

typedef struct {
    int tasks[MAX_TASKS];
    int count;
    int shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
} TaskQueue;

TaskQueue queue = {
    .count = 0,
    .shutdown = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond_not_empty = PTHREAD_COND_INITIALIZER
};

void *worker(void *arg) {
    int id = *(int *)arg;

    while (1) {
        pthread_mutex_lock(&queue.mutex);

        while (queue.count == 0 && !queue.shutdown) {
            pthread_cond_wait(&queue.cond_not_empty, &queue.mutex);
        }

        if (queue.shutdown && queue.count == 0) {
            pthread_mutex_unlock(&queue.mutex);
            printf("Worker %d : arret\n", id);
            break;
        }

        int task = queue.tasks[--queue.count];
        pthread_mutex_unlock(&queue.mutex);

        printf("Worker %d : traite tache %d\n", id, task);
        usleep(500000);
    }

    return NULL;
}

int main(void) {
    pthread_t workers[NUM_WORKERS];
    int worker_ids[NUM_WORKERS];

    for (int i = 0; i < NUM_WORKERS; i++) {
        worker_ids[i] = i + 1;
        pthread_create(&workers[i], NULL, worker, &worker_ids[i]);
    }

    /* Ajouter des taches */
    for (int i = 1; i <= 10; i++) {
        pthread_mutex_lock(&queue.mutex);
        queue.tasks[queue.count++] = i;
        pthread_mutex_unlock(&queue.mutex);
        pthread_cond_signal(&queue.cond_not_empty);

        usleep(200000);
    }

    /* Signaler l'arret */
    sleep(1);
    printf("\n--- Arret du systeme ---\n\n");

    pthread_mutex_lock(&queue.mutex);
    queue.shutdown = 1;
    pthread_mutex_unlock(&queue.mutex);
    pthread_cond_broadcast(&queue.cond_not_empty);

    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }

    printf("Tous les workers arretes\n");
    return 0;
}
