// Ligne 474-505 : sémaphore complet
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *worker(void *arg) {
    sem_wait(&sem);  // Acquiert le sémaphore
    printf("Thread %d travaille\n", *(int*)arg);
    sleep(1);
    sem_post(&sem);  // Libère le sémaphore
    return NULL;
}

int main() {
    sem_init(&sem, 0, 2);  // Max 2 threads simultanés

    pthread_t threads[5];
    int ids[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem);
    return 0;
}
