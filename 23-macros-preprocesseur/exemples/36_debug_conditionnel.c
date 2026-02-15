/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Mode debug active/desactive avec macros variadiques
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

// Décommenter pour activer le mode debug
// #define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        printf("[DEBUG] %s:%d:%s(): " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

int calculer_somme(int a, int b) {
    DEBUG_PRINT("Calcul de %d + %d", a, b);
    int resultat = a + b;
    DEBUG_PRINT("Résultat = %d", resultat);
    return resultat;
}

int main(void) {
    printf("Début du programme\n");

    int somme = calculer_somme(10, 20);
    printf("Somme: %d\n", somme);

    DEBUG_PRINT("Programme terminé");

    return 0;
}
