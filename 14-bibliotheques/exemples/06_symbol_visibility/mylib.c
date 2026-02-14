/* ============================================================================
   Section 07 : Symbol visibility
   Description : Implementation avec controle de visibilite des symboles
   Fichier source : 07-symbol-visibility.md
   ============================================================================ */

/* MYLIB_BUILDING est defini via -DMYLIB_BUILDING a la compilation */
#include "mylib.h"
#include <stdio.h>

/* Fonction privee (pas dans le header, pas de MYLIB_API) */
static void internal_helper(void) {
    printf("Internal function\n");
}

/* Fonctions publiques (dans le header, avec MYLIB_API) */
void mylib_init(void) {
    printf("Library initialized\n");
    internal_helper();
}

void mylib_process(const char *data) {
    printf("Processing: %s\n", data);
}

int mylib_get_version(void) {
    return 1;
}
