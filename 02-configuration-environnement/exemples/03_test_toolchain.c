/* ============================================================================
   Section 2.5.3 : Configuration GCC/GDB/CMake/Valgrind
   Description : Test complet de la toolchain avec allocation memoire
   Fichier source : 05.3-configuration-outils.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("=== Test de l'environnement ===\n");

    /* Test allocation memoire */
    int *numbers = malloc(5 * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 5; i++) {
        numbers[i] = i * 10;
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }

    /* Liberation correcte */
    free(numbers);

    printf("=== Test réussi ! ===\n");
    return EXIT_SUCCESS;
}
