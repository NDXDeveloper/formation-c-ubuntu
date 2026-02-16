// Ligne 131-138 : leak ASan (fuite mémoire intentionnelle)
#include <stdlib.h>

int main() {
    int *ptr = malloc(100);
    // Oubli du free !
    return 0;
}
