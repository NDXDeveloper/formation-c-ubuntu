/* ============================================================================
   Section 34.2.2 : Expressions regulieres
   Description : Parser de logs Apache/Nginx avec regex POSIX
   Fichier source : 02.2-expressions-regulieres.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

typedef struct {
    char ip[16];
    char date[32];
    char methode[8];
    char url[256];
    int code_statut;
    long taille;
} LogEntry;

int parser_ligne_log(const char *ligne, LogEntry *entry) {
    regex_t regex;
    regmatch_t matches[7];

    // Pattern pour log Apache/Nginx
    const char *pattern =
        "^([0-9.]+) "                          // IP (groupe 1)
        "[^ ]+ [^ ]+ "                          // identité et user
        "\\[([^]]+)\\] "                       // Date (groupe 2)
        "\"([A-Z]+) "                          // Méthode HTTP (groupe 3)
        "([^ ]+) "                              // URL (groupe 4)
        "[^\"]+\" "                             // Version HTTP
        "([0-9]+) "                             // Code statut (groupe 5)
        "([0-9]+)";                             // Taille (groupe 6)

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        return -1;
    }

    if (regexec(&regex, ligne, 7, matches, 0) != 0) {
        regfree(&regex);
        return -1;
    }

    snprintf(entry->ip, sizeof(entry->ip), "%.*s",
             (int)(matches[1].rm_eo - matches[1].rm_so),
             ligne + matches[1].rm_so);

    snprintf(entry->date, sizeof(entry->date), "%.*s",
             (int)(matches[2].rm_eo - matches[2].rm_so),
             ligne + matches[2].rm_so);

    snprintf(entry->methode, sizeof(entry->methode), "%.*s",
             (int)(matches[3].rm_eo - matches[3].rm_so),
             ligne + matches[3].rm_so);

    snprintf(entry->url, sizeof(entry->url), "%.*s",
             (int)(matches[4].rm_eo - matches[4].rm_so),
             ligne + matches[4].rm_so);

    char code_str[8];
    snprintf(code_str, sizeof(code_str), "%.*s",
             (int)(matches[5].rm_eo - matches[5].rm_so),
             ligne + matches[5].rm_so);
    entry->code_statut = atoi(code_str);

    char taille_str[16];
    snprintf(taille_str, sizeof(taille_str), "%.*s",
             (int)(matches[6].rm_eo - matches[6].rm_so),
             ligne + matches[6].rm_so);
    entry->taille = atol(taille_str);

    regfree(&regex);
    return 0;
}

int main(void) {
    const char *lignes[] = {
        "192.168.1.100 - - [15/Jan/2025:14:30:45 +0000] "
        "\"GET /api/users HTTP/1.1\" 200 1234",

        "10.0.0.1 - admin [15/Jan/2025:14:31:00 +0000] "
        "\"POST /api/login HTTP/1.1\" 401 89",

        "172.16.0.50 - - [15/Jan/2025:14:31:15 +0000] "
        "\"GET /index.html HTTP/1.1\" 304 0",
    };

    for (int i = 0; i < 3; i++) {
        LogEntry entry;
        printf("=== Ligne %d ===\n", i + 1);

        if (parser_ligne_log(lignes[i], &entry) == 0) {
            printf("IP: %s\n", entry.ip);
            printf("Date: %s\n", entry.date);
            printf("Méthode: %s\n", entry.methode);
            printf("URL: %s\n", entry.url);
            printf("Code: %d\n", entry.code_statut);
            printf("Taille: %ld octets\n\n", entry.taille);
        } else {
            printf("Erreur de parsing\n\n");
        }
    }

    return 0;
}
