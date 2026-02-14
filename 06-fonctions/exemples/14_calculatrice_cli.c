/* ============================================================================
   Section 6.5 : La fonction main et ses parametres
   Description : Calculatrice en ligne de commande avec argc/argv
   Fichier source : 05-fonction-main.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage : %s <nombre1> <opération> <nombre2>\n", argv[0]);
        printf("Opérations : +, -, x, /\n");
        return 1;
    }

    int a = atoi(argv[1]);
    char *op = argv[2];
    int b = atoi(argv[3]);

    int resultat;

    if (strcmp(op, "+") == 0) {
        resultat = a + b;
    } else if (strcmp(op, "-") == 0) {
        resultat = a - b;
    } else if (strcmp(op, "x") == 0) {
        resultat = a * b;
    } else if (strcmp(op, "/") == 0) {
        if (b == 0) {
            printf("Erreur : division par zéro\n");
            return 1;
        }
        resultat = a / b;
    } else {
        printf("Erreur : opération inconnue '%s'\n", op);
        return 1;
    }

    printf("%d %s %d = %d\n", a, op, b, resultat);
    return 0;
}
