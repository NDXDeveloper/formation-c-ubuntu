/* ============================================================================
   Section 30.1.6 : Analyse Valgrind automatisee
   Description : Bug intentionnel - depassement de tampon heap (pour Valgrind)
   Fichier source : 01.6-analyse-valgrind.md
   ============================================================================ */
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *buffer = malloc(10);
    strcpy(buffer, "This is a very long string");  /* Depasse 10 ! */
    free(buffer);
    return 0;
}
