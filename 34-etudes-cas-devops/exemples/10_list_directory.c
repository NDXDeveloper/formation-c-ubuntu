/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Lister le contenu d'un repertoire avec opendir/readdir
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <dirent.h>
#include <string.h>

void lister_repertoire(const char *chemin) {
    DIR *dir = opendir(chemin);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    printf("Contenu de %s:\n", chemin);

    while ((entry = readdir(dir)) != NULL) {
        // Ignorer . et ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Type de fichier
        char type = '?';
        switch (entry->d_type) {
            case DT_REG: type = 'f'; break;  // Fichier
            case DT_DIR: type = 'd'; break;  // Répertoire
            case DT_LNK: type = 'l'; break;  // Lien symbolique
            case DT_FIFO: type = 'p'; break; // Pipe
            case DT_SOCK: type = 's'; break; // Socket
        }

        printf("  [%c] %s\n", type, entry->d_name);
    }

    closedir(dir);
}

int main(void) {
    lister_repertoire("/etc/apt");
    return 0;
}
