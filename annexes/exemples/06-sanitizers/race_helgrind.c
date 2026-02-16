// Ligne 561-578 : race condition (Helgrind)
#include <pthread.h>
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 1000; i++)
        counter++;  // Race !
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
