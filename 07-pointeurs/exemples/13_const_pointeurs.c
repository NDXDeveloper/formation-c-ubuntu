/* ============================================================================
   Section 7.6 : Pointeurs constants et const
   Description : Les 3 cas de const avec pointeurs
   Fichier source : 06-pointeurs-const.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int x = 10;
    int y = 20;

    printf("=== Cas 1 : const int *ptr (valeur constante) ===\n");
    {
        const int *ptr = &x;
        printf("*ptr = %d\n", *ptr);
        /* *ptr = 15;  -- INTERDIT : modification de la valeur */
        ptr = &y;  /* OK : changer l'adresse */
        printf("Apres ptr = &y : *ptr = %d\n\n", *ptr);
    }

    printf("=== Cas 2 : int *const ptr (pointeur constant) ===\n");
    {
        int *const ptr = &x;
        printf("*ptr = %d\n", *ptr);
        *ptr = 15;  /* OK : modification de la valeur */
        printf("Apres *ptr = 15 : *ptr = %d\n", *ptr);
        /* ptr = &y;  -- INTERDIT : changer le pointeur */
        printf("x = %d\n\n", x);
    }

    printf("=== Cas 3 : const int *const ptr (tout constant) ===\n");
    {
        const int *const ptr = &y;
        printf("*ptr = %d\n", *ptr);
        /* *ptr = 15;  -- INTERDIT */
        /* ptr = &x;   -- INTERDIT */
        printf("Lecture seule, rien ne peut etre modifie\n");
    }

    return 0;
}
