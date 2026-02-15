/* ============================================================================
   Section 25.4 : Assertions
   Description : static_assert pour verifier la taille des types a la compilation
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

/* Verifier que int fait bien 4 octets sur ce systeme */
static_assert(sizeof(int) == 4, "int doit faire 4 octets");

/* Verifier l'alignement d'un pointeur */
static_assert(sizeof(void*) == 8, "Ce code necessite un systeme 64 bits");

int main(void) {
    printf("sizeof(int)   = %zu octets\n", sizeof(int));
    printf("sizeof(void*) = %zu octets\n", sizeof(void*));
    printf("Toutes les assertions statiques sont passees\n");

    return 0;
}
