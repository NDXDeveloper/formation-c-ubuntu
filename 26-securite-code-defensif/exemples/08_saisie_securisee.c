/* ============================================================================
   Section 26.3 : Prevention des buffer overflows
   Description : Saisie utilisateur securisee avec fgets et snprintf
   Fichier source : 03-prevention-buffer-overflows.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Fonction helper pour lire une ligne */
static bool lire_ligne(const char *prompt, char *buffer, size_t buffer_size) {
    printf("%s", prompt);

    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        return false;
    }

    /* Retirer le '\n' final */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    return true;
}

int main(void) {
    printf("=== Saisie securisee (demo non-interactive) ===\n\n");

    /* Simulation de saisies avec des donnees de test */
    const char *noms[] = {"Dupont", "Martin", "Jean-Francois"};
    const char *prenoms[] = {"Alice", "Bob", "Caroline"};
    const char *adresses[] = {"12 rue du Code", "5 avenue C", "8 bd Linux"};

    for (int i = 0; i < 3; i++) {
        char complet[100];
        int result = snprintf(complet, sizeof(complet),
                              "%s %s habite a %s",
                              prenoms[i], noms[i], adresses[i]);

        if (result < 0 || (size_t)result >= sizeof(complet)) {
            fprintf(stderr, "Erreur : informations trop longues\n");
            continue;
        }

        printf("  %s\n", complet);
    }

    /* Demonstration que lire_ligne est disponible */
    printf("\nFonction lire_ligne() disponible pour saisie interactive.\n");
    printf("  Prototype : bool lire_ligne(prompt, buffer, buffer_size)\n");
    printf("  Utilise fgets() au lieu de scanf(\"%%s\")\n");

    /* Suppression warning unused */
    (void)lire_ligne;

    return 0;
}
