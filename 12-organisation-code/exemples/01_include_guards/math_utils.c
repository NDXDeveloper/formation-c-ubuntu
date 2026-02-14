#include "math_utils.h"

int carre(int n) {
    return n * n;
}

/* Approximation par methode de Newton (Heron) */
double racine_carree_approx(double x) {
    if (x < 0.0) {
        return -1.0;
    }
    if (x == 0.0) {
        return 0.0;
    }

    double guess = x / 2.0;
    for (int i = 0; i < 20; i++) {
        guess = (guess + x / guess) / 2.0;
    }
    return guess;
}
