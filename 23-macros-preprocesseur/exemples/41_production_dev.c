/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Code de production vs developpement avec assertions
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>
#include <assert.h>

// En production, NDEBUG est défini
// #define NDEBUG

#ifndef NDEBUG
    #define VERIFICATION_ACTIVE 1
#else
    #define VERIFICATION_ACTIVE 0
#endif

void traiter_donnees(int* data, size_t taille) {
#if VERIFICATION_ACTIVE
    // Vérifications coûteuses en mode debug uniquement
    printf("Vérification des données...\n");
    assert(data != NULL);
    assert(taille > 0);

    for (size_t i = 0; i < taille; i++) {
        assert(data[i] >= 0);  // Vérifier que toutes les valeurs sont positives
    }
#endif

    // Traitement principal
    printf("Traitement de %zu éléments\n", taille);
}

int main(void) {
    int donnees[] = {1, 2, 3, 4, 5};
    traiter_donnees(donnees, 5);

    return 0;
}
