/* ============================================================================
   Section 25.5 : Design by contract
   Description : Pile (Stack) avec contrat rigoureux DbC
   Fichier source : 05-design-by-contract.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define PRECONDITION(cond)  assert((cond) && "Precondition")
#define POSTCONDITION(cond) assert((cond) && "Postcondition")
#define INVARIANT(cond)     assert((cond) && "Invariant")

/* Structure Pile */
typedef struct {
    int *elements;
    size_t taille;      /* Nombre d'elements actuels */
    size_t capacite;    /* Capacite maximale */
} Pile;

static void pile_verifier_invariant(const Pile *p) {
    INVARIANT(p != NULL);
    INVARIANT(p->taille <= p->capacite);
    INVARIANT((p->capacite == 0) == (p->elements == NULL));
    INVARIANT((p->capacite > 0) == (p->elements != NULL));
}

Pile* pile_creer(size_t capacite) {
    PRECONDITION(capacite > 0);

    Pile *p = malloc(sizeof(Pile));
    POSTCONDITION(p != NULL);

    p->elements = malloc(capacite * sizeof(int));
    POSTCONDITION(p->elements != NULL);

    p->taille = 0;
    p->capacite = capacite;

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == 0);
    POSTCONDITION(p->capacite == capacite);

    return p;
}

bool pile_est_vide(const Pile *p) {
    PRECONDITION(p != NULL);
    pile_verifier_invariant(p);

    bool resultat = (p->taille == 0);

    pile_verifier_invariant(p);
    return resultat;
}

bool pile_est_pleine(const Pile *p) {
    PRECONDITION(p != NULL);
    pile_verifier_invariant(p);

    bool resultat = (p->taille == p->capacite);

    pile_verifier_invariant(p);
    return resultat;
}

void pile_empiler(Pile *p, int valeur) {
    PRECONDITION(p != NULL);
    PRECONDITION(!pile_est_pleine(p));
    pile_verifier_invariant(p);

    size_t ancienne_taille = p->taille;

    p->elements[p->taille] = valeur;
    p->taille++;

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == ancienne_taille + 1);
    POSTCONDITION(p->elements[p->taille - 1] == valeur);
}

int pile_depiler(Pile *p) {
    PRECONDITION(p != NULL);
    PRECONDITION(!pile_est_vide(p));
    pile_verifier_invariant(p);

    size_t ancienne_taille = p->taille;

    p->taille--;
    int valeur = p->elements[p->taille];

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == ancienne_taille - 1);

    return valeur;
}

int pile_sommet(const Pile *p) {
    PRECONDITION(p != NULL);
    PRECONDITION(!pile_est_vide(p));
    pile_verifier_invariant(p);

    int valeur = p->elements[p->taille - 1];

    pile_verifier_invariant(p);
    return valeur;
}

void pile_detruire(Pile *p) {
    if (p != NULL) {
        pile_verifier_invariant(p);
        free(p->elements);
        free(p);
    }
}

int main(void) {
    printf("=== Test de la pile avec DbC ===\n\n");

    /* Creer une pile */
    Pile *p = pile_creer(5);
    printf("Pile creee (capacite: 5)\n");

    /* Empiler des elements */
    printf("\nEmpilage de 10, 20, 30\n");
    pile_empiler(p, 10);
    pile_empiler(p, 20);
    pile_empiler(p, 30);

    /* Consulter le sommet */
    printf("Sommet : %d\n", pile_sommet(p));

    /* Depiler */
    printf("\nDepilage :\n");
    while (!pile_est_vide(p)) {
        printf("  Depile : %d\n", pile_depiler(p));
    }

    printf("\nPile vide : %s\n", pile_est_vide(p) ? "oui" : "non");

    pile_detruire(p);
    printf("\nPile detruite\n");

    return 0;
}
