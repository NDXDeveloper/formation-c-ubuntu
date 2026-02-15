/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Macros predefinies standards (__FILE__, __LINE__, etc.)
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    printf("Fichier: %s\n", __FILE__);
    printf("Ligne: %d\n", __LINE__);
    printf("Date de compilation: %s\n", __DATE__);
    printf("Heure de compilation: %s\n", __TIME__);
    printf("Fonction: %s\n", __func__);  // C99+

#ifdef __STDC__
    printf("Compilateur conforme au standard C\n");
#endif

#ifdef __STDC_VERSION__
    printf("Version du standard: %ld\n", __STDC_VERSION__);
#endif

    return 0;
}
