#include <stdio.h>
#include "mon_projet.h"

int main(void) {
    int result = ma_lib_add(2, 3);
    if (result != 5) {
        fprintf(stderr, "ERREUR: 2+3 = %d (attendu 5)\n", result);
        return 1;
    }
    printf("Test OK\n");
    return 0;
}
