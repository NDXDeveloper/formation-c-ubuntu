/* ============================================================================
   Section 22.5 : Fonctions variadiques
   Description : Concatenation de plusieurs chaines avec fonction variadique
   Fichier source : 05-fonctions-variadiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * Concatene plusieurs chaines en une seule
 * @param dest Buffer de destination
 * @param size Taille du buffer
 * @param count Nombre de chaines a concatener
 * @param ... Les chaines a concatener
 * @return Longueur totale de la chaine resultante
 */
size_t concatener(char *dest, size_t size, int count, ...) {
    if (dest == NULL || size == 0) {
        return 0;
    }

    dest[0] = '\0';  // Initialiser a chaine vide
    size_t total = 0;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        const char *str = va_arg(args, const char*);

        if (str != NULL) {
            size_t len = strlen(str);
            if (total + len < size) {
                strcat(dest, str);
                total += len;
            } else {
                // Plus de place
                break;
            }
        }
    }

    va_end(args);
    return total;
}

int main(void) {
    char buffer[100];

    concatener(buffer, sizeof(buffer), 4,
               "Bonjour ", "le ", "monde ", "!");
    printf("%s\n", buffer);

    concatener(buffer, sizeof(buffer), 3,
               "C", " est ", "genial");
    printf("%s\n", buffer);

    return 0;
}
