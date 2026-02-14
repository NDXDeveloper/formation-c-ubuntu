/* ============================================================================
   Section 1.3.2 : Verification du support compilateur
   Description : Test du support des types atomiques (C11)
   Fichier source : 03.2-verification-support-compilateur.md
   ============================================================================ */
#include <stdio.h>
#include <stdatomic.h>

int main(void) {
    atomic_int counter = 0;
    atomic_fetch_add(&counter, 5);
    printf("Compteur atomique : %d\n", atomic_load(&counter));
    return 0;
}
