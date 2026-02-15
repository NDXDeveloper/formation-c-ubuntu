/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Passage de tableaux de pointeurs de fonctions en parametre
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

typedef int (*OperationBinaire)(int, int);

int addition(int a, int b) { return a + b; }
int multiplication(int a, int b) { return a * b; }

// Fonction qui recoit un tableau de pointeurs de fonctions
void appliquer_operations(int x, int y,
                          OperationBinaire ops[],
                          int nb_ops) {
    printf("Traitement de %d et %d :\n", x, y);
    for (int i = 0; i < nb_ops; i++) {
        if (ops[i] != NULL) {
            printf("  Operation %d : %d\n", i, ops[i](x, y));
        }
    }
}

int main(void) {
    OperationBinaire mes_operations[] = {addition, multiplication};
    int nombre_operations = (int)(sizeof(mes_operations) / sizeof(mes_operations[0]));

    appliquer_operations(7, 3, mes_operations, nombre_operations);

    return 0;
}
