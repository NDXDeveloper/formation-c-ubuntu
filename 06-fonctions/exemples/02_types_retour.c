/* ============================================================================
   Section 6.1 : Declaration, definition et prototypes
   Description : Fonctions avec differents types de retour (int, bool, void)
   Fichier source : 01-declaration-definition.md
   ============================================================================ */
#include <stdio.h>
#include <stdbool.h>

// Prototypes
int maximum(int a, int b);
bool est_pair(int n);
void afficher_separateur(void);

int main(void) {
    int x = 15, y = 23;

    printf("Maximum entre %d et %d : %d\n", x, y, maximum(x, y));

    if (est_pair(x)) {
        printf("%d est pair\n", x);
    } else {
        printf("%d est impair\n", x);
    }

    afficher_separateur();

    return 0;
}

// Retourne le plus grand des deux nombres
int maximum(int a, int b) {
    return (a > b) ? a : b;
}

// Retourne true si n est pair, false sinon
bool est_pair(int n) {
    return (n % 2 == 0);
}

// Ne retourne rien (void)
void afficher_separateur(void) {
    printf("====================\n");
}
