/* ============================================================================
   Section 10.2 : Alignement memoire et padding
   Description : Visualiser le padding avec offsetof
   Fichier source : 02-alignement-padding.md
   ============================================================================ */

#include <stdio.h>
#include <stddef.h>

struct Exemple {
    char c;
    int i;
    char c2;
};

int main(void) {
    printf("Taille totale : %zu octets\n\n", sizeof(struct Exemple));

    printf("Offset de c  : %zu\n", offsetof(struct Exemple, c));
    printf("Offset de i  : %zu\n", offsetof(struct Exemple, i));
    printf("Offset de c2 : %zu\n", offsetof(struct Exemple, c2));

    printf("\nPadding apres c  : %zu octets\n",
           offsetof(struct Exemple, i) - offsetof(struct Exemple, c) - sizeof(char));
    printf("Padding apres c2 : %zu octets\n",
           sizeof(struct Exemple) - offsetof(struct Exemple, c2) - sizeof(char));

    return 0;
}
