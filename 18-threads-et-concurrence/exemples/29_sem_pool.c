/* ============================================================================
   Section 18.9 : Semaphores
   Description : Pool de connexions avec semaphore compteur
   Fichier source : 09-semaphores.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CONNEXIONS 3
#define NUM_CLIENTS 10

sem_t pool_connexions;

void *client(void *arg) {
    int id = *(int *)arg;

    printf("Client %d : Demande connexion\n", id);

    sem_wait(&pool_connexions);

    printf("Client %d : Connexion obtenue, utilise...\n", id);
    sleep(2);

    printf("Client %d : Libere connexion\n", id);

    sem_post(&pool_connexions);

    return NULL;
}

int main(void) {
    pthread_t clients[NUM_CLIENTS];
    int ids[NUM_CLIENTS];

    sem_init(&pool_connexions, 0, MAX_CONNEXIONS);

    printf("Pool de %d connexions disponibles\n", MAX_CONNEXIONS);

    for (int i = 0; i < NUM_CLIENTS; i++) {
        ids[i] = i;
        pthread_create(&clients[i], NULL, client, &ids[i]);
        usleep(100000);
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    sem_destroy(&pool_connexions);

    return 0;
}
