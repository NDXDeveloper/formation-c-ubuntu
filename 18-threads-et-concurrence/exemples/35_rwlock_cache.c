/* ============================================================================
   Section 18.11 : Read-write locks
   Description : Cache de configuration avec lectures partagees / ecritures exclusives
   Fichier source : 11-read-write-locks.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char config[256];
    int version;
    pthread_rwlock_t rwlock;
} ConfigCache;

ConfigCache cache = {
    .config = "config_initiale",
    .version = 1,
    .rwlock = PTHREAD_RWLOCK_INITIALIZER
};

void *lecteur(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&cache.rwlock);

        printf("[Lecteur %d] Config: %s (v%d)\n",
               id, cache.config, cache.version);

        pthread_rwlock_unlock(&cache.rwlock);

        usleep(100000);
    }

    return NULL;
}

void *ecrivain(void *arg) {
    int id = *(int *)arg;

    sleep(1);

    pthread_rwlock_wrlock(&cache.rwlock);

    printf("[Ecrivain %d] Mise a jour de la config...\n", id);
    snprintf(cache.config, sizeof(cache.config),
             "nouvelle_config_%d", id);
    cache.version++;
    sleep(1);

    printf("[Ecrivain %d] Mise a jour terminee\n", id);

    pthread_rwlock_unlock(&cache.rwlock);

    return NULL;
}

int main(void) {
    pthread_t lecteurs[5], ecrivains[2];
    int ids_lect[5], ids_ecr[2];

    for (int i = 0; i < 5; i++) {
        ids_lect[i] = i;
        pthread_create(&lecteurs[i], NULL, lecteur, &ids_lect[i]);
    }

    for (int i = 0; i < 2; i++) {
        ids_ecr[i] = i;
        pthread_create(&ecrivains[i], NULL, ecrivain, &ids_ecr[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(lecteurs[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(ecrivains[i], NULL);
    }

    pthread_rwlock_destroy(&cache.rwlock);

    return 0;
}
