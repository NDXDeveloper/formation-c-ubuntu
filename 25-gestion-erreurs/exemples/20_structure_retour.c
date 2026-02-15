/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Structure de retour avec etat (parser de configuration)
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SUCCES = 0,
    ERREUR_SYNTAXE = 1,
    ERREUR_VALEUR = 2,
    ERREUR_FICHIER = 3
} CodeErreur;

typedef struct {
    CodeErreur code;
    int ligne;
    char message[256];
    int valeur;  /* Resultat si succes */
} ResultatParsing;

ResultatParsing parser_entier(const char *texte, int ligne) {
    ResultatParsing resultat = {0, 0, "", 0};
    resultat.ligne = ligne;

    if (texte == NULL || texte[0] == '\0') {
        resultat.code = ERREUR_SYNTAXE;
        snprintf(resultat.message, sizeof(resultat.message),
                 "Texte vide a la ligne %d", ligne);
        return resultat;
    }

    char *fin;
    long valeur = strtol(texte, &fin, 10);

    if (*fin != '\0') {
        resultat.code = ERREUR_SYNTAXE;
        snprintf(resultat.message, sizeof(resultat.message),
                 "Caractere invalide '%c' a la ligne %d", *fin, ligne);
        return resultat;
    }

    if (valeur < 0 || valeur > 100) {
        resultat.code = ERREUR_VALEUR;
        snprintf(resultat.message, sizeof(resultat.message),
                 "Valeur %ld hors limites [0-100] a la ligne %d", valeur, ligne);
        return resultat;
    }

    /* Succes */
    resultat.code = SUCCES;
    resultat.valeur = (int)valeur;
    snprintf(resultat.message, sizeof(resultat.message), "OK");

    return resultat;
}

int main(void) {
    const char *tests[] = {"42", "150", "abc", ""};

    for (int i = 0; i < 4; i++) {
        ResultatParsing r = parser_entier(tests[i], i + 1);

        if (r.code == SUCCES) {
            printf("Ligne %d : valeur = %d\n", r.ligne, r.valeur);
        } else {
            fprintf(stderr, "Erreur : %s\n", r.message);
        }
    }

    return 0;
}
