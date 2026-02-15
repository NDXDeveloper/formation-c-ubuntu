/* ============================================================================
   Section 18.12 : Thread-local storage
   Description : _Thread_local - chaque thread a sa propre copie des variables
   Fichier source : 12-thread-local-storage.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

_Thread_local int thread_id = 0;
_Thread_local int compteur = 0;

void *thread_func(void *arg) {
    thread_id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        compteur++;
        printf("Thread %d : compteur = %d\n", thread_id, compteur);
        sleep(1);
    }

    return NULL;
}

int main(void) {
    pthread_t t1, t2, t3;
    int ids[] = {1, 2, 3};

    pthread_create(&t1, NULL, thread_func, &ids[0]);
    pthread_create(&t2, NULL, thread_func, &ids[1]);
    pthread_create(&t3, NULL, thread_func, &ids[2]);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
