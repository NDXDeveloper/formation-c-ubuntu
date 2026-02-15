/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Directive #ifdef/#else pour branche alternative
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#define PRODUCTION  // Commentez cette ligne pour tester

int main(void) {
#ifdef PRODUCTION
    printf("Mode PRODUCTION - Logs désactivés\n");
#else
    printf("Mode DÉVELOPPEMENT - Logs activés\n");
    printf("Informations de débogage détaillées...\n");
#endif

    return 0;
}
