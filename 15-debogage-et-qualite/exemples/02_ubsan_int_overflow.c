/* ============================================================================
   Section 01.2 : UndefinedBehaviorSanitizer (UBSan)
   Description : Debordement d'entier signe - comportement indefini
   Fichier source : 01.2-undefined-behavior-sanitizer.md
   ============================================================================ */

#include <stdio.h>
#include <limits.h>

int main(void) {
    int max = INT_MAX;  /* 2147483647 */

    int resultat = max + 1;  /* Comportement indefini */

    printf("Résultat : %d\n", resultat);
    return 0;
}
