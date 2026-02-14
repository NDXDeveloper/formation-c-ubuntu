/* ============================================================================
   Section 10.2 : Alignement memoire et padding
   Description : Comparaison avant/apres optimisation de l'ordre des membres
   Fichier source : 02-alignement-padding.md
   ============================================================================ */

#include <stdio.h>

struct Avant {
    char c;
    double d;
    short s;
    int i;
};

struct Apres {
    double d;
    int i;
    short s;
    char c;
};

int main(void) {
    printf("Avant optimisation : %zu octets\n", sizeof(struct Avant));
    printf("Apres optimisation : %zu octets\n", sizeof(struct Apres));
    printf("Gain               : %zu octets (%.1f%%)\n",
           sizeof(struct Avant) - sizeof(struct Apres),
           100.0 * (sizeof(struct Avant) - sizeof(struct Apres)) / sizeof(struct Avant));
    return 0;
}
