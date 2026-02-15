/* ============================================================================
   Section 18.10.1 : Introduction a stdatomic
   Description : Premier exemple - compteur atomique vs race condition
   Fichier source : 10.1-introduction-stdatomic.md
   ============================================================================ */

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

atomic_int compteur = 0;

void *incrementer(void *arg) {
    (void)arg;
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&compteur, 1);
    }
    return NULL;
}

int main(void) {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, incrementer, NULL);
    pthread_create(&thread2, NULL, incrementer, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    int valeur_finale = atomic_load(&compteur);

    printf("Valeur finale du compteur : %d\n", valeur_finale);
    printf("Valeur attendue : 2000000\n");

    return 0;
}
