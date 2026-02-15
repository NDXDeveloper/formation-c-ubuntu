/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Variables de localisation (LANG, LC_ALL)
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

int main(void) {
    /* Lire la locale depuis l'environnement */
    char *lang = getenv("LANG");
    char *lc_all = getenv("LC_ALL");

    printf("Configuration de localisation:\n");
    printf("  LANG:   %s\n", lang ? lang : "(non defini)");
    printf("  LC_ALL: %s\n", lc_all ? lc_all : "(non defini)");

    /* Configurer la locale pour le programme */
    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "Erreur lors de la configuration de la locale\n");
    }

    /* Afficher des nombres avec formatage local */
    printf("\nExemple de formatage:\n");
    printf("  Nombre: %d\n", 1234567);

    return 0;
}
