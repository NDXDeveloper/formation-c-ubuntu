/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Creation et manipulation d'un tableau dynamique de chaines
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** creer_tableau_chaines(int nb_chaines) {
    char **tableau = malloc(((size_t)nb_chaines + 1) * sizeof(char*));
    if (tableau == NULL) {
        return NULL;
    }
    tableau[nb_chaines] = NULL;  // Terminateur
    return tableau;
}

void ajouter_chaine(char **tableau, int index, const char *chaine) {
    tableau[index] = malloc(strlen(chaine) + 1);
    if (tableau[index] != NULL) {
        strcpy(tableau[index], chaine);
    }
}

void liberer_tableau_chaines(char **tableau) {
    if (tableau == NULL) return;

    for (int i = 0; tableau[i] != NULL; i++) {
        free(tableau[i]);
    }
    free(tableau);
}

void afficher_chaines(char **tableau) {
    for (int i = 0; tableau[i] != NULL; i++) {
        printf("%d: %s\n", i, tableau[i]);
    }
}

int main(void) {
    // Creation d'un tableau de 3 chaines
    char **langages = creer_tableau_chaines(3);
    if (langages == NULL) {
        return 1;
    }

    // Ajout de chaines
    ajouter_chaine(langages, 0, "C");
    ajouter_chaine(langages, 1, "Python");
    ajouter_chaine(langages, 2, "Rust");

    // Affichage
    printf("Langages de programmation :\n");
    afficher_chaines(langages);

    // Liberation
    liberer_tableau_chaines(langages);

    return 0;
}
