/* ============================================================================
   Section 31.4 : QEMU pour tester les binaires
   Description : Programme multi-thread pour test QEMU avec pthread
   Fichier source : 04-qemu-tests.md
   ============================================================================ */
#include <stdio.h>
#include <pthread.h>

void *thread_func(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d s'execute\n", id);
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tous les threads ont termine\n");
    return 0;
}
