/* ============================================================================
   Section 18.6 : Mutex et sections critiques
   Description : Statistiques partagees avec mutex integre dans la structure
   Fichier source : 06-mutex.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>

typedef struct {
    long total;
    int count;
    double moyenne;
    pthread_mutex_t mutex;
} Stats;

Stats stats;

void init_stats(void) {
    stats.total = 0;
    stats.count = 0;
    stats.moyenne = 0.0;
    pthread_mutex_init(&stats.mutex, NULL);
}

void destroy_stats(void) {
    pthread_mutex_destroy(&stats.mutex);
}

void *ajouter_valeur(void *arg) {
    int valeur = *(int *)arg;

    pthread_mutex_lock(&stats.mutex);

    stats.total += valeur;
    stats.count++;
    stats.moyenne = (double)stats.total / stats.count;

    pthread_mutex_unlock(&stats.mutex);

    return NULL;
}

void afficher_stats(void) {
    pthread_mutex_lock(&stats.mutex);
    printf("Total: %ld, Count: %d, Moyenne: %.2f\n",
           stats.total, stats.count, stats.moyenne);
    pthread_mutex_unlock(&stats.mutex);
}

int main(void) {
    pthread_t threads[10];
    int valeurs[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    init_stats();

    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, ajouter_valeur, &valeurs[i]);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    afficher_stats();

    destroy_stats();
    return 0;
}
