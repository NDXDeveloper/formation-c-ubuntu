/* ============================================================================
   Section 10.5 : enum - Code lisible et gestion d'etats
   Description : Iteration sur enumeration avec compteur NB_JOURS
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
    DIMANCHE,
    NB_JOURS
};

const char* noms_jours[] = {
    "Lundi",
    "Mardi",
    "Mercredi",
    "Jeudi",
    "Vendredi",
    "Samedi",
    "Dimanche"
};

int main(void) {
    printf("Jours de la semaine :\n");

    for (enum Jour j = LUNDI; j < NB_JOURS; j++) {
        printf("%d : %s\n", j, noms_jours[j]);
    }

    return 0;
}
