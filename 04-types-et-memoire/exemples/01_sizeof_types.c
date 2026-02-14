/* ============================================================================
   Section 4.1 : Types primitifs et leur taille
   Description : Verification de la taille des types avec sizeof
   Fichier source : 01-types-primitifs.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    printf("Taille de char : %zu octets\n", sizeof(char));
    printf("Taille de int : %zu octets\n", sizeof(int));
    printf("Taille de float : %zu octets\n", sizeof(float));
    printf("Taille de double : %zu octets\n", sizeof(double));
    printf("Taille de long long : %zu octets\n", sizeof(long long));

    return 0;
}
