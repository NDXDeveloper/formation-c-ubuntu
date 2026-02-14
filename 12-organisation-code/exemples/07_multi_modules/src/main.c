#include <stdio.h>
#include <string.h>
#include "math_utils.h"
#include "string_utils.h"
#include "config.h"

int main(void) {
    printf("=== %s v%s ===\n", APP_NAME, VERSION);

    /* Tests mathematiques */
    printf("\nTests mathematiques :\n");
    printf("5 + 3 = %d\n", addition(5, 3));
    printf("5 * 3 = %d\n", multiplication(5, 3));
    printf("2^10 = %d\n", puissance(2, 10));

    /* Tests de chaines */
    printf("\nTests de chaines :\n");
    char texte[] = "Bonjour";
    printf("Texte original : %s\n", texte);
    printf("Longueur : %zu\n", longueur_chaine(texte));

    inverser_chaine(texte);
    printf("Texte inverse : %s\n", texte);

    /* Configuration */
    if (debug_mode) {
        printf("\nMode debug active\n");
    }

    return 0;
}
