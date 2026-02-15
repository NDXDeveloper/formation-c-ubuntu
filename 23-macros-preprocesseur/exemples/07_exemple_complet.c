/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Exemple complet combinant constantes, macros et debug
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

// Constantes
#define TAILLE_MAX 100
#define VERSION "1.0.0"

// Macro simple avec parenthèses
#define CARRE(x) ((x) * (x))

// Macro multi-paramètres
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Macro multi-ligne avec do-while
#define AFFICHER_SEPARATEUR() \
    do { \
        printf("========================\n"); \
    } while(0)

// Macro de débogage conditionnelle
#ifdef DEBUG
    #define LOG(msg) printf("[LOG] %s\n", msg)
#else
    #define LOG(msg) do {} while(0)
#endif

int main(void) {
    printf("Programme version %s\n", VERSION);
    AFFICHER_SEPARATEUR();

    // Utilisation de CARRE
    int nombre = 7;
    printf("Le carré de %d est %d\n", nombre, CARRE(nombre));

    // Utilisation de MAX/MIN
    int a = 15, b = 23;
    printf("Max(%d, %d) = %d\n", a, b, MAX(a, b));
    printf("Min(%d, %d) = %d\n", a, b, MIN(a, b));

    // Les macros fonctionnent avec différents types
    double x = 3.14, y = 2.71;
    printf("Max(%.2f, %.2f) = %.2f\n", x, y, MAX(x, y));

    AFFICHER_SEPARATEUR();
    LOG("Fin du programme");

    (void)TAILLE_MAX;

    return 0;
}
