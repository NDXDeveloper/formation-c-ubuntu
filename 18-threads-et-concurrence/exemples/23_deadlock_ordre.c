/* ============================================================================
   Section 18.7 : Deadlocks
   Description : Prevention de deadlock par tri des mutex (transfert bancaire)
   Fichier source : 07-deadlocks.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int solde;
    pthread_mutex_t mutex;
    const char *nom;
} Compte;

Compte compte_A;
Compte compte_B;

void transfert(Compte *source, Compte *dest, int montant) {
    pthread_mutex_t *premier, *second;

    /* Trier les mutex par adresse memoire pour eviter le deadlock */
    if (&source->mutex < &dest->mutex) {
        premier = &source->mutex;
        second = &dest->mutex;
    } else {
        premier = &dest->mutex;
        second = &source->mutex;
    }

    pthread_mutex_lock(premier);
    pthread_mutex_lock(second);

    if (source->solde >= montant) {
        source->solde -= montant;
        dest->solde += montant;
        printf("Transfert de %d EUR de %s vers %s\n",
               montant, source->nom, dest->nom);
    } else {
        printf("Solde insuffisant sur %s\n", source->nom);
    }

    pthread_mutex_unlock(second);
    pthread_mutex_unlock(premier);
}

void *transfert_A_vers_B(void *arg) {
    (void)arg;
    for (int i = 0; i < 5; i++) {
        transfert(&compte_A, &compte_B, 100);
        usleep(100000);
    }
    return NULL;
}

void *transfert_B_vers_A(void *arg) {
    (void)arg;
    for (int i = 0; i < 5; i++) {
        transfert(&compte_B, &compte_A, 50);
        usleep(100000);
    }
    return NULL;
}

int main(void) {
    compte_A.solde = 1000;
    compte_A.nom = "Compte A";
    pthread_mutex_init(&compte_A.mutex, NULL);

    compte_B.solde = 1000;
    compte_B.nom = "Compte B";
    pthread_mutex_init(&compte_B.mutex, NULL);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, transfert_A_vers_B, NULL);
    pthread_create(&t2, NULL, transfert_B_vers_A, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nSolde final A : %d EUR\n", compte_A.solde);
    printf("Solde final B : %d EUR\n", compte_B.solde);
    printf("Total : %d EUR (doit etre 2000)\n",
           compte_A.solde + compte_B.solde);

    pthread_mutex_destroy(&compte_A.mutex);
    pthread_mutex_destroy(&compte_B.mutex);

    return 0;
}
