// Ligne 487-501 : leak Valgrind (fuite + out of bounds intentionnels)
#include <stdlib.h>
#include <string.h>

int main() {
    char *str = malloc(100);
    strcpy(str, "Hello");
    // Oubli du free !

    int *arr = malloc(50 * sizeof(int));
    arr[60] = 42;  // Out of bounds
    free(arr);

    return 0;
}
