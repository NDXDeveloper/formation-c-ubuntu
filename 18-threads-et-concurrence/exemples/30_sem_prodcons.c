/* ============================================================================
   Section 18.9 : Semaphores
   Description : Producteur-consommateur avec semaphores (buffer circulaire)
   Fichier source : 09-semaphores.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty;
sem_t full;

void *producteur(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        sem_wait(&empty);

        buffer[in] = i;
        printf("Produit : %d (position %d)\n", i, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&full);

        usleep(100000);
    }
    return NULL;
}

void *consommateur(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        sem_wait(&full);

        int item = buffer[out];
        printf("Consomme : %d (position %d)\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&empty);

        usleep(200000);
    }
    return NULL;
}

int main(void) {
    pthread_t prod, cons;

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    pthread_create(&prod, NULL, producteur, NULL);
    pthread_create(&cons, NULL, consommateur, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
