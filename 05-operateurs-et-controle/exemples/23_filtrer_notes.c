/* ============================================================================
   Section 5.5 : Boucles
   Description : Filtrage de notes invalides avec continue et calcul de moyenne
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int notes[] = {15, -1, 18, -1, 12, 16, -1, 14};
    int taille = (int)(sizeof(notes) / sizeof(notes[0]));
    int somme = 0;
    int compteur = 0;

    printf("Notes valides :\n");
    for (int i = 0; i < taille; i++) {
        if (notes[i] == -1) {
            continue;
        }

        printf("%d ", notes[i]);
        somme += notes[i];
        compteur++;
    }

    if (compteur > 0) {
        printf("\nMoyenne : %.2f\n", (float)somme / compteur);
    }

    return 0;
}
