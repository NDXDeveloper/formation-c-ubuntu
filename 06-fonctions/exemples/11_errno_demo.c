/* ============================================================================
   Section 6.4 : Valeurs de retour et codes d'erreur
   Description : Utilisation de errno et strerror pour les erreurs systeme
   Fichier source : 04-valeurs-retour.md
   ============================================================================ */
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    FILE *fichier = fopen("inexistant.txt", "r");

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture\n");
        printf("Code errno : %d\n", errno);
        printf("Message : %s\n", strerror(errno));
    }

    return 0;
}
