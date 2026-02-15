/* ============================================================================
   Section 26.4 : Format string vulnerabilities
   Description : Systeme de logging securise avec formats constants
   Fichier source : 04-format-string-vulnerabilities.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>
#include <string.h>

/* Formats definis comme constantes */
#define FMT_USER_INFO "Utilisateur: %s, ID: %d\n"
#define FMT_ERROR     "Erreur: %s\n"
#define FMT_LOG       "[%s] %s%s\n"

/* Logging securise avec timestamp */
static void log_event(const char *event) {
    time_t now = time(NULL);
    char *time_str = ctime(&now);

    /* Retirer le \n de ctime */
    size_t len = strlen(time_str);
    if (len > 0 && time_str[len - 1] == '\n') {
        time_str[len - 1] = '\0';
    }

    /* SECURISE : format fixe avec %s */
    printf(FMT_LOG, time_str, "EVENT: ", event);
}

/* Logging avec niveaux securise */
static void log_with_level(int level, const char *message) {
    const char *prefixes[] = {"INFO: ", "WARN: ", "ERROR: "};
    if (level < 0 || level > 2) {
        level = 0;
    }
    /* SECURISE : format fixe */
    printf("[%s] %s%s\n", "LOG", prefixes[level], message);
}

/* Wrapper securise pour fprintf */
static void safe_fprintf(FILE *stream, const char *message) {
    fprintf(stream, "%s", message);
}

int main(void) {
    printf("=== Systeme de logging securise ===\n\n");

    /* Logging avec timestamp */
    printf("--- Log avec timestamp ---\n");
    log_event("Demarrage du serveur");
    log_event("Connexion client 192.168.1.10");
    log_event("Tentative avec %%x %%x %%x");  /* Pas interprete */

    /* Logging avec niveaux */
    printf("\n--- Log avec niveaux ---\n");
    log_with_level(0, "Serveur demarre sur port 8080");
    log_with_level(1, "Memoire disponible faible");
    log_with_level(2, "Connexion base de donnees perdue");

    /* Messages d'information securises */
    printf("\n--- Messages formattes ---\n");
    printf(FMT_USER_INFO, "Alice", 1001);
    printf(FMT_USER_INFO, "Bob", 1002);

    /* safe_fprintf */
    printf("\n--- safe_fprintf ---\n");
    safe_fprintf(stdout, "Message securise via safe_fprintf\n");
    safe_fprintf(stdout, "Meme avec %%n %%x -> affiche tel quel\n");

    printf("\n=== Logging securise termine ===\n");
    return 0;
}
