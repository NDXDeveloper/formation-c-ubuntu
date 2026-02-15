/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : Affichage complet des informations de build
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>

void afficher_build_info(void) {
    printf("========================================\n");
    printf("Informations de Build\n");
    printf("========================================\n\n");

    // Informations de compilation
    printf("Date de compilation : %s\n", __DATE__);
    printf("Heure de compilation : %s\n", __TIME__);
    printf("Fichier : %s\n", __FILE__);
    printf("\n");

    // Standard C
    printf("Standard C : ");
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 202311L
        printf("C23+\n");
    #elif __STDC_VERSION__ >= 201710L
        printf("C17\n");
    #elif __STDC_VERSION__ >= 201112L
        printf("C11\n");
    #elif __STDC_VERSION__ >= 199901L
        printf("C99\n");
    #else
        printf("C95\n");
    #endif
#else
    printf("C89/C90\n");
#endif
    printf("\n");

    // Compilateur
    printf("Compilateur : ");
#if defined(__clang__)
    printf("Clang %d.%d.%d\n", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("MSVC %d\n", _MSC_VER);
#else
    printf("Inconnu\n");
#endif
    printf("\n");

    // Système d'exploitation
    printf("OS cible : ");
#if defined(_WIN32)
    printf("Windows\n");
#elif defined(__APPLE__)
    printf("macOS\n");
#elif defined(__linux__)
    printf("Linux\n");
#elif defined(__unix__)
    printf("Unix\n");
#else
    printf("Inconnu\n");
#endif
    printf("\n");

    // Architecture
    printf("Architecture : ");
#if defined(__x86_64__) || defined(_M_X64)
    printf("x86_64\n");
#elif defined(__i386__) || defined(_M_IX86)
    printf("x86\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("ARM64\n");
#elif defined(__arm__) || defined(_M_ARM)
    printf("ARM\n");
#else
    printf("Inconnue\n");
#endif

    printf("Taille du pointeur : %zu octets\n", sizeof(void*));
    printf("\n");

    // Configuration de build
    printf("Configuration : ");
#ifdef NDEBUG
    printf("Release (optimisé)\n");
#else
    printf("Debug\n");
#endif

    printf("========================================\n");
}

int main(void) {
    afficher_build_info();
    return 0;
}
