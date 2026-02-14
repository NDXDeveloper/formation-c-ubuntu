/* ============================================================================
   Section 3.2 : Visualisation du pipeline
   Description : Programme avec macros AUTEUR/ANNEE pour visualiser le pipeline
   Fichier source : 02-visualisation-pipeline.md
   ============================================================================ */
#include <stdio.h>

#define AUTEUR "Alice"
#define ANNEE 2025

/* Fonction principale */
int main(void) {
    printf("Programme créé par %s en %d\n", AUTEUR, ANNEE);
    return 0;
}
