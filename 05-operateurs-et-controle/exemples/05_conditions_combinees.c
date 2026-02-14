/* ============================================================================
   Section 5.2 : Operateurs logiques
   Description : Combinaison de conditions avec && || et !
   Fichier source : 02-operateurs-logiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int temperature = 22;
    int humidite = 65;
    int pluie = 0;

    if ((temperature >= 15 && temperature <= 25) &&
        (humidite < 80) &&
        !pluie) {
        printf("Excellentes conditions pour une randonnée !\n");
    } else {
        printf("Les conditions ne sont pas idéales.\n");
    }

    return 0;
}
