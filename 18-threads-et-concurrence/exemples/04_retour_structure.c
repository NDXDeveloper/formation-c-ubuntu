/* ============================================================================
   Section 18.2 : Creation et terminaison de threads
   Description : Recuperation d'une structure en valeur de retour
   Fichier source : 02-creation-terminaison.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int count;
    double average;
    char status[20];
} Result;

void *compute_stats(void *arg) {
    (void)arg;
    sleep(1);

    Result *res = malloc(sizeof(Result));
    if (res == NULL) {
        return NULL;
    }

    res->count = 100;
    res->average = 75.5;
    snprintf(res->status, sizeof(res->status), "Completed");

    return res;
}

int main(void) {
    pthread_t tid;
    void *retval;

    printf("Demarrage du calcul...\n");
    pthread_create(&tid, NULL, compute_stats, NULL);

    printf("Attente du resultat...\n");
    pthread_join(tid, &retval);

    if (retval != NULL) {
        Result *result = (Result *)retval;
        printf("Resultats:\n");
        printf("  Count: %d\n", result->count);
        printf("  Average: %.2f\n", result->average);
        printf("  Status: %s\n", result->status);
        free(result);
    } else {
        printf("Erreur lors du calcul\n");
    }

    return 0;
}
