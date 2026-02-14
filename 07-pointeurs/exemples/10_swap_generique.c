/* ============================================================================
   Section 7.4 : Pointeurs void* et casting
   Description : Swap générique avec void* et memcpy
   Fichier source : 04-pointeurs-void.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

void swap_generique(void *a, void *b, size_t taille) {
    unsigned char temp[taille];

    memcpy(temp, a, taille);
    memcpy(a, b, taille);
    memcpy(b, temp, taille);
}

int main(void) {
    int x = 10, y = 20;
    float f1 = 3.14f, f2 = 2.71f;

    printf("Avant swap (int) : x = %d, y = %d\n", x, y);
    swap_generique(&x, &y, sizeof(int));
    printf("Apres swap (int) : x = %d, y = %d\n\n", x, y);

    printf("Avant swap (float) : f1 = %.2f, f2 = %.2f\n", f1, f2);
    swap_generique(&f1, &f2, sizeof(float));
    printf("Apres swap (float) : f1 = %.2f, f2 = %.2f\n", f1, f2);

    return 0;
}
