/* ============================================================================
   Section 34.2.2 : Expressions regulieres
   Description : Base POSIX regex - compilation, match et extraction de groupes
   Fichier source : 02.2-expressions-regulieres.md
   ============================================================================ */
#include <stdio.h>
#include <regex.h>
#include <string.h>

int main(void) {
    regex_t regex;
    regmatch_t matches[10];

    // Pattern avec groupes capturants (date ISO)
    const char *pattern = "([0-9]{4})-([0-9]{2})-([0-9]{2})";

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Erreur compilation regex\n");
        return 1;
    }

    const char *texte = "Date: 2025-01-15";

    if (regexec(&regex, texte, 10, matches, 0) == 0) {
        printf("Match complet: %.*s\n",
               (int)(matches[0].rm_eo - matches[0].rm_so),
               texte + matches[0].rm_so);

        printf("Année: %.*s\n",
               (int)(matches[1].rm_eo - matches[1].rm_so),
               texte + matches[1].rm_so);

        printf("Mois: %.*s\n",
               (int)(matches[2].rm_eo - matches[2].rm_so),
               texte + matches[2].rm_so);

        printf("Jour: %.*s\n",
               (int)(matches[3].rm_eo - matches[3].rm_so),
               texte + matches[3].rm_so);
    } else {
        printf("Pas de match\n");
    }

    regfree(&regex);
    return 0;
}
