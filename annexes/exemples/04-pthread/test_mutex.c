// Ligne 252-280 : mutex complet
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
int compteur_partage = 0;

void *incrementer(void *arg) {
    (void)arg;
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&mutex);
        compteur_partage++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, incrementer, NULL);
    pthread_create(&t2, NULL, incrementer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Compteur final : %d\n", compteur_partage);  // Devrait être 2000

    pthread_mutex_destroy(&mutex);
    return 0;
}
