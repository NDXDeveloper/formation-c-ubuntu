/* ============================================================================
   Section 26.5 : Integer overflow
   Description : Detection d'overflow avec les builtins GCC
                 (__builtin_add_overflow, __builtin_mul_overflow, etc.)
   Fichier source : 05-integer-overflow.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

static bool addition_builtin(int a, int b, int *result) {
    if (__builtin_add_overflow(a, b, result)) {
        fprintf(stderr, "  Overflow detecte : %d + %d\n", a, b);
        return false;
    }
    return true;
}

static bool multiplication_builtin(int a, int b, int *result) {
    if (__builtin_mul_overflow(a, b, result)) {
        fprintf(stderr, "  Overflow detecte : %d * %d\n", a, b);
        return false;
    }
    return true;
}

static bool soustraction_builtin(int a, int b, int *result) {
    if (__builtin_sub_overflow(a, b, result)) {
        fprintf(stderr, "  Overflow detecte : %d - %d\n", a, b);
        return false;
    }
    return true;
}

int main(void) {
    printf("=== Detection d'overflow avec builtins GCC ===\n\n");

    int result;

    /* Test addition */
    printf("--- Addition ---\n");
    if (addition_builtin(100, 200, &result)) {
        printf("  100 + 200 = %d -> OK\n", result);
    }
    if (addition_builtin(INT_MAX, 1, &result)) {
        printf("  INT_MAX + 1 = %d\n", result);
    } else {
        printf("  INT_MAX + 1 -> REFUSE\n");
    }
    if (addition_builtin(INT_MIN, -1, &result)) {
        printf("  INT_MIN + (-1) = %d\n", result);
    } else {
        printf("  INT_MIN + (-1) -> REFUSE\n");
    }

    /* Test multiplication */
    printf("\n--- Multiplication ---\n");
    if (multiplication_builtin(100, 200, &result)) {
        printf("  100 * 200 = %d -> OK\n", result);
    }
    if (multiplication_builtin(100000, 30000, &result)) {
        printf("  100000 * 30000 = %d\n", result);
    } else {
        printf("  100000 * 30000 -> REFUSE\n");
    }
    if (multiplication_builtin(INT_MAX, 2, &result)) {
        printf("  INT_MAX * 2 = %d\n", result);
    } else {
        printf("  INT_MAX * 2 -> REFUSE\n");
    }

    /* Test soustraction */
    printf("\n--- Soustraction ---\n");
    if (soustraction_builtin(100, 50, &result)) {
        printf("  100 - 50 = %d -> OK\n", result);
    }
    if (soustraction_builtin(INT_MIN, 1, &result)) {
        printf("  INT_MIN - 1 = %d\n", result);
    } else {
        printf("  INT_MIN - 1 -> REFUSE\n");
    }

    /* Exemple pratique : allocation securisee */
    printf("\n--- Allocation securisee ---\n");
    size_t a = 1000000;
    size_t b = 4;  /* sizeof(int) */
    size_t taille;

    if (__builtin_mul_overflow(a, b, &taille)) {
        printf("  %zu * %zu -> Overflow\n", a, b);
    } else {
        printf("  %zu * %zu = %zu octets -> OK pour malloc\n", a, b, taille);
    }

    printf("\n=== Fin des tests builtins ===\n");
    return 0;
}
