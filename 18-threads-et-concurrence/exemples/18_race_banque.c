/* ============================================================================
   Section 18.5 : Race conditions
   Description : Race condition check-then-act sur un compte bancaire
   Fichier source : 05-race-conditions.md
   NOTE : Programme volontairement bugue (pedagogique)
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

double solde = 1000.0;

void *retirer(void *arg) {
    double montant = *(double *)arg;

    /* RACE CONDITION : Check-then-act */
    if (solde >= montant) {
        sleep(1);  /* Simule verifications */
        solde -= montant;
        printf("Retrait de %.2f OK\n", montant);
    } else {
        printf("Solde insuffisant\n");
    }

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

    printf("Solde final : %.2f\n", solde);

    return 0;
}
