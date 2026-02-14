/* ============================================================================
   Section 5.1 : Operateurs arithmetiques
   Description : Calcul de moyenne avec division flottante
   Fichier source : 01-operateurs-arithmetiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int note1 = 15;
    int note2 = 12;
    int note3 = 18;

    float moyenne = (note1 + note2 + note3) / 3.0f;

    printf("Moyenne : %.2f\n", moyenne);

    return 0;
}
