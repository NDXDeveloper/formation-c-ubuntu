// Ligne 101-110 : bug ASan (heap buffer overflow intentionnel)
#include <stdlib.h>
#include <stdio.h>

int main() {
    int *arr = malloc(10 * sizeof(int));
    arr[10] = 42;  // Buffer overflow !
    free(arr);
    return 0;
}
