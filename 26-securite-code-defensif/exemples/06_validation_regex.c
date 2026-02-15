/* ============================================================================
   Section 26.2 : Validation des entrees
   Description : Validation avec expressions regulieres POSIX
   Fichier source : 02-validation-entrees.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>
#include <regex.h>

bool valider_regex(const char *chaine, const char *pattern) {
    regex_t regex;
    int resultat;

    /* Compiler l'expression reguliere */
    resultat = regcomp(&regex, pattern, REG_EXTENDED);
    if (resultat != 0) {
        fprintf(stderr, "Erreur : regex invalide\n");
        return false;
    }

    /* Tester la correspondance */
    resultat = regexec(&regex, chaine, 0, NULL, 0);

    /* Liberer la regex */
    regfree(&regex);

    return (resultat == 0);
}

int main(void) {
    printf("=== Validation avec expressions regulieres POSIX ===\n\n");

    /* Pattern email simplifie */
    const char *pattern_email = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";

    printf("Pattern email : %s\n\n", pattern_email);

    const char *emails[] = {
        "user@example.com",
        "alice.bob@test.org",
        "invalide",
        "@missing.com",
        "user@.com",
        "nom+tag@domaine.fr"
    };
    int nb_emails = (int)(sizeof(emails) / sizeof(emails[0]));

    for (int i = 0; i < nb_emails; i++) {
        printf("  \"%s\" -> %s\n", emails[i],
               valider_regex(emails[i], pattern_email) ? "MATCH" : "NO MATCH");
    }

    /* Pattern pour un numero de telephone francais */
    printf("\nPattern telephone FR : ^0[1-9][0-9]{8}$\n\n");
    const char *pattern_tel = "^0[1-9][0-9]{8}$";

    const char *telephones[] = {
        "0612345678",
        "0112345678",
        "1234567890",
        "06123456",
        "0612345678901"
    };
    int nb_tel = (int)(sizeof(telephones) / sizeof(telephones[0]));

    for (int i = 0; i < nb_tel; i++) {
        printf("  \"%s\" -> %s\n", telephones[i],
               valider_regex(telephones[i], pattern_tel) ? "MATCH" : "NO MATCH");
    }

    return 0;
}
