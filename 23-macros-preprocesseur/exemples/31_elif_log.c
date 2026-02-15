/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Directive #elif - chaine de conditions (niveaux de log)
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#define NIVEAU_LOG 2
// 0 = Aucun, 1 = Erreurs, 2 = Warnings, 3 = Info, 4 = Debug

int main(void) {
#if NIVEAU_LOG == 0
    printf("Logs désactivés\n");
#elif NIVEAU_LOG == 1
    printf("Logs: Erreurs uniquement\n");
#elif NIVEAU_LOG == 2
    printf("Logs: Erreurs + Warnings\n");
#elif NIVEAU_LOG == 3
    printf("Logs: Erreurs + Warnings + Info\n");
#elif NIVEAU_LOG >= 4
    printf("Logs: Mode complet (Debug)\n");
#else
    printf("Niveau de log invalide\n");
#endif

    return 0;
}
