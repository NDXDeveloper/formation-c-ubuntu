/* ============================================================================
   Section 5.1 : Operateurs arithmetiques
   Description : Division entiere et reste (quotient et modulo)
   Fichier source : 01-operateurs-arithmetiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int dividende = 17;
    int diviseur = 5;

    int quotient = dividende / diviseur;
    int reste = dividende % diviseur;

    printf("%d / %d = %d reste %d\n", dividende, diviseur, quotient, reste);

    return 0;
}
