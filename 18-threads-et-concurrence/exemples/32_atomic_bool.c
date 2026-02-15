/* ============================================================================
   Section 18.10.2 : Types atomiques
   Description : atomic_bool comme flag d'arret gracieux entre threads
   Fichier source : 10.2-types-atomiques.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>

atomic_bool continuer = true;
atomic_int compteur_travail = 0;

void *travailleur(void *arg) {
    int id = *(int *)arg;

    while (atomic_load(&continuer)) {
        atomic_fetch_add(&compteur_travail, 1);
        usleep(1000);
    }

    printf("Thread %d s'arrete.\n", id);
    return NULL;
}

int main(void) {
    pthread_t threads[3];
    int ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, travailleur, &ids[i]);
    }

    sleep(2);

    printf("Envoi du signal d'arret...\n");
    atomic_store(&continuer, false);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Travail total effectue : %d\n", atomic_load(&compteur_travail));

    return 0;
}
