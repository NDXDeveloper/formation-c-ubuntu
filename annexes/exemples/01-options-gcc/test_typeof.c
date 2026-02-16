// Test du fragment ligne 200-202 : typeof avec -pedantic
#include <stdio.h>

int main(void) {
    // Extension GNU : typeof
    typeof(int) x = 5;  // Warning avec -pedantic
    printf("%d\n", x);
    return 0;
}
