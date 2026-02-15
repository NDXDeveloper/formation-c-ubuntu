/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Feature flags - activation/desactivation de fonctionnalites
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

// Activation/désactivation de fonctionnalités
#define FEATURE_ANALYTICS 1
#define FEATURE_CLOUD_SYNC 0
#define FEATURE_EXPORT 1

void sauvegarder_donnees(void) {
    printf("Sauvegarde locale effectuée\n");

#if FEATURE_CLOUD_SYNC
    printf("Synchronisation cloud en cours...\n");
#endif
}

void generer_rapport(void) {
    printf("Génération du rapport...\n");

#if FEATURE_ANALYTICS
    printf("Ajout des statistiques d'usage\n");
#endif

#if FEATURE_EXPORT
    printf("Export en PDF disponible\n");
#endif
}

int main(void) {
    sauvegarder_donnees();
    generer_rapport();

    return 0;
}
