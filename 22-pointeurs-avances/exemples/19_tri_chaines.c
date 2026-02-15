/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Tri de chaines de caracteres par echange de pointeurs
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

void trier_chaines(char **chaines, int nb) {
    char *temp;

    for (int i = 0; i < nb - 1; i++) {
        for (int j = i + 1; j < nb; j++) {
            if (strcmp(chaines[i], chaines[j]) > 0) {
                // Echange des pointeurs
                temp = chaines[i];
                chaines[i] = chaines[j];
                chaines[j] = temp;
            }
        }
    }
}

int main(void) {
    char *fruits[] = {
        "banane",
        "pomme",
        "orange",
        "fraise",
        "kiwi"
    };

    int nb_fruits = (int)(sizeof(fruits) / sizeof(fruits[0]));

    printf("Avant tri :\n");
    for (int i = 0; i < nb_fruits; i++) {
        printf("  %s\n", fruits[i]);
    }

    trier_chaines(fruits, nb_fruits);

    printf("\nApres tri :\n");
    for (int i = 0; i < nb_fruits; i++) {
        printf("  %s\n", fruits[i]);
    }

    return 0;
}
