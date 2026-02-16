// Test du fragment ligne 142-148 : warning variable non initialisée
#include <stdio.h>

int main() {
    int x;
    printf("%d\n", x);  // Warning : x non initialisé
    return 0;
}
