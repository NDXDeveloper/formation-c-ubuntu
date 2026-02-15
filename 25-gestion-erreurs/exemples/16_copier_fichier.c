/* ============================================================================
   Section 25.2 : La variable errno
   Description : Gestionnaire d'erreurs robuste - copie de fichier avec goto
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* Fonction utilitaire pour afficher les erreurs */
void afficher_erreur(const char *fonction, const char *fichier) {
    int erreur = errno;  /* Sauvegarde immediate */

    fprintf(stderr, "Erreur dans %s pour '%s': ", fonction, fichier);

    switch (erreur) {
        case ENOENT:
            fprintf(stderr, "Fichier inexistant\n");
            break;
        case EACCES:
            fprintf(stderr, "Permission refusee\n");
            break;
        case EISDIR:
            fprintf(stderr, "C'est un repertoire\n");
            break;
        case ENOMEM:
            fprintf(stderr, "Memoire insuffisante\n");
            break;
        default:
            fprintf(stderr, "%s (code %d)\n", strerror(erreur), erreur);
    }
}

int copier_fichier(const char *source, const char *destination) {
    int fd_src = -1, fd_dest = -1;
    int resultat = -1;
    char buffer[4096];
    ssize_t octets_lus, octets_ecrits;

    /* Ouverture du fichier source */
    fd_src = open(source, O_RDONLY);
    if (fd_src == -1) {
        afficher_erreur("open (source)", source);
        goto cleanup;
    }

    /* Creation du fichier destination */
    fd_dest = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        afficher_erreur("open (destination)", destination);
        goto cleanup;
    }

    /* Copie des donnees */
    while ((octets_lus = read(fd_src, buffer, sizeof(buffer))) > 0) {
        octets_ecrits = write(fd_dest, buffer, (size_t)octets_lus);

        if (octets_ecrits == -1) {
            afficher_erreur("write", destination);
            goto cleanup;
        }

        if (octets_ecrits != octets_lus) {
            fprintf(stderr, "Erreur : ecriture partielle\n");
            goto cleanup;
        }
    }

    /* Verifier si la lecture s'est terminee par une erreur */
    if (octets_lus == -1) {
        afficher_erreur("read", source);
        goto cleanup;
    }

    printf("Copie reussie : %s -> %s\n", source, destination);
    resultat = 0;  /* Succes */

cleanup:
    if (fd_src != -1) close(fd_src);
    if (fd_dest != -1) close(fd_dest);

    return resultat;
}

int main(void) {
    /* Test 1 : copie reussie */
    printf("Test 1 : copie de /etc/hostname\n");
    int code = copier_fichier("/etc/hostname", "/tmp/hostname_copie.txt");
    printf("Code retour : %d\n\n", code);

    /* Test 2 : fichier source inexistant */
    printf("Test 2 : fichier inexistant\n");
    code = copier_fichier("inexistant.txt", "/tmp/dest.txt");
    printf("Code retour : %d\n\n", code);

    /* Test 3 : permission refusee */
    printf("Test 3 : permission refusee\n");
    code = copier_fichier("/etc/shadow", "/tmp/dest.txt");
    printf("Code retour : %d\n", code);

    /* Nettoyage */
    unlink("/tmp/hostname_copie.txt");
    unlink("/tmp/dest.txt");

    return 0;
}
