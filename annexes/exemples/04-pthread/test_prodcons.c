// Ligne 370-394 : producteur-consommateur (ajout main + includes)
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int buffer = 0;
int buffer_plein = 0;

void *producteur(void *arg) {
    (void)arg;
    pthread_mutex_lock(&mutex);
    buffer = 42;
    buffer_plein = 1;
    pthread_cond_signal(&cond);  // Signale que le buffer est prêt
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *consommateur(void *arg) {
    (void)arg;
    pthread_mutex_lock(&mutex);
    while (!buffer_plein) {
        pthread_cond_wait(&cond, &mutex);  // Attend les données
    }
    printf("Données reçues : %d\n", buffer);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t tp, tc;
    pthread_create(&tc, NULL, consommateur, NULL);
    pthread_create(&tp, NULL, producteur, NULL);
    pthread_join(tp, NULL);
    pthread_join(tc, NULL);
    return 0;
}
