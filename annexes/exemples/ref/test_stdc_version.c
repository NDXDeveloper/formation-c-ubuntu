// Ligne 386-411 : détection du standard C
#include <stdio.h>

int main(void) {
#ifdef __STDC_VERSION__
    printf("Standard C détecté : ");

    #if __STDC_VERSION__ == 199409L
        printf("C94\n");
    #elif __STDC_VERSION__ == 199901L
        printf("C99\n");
    #elif __STDC_VERSION__ == 201112L
        printf("C11\n");
    #elif __STDC_VERSION__ == 201710L
        printf("C17\n");
    #elif __STDC_VERSION__ > 201710L
        printf("C23 (ou plus récent)\n");
    #else
        printf("Version inconnue\n");
    #endif
#else
    printf("Standard C89/C90 (pré-C99)\n");
#endif

    return 0;
}
