/* ============================================================================
   Section 7.3 : L'arithmétique des pointeurs
   Description : Recherche d'un élément dans un tableau avec pointeurs
   Fichier source : 03-arithmetique-pointeurs.md
   ============================================================================ */

#include <stdio.h>
#include <stddef.h>

int *trouver(int *debut, int *fin, int valeur) {
    while (debut < fin) {
        if (*debut == valeur) {
            return debut;
        }
        debut++;
    }
    return NULL;
}

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *resultat;

    resultat = trouver(tableau, tableau + 5, 30);

    if (resultat != NULL) {
        printf("Valeur trouvee a l'index : %ld\n", resultat - tableau);
        printf("Valeur : %d\n", *resultat);
    } else {
        printf("Valeur non trouvee\n");
    }

    resultat = trouver(tableau, tableau + 5, 99);

    if (resultat != NULL) {
        printf("Valeur trouvee a l'index : %ld\n", resultat - tableau);
    } else {
        printf("Valeur 99 non trouvee\n");
    }

    return 0;
}
