/* ============================================================================
   Section 25.4 : Assertions
   Description : static_assert pour verifier la taille d'une structure packed
   Fichier source : 04-assertions.md
   Compilation : sans -pedantic (__attribute__((packed)))
   ============================================================================ */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t type;
    uint32_t valeur;
} __attribute__((packed)) Paquet;

/* S'assurer que la structure a la taille attendue */
static_assert(sizeof(Paquet) == 5, "Paquet doit faire 5 octets");

int main(void) {
    printf("sizeof(Paquet) = %zu octets\n", sizeof(Paquet));

    Paquet p = {.type = 1, .valeur = 42};
    printf("Paquet: type=%u, valeur=%u\n", p.type, p.valeur);

    printf("Assertion statique verifiee : Paquet fait bien 5 octets\n");

    return 0;
}
