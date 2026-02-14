/* ============================================================================
   Section 01.4 : LeakSanitizer (LSan)
   Description : Fuite memoire - allocation sans liberation
   Fichier source : 01.4-leak-sanitizer.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /* Allocation 1 : OK, sera liberee */
    int *tableau1 = malloc(10 * sizeof(int));

    /* Allocation 2 : FUITE, jamais liberee */
    int *tableau2 = malloc(20 * sizeof(int));

    /* Utilisation */
    tableau1[0] = 42;
    tableau2[0] = 100;

    printf("Valeurs : %d, %d\n", tableau1[0], tableau2[0]);

    /* On libere seulement tableau1 */
    free(tableau1);

    /* Oubli de free(tableau2) -> Fuite memoire */

    return 0;
}
