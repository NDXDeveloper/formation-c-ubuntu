/* ============================================================================
   Section 7.3 : L'arithmétique des pointeurs
   Description : Exemple récapitulatif complet d'arithmétique de pointeurs
   Fichier source : 03-arithmetique-pointeurs.md
   ============================================================================ */

#include <stdio.h>

void afficher_tableau(int *ptr, size_t taille) {
    printf("[ ");
    for (size_t i = 0; i < taille; i++) {
        printf("%d ", *(ptr + i));
    }
    printf("]\n");
}

int main(void) {
    int nombres[6] = {100, 200, 300, 400, 500, 600};
    int *ptr = nombres;

    printf("=== Tableau initial ===\n");
    afficher_tableau(nombres, 6);

    printf("\n=== Navigation avec arithmetique ===\n");
    printf("Premier element (*ptr)       : %d\n", *ptr);
    printf("Troisieme element (*(ptr+2)) : %d\n", *(ptr + 2));
    printf("Dernier element (*(ptr+5))   : %d\n", *(ptr + 5));

    printf("\n=== Avancer le pointeur ===\n");
    ptr = ptr + 2;
    printf("Apres ptr + 2, *ptr = %d\n", *ptr);

    printf("\n=== Incrementer ===\n");
    ptr++;
    printf("Apres ptr++, *ptr = %d\n", *ptr);

    printf("\n=== Distance ===\n");
    printf("Distance de ptr au debut : %ld elements\n", ptr - nombres);

    printf("\n=== Modification via pointeur ===\n");
    *ptr = 999;
    afficher_tableau(nombres, 6);

    return 0;
}
