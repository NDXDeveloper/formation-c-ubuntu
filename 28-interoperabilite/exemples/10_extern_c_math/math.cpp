/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Implementation C++ des operations mathematiques
                 avec linkage C (extern "C" via le header)
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include "math.h"

int addition(int a, int b) {
    return a + b;
}

int soustraction(int a, int b) {
    return a - b;
}
