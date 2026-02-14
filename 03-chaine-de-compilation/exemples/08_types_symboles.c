/* ============================================================================
   Section 3.6 : Symboles et table des symboles
   Description : Illustration des differents types de symboles (D, B, R, d, t, T, U)
   Fichier source : 06-symboles.md
   Note : Compile avec -c pour analyse avec nm (gcc -c 08_types_symboles.c)
   ============================================================================ */
#include <stdio.h>

/* Variables globales */
int var_init = 42;              /* D - data, initialisee */
int var_non_init;               /* B - bss, non initialisee */
const int constante = 100;      /* R - rodata, lecture seule */

/* Variables statiques (locales au fichier) */
static int var_static = 10;     /* d - data, locale */
static void helper(void) { }    /* t - text, locale */

/* Fonction globale */
void publique(void) {           /* T - text, globale */
    printf("Hello\n");          /* U - undefined (externe) */
}

int main(void) {                /* T - text, globale */
    return 0;
}
