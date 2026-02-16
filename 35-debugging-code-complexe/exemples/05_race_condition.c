/* ============================================================================
   Section 35.3 : Investigation des race conditions
   Description : Bug intentionnel - race condition sur compteur partage
   Fichier source : 03-investigation-race-conditions.md
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

    printf("Counter: %d (attendu: 200000)\n", counter);
    return 0;
}
