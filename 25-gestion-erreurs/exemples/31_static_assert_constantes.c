/* ============================================================================
   Section 25.4 : Assertions
   Description : static_assert pour verifier des constantes et enumerations
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <assert.h>
#include <stdio.h>

#define TAILLE_BUFFER 1024
#define TAILLE_MIN 512

/* Verifier que le buffer est assez grand */
static_assert(TAILLE_BUFFER >= TAILLE_MIN,
              "Le buffer doit faire au moins 512 octets");

typedef enum {
    PRIORITE_BASSE = 0,
    PRIORITE_NORMALE = 1,
    PRIORITE_HAUTE = 2
} Priorite;

/* Verifier que l'ordre est correct */
static_assert(PRIORITE_BASSE < PRIORITE_NORMALE,
              "Ordre des priorites invalide");
static_assert(PRIORITE_NORMALE < PRIORITE_HAUTE,
              "Ordre des priorites invalide");

int main(void) {
    char buffer[TAILLE_BUFFER];
    (void)buffer;

    printf("TAILLE_BUFFER = %d (>= %d : OK)\n", TAILLE_BUFFER, TAILLE_MIN);
    printf("PRIORITE_BASSE=%d < PRIORITE_NORMALE=%d < PRIORITE_HAUTE=%d\n",
           PRIORITE_BASSE, PRIORITE_NORMALE, PRIORITE_HAUTE);
    printf("Toutes les assertions statiques sont passees\n");

    return 0;
}
