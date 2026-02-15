/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Ecrivain FIFO - envoie des messages (paire avec 112_reader)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    printf("[Writer] Ouverture du FIFO en ecriture...\n");
    printf("[Writer] (bloque jusqu'a ce qu'un lecteur se connecte)\n");

    int fd = open(fifo_path, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("[Writer] Connecte! Envoi de messages...\n");

    const char *messages[] = {
        "Premier message",
        "Deuxieme message",
        "Troisieme message",
        NULL
    };

    for (int i = 0; messages[i] != NULL; i++) {
        write(fd, messages[i], strlen(messages[i]) + 1);
        printf("[Writer] Envoye: %s\n", messages[i]);
        sleep(1);
    }

    close(fd);
    printf("[Writer] Termine\n");

    return 0;
}
