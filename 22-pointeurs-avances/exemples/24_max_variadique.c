/* ============================================================================
   Section 22.5 : Fonctions variadiques
   Description : Trouver le maximum parmi un nombre variable d'entiers
   Fichier source : 05-fonctions-variadiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

int max(int count, ...) {
    if (count <= 0) {
        return INT_MIN;  // Valeur par defaut pour liste vide
    }

    va_list args;
    va_start(args, count);

    int maximum = va_arg(args, int);  // Premier element

    for (int i = 1; i < count; i++) {
        int valeur = va_arg(args, int);
        if (valeur > maximum) {
            maximum = valeur;
        }
    }

    va_end(args);
    return maximum;
}

int main(void) {
    printf("Max(3, 7, 2) = %d\n", max(3, 3, 7, 2));
    printf("Max(5, 15, 8, 23, 12, 19) = %d\n", max(5, 15, 8, 23, 12, 19));

    return 0;
}
