/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Directive #ifndef - valeur par defaut si macro non definie
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
#ifndef VERSION
    printf("Version non définie, utilisation de la version par défaut\n");
    #define VERSION "1.0.0"
#endif

    printf("Version du programme: %s\n", VERSION);

    return 0;
}
