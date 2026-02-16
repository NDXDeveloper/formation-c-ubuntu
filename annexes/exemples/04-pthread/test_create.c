// Ligne 50-70 : pthread_create
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void *ma_fonction(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d en cours\n", id);
    return NULL;
}

int main() {
    pthread_t thread;
    int id = 1;

    int ret = pthread_create(&thread, NULL, ma_fonction, &id);
    if (ret != 0) {
        fprintf(stderr, "Erreur création thread: %s\n", strerror(ret));
        return 1;
    }

    pthread_join(thread, NULL);  // Attendre la fin du thread
    return 0;
}
