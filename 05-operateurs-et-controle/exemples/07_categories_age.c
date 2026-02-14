/* ============================================================================
   Section 5.2 : Operateurs logiques
   Description : Classification par categories d'age avec && et else if
   Fichier source : 02-operateurs-logiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int age = 16;

    if (age >= 0 && age <= 12) {
        printf("Catégorie : Enfant\n");
    }
    else if (age >= 13 && age <= 17) {
        printf("Catégorie : Adolescent\n");
    }
    else if (age >= 18) {
        printf("Catégorie : Adulte\n");
    }
    else {
        printf("Âge invalide\n");
    }

    return 0;
}
