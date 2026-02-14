/* ============================================================================
   Section 1.3.2 : Verification du support compilateur
   Description : Affichage des informations du compilateur via macros
   Fichier source : 03.2-verification-support-compilateur.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    #ifdef __GNUC__
        printf("Compilé avec GCC %d.%d.%d\n",
               __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    #endif

    #ifdef __clang__
        printf("Compilé avec Clang\n");
    #endif

    printf("Version C : %ld\n", __STDC_VERSION__);

    return 0;
}
