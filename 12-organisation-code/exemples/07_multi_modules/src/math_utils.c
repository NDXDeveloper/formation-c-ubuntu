#include "math_utils.h"

int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}

int puissance(int base, int exposant) {
    int resultat = 1;
    for (int i = 0; i < exposant; i++) {
        resultat *= base;
    }
    return resultat;
}
