/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Generation de structures et accesseurs avec # et ##
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

// Macro qui génère une structure et ses accesseurs
#define DEFINE_STRUCT(name) \
    typedef struct { \
        int id; \
        char nom[50]; \
    } name##_t; \
    \
    void print_##name(name##_t* obj) { \
        printf(#name " { id=%d, nom=%s }\n", obj->id, obj->nom); \
    }

// Génération automatique
DEFINE_STRUCT(Personne)
DEFINE_STRUCT(Produit)

int main(void) {
    Personne_t p1 = {1, "Alice"};
    Produit_t prod1 = {100, "Laptop"};

    print_Personne(&p1);
    print_Produit(&prod1);

    return 0;
}
