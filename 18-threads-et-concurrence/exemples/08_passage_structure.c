/* ============================================================================
   Section 18.3 : Passage de parametres
   Description : Passage de plusieurs parametres via une structure
   Fichier source : 03-passage-parametres.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>

typedef struct {
    int id;
    char nom[50];
    double coefficient;
} ParametresThread;

void *afficher_infos(void *arg) {
    ParametresThread *params = (ParametresThread *)arg;

    printf("Thread ID : %d\n", params->id);
    printf("Nom : %s\n", params->nom);
    printf("Coefficient : %.2f\n", params->coefficient);

    return NULL;
}

int main(void) {
    pthread_t tid;

    ParametresThread params = {
        .id = 1,
        .nom = "Worker",
        .coefficient = 1.5
    };

    pthread_create(&tid, NULL, afficher_infos, &params);
    pthread_join(tid, NULL);

    return 0;
}
