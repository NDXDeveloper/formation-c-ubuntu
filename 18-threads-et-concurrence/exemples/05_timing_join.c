/* ============================================================================
   Section 18.2 : Creation et terminaison de threads
   Description : Demonstration du comportement bloquant de pthread_join
   Fichier source : 02-creation-terminaison.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void *travail_long(void *arg) {
    (void)arg;
    printf("[Thread] Debut a %ld\n", time(NULL));
    sleep(3);
    printf("[Thread] Fin a %ld\n", time(NULL));
    return NULL;
}

int main(void) {
    pthread_t tid;

    printf("[Main] Creation thread a %ld\n", time(NULL));
    pthread_create(&tid, NULL, travail_long, NULL);

    printf("[Main] Avant join a %ld\n", time(NULL));
    pthread_join(tid, NULL);
    printf("[Main] Apres join a %ld\n", time(NULL));

    return 0;
}
