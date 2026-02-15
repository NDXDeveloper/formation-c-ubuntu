/* ============================================================================
   Section 18.12 : Thread-local storage
   Description : pthread_key_t avec destructeur automatique pour donnees par thread
   Fichier source : 12-thread-local-storage.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_key_t thread_data_key;

typedef struct {
    int id;
    int compteur;
} ThreadData;

void cleanup_thread_data(void *data) {
    printf("Nettoyage des donnees du thread\n");
    free(data);
}

void *thread_func(void *arg) {
    int id = *(int *)arg;

    ThreadData *data = malloc(sizeof(ThreadData));
    data->id = id;
    data->compteur = 0;

    pthread_setspecific(thread_data_key, data);

    for (int i = 0; i < 5; i++) {
        ThreadData *my_data = pthread_getspecific(thread_data_key);

        my_data->compteur++;
        printf("Thread %d : compteur = %d\n",
               my_data->id, my_data->compteur);

        sleep(1);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[3];
    int ids[] = {1, 2, 3};

    pthread_key_create(&thread_data_key, cleanup_thread_data);

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_key_delete(thread_data_key);

    return 0;
}
