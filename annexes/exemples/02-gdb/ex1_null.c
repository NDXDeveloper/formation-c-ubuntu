// Exemple 1 : Pointeur NULL (ligne 921-932)
#include <stdio.h>

void crash() {
    int *ptr = NULL;
    *ptr = 42;  // CRASH !
}

int main() {
    crash();
    return 0;
}
