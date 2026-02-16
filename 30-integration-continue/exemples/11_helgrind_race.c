/* ============================================================================
   Section 30.1.6 : Analyse Valgrind automatisee
   Description : Bug intentionnel - race condition (pour Helgrind)
   Fichier source : 01.6-analyse-valgrind.md
   ============================================================================ */
#include <pthread.h>
#include <stdio.h>

int counter = 0;  /* Variable partagee non protegee */

void *increment(void *arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        counter++;  /* Race condition ! */
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter: %d\n", counter);
    return 0;
}
