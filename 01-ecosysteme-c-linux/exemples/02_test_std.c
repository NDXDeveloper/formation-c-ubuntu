/* ============================================================================
   Section 1.3.2 : Verification du support compilateur
   Description : Programme de test du standard C detecte
   Fichier source : 03.2-verification-support-compilateur.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    #ifdef __STDC_VERSION__
        printf("Standard C détecté : ");

        #if __STDC_VERSION__ == 199901L
            printf("C99\n");
        #elif __STDC_VERSION__ == 201112L
            printf("C11\n");
        #elif __STDC_VERSION__ == 201710L
            printf("C17\n");
        #elif __STDC_VERSION__ == 202311L
            printf("C23\n");
        #else
            printf("Version : %ld\n", __STDC_VERSION__);
        #endif
    #else
        printf("C90 ou antérieur\n");
    #endif

    return 0;
}
