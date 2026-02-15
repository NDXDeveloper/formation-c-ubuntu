/* ============================================================================
   Section 18.8 : Variables de condition
   Description : Exemple basique waiter/signaler avec pthread_cond_wait
   Fichier source : 08-variables-condition.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *waiter(void *arg) {
    (void)arg;
    printf("Waiter: J'attends que ready devienne 1...\n");

    pthread_mutex_lock(&mutex);

    while (!ready) {
        printf("Waiter: Condition fausse, je m'endors\n");
        pthread_cond_wait(&cond, &mutex);
        printf("Waiter: Je suis reveille, je verifie la condition\n");
    }

    printf("Waiter: Condition vraie ! Je continue\n");

    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *signaler(void *arg) {
    (void)arg;
    sleep(2);

    printf("Signaler: Je prepare les donnees\n");

    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_mutex_unlock(&mutex);

    printf("Signaler: J'envoie le signal\n");
    pthread_cond_signal(&cond);

    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, waiter, NULL);
    pthread_create(&t2, NULL, signaler, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
