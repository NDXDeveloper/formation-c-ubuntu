/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Securite - validation des variables et nettoyage de l'environnement
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>

/* Exemple securise : validation avant utilisation */
void good_example(void) {
    char *path = getenv("FILE_PATH");

    if (path == NULL) {
        fprintf(stderr, "FILE_PATH non defini\n");
        return;
    }

    /* Valider le chemin */
    if (strlen(path) > PATH_MAX || strchr(path, ';') != NULL) {
        fprintf(stderr, "Chemin invalide\n");
        return;
    }

    /* Utiliser une API sure */
    FILE *fp = fopen(path, "r");
    if (fp != NULL) {
        printf("Fichier ouvert avec succes: %s\n", path);
        fclose(fp);
    } else {
        printf("Impossible d'ouvrir: %s\n", path);
    }
}

void clean_environment(void) {
    /* Supprimer les variables potentiellement dangereuses */
    unsetenv("LD_PRELOAD");
    unsetenv("LD_LIBRARY_PATH");
    unsetenv("IFS");

    /* Definir un PATH sur */
    setenv("PATH", "/usr/bin:/bin", 1);
    printf("Environnement nettoye\n");
}

int main(void) {
    printf("=== Nettoyage de l'environnement ===\n");

    /* Pour un programme setuid ou privilegie */
    if (getuid() != geteuid()) {
        clean_environment();
    } else {
        printf("Pas de privileges speciaux, nettoyage non necessaire\n");
    }

    printf("\n=== Validation de FILE_PATH ===\n");
    good_example();

    return 0;
}
