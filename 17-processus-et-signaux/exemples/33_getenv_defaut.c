/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Verification d'existence et valeur par defaut
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *editor = getenv("EDITOR");

    if (editor == NULL || strlen(editor) == 0) {
        printf("EDITOR non defini, utilisation de 'vi' par defaut\n");
        editor = "vi";
    }

    printf("Editeur configure: %s\n", editor);

    return 0;
}
