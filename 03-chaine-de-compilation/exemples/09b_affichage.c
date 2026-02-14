/* ============================================================================
   Section 3.6 : Symboles et table des symboles - Resolution des symboles
   Description : Fichier secondaire de l'exemple multi-fichiers (resolution de symboles)
   Fichier source : 06-symboles.md
   ============================================================================ */
#include <stdio.h>
extern int compteur;

void afficher(void) {
    printf("Compteur: %d\n", compteur);
}
