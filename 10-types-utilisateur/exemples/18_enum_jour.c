/* ============================================================================
   Section 10.5 : enum - Code lisible et gestion d'etats
   Description : Premier exemple d'enumeration (jours de la semaine)
   Fichier source : 05-enumerations.md
   ============================================================================ */

#include <stdio.h>

enum Jour {
    LUNDI,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE
};

int main(void) {
    enum Jour aujourd_hui = MERCREDI;

    printf("Aujourd'hui est le jour numero : %d\n", aujourd_hui);

    if (aujourd_hui == MERCREDI) {
        printf("C'est mercredi !\n");
    }

    return 0;
}
