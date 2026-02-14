/* ============================================================================
   Section 06 : Creation d'une API propre
   Description : Programme de test de l'API queue (type opaque)
   Fichier source : 06-creation-api-propre.md
   ============================================================================ */

#include <stdio.h>
#include "queue.h"

int main(void) {
    /* Creer la file */
    queue_t *q = queue_create();
    if (!q) {
        fprintf(stderr, "Erreur: impossible de creer la file\n");
        return 1;
    }

    /* Enfiler des valeurs */
    queue_push(q, 10);
    queue_push(q, 20);
    queue_push(q, 30);
    printf("Taille apres 3 push : %zu\n", queue_size(q));

    /* Consulter le premier element */
    int val;
    if (queue_peek(q, &val)) {
        printf("Premier element (peek) : %d\n", val);
    }

    /* Defiler les valeurs */
    while (!queue_is_empty(q)) {
        if (queue_pop(q, &val)) {
            printf("Pop : %d\n", val);
        }
    }

    printf("File vide : %s\n", queue_is_empty(q) ? "oui" : "non");

    /* Liberer la memoire */
    queue_destroy(q);

    return 0;
}
