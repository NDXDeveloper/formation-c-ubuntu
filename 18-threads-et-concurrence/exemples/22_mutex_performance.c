/* ============================================================================
   Section 18.6 : Mutex et sections critiques
   Description : Mesure de l'overhead d'un mutex (benchmark)
   Fichier source : 06-mutex.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int compteur = 0;

int main(void) {
    struct timespec start, end;

    /* Sans mutex */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        compteur++;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    long ns_sans = (end.tv_sec - start.tv_sec) * 1000000000L +
                   (end.tv_nsec - start.tv_nsec);

    /* Avec mutex */
    compteur = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        compteur++;
        pthread_mutex_unlock(&mutex);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    long ns_avec = (end.tv_sec - start.tv_sec) * 1000000000L +
                   (end.tv_nsec - start.tv_nsec);

    printf("Sans mutex : %ld ns (%.2f ns/op)\n",
           ns_sans, (double)ns_sans / ITERATIONS);
    printf("Avec mutex : %ld ns (%.2f ns/op)\n",
           ns_avec, (double)ns_avec / ITERATIONS);
    printf("Overhead   : %.2fx\n", (double)ns_avec / (double)ns_sans);

    return 0;
}
