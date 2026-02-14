/* ============================================================================
   Section 8.4 : Manipulation avancee des chaines
   Description : Analyseur de logs avec parsing de lignes structurees
   Fichier source : 04-manipulation-chaines.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char timestamp[20];
    char niveau[10];
    char message[200];
} LogEntry;

int parser_log(const char *ligne, LogEntry *entry) {
    if (ligne[0] != '[') return 0;

    const char *fermeture = strchr(ligne, ']');
    if (!fermeture) return 0;

    size_t len_timestamp = fermeture - ligne - 1;
    if (len_timestamp >= sizeof(entry->timestamp)) return 0;

    strncpy(entry->timestamp, ligne + 1, len_timestamp);
    entry->timestamp[len_timestamp] = '\0';

    const char *ptr = fermeture + 1;
    while (*ptr && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    const char *deux_points = strchr(ptr, ':');
    if (!deux_points) return 0;

    size_t len_niveau = deux_points - ptr;
    if (len_niveau >= sizeof(entry->niveau)) return 0;

    strncpy(entry->niveau, ptr, len_niveau);
    entry->niveau[len_niveau] = '\0';

    ptr = deux_points + 1;
    while (*ptr && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    strncpy(entry->message, ptr, sizeof(entry->message) - 1);
    entry->message[sizeof(entry->message) - 1] = '\0';

    return 1;
}

void analyser_logs(const char *logs[], int nb_lignes) {
    int erreurs = 0, warnings = 0, infos = 0;

    printf("=== ANALYSE DES LOGS ===\n\n");

    for (int i = 0; i < nb_lignes; i++) {
        LogEntry entry;

        if (parser_log(logs[i], &entry)) {
            printf("Ligne %d:\n", i + 1);
            printf("  Timestamp: %s\n", entry.timestamp);
            printf("  Niveau: %s\n", entry.niveau);
            printf("  Message: %s\n\n", entry.message);

            if (strcmp(entry.niveau, "ERROR") == 0) {
                erreurs++;
            } else if (strcmp(entry.niveau, "WARNING") == 0) {
                warnings++;
            } else if (strcmp(entry.niveau, "INFO") == 0) {
                infos++;
            }
        }
    }

    printf("=== STATISTIQUES ===\n");
    printf("Erreurs: %d\n", erreurs);
    printf("Warnings: %d\n", warnings);
    printf("Infos: %d\n", infos);
}

int main(void) {
    const char *logs[] = {
        "[2024-01-15 10:30:45] INFO: Demarrage de l'application",
        "[2024-01-15 10:31:12] WARNING: Memoire disponible faible",
        "[2024-01-15 10:32:03] ERROR: Connexion a la base de donnees echouee",
        "[2024-01-15 10:33:21] INFO: Nouvelle tentative de connexion",
        "[2024-01-15 10:33:25] INFO: Connexion reussie"
    };

    analyser_logs(logs, 5);

    return 0;
}
