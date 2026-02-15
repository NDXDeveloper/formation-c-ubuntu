/* ============================================================================
   Section 23.7 : X-Macros
   Description : Principe de base des X-Macros avec liste de fruits
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>

// ÉTAPE 1: Définir la liste de fruits
#define FRUIT_LIST \
    X(POMME) \
    X(BANANE) \
    X(ORANGE) \
    X(RAISIN)

// ÉTAPE 2a: Générer l'énumération
#define X(fruit) FRUIT_##fruit,
typedef enum {
    FRUIT_LIST
    FRUIT_COUNT
} Fruit;
#undef X

// ÉTAPE 2b: Générer les noms
#define X(fruit) #fruit,
const char* fruit_names[] = {
    FRUIT_LIST
};
#undef X

// ÉTAPE 3: Utilisation
int main(void) {
    printf("Il y a %d fruits\n", FRUIT_COUNT);

    for (int i = 0; i < FRUIT_COUNT; i++) {
        printf("Fruit %d: %s\n", i, fruit_names[i]);
    }

    return 0;
}
