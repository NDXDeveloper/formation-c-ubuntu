/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Lecteur file d'attente FIFO (paire avec 116_writer)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const char *fifo = "/tmp/queue_fifo";

    int fd = open(fifo, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("[Reader] Reception des messages...\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        printf("[Reader] Message: %s\n", buffer);
    }

    close(fd);

    return 0;
}
