/* ============================================================================
   Section 22.1 : Pointeurs de fonctions (Callbacks)
   Description : Tri personnalise avec qsort et fonctions de comparaison
   Fichier source : 01-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// Fonction de comparaison pour tri croissant
int comparer_croissant(const void *a, const void *b) {
    int valeur_a = *(const int *)a;
    int valeur_b = *(const int *)b;
    return valeur_a - valeur_b;
}

// Fonction de comparaison pour tri decroissant
int comparer_decroissant(const void *a, const void *b) {
    int valeur_a = *(const int *)a;
    int valeur_b = *(const int *)b;
    return valeur_b - valeur_a;
}

void afficher_tableau(int tableau[], int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");
}

int main(void) {
    int nombres[] = {64, 34, 25, 12, 22, 11, 90};
    int taille = (int)(sizeof(nombres) / sizeof(nombres[0]));

    printf("Tableau original : ");
    afficher_tableau(nombres, taille);

    // Tri croissant en utilisant qsort avec notre callback
    qsort(nombres, (size_t)taille, sizeof(int), comparer_croissant);
    printf("Tri croissant : ");
    afficher_tableau(nombres, taille);

    // Tri decroissant
    qsort(nombres, (size_t)taille, sizeof(int), comparer_decroissant);
    printf("Tri decroissant : ");
    afficher_tableau(nombres, taille);

    return 0;
}
