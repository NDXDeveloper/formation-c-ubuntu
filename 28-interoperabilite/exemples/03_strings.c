/* ============================================================================
   Section 28.1 : Appeler du C depuis Python
   Description : Fonctions de manipulation de chaines compilees en bibliotheque
                 partagee (majuscules sur place, longueur)
   Fichier source : 01-c-depuis-python.md
   ============================================================================ */

#include <string.h>

/* Affiche une chaine en majuscules (modifie sur place) */
void mettre_en_majuscules(char *chaine) {
    for (int i = 0; chaine[i] != '\0'; i++) {
        if (chaine[i] >= 'a' && chaine[i] <= 'z') {
            chaine[i] = chaine[i] - 32;
        }
    }
}

/* Retourne la longueur d'une chaine */
int longueur_chaine(const char *chaine) {
    return (int)strlen(chaine);
}
