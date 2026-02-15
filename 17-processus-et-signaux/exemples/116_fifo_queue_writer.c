/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Ecrivain file d'attente FIFO (paire avec 117_reader)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    const char *fifo = "/tmp/queue_fifo";

    printf("[%d] Ouverture du FIFO...\n", getpid());
    int fd = open(fifo, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("[%d] Envoi: %s\n", getpid(), argv[1]);
    write(fd, argv[1], strlen(argv[1]) + 1);

    close(fd);

    return 0;
}
