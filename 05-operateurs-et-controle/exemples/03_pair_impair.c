/* ============================================================================
   Section 5.1 : Operateurs arithmetiques
   Description : Test de parite avec l'operateur modulo
   Fichier source : 01-operateurs-arithmetiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int nombre = 42;

    if (nombre % 2 == 0) {
        printf("%d est pair\n", nombre);
    } else {
        printf("%d est impair\n", nombre);
    }

    return 0;
}
