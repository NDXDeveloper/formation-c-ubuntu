/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Generation de getters et setters complets avec ##
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define DEFINE_PROPERTY(type, name) \
    static type _##name; \
    type get_##name(void) { return _##name; } \
    void set_##name(type value) { _##name = value; }

// Génère automatiquement :
// - Une variable privée _id
// - Un getter get_id()
// - Un setter set_id()
DEFINE_PROPERTY(int, id)
DEFINE_PROPERTY(double, prix)

int main(void) {
    set_id(12345);
    set_prix(99.99);

    printf("ID : %d\n", get_id());
    printf("Prix : %.2f\n", get_prix());

    return 0;
}
