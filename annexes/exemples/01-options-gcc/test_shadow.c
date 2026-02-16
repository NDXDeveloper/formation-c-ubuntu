// Test du fragment ligne 213-217 : -Wshadow
#include <stdio.h>

int x = 10;
void func(void) {
    int x = 20;  // Warning : masque la variable globale x
    printf("%d\n", x);
}

int main(void) {
    func();
    return 0;
}
