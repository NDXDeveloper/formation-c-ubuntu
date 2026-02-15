/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Client FIFO bidirectionnel (paire avec 114_server)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_REQUEST  "/tmp/fifo_request"
#define FIFO_RESPONSE "/tmp/fifo_response"

int main(void) {
    printf("[Client] Connexion au serveur...\n");

    /* Ouvrir les FIFO */
    int fd_req = open(FIFO_REQUEST, O_WRONLY);
    int fd_resp = open(FIFO_RESPONSE, O_RDONLY);

    if (fd_req == -1 || fd_resp == -1) {
        perror("open");
        return 1;
    }

    printf("[Client] Connecte!\n");

    /* Envoyer des requetes */
    const char *requests[] = {
        "hello",
        "world",
        "test",
        NULL
    };

    for (int i = 0; requests[i] != NULL; i++) {
        /* Envoyer la requete */
        printf("[Client] Envoi: %s\n", requests[i]);
        write(fd_req, requests[i], strlen(requests[i]) + 1);

        /* Recevoir la reponse */
        char buffer[100];
        read(fd_resp, buffer, sizeof(buffer));
        printf("[Client] Recu: %s\n\n", buffer);
    }

    close(fd_req);
    close(fd_resp);

    printf("[Client] Deconnecte\n");

    return 0;
}
