/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Parametre overwrite de setenv() (0 = ne pas ecraser, 1 = ecraser)
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /* Definir une variable */
    setenv("TEST", "valeur1", 1);
    printf("TEST = %s\n", getenv("TEST"));

    /* Tenter de la modifier avec overwrite = 0 */
    setenv("TEST", "valeur2", 0);  /* N'ecrase PAS */
    printf("TEST apres setenv(..., 0) = %s\n", getenv("TEST"));

    /* Modifier avec overwrite = 1 */
    setenv("TEST", "valeur3", 1);  /* Ecrase */
    printf("TEST apres setenv(..., 1) = %s\n", getenv("TEST"));

    return 0;
}
