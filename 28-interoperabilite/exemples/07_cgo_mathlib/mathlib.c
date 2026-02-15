/* ============================================================================
   Section 28.2 : Appeler du C depuis Go
   Description : Bibliotheque C mathematique (factorielle, racine cubique)
                 utilisee depuis Go via cgo
   Fichier source : 02-c-depuis-go.md
   ============================================================================ */

#include "mathlib.h"
#include <math.h>

int factorielle(int n) {
    if (n <= 1) return 1;
    return n * factorielle(n - 1);
}

double racine_cubique(double x) {
    return cbrt(x);
}
