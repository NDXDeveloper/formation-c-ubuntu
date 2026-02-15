/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Programme de test et diagnostic d'un FIFO
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void test_fifo(const char *path) {
    printf("Test du FIFO: %s\n", path);

    struct stat st;

    /* Verifier l'existence */
    if (stat(path, &st) == -1) {
        printf("  Erreur: FIFO n'existe pas\n");
        return;
    }

    /* Verifier que c'est un FIFO */
    if (!S_ISFIFO(st.st_mode)) {
        printf("  Erreur: N'est pas un FIFO\n");
        return;
    }

    printf("  FIFO existe\n");
    printf("  Permissions: %o\n", st.st_mode & 0777);

    /* Tester l'ouverture non-bloquante */
    int fd = open(path, O_WRONLY | O_NONBLOCK);

    if (fd == -1) {
        if (errno == ENXIO) {
            printf("  Aucun lecteur connecte\n");
        } else {
            perror("  Erreur open");
        }
    } else {
        printf("  Ouverture reussie\n");
        close(fd);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        /* Mode demo : creer un FIFO de test */
        const char *demo_path = "/tmp/fifo_test_demo";
        mkfifo(demo_path, 0666);
        test_fifo(demo_path);
        unlink(demo_path);
    } else {
        test_fifo(argv[1]);
    }

    return 0;
}
