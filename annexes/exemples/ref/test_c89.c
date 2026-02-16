// Ligne 63-76 : exemple C89/C90
#include <stdio.h>

/* Prototype obligatoire */
int add(int a, int b);

int add(int a, int b) {
    return a + b;
}

int main(void) {
    printf("Résultat : %d\n", add(3, 4));
    return 0;
}
