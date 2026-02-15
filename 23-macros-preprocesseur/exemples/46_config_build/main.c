/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Programme principal utilisant la configuration de build
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>
#include "config.h"

void fonction_critique(int* ptr) {
    ASSERT(ptr != NULL);
    LOG_DEBUG("Entrée dans fonction_critique");

    *ptr = 42;

    LOG_INFO("Valeur assignée");
}

int main(void) {
    printf("=== Configuration du build ===\n");

#if DEBUG
    printf("Mode: DEBUG\n");
#else
    printf("Mode: RELEASE\n");
#endif

    printf("Niveau de logging: %d\n", LOGGING_LEVEL);
    printf("Optimisation: %d\n", OPTIMISATION);
    printf("Assertions: %s\n", ASSERTIONS_ENABLED ? "Oui" : "Non");
    printf("\n");

    LOG_INFO("Démarrage du programme");

    int valeur = 0;
    fonction_critique(&valeur);

    printf("Valeur finale: %d\n", valeur);

    LOG_INFO("Programme terminé");

    (void)PROFILING_ENABLED;

    return 0;
}
