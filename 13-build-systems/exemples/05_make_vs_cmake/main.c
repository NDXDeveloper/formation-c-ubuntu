/*
 * Section  : 03 - Make vs CMake
 * Description : Calculatrice comparant Make et CMake
 * Fichier source : 05_make_vs_cmake/main.c
 */

#include <stdio.h>
#include "operations.h"

int main(void) {
    double a = 10.0, b = 3.0;

    printf("Calculatrice - Make vs CMake\n");
    printf("============================\n");
    printf("%.1f + %.1f = %.2f\n", a, b, additionner(a, b));
    printf("%.1f - %.1f = %.2f\n", a, b, soustraire(a, b));
    printf("%.1f * %.1f = %.2f\n", a, b, multiplier(a, b));
    printf("%.1f / %.1f = %.2f\n", a, b, diviser(a, b));

    return 0;
}
