/*
 * Section  : 03 - Make vs CMake
 * Description : Implementation des operations de la calculatrice
 * Fichier source : 05_make_vs_cmake/operations.c
 */

#include <stdio.h>
#include "operations.h"

double additionner(double a, double b) {
    return a + b;
}

double soustraire(double a, double b) {
    return a - b;
}

double multiplier(double a, double b) {
    return a * b;
}

double diviser(double a, double b) {
    if (b == 0.0) {
        printf("Erreur : division par zero\n");
        return 0.0;
    }
    return a / b;
}
