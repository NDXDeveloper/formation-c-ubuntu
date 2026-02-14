/* ============================================================================
   Section 9.3 : Les fonctions malloc, calloc, realloc et free
   Description : Gestionnaire de tableau dynamique complet (create/add/display/free)
   Fichier source : 03-fonctions-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int* data;
    size_t taille;
    size_t capacite;
} TableauDynamique;

TableauDynamique* creer_tableau(size_t capacite_initiale) {
    TableauDynamique* tab = malloc(sizeof(TableauDynamique));
    if (tab == NULL) return NULL;

    tab->data = malloc(capacite_initiale * sizeof(int));
    if (tab->data == NULL) {
        free(tab);
        return NULL;
    }

    tab->taille = 0;
    tab->capacite = capacite_initiale;

    return tab;
}

int ajouter_element(TableauDynamique* tab, int valeur) {
    if (tab == NULL) return 0;

    if (tab->taille == tab->capacite) {
        size_t nouvelle_capacite = tab->capacite * 2;
        int* nouveau_data = realloc(tab->data, nouvelle_capacite * sizeof(int));

        if (nouveau_data == NULL) {
            return 0;
        }

        tab->data = nouveau_data;
        tab->capacite = nouvelle_capacite;

        printf("Tableau redimensionne : capacite %zu\n", tab->capacite);
    }

    tab->data[tab->taille++] = valeur;
    return 1;
}

void afficher_tableau(const TableauDynamique* tab) {
    if (tab == NULL) return;

    printf("Tableau [taille=%zu, capacite=%zu] : ", tab->taille, tab->capacite);
    for (size_t i = 0; i < tab->taille; i++) {
        printf("%d ", tab->data[i]);
    }
    printf("\n");
}

void liberer_tableau(TableauDynamique* tab) {
    if (tab != NULL) {
        free(tab->data);
        free(tab);
    }
}

int main(void) {
    TableauDynamique* tab = creer_tableau(2);

    if (tab == NULL) {
        fprintf(stderr, "Erreur de creation\n");
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        if (!ajouter_element(tab, i * 10)) {
            fprintf(stderr, "Erreur d'ajout\n");
            liberer_tableau(tab);
            return 1;
        }
        afficher_tableau(tab);
    }

    liberer_tableau(tab);

    return 0;
}
