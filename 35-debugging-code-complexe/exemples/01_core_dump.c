/* ============================================================================
   Section 35.1 : Core dumps en production
   Description : Bug intentionnel - buffer overflow declenchant un crash
   Fichier source : 01-core-dumps-production.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

void process_data(int *array, int size) {
    for (int i = 0; i <= size; i++) {  /* Bug : <= au lieu de < */
        array[i] = i * 2;
    }
}

int main(void) {
    int *data = malloc(10 * sizeof(int));
    process_data(data, 10);
    free(data);
    return 0;
}
