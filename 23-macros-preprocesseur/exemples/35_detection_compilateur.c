/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Detection du compilateur (GCC, Clang, MSVC)
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
#if defined(__clang__)
    // Attention : Clang définit aussi __GNUC__, donc tester __clang__ EN PREMIER
    printf("Compilateur: Clang version %d.%d.%d\n",
           __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("Compilateur: GCC version %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("Compilateur: Microsoft Visual C++ version %d\n", _MSC_VER);
#else
    printf("Compilateur: Inconnu\n");
#endif

    return 0;
}
