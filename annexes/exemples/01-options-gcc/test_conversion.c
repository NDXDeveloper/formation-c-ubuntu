// Test du fragment ligne 228 : -Wconversion
#include <stdio.h>

int main(void) {
    int x = 3.14;  // Warning : perte de précision
    printf("%d\n", x);
    return 0;
}
