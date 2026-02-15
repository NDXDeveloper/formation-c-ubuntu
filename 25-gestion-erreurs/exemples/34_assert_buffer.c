/* ============================================================================
   Section 25.4 : Assertions
   Description : Gestionnaire de memoire avec assertions et static_assert
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

/* Verifications statiques */
static_assert(sizeof(void*) == 8, "Ce code necessite un systeme 64 bits");
static_assert(sizeof(size_t) >= 4, "size_t doit faire au moins 4 octets");

typedef struct {
    void *donnees;
    size_t taille_utilisee;
    size_t taille_allouee;
} Buffer;

/* Fonction pour verifier l'invariant */
static inline int invariant_buffer_valide(const Buffer *b) {
    if (b == NULL) return 0;
    if (b->taille_utilisee > b->taille_allouee) return 0;
    if (b->taille_allouee > 0 && b->donnees == NULL) return 0;
    if (b->taille_allouee == 0 && b->donnees != NULL) return 0;
    return 1;
}

/* Initialiser un buffer */
Buffer* buffer_creer(size_t taille_initiale) {
    /* Precondition */
    assert(taille_initiale > 0 && taille_initiale < SIZE_MAX / 2);

    Buffer *b = malloc(sizeof(Buffer));
    assert(b != NULL);

    b->donnees = malloc(taille_initiale);
    assert(b->donnees != NULL);

    b->taille_utilisee = 0;
    b->taille_allouee = taille_initiale;

    /* Postcondition */
    assert(invariant_buffer_valide(b));

    return b;
}

/* Ajouter des donnees */
void buffer_ajouter(Buffer *b, const void *donnees, size_t taille) {
    /* Preconditions */
    assert(invariant_buffer_valide(b));
    assert(donnees != NULL || taille == 0);
    assert(taille < SIZE_MAX - b->taille_utilisee);  /* Pas d'overflow */

    /* Reallocation si necessaire */
    if (b->taille_utilisee + taille > b->taille_allouee) {
        size_t nouvelle_taille = (b->taille_allouee + taille) * 2;
        assert(nouvelle_taille > b->taille_allouee);  /* Pas d'overflow */

        void *nouveau = realloc(b->donnees, nouvelle_taille);
        assert(nouveau != NULL);

        b->donnees = nouveau;
        b->taille_allouee = nouvelle_taille;
    }

    /* Copie des donnees */
    if (taille > 0) {
        memcpy((char*)b->donnees + b->taille_utilisee, donnees, taille);
        b->taille_utilisee += taille;
    }

    /* Postcondition */
    assert(invariant_buffer_valide(b));
}

/* Liberer le buffer */
void buffer_detruire(Buffer *b) {
    if (b != NULL) {
        assert(invariant_buffer_valide(b));

        free(b->donnees);
        free(b);
    }
}

int main(void) {
    printf("=== Test du buffer ===\n");

    Buffer *b = buffer_creer(16);
    printf("Buffer cree : %zu octets alloues\n", b->taille_allouee);

    const char *msg1 = "Hello, ";
    buffer_ajouter(b, msg1, strlen(msg1));
    printf("Apres ajout 1 : %zu/%zu octets\n",
           b->taille_utilisee, b->taille_allouee);

    const char *msg2 = "World!";
    buffer_ajouter(b, msg2, strlen(msg2));
    printf("Apres ajout 2 : %zu/%zu octets\n",
           b->taille_utilisee, b->taille_allouee);

    /* Afficher le contenu */
    printf("Contenu : %.*s\n", (int)b->taille_utilisee, (char*)b->donnees);

    buffer_detruire(b);
    printf("Buffer detruit\n");

    return 0;
}
