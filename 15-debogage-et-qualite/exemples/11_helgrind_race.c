/* ============================================================================
   Section 05.1 : Helgrind
   Description : Race condition detectee par Helgrind
   Fichier source : 05.1-helgrind.md
   ============================================================================ */

#include <pthread.h>
#include <stdio.h>

int compteur = 0;  /* Variable partagee */

void* incrementer(void* arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        compteur++;  /* Pas de protection ! */
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
    /* Attendu : 200000, reel : imprevisible */
    return 0;
}
