/* ============================================================================
   Section 3.1 : Les quatre etapes de la compilation
   Description : Programme hello avec macro #define MESSAGE
   Fichier source : 01-quatre-etapes.md
   ============================================================================ */
#include <stdio.h>

#define MESSAGE "Bonjour le monde!"

int main(void) {
    printf("%s\n", MESSAGE);
    return 0;
}
