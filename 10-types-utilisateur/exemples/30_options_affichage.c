/* ============================================================================
   Section 10.7 : Structures bit-fields
   Description : Options d'affichage avec bit-fields (gras, italique, etc.)
   Fichier source : 07-bit-fields.md
   ============================================================================ */

#include <stdio.h>

struct OptionsAffichage {
    unsigned int gras      : 1;
    unsigned int italique  : 1;
    unsigned int souligne  : 1;
    unsigned int barree    : 1;
};

void afficher_options(struct OptionsAffichage opt) {
    printf("Options actives : ");
    if (opt.gras)     printf("Gras ");
    if (opt.italique) printf("Italique ");
    if (opt.souligne) printf("Souligne ");
    if (opt.barree)   printf("Barre ");
    printf("\n");
}

int main(void) {
    struct OptionsAffichage texte = {
        .gras = 1,
        .italique = 1,
        .souligne = 0,
        .barree = 0
    };

    printf("Taille : %zu octet(s)\n", sizeof(struct OptionsAffichage));
    afficher_options(texte);

    texte.souligne = 1;
    afficher_options(texte);

    return 0;
}
