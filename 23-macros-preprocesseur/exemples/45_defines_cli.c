/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Definitions de macros depuis la ligne de commande (-D)
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
#ifdef DEBUG
    printf("Mode DEBUG activé\n");
#endif

#ifdef FEATURE_A
    printf("Feature A activée\n");
#endif

#ifndef VERSION
    #define VERSION 1
#endif
    printf("Version: %d\n", VERSION);

    return 0;
}
