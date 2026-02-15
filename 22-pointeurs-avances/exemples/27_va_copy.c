/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Utilisation de va_copy pour parcourir les arguments deux fois
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>

void traiter_deux_fois(int count, ...) {
    va_list args_original;
    va_list args_copie;

    va_start(args_original, count);
    va_copy(args_copie, args_original);  // Copie independante

    // Premier parcours
    printf("Premier parcours : ");
    for (int i = 0; i < count; i++) {
        printf("%d ", va_arg(args_original, int));
    }
    printf("\n");

    // Deuxieme parcours (avec la copie)
    printf("Deuxieme parcours : ");
    for (int i = 0; i < count; i++) {
        printf("%d ", va_arg(args_copie, int));
    }
    printf("\n");

    va_end(args_original);
    va_end(args_copie);  // Important : nettoyer les deux !
}

int main(void) {
    traiter_deux_fois(4, 10, 20, 30, 40);
    return 0;
}
