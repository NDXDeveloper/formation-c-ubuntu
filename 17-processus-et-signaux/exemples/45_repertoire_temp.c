/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Recherche du repertoire temporaire via variables d'environnement
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_temp_dir(void) {
    char *tmpdir = getenv("TMPDIR");
    if (tmpdir == NULL || strlen(tmpdir) == 0) {
        tmpdir = getenv("TEMP");
    }
    if (tmpdir == NULL || strlen(tmpdir) == 0) {
        tmpdir = getenv("TMP");
    }
    if (tmpdir == NULL || strlen(tmpdir) == 0) {
        tmpdir = "/tmp";  /* Valeur par defaut */
    }
    return tmpdir;
}

int main(void) {
    char *tmpdir = get_temp_dir();
    printf("Repertoire temporaire: %s\n", tmpdir);

    /* Construire un chemin vers un fichier temporaire */
    char temp_file[1024];
    snprintf(temp_file, sizeof(temp_file), "%s/mon_fichier_temp.txt", tmpdir);
    printf("Fichier temporaire: %s\n", temp_file);

    return 0;
}
