/* ============================================================================
   Section 18.4 : Threads detaches vs joinable
   Description : Thread joinable avec recuperation de valeur de retour
   Fichier source : 04-detaches-vs-joinable.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *calculer_carre(void *arg) {
    int n = *(int *)arg;
    int *resultat = malloc(sizeof(int));
    if (resultat == NULL) {
        return NULL;
    }
    *resultat = n * n;
    return resultat;
}

int main(void) {
    pthread_t tid;
    int nombre = 7;
    void *retval;

    pthread_create(&tid, NULL, calculer_carre, &nombre);
    pthread_join(tid, &retval);

    if (retval != NULL) {
        int carre = *(int *)retval;
        printf("Carre de %d = %d\n", nombre, carre);
        free(retval);
    }

    return 0;
}
