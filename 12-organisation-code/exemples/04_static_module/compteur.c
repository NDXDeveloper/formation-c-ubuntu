/* ============================================================================
   Section 12.2 : La portee des variables
   Description : Module avec etat interne (static fichier) - encapsulation
                 en C via des variables privees au fichier
   Fichier source : 02-portee-variables.md
   ============================================================================ */

#include <stdio.h>

/* Variable privee au module (pas accessible depuis l'exterieur) */
static int compteur_interne = 0;

void incrementer(void) {
    compteur_interne++;
}

void decrementer(void) {
    compteur_interne--;
}

int obtenir_valeur(void) {
    return compteur_interne;
}

void afficher(void) {
    printf("Compteur : %d\n", compteur_interne);
}
