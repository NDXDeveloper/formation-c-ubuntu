/* ============================================================================
   Section 4.2 : Signed vs Unsigned
   Description : Macros de limites des types entiers (<limits.h>)
   Fichier source : 02-signed-vs-unsigned.md
   ============================================================================ */
#include <limits.h>
#include <stdio.h>

int main(void) {
    printf("CHAR_MIN : %d\n", CHAR_MIN);
    printf("CHAR_MAX : %d\n", CHAR_MAX);
    printf("UCHAR_MAX : %d\n", UCHAR_MAX);

    printf("SHRT_MIN : %d\n", SHRT_MIN);
    printf("SHRT_MAX : %d\n", SHRT_MAX);
    printf("USHRT_MAX : %d\n", USHRT_MAX);

    printf("INT_MIN : %d\n", INT_MIN);
    printf("INT_MAX : %d\n", INT_MAX);
    printf("UINT_MAX : %u\n", UINT_MAX);

    return 0;
}
