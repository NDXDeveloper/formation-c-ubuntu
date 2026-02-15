/* ============================================================================
   Section 18.4 : Threads detaches vs joinable
   Description : Thread detache avec pthread_detach
   Fichier source : 04-detaches-vs-joinable.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void *tache_independante(void *arg) {
    int id = *(int *)arg;

    for (int i = 1; i <= 5; i++) {
        printf("Thread %d : etape %d/5\n", id, i);
        sleep(1);
    }

    printf("Thread %d termine\n", id);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int id = 1;

    printf("Creation du thread\n");
    pthread_create(&tid, NULL, tache_independante, &id);

    printf("Detachement du thread\n");
    int result = pthread_detach(tid);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_detach: %s\n", strerror(result));
        return 1;
    }

    printf("Thread principal continue (pas de join necessaire)\n");

    sleep(6);

    printf("Programme termine\n");
    return 0;
}
