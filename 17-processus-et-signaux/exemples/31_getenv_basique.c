/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Lecture basique d'une variable avec getenv()
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *path = getenv("PATH");

    if (path != NULL) {
        printf("PATH = %s\n", path);
    } else {
        printf("PATH n'est pas defini\n");
    }

    return 0;
}
