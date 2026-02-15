/* ============================================================================
   Section 25.5 : Design by contract
   Description : File d'attente (Queue) circulaire avec DbC complet
   Fichier source : 05-design-by-contract.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define PRECONDITION(c)  assert((c) && "Precondition")
#define POSTCONDITION(c) assert((c) && "Postcondition")
#define INVARIANT(c)     assert((c) && "Invariant")

typedef struct {
    int *elements;
    size_t debut;       /* Indice du premier element */
    size_t fin;         /* Indice apres le dernier element */
    size_t capacite;
} File;

static void file_verifier_invariant(const File *f) {
    INVARIANT(f != NULL);
    INVARIANT((f->capacite == 0) == (f->elements == NULL));
    INVARIANT(f->capacite == 0 || f->debut < f->capacite);
    INVARIANT(f->capacite == 0 || f->fin < f->capacite);
}

static size_t file_taille_interne(const File *f) {
    if (f->fin >= f->debut) {
        return f->fin - f->debut;
    } else {
        return f->capacite - f->debut + f->fin;
    }
}

File* file_creer(size_t capacite) {
    PRECONDITION(capacite > 0);

    File *f = malloc(sizeof(File));
    POSTCONDITION(f != NULL);

    f->elements = malloc(capacite * sizeof(int));
    POSTCONDITION(f->elements != NULL);

    f->debut = 0;
    f->fin = 0;
    f->capacite = capacite;

    file_verifier_invariant(f);
    POSTCONDITION(file_taille_interne(f) == 0);

    return f;
}

bool file_est_vide(const File *f) {
    PRECONDITION(f != NULL);
    file_verifier_invariant(f);

    bool vide = (f->debut == f->fin);

    file_verifier_invariant(f);
    POSTCONDITION(vide == (file_taille_interne(f) == 0));

    return vide;
}

bool file_est_pleine(const File *f) {
    PRECONDITION(f != NULL);
    file_verifier_invariant(f);

    bool pleine = ((f->fin + 1) % f->capacite == f->debut);

    file_verifier_invariant(f);

    return pleine;
}

void file_enfiler(File *f, int valeur) {
    PRECONDITION(f != NULL);
    PRECONDITION(!file_est_pleine(f));
    file_verifier_invariant(f);

    size_t ancienne_taille = file_taille_interne(f);

    f->elements[f->fin] = valeur;
    f->fin = (f->fin + 1) % f->capacite;

    file_verifier_invariant(f);
    POSTCONDITION(file_taille_interne(f) == ancienne_taille + 1);
}

int file_defiler(File *f) {
    PRECONDITION(f != NULL);
    PRECONDITION(!file_est_vide(f));
    file_verifier_invariant(f);

    size_t ancienne_taille = file_taille_interne(f);

    int valeur = f->elements[f->debut];
    f->debut = (f->debut + 1) % f->capacite;

    file_verifier_invariant(f);
    POSTCONDITION(file_taille_interne(f) == ancienne_taille - 1);

    return valeur;
}

void file_detruire(File *f) {
    if (f != NULL) {
        file_verifier_invariant(f);
        free(f->elements);
        free(f);
    }
}

int main(void) {
    printf("=== Test File avec DbC ===\n\n");

    File *f = file_creer(5);
    printf("File creee (capacite: 5)\n");

    printf("\nEnfilage : 10, 20, 30\n");
    file_enfiler(f, 10);
    file_enfiler(f, 20);
    file_enfiler(f, 30);

    printf("\nDefilage :\n");
    printf("  %d\n", file_defiler(f));
    printf("  %d\n", file_defiler(f));

    printf("\nEnfilage : 40, 50\n");
    file_enfiler(f, 40);
    file_enfiler(f, 50);

    printf("\nDefilage complet :\n");
    while (!file_est_vide(f)) {
        printf("  %d\n", file_defiler(f));
    }

    file_detruire(f);
    printf("\nFile detruite\n");

    return 0;
}
