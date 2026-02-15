/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Detection de la version du standard C
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    printf("Standard C: ");

#if __STDC_VERSION__ >= 202311L
    printf("C23 ou supérieur\n");
#elif __STDC_VERSION__ >= 201710L
    printf("C17\n");
#elif __STDC_VERSION__ >= 201112L
    printf("C11\n");
#elif __STDC_VERSION__ >= 199901L
    printf("C99\n");
#elif defined(__STDC__)
    printf("C89/C90\n");
#else
    printf("Pre-ANSI C\n");
#endif

    return 0;
}
