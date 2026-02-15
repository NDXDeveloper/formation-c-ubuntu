/* ============================================================================
   Section 18.3 : Passage de parametres
   Description : Calcul parallele - somme d'un tableau repartie sur 4 threads
   Fichier source : 03-passage-parametres.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define ARRAY_SIZE 100

typedef struct {
    int thread_id;
    int *tableau;
    int debut;
    int fin;
    int somme;
} ThreadData;

void *calculer_somme_partielle(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int somme = 0;

    printf("Thread %d : calcul de %d a %d\n",
           data->thread_id, data->debut, data->fin);

    for (int i = data->debut; i < data->fin; i++) {
        somme += data->tableau[i];
    }

    data->somme = somme;
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int tableau[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        tableau[i] = i + 1;
    }

    int elements_par_thread = ARRAY_SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].tableau = tableau;
        thread_data[i].debut = i * elements_par_thread;
        thread_data[i].fin = (i + 1) * elements_par_thread;
        thread_data[i].somme = 0;

        pthread_create(&threads[i], NULL,
                      calculer_somme_partielle, &thread_data[i]);
    }

    int somme_totale = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d : somme partielle = %d\n",
               i, thread_data[i].somme);
        somme_totale += thread_data[i].somme;
    }

    printf("\nSomme totale : %d\n", somme_totale);
    printf("Verification : %d (formule n(n+1)/2)\n",
           ARRAY_SIZE * (ARRAY_SIZE + 1) / 2);

    return 0;
}
