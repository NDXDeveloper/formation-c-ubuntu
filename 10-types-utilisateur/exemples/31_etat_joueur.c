/* ============================================================================
   Section 10.7 : Structures bit-fields
   Description : Etat d'un joueur avec bit-fields (vie, niveau, arme, etc.)
   Fichier source : 07-bit-fields.md
   ============================================================================ */

#include <stdio.h>

struct EtatJoueur {
    unsigned int vie         : 7;
    unsigned int niveau      : 5;
    unsigned int invincible  : 1;
    unsigned int ennemis_tues: 10;
    unsigned int arme        : 3;
};

void afficher_joueur(struct EtatJoueur j) {
    printf("=== ETAT DU JOUEUR ===\n");
    printf("Vie : %u/127\n", j.vie);
    printf("Niveau : %u\n", j.niveau);
    printf("Invincible : %s\n", j.invincible ? "Oui" : "Non");
    printf("Ennemis tues : %u\n", j.ennemis_tues);
    printf("Arme : %u\n", j.arme);
    printf("Taille : %zu octets\n", sizeof(struct EtatJoueur));
}

int main(void) {
    struct EtatJoueur joueur = {
        .vie = 100,
        .niveau = 5,
        .invincible = 0,
        .ennemis_tues = 42,
        .arme = 3
    };

    afficher_joueur(joueur);

    return 0;
}
