/* ============================================================================
   Section 24.1.4 : Limitations et portabilite
   Description : Demonstration de l'ordre de cleanup (LIFO)
   Fichier source : 01.4-limitations-portabilite.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// Fonctions de cleanup avec traces pour montrer l'ordre
void cleanup_a(void *p) {
    void **ptr = (void **)p;
    if (*ptr) {
        printf("[cleanup a] Liberation de a\n");
        free(*ptr);
        *ptr = NULL;
    }
}

void cleanup_b(void *p) {
    void **ptr = (void **)p;
    if (*ptr) {
        printf("[cleanup b] Liberation de b\n");
        free(*ptr);
        *ptr = NULL;
    }
}

void cleanup_c(void *p) {
    void **ptr = (void **)p;
    if (*ptr) {
        printf("[cleanup c] Liberation de c\n");
        free(*ptr);
        *ptr = NULL;
    }
}

void exemple_ordre(void) {
    printf("1. Debut\n");

    char *a __attribute__((cleanup(cleanup_a))) = malloc(10);
    printf("2. a declare\n");

    char *b __attribute__((cleanup(cleanup_b))) = malloc(10);
    printf("3. b declare\n");

    char *c __attribute__((cleanup(cleanup_c))) = malloc(10);
    printf("4. c declare\n");

    // Eviter warnings unused
    if (a) a[0] = 'A';
    if (b) b[0] = 'B';
    if (c) c[0] = 'C';

    printf("5. Fin du scope\n");
    // Ordre de cleanup : c, puis b, puis a (LIFO)
}

int main(void) {
    exemple_ordre();
    return 0;
}
