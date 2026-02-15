/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Lecteur FIFO - recoit des messages (paire avec 111_writer)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    printf("[Reader] Ouverture du FIFO en lecture...\n");
    printf("[Reader] (bloque jusqu'a ce qu'un ecrivain se connecte)\n");

    int fd = open(fifo_path, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("[Reader] Connecte! Reception de messages...\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        printf("[Reader] Recu: %s\n", buffer);
    }

    close(fd);
    printf("[Reader] Termine\n");

    return 0;
}
