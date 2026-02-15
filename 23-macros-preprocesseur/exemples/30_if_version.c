/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Directive #if - evaluation d'expressions constantes
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#define VERSION_MAJEURE 2
#define VERSION_MINEURE 5

int main(void) {
#if VERSION_MAJEURE > 1
    printf("Version majeure supérieure à 1\n");
#endif

#if VERSION_MAJEURE == 2 && VERSION_MINEURE >= 5
    printf("Version 2.5 ou supérieure détectée\n");
#endif

    return 0;
}
