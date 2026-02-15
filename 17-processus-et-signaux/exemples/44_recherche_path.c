/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Recherche d'un executable dans le PATH
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *find_in_path(const char *program) {
    char *path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }

    /* Copier PATH pour ne pas le modifier */
    char *path_copy = strdup(path);
    if (path_copy == NULL) {
        return NULL;
    }
    char *dir = strtok(path_copy, ":");
    static char full_path[1024];

    while (dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, program);

        /* Verifier si le fichier existe et est executable */
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path;
        }

        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <programme>\n", argv[0]);
        return 1;
    }

    char *path = find_in_path(argv[1]);

    if (path != NULL) {
        printf("'%s' trouve: %s\n", argv[1], path);
    } else {
        printf("'%s' non trouve dans PATH\n", argv[1]);
    }

    return 0;
}
