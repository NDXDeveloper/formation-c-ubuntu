/* ============================================================================
   Section 18.10.3 : Memory ordering
   Description : Pattern acquire/release pour synchronisation producteur-consommateur
   Fichier source : 10.3-memory-ordering.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>

#define TAILLE_BUFFER 10

int buffer[TAILLE_BUFFER];
atomic_int compteur_ecrit = 0;

void *producteur(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        while (atomic_load_explicit(&compteur_ecrit, memory_order_acquire)
               >= TAILLE_BUFFER) {
            usleep(1000);
        }

        int index = i % TAILLE_BUFFER;
        buffer[index] = i * 10;

        printf("Producteur ecrit : %d\n", i * 10);

        atomic_fetch_add_explicit(&compteur_ecrit, 1, memory_order_release);

        usleep(100000);
    }
    return NULL;
}

void *consommateur(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        while (atomic_load_explicit(&compteur_ecrit, memory_order_acquire) == 0) {
            usleep(1000);
        }

        int index = i % TAILLE_BUFFER;
        int valeur = buffer[index];

        printf("Consommateur lit : %d\n", valeur);

        atomic_fetch_sub_explicit(&compteur_ecrit, 1, memory_order_release);

        usleep(150000);
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
