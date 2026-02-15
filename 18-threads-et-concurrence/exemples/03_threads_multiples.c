/* ============================================================================
   Section 18.2 : Creation et terminaison de threads
   Description : Creation de plusieurs threads sequentiellement
   Fichier source : 02-creation-terminaison.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

void *print_hello(void *arg) {
    int thread_num = *(int *)arg;
    printf("Hello depuis le thread #%d (ID: %lu)\n",
           thread_num, pthread_self());
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;

        int result = pthread_create(&threads[i], NULL,
                                    print_hello, &thread_args[i]);
        if (result != 0) {
            fprintf(stderr, "Erreur creation thread %d\n", i);
            return 1;
        }
        printf("Thread %d cree\n", i);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tous les threads sont termines\n");
    return 0;
}
