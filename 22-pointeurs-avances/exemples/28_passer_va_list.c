/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Passer va_list a des fonctions auxiliaires
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>

// Fonction auxiliaire qui prend une va_list
int somme_va(int count, va_list args) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(args, int);
    }
    return total;
}

// Fonction principale variadique
int somme(int count, ...) {
    va_list args;
    va_start(args, count);

    int resultat = somme_va(count, args);

    va_end(args);
    return resultat;
}

// Autre fonction qui reutilise la logique
int somme_double(int count, ...) {
    va_list args;
    va_start(args, count);

    int resultat = somme_va(count, args) * 2;

    va_end(args);
    return resultat;
}

int main(void) {
    printf("Somme : %d\n", somme(3, 10, 20, 30));
    printf("Somme doublee : %d\n", somme_double(3, 10, 20, 30));
    return 0;
}
