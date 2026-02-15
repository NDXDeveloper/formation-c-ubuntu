/* ============================================================================
   Section 28.6 : Name Mangling
   Description : Demangling de symboles C++ via abi::__cxa_demangle (cxxabi.h)
                 Montre les noms mangles de fonctions, namespaces, classes,
                 templates, operateurs et constructeurs/destructeurs
   Fichier source : 06-name-mangling.md
   ============================================================================ */

#include <iostream>
#include <cxxabi.h>
#include <cstdlib>
#include <string>

std::string demangle(const char *mangled) {
    int status;
    char *demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);

    if (status == 0) {
        std::string result(demangled);
        std::free(demangled);
        return result;
    }

    switch (status) {
        case -1: return "(erreur allocation memoire)";
        case -2: return "(nom invalide)";
        case -3: return "(argument invalide)";
        default: return "(erreur inconnue)";
    }
}

int main() {
    std::cout << "=== Demangling de symboles C++ ===" << std::endl;
    std::cout << std::endl;

    /* Fonctions simples */
    std::cout << "--- Fonctions simples ---" << std::endl;
    std::cout << "  _Z8additionii       -> " << demangle("_Z8additionii") << std::endl;
    std::cout << "  _Z8additiondd       -> " << demangle("_Z8additiondd") << std::endl;
    std::cout << "  _Z8additioniii      -> " << demangle("_Z8additioniii") << std::endl;
    std::cout << "  _Z7traiteridc       -> " << demangle("_Z7traiteridc") << std::endl;
    std::cout << std::endl;

    /* Namespaces */
    std::cout << "--- Namespaces ---" << std::endl;
    std::cout << "  _ZN4math8additionEii   -> "
              << demangle("_ZN4math8additionEii") << std::endl;
    std::cout << "  _ZN6calcul8additionEii -> "
              << demangle("_ZN6calcul8additionEii") << std::endl;
    std::cout << std::endl;

    /* Classes et methodes */
    std::cout << "--- Classes ---" << std::endl;
    std::cout << "  _ZN9Rectangle13calculer_aireEii -> "
              << demangle("_ZN9Rectangle13calculer_aireEii") << std::endl;
    std::cout << "  _ZN6Cercle13calculer_aireEd     -> "
              << demangle("_ZN6Cercle13calculer_aireEd") << std::endl;
    std::cout << "  _ZN10Calculator3addEii           -> "
              << demangle("_ZN10Calculator3addEii") << std::endl;
    std::cout << std::endl;

    /* Namespace + Classe */
    std::cout << "--- Namespace + Classe ---" << std::endl;
    std::cout << "  _ZN3app7Service7traiterEid -> "
              << demangle("_ZN3app7Service7traiterEid") << std::endl;
    std::cout << std::endl;

    /* Constructeurs et destructeurs */
    std::cout << "--- Constructeurs/Destructeurs ---" << std::endl;
    std::cout << "  _ZN8MaClasseC1Ev  -> " << demangle("_ZN8MaClasseC1Ev") << std::endl;
    std::cout << "  _ZN8MaClasseC1Ei  -> " << demangle("_ZN8MaClasseC1Ei") << std::endl;
    std::cout << "  _ZN8MaClasseD1Ev  -> " << demangle("_ZN8MaClasseD1Ev") << std::endl;
    std::cout << std::endl;

    /* Pointeurs et references */
    std::cout << "--- Pointeurs et references ---" << std::endl;
    std::cout << "  _Z8fonctionPi   -> " << demangle("_Z8fonctionPi") << std::endl;
    std::cout << "  _Z8fonctionRi   -> " << demangle("_Z8fonctionRi") << std::endl;
    std::cout << "  _Z8fonctionPKi  -> " << demangle("_Z8fonctionPKi") << std::endl;
    std::cout << std::endl;

    /* Operateurs surcharges */
    std::cout << "--- Operateurs ---" << std::endl;
    std::cout << "  _ZN5PointplERKS_ -> " << demangle("_ZN5PointplERKS_") << std::endl;
    std::cout << "  _ZN5PointeqERKS_ -> " << demangle("_ZN5PointeqERKS_") << std::endl;
    std::cout << std::endl;

    /* Vtables */
    std::cout << "--- Vtables ---" << std::endl;
    std::cout << "  _ZTV4Base    -> " << demangle("_ZTV4Base") << std::endl;
    std::cout << "  _ZTV7Derivee -> " << demangle("_ZTV7Derivee") << std::endl;
    std::cout << std::endl;

    /* Nom invalide */
    std::cout << "--- Cas d'erreur ---" << std::endl;
    std::cout << "  addition (nom C) -> " << demangle("addition") << std::endl;

    std::cout << std::endl << "=== Fin ===" << std::endl;
    return 0;
}
