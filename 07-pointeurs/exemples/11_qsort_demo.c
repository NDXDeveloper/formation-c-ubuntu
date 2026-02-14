/* ============================================================================
   Section 7.4 : Pointeurs void* et casting
   Description : Tri avec qsort et callbacks void*
   Fichier source : 04-pointeurs-void.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int comparer_entiers(const void *a, const void *b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    int nombres[5] = {50, 10, 40, 20, 30};

    printf("Avant tri : ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    qsort(nombres, 5, sizeof(int), comparer_entiers);

    printf("Apres tri : ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    return 0;
}
