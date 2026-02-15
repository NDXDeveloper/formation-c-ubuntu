/* ============================================================================
   Section 26.5 : Integer overflow
   Description : Demonstration des debordements d'entiers (unsigned wraparound)
   Fichier source : 05-integer-overflow.md
   ============================================================================ */

#include <stdio.h>
#include <limits.h>
#include <stdint.h>

int main(void) {
    printf("=== Demonstration des integer overflows ===\n\n");

    /* 1. Overflow unsigned char */
    printf("--- Unsigned char (8 bits, max=%u) ---\n", UCHAR_MAX);
    unsigned char c = 255;
    printf("  c = %d\n", c);
    c = (unsigned char)(c + 1);  /* Wraparound defini */
    printf("  c + 1 = %d (wraparound)\n", c);

    /* 2. Unsigned int underflow */
    printf("\n--- Unsigned int underflow ---\n");
    unsigned int u = 0;
    u = u - 1;  /* Underflow : devient UINT_MAX */
    printf("  0 - 1 (unsigned) = %u (UINT_MAX)\n", u);

    /* 3. Limites des types */
    printf("\n--- Limites des types entiers ---\n");
    printf("  char      : %d a %d (unsigned: 0 a %u)\n",
           CHAR_MIN, CHAR_MAX, UCHAR_MAX);
    printf("  short     : %d a %d (unsigned: 0 a %u)\n",
           SHRT_MIN, SHRT_MAX, USHRT_MAX);
    printf("  int       : %d a %d (unsigned: 0 a %u)\n",
           INT_MIN, INT_MAX, UINT_MAX);
    printf("  long      : %ld a %ld\n", LONG_MIN, LONG_MAX);
    printf("  long long : %lld a %lld\n", LLONG_MIN, LLONG_MAX);
    printf("  size_t    : 0 a %zu\n", SIZE_MAX);

    /* 4. Verification avant allocation */
    printf("\n--- Verification overflow avant malloc ---\n");
    size_t nb_elements = 1000000;
    if (nb_elements > SIZE_MAX / sizeof(int)) {
        printf("  Overflow detecte pour %zu elements !\n", nb_elements);
    } else {
        size_t taille = nb_elements * sizeof(int);
        printf("  %zu elements * %zu octets = %zu octets -> OK\n",
               nb_elements, sizeof(int), taille);
    }

    /* Test avec valeur enorme */
    nb_elements = SIZE_MAX;
    if (nb_elements > SIZE_MAX / sizeof(int)) {
        printf("  Overflow detecte pour SIZE_MAX elements !\n");
    }

    /* 5. Soustraction unsigned securisee */
    printf("\n--- Soustraction unsigned securisee ---\n");
    unsigned int a = 10, b = 20;
    if (a < b) {
        printf("  %u - %u -> Underflow detecte, refuse\n", a, b);
    } else {
        printf("  %u - %u = %u\n", a, b, a - b);
    }

    a = 30;
    b = 20;
    if (a < b) {
        printf("  %u - %u -> Underflow detecte, refuse\n", a, b);
    } else {
        printf("  %u - %u = %u -> OK\n", a, b, a - b);
    }

    printf("\n=== Fin de la demonstration ===\n");
    return 0;
}
