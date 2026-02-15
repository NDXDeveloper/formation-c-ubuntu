/* ============================================================================
   Section 26.2 : Validation des entrees
   Description : Validation de type avec strtol (conversion chaine -> entier)
   Fichier source : 02-validation-entrees.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

bool lire_age(const char *entree, int *age) {
    char *fin;
    errno = 0;

    long valeur = strtol(entree, &fin, 10);

    /* Verifications multiples */
    if (errno == ERANGE) {
        fprintf(stderr, "Erreur : nombre trop grand ou trop petit\n");
        return false;
    }

    if (fin == entree) {
        fprintf(stderr, "Erreur : aucun chiffre trouve\n");
        return false;
    }

    if (*fin != '\0' && *fin != '\n') {
        fprintf(stderr, "Erreur : caracteres invalides apres le nombre\n");
        return false;
    }

    if (valeur < INT_MIN || valeur > INT_MAX) {
        fprintf(stderr, "Erreur : valeur hors limites pour un int\n");
        return false;
    }

    *age = (int)valeur;
    return true;
}

int main(void) {
    printf("=== Validation de type (strtol) ===\n\n");

    /* Tests avec differentes entrees */
    const char *tests[] = {
        "25",
        "abc",
        "25abc",
        "-5",
        "150",
        "0",
        "999999999999999999999",
        "",
        "42\n"
    };
    int nb_tests = (int)(sizeof(tests) / sizeof(tests[0]));

    for (int i = 0; i < nb_tests; i++) {
        int age;
        printf("Test \"%s\" : ", tests[i]);
        if (lire_age(tests[i], &age)) {
            printf("OK -> age = %d\n", age);
        } else {
            printf("REJETE\n");
        }
    }

    return 0;
}
