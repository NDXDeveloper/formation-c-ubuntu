/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Table de dispatch remplacant un switch-case
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

typedef int (*Operation)(int, int);

int addition(int a, int b) { return a + b; }
int soustraction(int a, int b) { return a - b; }
int multiplication(int a, int b) { return a * b; }
int division(int a, int b) { return (b != 0) ? a / b : 0; }

// Structure pour associer un symbole a une fonction
typedef struct {
    char symbole;
    Operation fonction;
} EntreeOperation;

int calculer_avec_tableau(int a, int b, char operateur) {
    // Table de dispatch
    EntreeOperation operations[] = {
        {'+', addition},
        {'-', soustraction},
        {'*', multiplication},
        {'/', division}
    };

    int nb_operations = (int)(sizeof(operations) / sizeof(operations[0]));

    // Recherche de l'operation correspondante
    for (int i = 0; i < nb_operations; i++) {
        if (operations[i].symbole == operateur) {
            return operations[i].fonction(a, b);
        }
    }

    printf("Operateur inconnu\n");
    return 0;
}

int main(void) {
    printf("10 + 5 = %d\n", calculer_avec_tableau(10, 5, '+'));
    printf("10 - 5 = %d\n", calculer_avec_tableau(10, 5, '-'));
    printf("10 * 5 = %d\n", calculer_avec_tableau(10, 5, '*'));
    printf("10 / 5 = %d\n", calculer_avec_tableau(10, 5, '/'));

    return 0;
}
