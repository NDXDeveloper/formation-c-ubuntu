/* ============================================================================
   Section 26.4 : Format string vulnerabilities
   Description : Demonstration de la difference entre printf vulnerable
                 et printf securise (sans exploitation reelle)
   Fichier source : 04-format-string-vulnerabilities.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

/* Version SECURISEE : toujours utiliser %s */
static void afficher_securise(const char *message) {
    printf("%s", message);
}

/* Demonstration sans exploitation */
int main(void) {
    printf("=== Format string vulnerabilities ===\n\n");

    /* Entrees de test simulant un utilisateur */
    const char *entrees[] = {
        "Alice",
        "Bonjour tout le monde",
        "%%x %%x %%x %%x",       /* Affiche litteralement les %x */
        "%%n est dangereux",
        "texte <normal>"
    };
    int nb = (int)(sizeof(entrees) / sizeof(entrees[0]));

    printf("--- Version SECURISEE (printf(\"%%s\", input)) ---\n");
    for (int i = 0; i < nb; i++) {
        printf("  Entree %d : ", i + 1);
        afficher_securise(entrees[i]);
        printf("\n");
    }

    printf("\n--- Pourquoi c'est important ---\n");
    printf("  printf(input)       -> VULNERABLE : les %%x, %%s, %%n sont interpretes\n");
    printf("  printf(\"%%s\", input) -> SUR : tout est affiche litteralement\n");

    printf("\n--- Fonctions de logging securisees ---\n");

    /* Pattern securise pour le logging */
    const char *messages_log[] = {
        "Connexion reussie",
        "Erreur de fichier",
        "Deconnexion"
    };
    const char *prefixes[] = {"INFO", "ERREUR", "INFO"};

    for (int i = 0; i < 3; i++) {
        /* Toujours utiliser un format fixe */
        printf("  [%s] %s\n", prefixes[i], messages_log[i]);
    }

    printf("\n--- Regles d'or ---\n");
    printf("  1. printf(\"%%s\", data) = TOUJOURS SUR\n");
    printf("  2. printf(data) = TOUJOURS DANGEREUX\n");
    printf("  3. Les formats sont du CODE, pas des DONNEES\n");
    printf("  4. Compiler avec -Wformat-security\n");

    return 0;
}
