/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : X-macros pour associer noms de chaines aux valeurs d'enum
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

// Liste des couleurs
#define COLOR_LIST \
    X(ROUGE) \
    X(VERT) \
    X(BLEU) \
    X(JAUNE)

// Génération de l'enum
#define X(name) COLOR_##name,
typedef enum {
    COLOR_LIST
    COLOR_COUNT
} Color;
#undef X

// Génération du tableau de chaînes
#define X(name) #name,
const char* color_names[] = {
    COLOR_LIST
};
#undef X

int main(void) {
    Color ma_couleur = COLOR_VERT;

    printf("Couleur sélectionnée: %s\n", color_names[ma_couleur]);

    printf("\nToutes les couleurs:\n");
    for (int i = 0; i < COLOR_COUNT; i++) {
        printf("  %d: %s\n", i, color_names[i]);
    }

    return 0;
}
