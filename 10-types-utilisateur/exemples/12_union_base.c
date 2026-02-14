/* ============================================================================
   Section 10.4 : union - Optimisation memoire
   Description : Exemple basique d'union (partage memoire entre types)
   Fichier source : 04-unions.md
   ============================================================================ */

#include <stdio.h>

union Data {
    int i;
    float f;
    char c;
};

int main(void) {
    union Data data;

    printf("Taille de l'union : %zu octets\n\n", sizeof(union Data));

    data.i = 42;
    printf("Entier : %d\n", data.i);

    data.f = 3.14f;
    printf("Float  : %.2f\n", data.f);
    printf("Entier : %d (valeur invalide !)\n\n", data.i);

    data.c = 'A';
    printf("Char   : %c\n", data.c);
    printf("Float  : %.2f (valeur corrompue, seul le 1er octet a change !)\n", data.f);

    return 0;
}
