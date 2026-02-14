/* ============================================================================
   Section 12.2 : La portee des variables
   Description : Demonstration du masquage de variables (shadowing) -
                 une variable interne masque temporairement la variable externe
   Fichier source : 02-portee-variables.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int x = 10;
    printf("x externe : %d\n", x);

    {
        int x = 20;  /* Nouvelle variable qui "masque" la premiere */
        printf("x interne : %d\n", x);
    }

    printf("x externe apres : %d\n", x);  /* Inchange */

    return 0;
}
