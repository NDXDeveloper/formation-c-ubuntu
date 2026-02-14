/* ============================================================================
   Section 10.6 : typedef - Simplification de la syntaxe
   Description : Callbacks avec typedef de pointeurs de fonctions
   Fichier source : 06-typedef.md
   ============================================================================ */

#include <stdio.h>

typedef void (*Callback)(int);

void traiter_donnees(int *data, size_t n, Callback cb) {
    for (size_t i = 0; i < n; i++) {
        cb(data[i]);
    }
}

void afficher(int x) {
    printf("%d ", x);
}

void doubler(int x) {
    printf("%d ", x * 2);
}

int main(void) {
    int nombres[] = {1, 2, 3, 4, 5};

    printf("Valeurs : ");
    traiter_donnees(nombres, 5, afficher);
    printf("\n");

    printf("Doubles : ");
    traiter_donnees(nombres, 5, doubler);
    printf("\n");

    return 0;
}
