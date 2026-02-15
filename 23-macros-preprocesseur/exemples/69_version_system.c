/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : Systeme de version automatique avec macros
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>

// Définir les informations de version
#define VERSION_MAJOR 2
#define VERSION_MINOR 5
#define VERSION_PATCH 3

// Créer une chaîne de version
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define VERSION_STRING \
    TO_STRING(VERSION_MAJOR) "." \
    TO_STRING(VERSION_MINOR) "." \
    TO_STRING(VERSION_PATCH)

// Numéro de version sous forme d'entier
#define VERSION_NUMBER \
    ((VERSION_MAJOR * 10000) + (VERSION_MINOR * 100) + VERSION_PATCH)

// Build info complète
#define BUILD_INFO \
    "v" VERSION_STRING " (compilé le " __DATE__ " à " __TIME__ ")"

void afficher_version(void) {
    printf("Programme version %s\n", VERSION_STRING);
    printf("Build: %s\n", BUILD_INFO);
    printf("Version numérique: %d\n", VERSION_NUMBER);
}

// Vérification de version minimale
#define VERSION_AT_LEAST(major, minor, patch) \
    (VERSION_NUMBER >= ((major) * 10000 + (minor) * 100 + (patch)))

int main(void) {
    afficher_version();
    printf("\n");

    // Tests de version
    if (VERSION_AT_LEAST(2, 5, 0)) {
        printf("Version 2.5.0 ou supérieure\n");
    }

    if (VERSION_AT_LEAST(3, 0, 0)) {
        printf("Version 3.0.0 ou supérieure\n");
    } else {
        printf("Version inférieure à 3.0.0\n");
    }

    return 0;
}
