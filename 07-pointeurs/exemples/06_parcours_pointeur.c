/* ============================================================================
   Section 7.3 : L'arithmétique des pointeurs
   Description : Parcourir un tableau avec un pointeur
   Fichier source : 03-arithmetique-pointeurs.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr = tableau;

    printf("Parcours du tableau avec un pointeur :\n");
    for (int i = 0; i < 5; i++) {
        printf("Adresse : %p, Valeur : %d\n", (void*)ptr, *ptr);
        ptr++;
    }

    return 0;
}
