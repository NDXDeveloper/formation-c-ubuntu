/* ============================================================================
   Section 7.2 : Déclaration, déréférencement et adresse
   Description : Plusieurs pointeurs vers la même variable
   Fichier source : 02-declaration-dereferencement.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int valeur = 50;
    int *ptr1 = &valeur;
    int *ptr2 = &valeur;

    printf("valeur = %d\n", valeur);
    printf("*ptr1 = %d\n", *ptr1);
    printf("*ptr2 = %d\n", *ptr2);

    *ptr1 = 60;
    printf("\nApres *ptr1 = 60 :\n");
    printf("valeur = %d\n", valeur);
    printf("*ptr2 = %d\n", *ptr2);

    return 0;
}
