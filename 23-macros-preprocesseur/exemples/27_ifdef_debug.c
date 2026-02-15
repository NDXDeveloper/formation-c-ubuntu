/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Directive #ifdef - inclusion de code si macro definie
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#define DEBUG

int main(void) {
    int x = 42;

#ifdef DEBUG
    printf("Mode debug activé\n");
    printf("Valeur de x: %d\n", x);
#endif

    printf("Programme en cours d'exécution\n");

    return 0;
}
