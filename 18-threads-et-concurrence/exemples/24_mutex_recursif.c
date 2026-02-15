/* ============================================================================
   Section 18.7 : Deadlocks
   Description : Mutex recursif pour eviter l'auto-deadlock
   Fichier source : 07-deadlocks.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex;

void fonction_interne(void) {
    pthread_mutex_lock(&mutex);
    printf("  fonction_interne : mutex obtenu\n");
    pthread_mutex_unlock(&mutex);
}

void fonction_externe(void) {
    pthread_mutex_lock(&mutex);
    printf("fonction_externe : mutex obtenu\n");

    /* Avec mutex recursif, ceci ne cause pas de deadlock */
    fonction_interne();

    pthread_mutex_unlock(&mutex);
    printf("fonction_externe : mutex libere\n");
}

int main(void) {
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int result = pthread_mutex_init(&mutex, &attr);
    if (result != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(result));
        return EXIT_FAILURE;
    }
    pthread_mutexattr_destroy(&attr);

    printf("Test mutex recursif :\n");
    fonction_externe();

    printf("\nTest double lock direct :\n");
    pthread_mutex_lock(&mutex);
    printf("Premier lock OK\n");
    pthread_mutex_lock(&mutex);
    printf("Deuxieme lock OK (recursif)\n");
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex);
    printf("Deux unlocks OK\n");

    pthread_mutex_destroy(&mutex);
    return 0;
}
