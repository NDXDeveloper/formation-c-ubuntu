/* ============================================================================
   Section 22.1 : Pointeurs de fonctions (Callbacks)
   Description : Exemple simple de pointeur de fonction avec addition et
                 soustraction
   Fichier source : 01-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

// Une fonction simple qui additionne deux nombres
int addition(int a, int b) {
    return a + b;
}

// Une autre fonction qui soustrait deux nombres
int soustraction(int a, int b) {
    return a - b;
}

int main(void) {
    // Declaration d'un pointeur de fonction
    int (*operation)(int, int);

    // On fait pointer notre pointeur vers la fonction addition
    operation = addition;

    // On appelle la fonction via le pointeur
    int resultat = operation(5, 3);
    printf("Resultat de l'addition : %d\n", resultat);

    // On peut changer la fonction pointee
    operation = soustraction;
    resultat = operation(5, 3);
    printf("Resultat de la soustraction : %d\n", resultat);

    return 0;
}
