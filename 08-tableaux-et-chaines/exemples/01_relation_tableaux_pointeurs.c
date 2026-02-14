/* ============================================================================
   Section 8.1 : Relation entre tableaux et pointeurs
   Description : Manipulation d'un tableau avec pointeurs (affichage, adresses, doublement)
   Fichier source : 01-relation-tableaux-pointeurs.md
   ============================================================================ */

#include <stdio.h>

void afficher_valeurs(const int *tab, size_t taille) {
    printf("Valeurs : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%d ", tab[i]);
    }
    printf("\n");
}

void afficher_adresses(const int *tab, size_t taille) {
    printf("Adresses : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%p ", (void*)(tab + i));
    }
    printf("\n");
}

void doubler_valeurs(int *tab, size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        tab[i] *= 2;
    }
}

int main(void) {
    int nombres[5] = {10, 20, 30, 40, 50};

    printf("Tableau initial :\n");
    afficher_valeurs(nombres, 5);
    afficher_adresses(nombres, 5);

    printf("\nNom du tableau : %p\n", (void*)nombres);
    printf("Adresse 1er element : %p\n", (void*)&nombres[0]);
    printf("Taille du tableau : %zu octets\n", sizeof(nombres));

    doubler_valeurs(nombres, 5);

    printf("\nApres doublement :\n");
    afficher_valeurs(nombres, 5);

    printf("\nParcours avec pointeur iteratif : ");
    int *ptr = nombres;
    for (size_t i = 0; i < 5; i++) {
        printf("%d ", *ptr);
        ptr++;
    }
    printf("\n");

    return 0;
}
