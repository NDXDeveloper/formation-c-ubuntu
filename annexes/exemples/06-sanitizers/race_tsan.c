// Ligne 289-310 : race condition (TSan)
#include <pthread.h>
#include <stdio.h>

int counter = 0;  // Variable partagée non protégée

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // Race condition !
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter: %d\n", counter);
    return 0;
}
