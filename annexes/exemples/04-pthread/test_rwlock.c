// Ligne 567-583 : rwlock (ajout main + includes)
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <pthread.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int donnee_partagee = 0;

void *lecteur(void *arg) {
    (void)arg;
    pthread_rwlock_rdlock(&rwlock);  // Plusieurs lecteurs OK
    printf("Lecture : %d\n", donnee_partagee);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *ecrivain(void *arg) {
    (void)arg;
    pthread_rwlock_wrlock(&rwlock);  // Exclusif
    donnee_partagee++;
    printf("Écriture : %d\n", donnee_partagee);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main(void) {
    pthread_t r1, r2, w1;
    pthread_create(&w1, NULL, ecrivain, NULL);
    pthread_join(w1, NULL);
    pthread_create(&r1, NULL, lecteur, NULL);
    pthread_create(&r2, NULL, lecteur, NULL);
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_rwlock_destroy(&rwlock);
    return 0;
}
