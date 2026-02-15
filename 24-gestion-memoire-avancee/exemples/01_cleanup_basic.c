/* ============================================================================
   Section 24.1.1 : Extension GNU - __attribute__((cleanup))
   Description : Liberation automatique de memoire avec cleanup
   Fichier source : 01.1-extension-gnu.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// Fonction de nettoyage pour un pointeur
void cleanup_free(void *p) {
    void **ptr = (void **)p;  // On recoit l'adresse du pointeur
    if (*ptr) {
        printf("Nettoyage automatique : liberation de %p\n", *ptr);
        free(*ptr);
        *ptr = NULL;  // Bonne pratique : evite les double-free
    }
}

void exemple_simple(void) {
    printf("=== Debut de la fonction ===\n");

    // Declaration avec cleanup
    char *buffer __attribute__((cleanup(cleanup_free))) = malloc(100);

    if (buffer == NULL) {
        printf("Erreur allocation\n");
        return;  // cleanup_free sera quand meme appele !
    }

    sprintf(buffer, "Hello, cleanup!");
    printf("Contenu : %s\n", buffer);

    // Pas besoin de free(buffer) explicite !
    printf("=== Fin de la fonction ===\n");
    // cleanup_free(&buffer) est appele automatiquement ici
}

int main(void) {
    exemple_simple();
    return 0;
}
