/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Modifier un pointeur dans une fonction via pointeur double
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// INCORRECT : Ne modifie pas le pointeur original
void allouer_incorrect(int *ptr) {
    ptr = malloc(sizeof(int));  // Modifie la copie locale
    if (ptr != NULL) {
        *ptr = 100;
        free(ptr);  // On libere ici car le pointeur sera perdu
    }
}

// CORRECT : Modifie le pointeur original
void allouer_correct(int **ptr) {
    *ptr = malloc(sizeof(int));  // Modifie le pointeur via son adresse
    if (*ptr != NULL) {
        **ptr = 100;
    }
}

int main(void) {
    // Test avec la version incorrecte
    int *mon_ptr = NULL;
    allouer_incorrect(mon_ptr);

    if (mon_ptr == NULL) {
        printf("mon_ptr est toujours NULL ! (version incorrecte)\n");
    }

    // Test avec la version correcte
    allouer_correct(&mon_ptr);  // On passe l'adresse du pointeur

    if (mon_ptr != NULL) {
        printf("Allocation reussie ! Valeur : %d\n", *mon_ptr);
        free(mon_ptr);
    }

    return 0;
}
