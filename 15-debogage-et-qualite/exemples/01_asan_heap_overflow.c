/* ============================================================================
   Section 01.1 : AddressSanitizer (ASan)
   Description : Heap buffer overflow - ecriture au-dela du buffer alloue
   Fichier source : 01.1-address-sanitizer.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *tableau = malloc(5 * sizeof(int));

    for (int i = 0; i < 10; i++) {  /* Boucle va trop loin ! */
        tableau[i] = i * 2;
    }

    printf("Terminé !\n");
    free(tableau);
    return 0;
}
