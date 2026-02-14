/* ============================================================================
   Section 7.6 : Pointeurs constants et const
   Description : Recherche dans un tableau avec const-correctness
   Fichier source : 06-pointeurs-const.md
   ============================================================================ */

#include <stdio.h>
#include <stddef.h>

const int *trouver_valeur(const int *tableau, size_t taille, int valeur) {
    for (size_t i = 0; i < taille; i++) {
        if (tableau[i] == valeur) {
            return &tableau[i];
        }
    }
    return NULL;
}

int main(void) {
    int nombres[] = {10, 20, 30, 40, 50};

    const int *resultat = trouver_valeur(nombres, 5, 30);

    if (resultat != NULL) {
        printf("Valeur trouvee : %d\n", *resultat);
        printf("Index : %ld\n", resultat - nombres);
    } else {
        printf("Valeur non trouvee\n");
    }

    resultat = trouver_valeur(nombres, 5, 99);

    if (resultat != NULL) {
        printf("Valeur trouvee : %d\n", *resultat);
    } else {
        printf("Valeur 99 non trouvee\n");
    }

    return 0;
}
