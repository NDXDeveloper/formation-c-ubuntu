/* ============================================================================
   Section 10.6 : typedef - Simplification de la syntaxe
   Description : Tri bulle generique avec typedef CompareFn et void*
   Fichier source : 06-typedef.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*CompareFn)(const void*, const void*);

int comparer_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void tri_bulle(void *base, size_t n, size_t size, CompareFn cmp) {
    char *arr = (char*)base;
    char *temp = malloc(size);
    if (temp == NULL) return;

    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            void *elem1 = arr + j * size;
            void *elem2 = arr + (j + 1) * size;

            if (cmp(elem1, elem2) > 0) {
                memcpy(temp, elem1, size);
                memcpy(elem1, elem2, size);
                memcpy(elem2, temp, size);
            }
        }
    }

    free(temp);
}

int main(void) {
    int nombres[] = {5, 2, 8, 1, 9, 3};
    size_t n = sizeof(nombres) / sizeof(nombres[0]);

    printf("Avant tri : ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    tri_bulle(nombres, n, sizeof(int), comparer_int);

    printf("Apres tri : ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    return 0;
}
