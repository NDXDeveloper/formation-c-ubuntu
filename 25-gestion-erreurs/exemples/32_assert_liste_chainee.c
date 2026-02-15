/* ============================================================================
   Section 25.4 : Assertions
   Description : Assertions dans une liste chainee (pre/postconditions)
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Noeud {
    int valeur;
    struct Noeud *suivant;
} Noeud;

/* Creer un nouveau noeud */
Noeud* creer_noeud(int valeur) {
    Noeud *nouveau = malloc(sizeof(Noeud));

    /* Postcondition : l'allocation doit reussir */
    assert(nouveau != NULL);

    nouveau->valeur = valeur;
    nouveau->suivant = NULL;

    return nouveau;
}

/* Inserer en tete */
void inserer_tete(Noeud **tete, int valeur) {
    /* Precondition : pointeur valide */
    assert(tete != NULL);

    Noeud *nouveau = creer_noeud(valeur);
    nouveau->suivant = *tete;
    *tete = nouveau;

    /* Postcondition : la tete pointe sur le nouveau noeud */
    assert(*tete == nouveau);
}

/* Compter les elements */
int compter(Noeud *tete) {
    int compte = 0;

    for (Noeud *courant = tete; courant != NULL; courant = courant->suivant) {
        compte++;

        /* Invariant : pas de boucle infinie */
        assert(compte < 10000 && "Possible boucle infinie detectee");
    }

    return compte;
}

/* Liberer la liste */
void liberer(Noeud *tete) {
    while (tete != NULL) {
        Noeud *suivant = tete->suivant;
        free(tete);
        tete = suivant;
    }
}

int main(void) {
    Noeud *liste = NULL;

    inserer_tete(&liste, 10);
    inserer_tete(&liste, 20);
    inserer_tete(&liste, 30);

    int nb = compter(liste);
    assert(nb == 3);  /* Verification */

    printf("Nombre d'elements : %d\n", nb);

    /* Afficher la liste */
    printf("Liste : ");
    for (Noeud *n = liste; n != NULL; n = n->suivant) {
        printf("%d ", n->valeur);
    }
    printf("\n");

    liberer(liste);
    return 0;
}
