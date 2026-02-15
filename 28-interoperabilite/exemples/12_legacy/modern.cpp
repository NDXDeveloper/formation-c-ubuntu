/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Programme C++ moderne appelant du code C legacy via extern "C"
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include <iostream>
#include "legacy.h"

class Calculator {
public:
    void compute(int a, int b) {
        /* Appeler du code C depuis du code C++ */
        int resultat = calcul_complexe(a, b);
        std::cout << "  Resultat a*a + b*b = " << resultat << std::endl;

        ancienne_fonction(resultat);
    }
};

int main() {
    std::cout << "=== C++ appelle du code C legacy ===" << std::endl;
    std::cout << std::endl;

    Calculator calc;
    std::cout << "  compute(3, 4) :" << std::endl;
    calc.compute(3, 4);

    std::cout << std::endl;
    std::cout << "  compute(5, 12) :" << std::endl;
    calc.compute(5, 12);

    std::cout << std::endl << "=== Fin ===" << std::endl;
    return 0;
}
