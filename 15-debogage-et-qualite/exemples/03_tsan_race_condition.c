/* ============================================================================
   Section 01.3 : ThreadSanitizer (TSan)
   Description : Race condition - acces concurrent non protege
   Fichier source : 01.3-thread-sanitizer.md
   ============================================================================ */

#include <pthread.h>
#include <stdio.h>

int compteur = 0;  /* Variable partagee */

void* incrementer(void* arg) {
    (void)arg;
    for (int i = 0; i < 1000000; i++) {
        compteur++;  /* Race condition : acces concurrent non protege */
    }
    return NULL;
}

int main(void) {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, incrementer, NULL);
    pthread_create(&thread2, NULL, incrementer, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Compteur final : %d\n", compteur);
    /* Devrait etre 2000000, mais sera aleatoire */
    return 0;
}
