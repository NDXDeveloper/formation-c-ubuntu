/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Generation de fonctions de conversion avec # et ##
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

// Macro pour créer des fonctions de conversion
#define DEFINE_CONVERTER(from, to) \
    double from##_to_##to(double value) { \
        printf("Conversion " #from " -> " #to ": %.2f\n", value); \
        return value; /* Conversion simplifiée */ \
    }

DEFINE_CONVERTER(celsius, fahrenheit)
DEFINE_CONVERTER(metres, kilometres)
DEFINE_CONVERTER(euros, dollars)

int main(void) {
    celsius_to_fahrenheit(25.0);
    metres_to_kilometres(1500.0);
    euros_to_dollars(100.0);

    return 0;
}
