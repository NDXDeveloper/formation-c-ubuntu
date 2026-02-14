/* ============================================================================
   Section 9.3 : Les fonctions malloc, calloc, realloc et free
   Description : Liste dynamique avec redimensionnement automatique (realloc)
   Fichier source : 03-fonctions-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int capacite = 2;
    int taille = 0;
    int* liste = malloc(capacite * sizeof(int));

    if (liste == NULL) return 1;

    for (int i = 0; i < 10; i++) {
        if (taille == capacite) {
            capacite *= 2;

            int* temp = realloc(liste, capacite * sizeof(int));
            if (temp == NULL) {
                free(liste);
                return 1;
            }
            liste = temp;

            printf("Redimensionne a capacite %d\n", capacite);
        }

        liste[taille++] = i;
    }

    printf("Liste finale : ");
    for (int i = 0; i < taille; i++) {
        printf("%d ", liste[i]);
    }
    printf("\n");

    free(liste);
    return 0;
}
