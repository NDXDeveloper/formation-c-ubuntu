/* ============================================================================
   Section 26.2 : Validation des entrees
   Description : Validation de format (adresse email)
   Fichier source : 02-validation-entrees.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool valider_email(const char *email) {
    if (email == NULL || *email == '\0') {
        return false;
    }

    size_t longueur = strlen(email);
    if (longueur < 3 || longueur > 254) {  /* RFC 5321 */
        return false;
    }

    /* Rechercher le '@' */
    const char *arobase = strchr(email, '@');
    if (arobase == NULL) {
        return false;  /* Pas de '@' */
    }

    if (arobase == email) {
        return false;  /* '@' au debut */
    }

    if (arobase == email + longueur - 1) {
        return false;  /* '@' a la fin */
    }

    /* Verifier qu'il n'y a qu'un seul '@' */
    if (strchr(arobase + 1, '@') != NULL) {
        return false;  /* Plusieurs '@' */
    }

    /* Verifier la presence d'un point apres le '@' */
    const char *point = strchr(arobase + 1, '.');
    if (point == NULL || point == arobase + 1 || point == email + longueur - 1) {
        return false;
    }

    return true;
}

int main(void) {
    printf("=== Validation d'adresse email ===\n\n");

    const char *emails[] = {
        "utilisateur@example.com",
        "user@test.org",
        "@example.com",
        "user@",
        "user@@example.com",
        "user@.com",
        "user@example.",
        "",
        "ab",
        "simple@domaine.fr",
        "user.name+tag@sub.domain.com"
    };
    int nb = (int)(sizeof(emails) / sizeof(emails[0]));

    for (int i = 0; i < nb; i++) {
        printf("  \"%s\" -> %s\n", emails[i],
               valider_email(emails[i]) ? "VALIDE" : "INVALIDE");
    }

    return 0;
}
