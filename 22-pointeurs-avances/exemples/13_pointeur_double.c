/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Concept de base du pointeur double (pointeur de pointeur)
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int valeur = 42;           // Variable normale
    int *ptr = &valeur;        // Pointeur vers valeur
    int **pptr = &ptr;         // Pointeur vers ptr

    printf("Valeur directe : %d\n", valeur);
    printf("Via ptr : %d\n", *ptr);
    printf("Via pptr : %d\n", **pptr);

    // Affichage des adresses
    printf("\nAdresses :\n");
    printf("Adresse de valeur : %p\n", (void*)&valeur);
    printf("Contenu de ptr : %p (pointe vers valeur)\n", (void*)ptr);
    printf("Adresse de ptr : %p\n", (void*)&ptr);
    printf("Contenu de pptr : %p (pointe vers ptr)\n", (void*)pptr);

    return 0;
}
