/* ============================================================================
   Section 18.4 : Threads detaches vs joinable
   Description : Creation d'un thread directement detache via attributs
   Fichier source : 04-detaches-vs-joinable.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d demarre (cree detache)\n", id);
    sleep(2);
    printf("Thread %d termine\n", id);
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_attr_t attr;
    int id = 1;
    int result;

    result = pthread_attr_init(&attr);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_attr_init: %s\n", strerror(result));
        return 1;
    }

    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_attr_setdetachstate: %s\n",
                strerror(result));
        pthread_attr_destroy(&attr);
        return 1;
    }

    result = pthread_create(&tid, &attr, worker, &id);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_create: %s\n", strerror(result));
        pthread_attr_destroy(&attr);
        return 1;
    }

    pthread_attr_destroy(&attr);

    printf("Thread detache cree\n");

    sleep(3);

    return 0;
}
