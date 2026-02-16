// Ligne 596-602 : Thread-Local Storage __thread (ajout main + includes)
#include <stdio.h>
#include <pthread.h>

__thread int variable_locale_thread = 0;

void *fonction(void *arg) {
    variable_locale_thread = *(int*)arg;  // Chaque thread a sa propre copie
    printf("Thread %lu : %d\n", pthread_self(), variable_locale_thread);
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    int id1 = 10, id2 = 20;
    pthread_create(&t1, NULL, fonction, &id1);
    pthread_create(&t2, NULL, fonction, &id2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
