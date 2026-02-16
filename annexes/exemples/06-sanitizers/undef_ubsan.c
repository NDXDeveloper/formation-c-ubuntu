// Ligne 216-230 : undefined behavior (UBSan)
#include <stdio.h>

int main() {
    int a = 2147483647;  // INT_MAX
    int b = a + 1;       // Signed integer overflow !

    int arr[5];
    arr[10] = 42;        // Out of bounds !

    int *ptr = NULL;
    *ptr = 10;           // Null pointer dereference !

    return 0;
}
