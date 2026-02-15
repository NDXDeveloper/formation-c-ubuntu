/* ============================================================================
   Section 18.1 : Introduction a pthread
   Description : Premier programme multi-threade - Hello World
   Fichier source : 01-introduction-pthread.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg) {
    (void)arg;
    printf("Hello depuis le thread secondaire !\n");
    printf("ID du thread : %lu\n", pthread_self());
    sleep(1);
    printf("Le thread secondaire se termine.\n");
    return NULL;
}

int main(void) {
    pthread_t thread_id;
    int result;

    printf("Hello depuis le thread principal (main) !\n");
    printf("ID du thread principal : %lu\n", pthread_self());

    result = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (result != 0) {
        fprintf(stderr, "Erreur lors de la creation du thread\n");
        return EXIT_FAILURE;
    }

    printf("Thread secondaire cree avec succes (ID: %lu)\n", thread_id);

    result = pthread_join(thread_id, NULL);
    if (result != 0) {
        fprintf(stderr, "Erreur lors de l'attente du thread\n");
        return EXIT_FAILURE;
    }

    printf("Le thread principal se termine.\n");
    return EXIT_SUCCESS;
}
