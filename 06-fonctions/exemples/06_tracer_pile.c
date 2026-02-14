/* ============================================================================
   Section 6.2 : La Pile - Stack Frames
   Description : Tracer manuellement la pile avec des appels imbriques
   Fichier source : 02-stack-frames.md
   ============================================================================ */
#include <stdio.h>

int double_nombre(int n) {
    printf("  [double_nombre] n = %d\n", n);
    return n * 2;
}

int triple_nombre(int n) {
    printf(" [triple_nombre] n = %d\n", n);
    int result = double_nombre(n);
    printf(" [triple_nombre] result = %d\n", result);
    return result * 3;
}

int main(void) {
    printf("[main] Début\n");
    int x = 5;
    int y = triple_nombre(x);
    printf("[main] y = %d\n", y);
    return 0;
}
