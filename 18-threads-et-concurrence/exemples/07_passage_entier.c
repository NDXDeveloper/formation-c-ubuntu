/* ============================================================================
   Section 18.3 : Passage de parametres
   Description : Trois methodes de passage d'un entier a un thread
   Fichier source : 03-passage-parametres.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Methode 1 : Passage par adresse */
void *afficher_par_adresse(void *arg) {
    int nombre = *(int *)arg;
    printf("[Adresse] Thread recu : %d\n", nombre);
    return NULL;
}

/* Methode 2 : Cast direct */
void *afficher_par_cast(void *arg) {
    int nombre = (int)(long)arg;
    printf("[Cast]    Thread recu : %d\n", nombre);
    return NULL;
}

/* Methode 3 : Allocation dynamique */
void *afficher_par_malloc(void *arg) {
    int nombre = *(int *)arg;
    printf("[Malloc]  Thread recu : %d\n", nombre);
    free(arg);
    return NULL;
}

int main(void) {
    pthread_t tid1, tid2, tid3;
    int valeur = 42;

    /* Methode 1 */
    pthread_create(&tid1, NULL, afficher_par_adresse, &valeur);
    pthread_join(tid1, NULL);

    /* Methode 2 */
    pthread_create(&tid2, NULL, afficher_par_cast, (void *)(long)valeur);
    pthread_join(tid2, NULL);

    /* Methode 3 */
    int *val_dyn = malloc(sizeof(int));
    if (val_dyn == NULL) {
        fprintf(stderr, "Erreur malloc\n");
        return 1;
    }
    *val_dyn = valeur;
    pthread_create(&tid3, NULL, afficher_par_malloc, val_dyn);
    pthread_join(tid3, NULL);

    return 0;
}
