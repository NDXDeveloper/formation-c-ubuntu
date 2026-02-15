/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Generation automatique de fonctions getter avec ##
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

// Macro qui génère des fonctions getter
#define DEFINE_GETTER(type, name) \
    type get_##name(void) { \
        return name; \
    }

// Variables globales (pour l'exemple)
static int age = 30;
static double salaire = 45000.50;

// Génération automatique des getters
DEFINE_GETTER(int, age)
DEFINE_GETTER(double, salaire)

// Équivalent à écrire :
// int get_age(void) { return age; }
// double get_salaire(void) { return salaire; }

int main(void) {
    printf("Âge : %d\n", get_age());
    printf("Salaire : %.2f\n", get_salaire());

    return 0;
}
