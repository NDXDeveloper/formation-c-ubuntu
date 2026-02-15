/* ============================================================================
   Section 18.8 : Variables de condition
   Description : Producteur-consommateur simple avec variables de condition
   Fichier source : 08-variables-condition.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
} Buffer;

Buffer buf = {
    .count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond_not_empty = PTHREAD_COND_INITIALIZER,
    .cond_not_full = PTHREAD_COND_INITIALIZER
};

void *producteur(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&buf.mutex);

        while (buf.count == BUFFER_SIZE) {
            printf("Producteur: Buffer plein, j'attends\n");
            pthread_cond_wait(&buf.cond_not_full, &buf.mutex);
        }

        buf.buffer[buf.count++] = i;
        printf("Produit : %d (count=%d)\n", i, buf.count);

        pthread_cond_signal(&buf.cond_not_empty);

        pthread_mutex_unlock(&buf.mutex);

        usleep(100000);
    }
    return NULL;
}

void *consommateur(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&buf.mutex);

        while (buf.count == 0) {
            printf("Consommateur: Buffer vide, j'attends\n");
            pthread_cond_wait(&buf.cond_not_empty, &buf.mutex);
        }

        int item = buf.buffer[--buf.count];
        printf("Consomme : %d (count=%d)\n", item, buf.count);

        pthread_cond_signal(&buf.cond_not_full);

        pthread_mutex_unlock(&buf.mutex);

        usleep(200000);
    }
    return NULL;
}

int main(void) {
    pthread_t prod, cons;

    pthread_create(&prod, NULL, producteur, NULL);
    pthread_create(&cons, NULL, consommateur, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
