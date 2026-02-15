/* ============================================================================
   Section 18.6 : Mutex et sections critiques
   Description : Compte bancaire protege par mutex (corrige le check-then-act)
   Fichier source : 06-mutex.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

double solde = 1000.0;
pthread_mutex_t mutex_compte = PTHREAD_MUTEX_INITIALIZER;

void *retirer(void *arg) {
    double montant = *(double *)arg;

    pthread_mutex_lock(&mutex_compte);

    if (solde >= montant) {
        printf("Verification OK pour %.2f EUR\n", montant);
        sleep(1);
        solde -= montant;
        printf("Retrait de %.2f EUR effectue\n", montant);
    } else {
        printf("Solde insuffisant pour %.2f EUR\n", montant);
    }

    pthread_mutex_unlock(&mutex_compte);

    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    double montant1 = 600.0;
    double montant2 = 600.0;

    pthread_create(&t1, NULL, retirer, &montant1);
    pthread_create(&t2, NULL, retirer, &montant2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Solde final : %.2f EUR\n", solde);

    return 0;
}
