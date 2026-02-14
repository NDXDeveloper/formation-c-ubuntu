/* ============================================================================
   Section 1.3 : Normes du langage
   Description : Detection de la version du standard C utilisee
   Fichier source : 03-normes-du-langage.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    #if defined(__STDC__)
        printf("Compilateur conforme au standard C\n");

        #ifdef __STDC_VERSION__
            #if __STDC_VERSION__ == 199409L
                printf("Version: C94 (amendement)\n");
            #elif __STDC_VERSION__ == 199901L
                printf("Version: C99\n");
            #elif __STDC_VERSION__ == 201112L
                printf("Version: C11\n");
            #elif __STDC_VERSION__ == 201710L
                printf("Version: C17\n");
            #elif __STDC_VERSION__ == 202311L
                printf("Version: C23\n");
            #else
                printf("Version: %ld\n", __STDC_VERSION__);
            #endif
        #else
            printf("Version: C89/C90 (pas de __STDC_VERSION__)\n");
        #endif
    #else
        printf("Compilateur non conforme au standard C\n");
    #endif

    return 0;
}
