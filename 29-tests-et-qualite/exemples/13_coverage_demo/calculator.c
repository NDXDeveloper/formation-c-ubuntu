/* ============================================================================
   Section 29.6 : Mesure de couverture
   Description : Calculatrice pour demonstration gcov (implementation)
   Fichier source : 06-mesure-couverture.md
   ============================================================================ */
#include "calculator.h"
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide_safe(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return 0;
    }
    return a / b;
}
