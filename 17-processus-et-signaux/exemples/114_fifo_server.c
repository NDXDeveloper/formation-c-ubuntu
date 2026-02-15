/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Serveur FIFO bidirectionnel (paire avec 115_client)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define FIFO_REQUEST  "/tmp/fifo_request"
#define FIFO_RESPONSE "/tmp/fifo_response"

int main(void) {
    /* Creer les FIFO */
    mkfifo(FIFO_REQUEST, 0666);
    mkfifo(FIFO_RESPONSE, 0666);

    printf("[Serveur] Demarrage...\n");

    /* Ouvrir les FIFO */
    int fd_req = open(FIFO_REQUEST, O_RDONLY);
    int fd_resp = open(FIFO_RESPONSE, O_WRONLY);

    printf("[Serveur] Pret a recevoir des requetes\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd_req, buffer, sizeof(buffer))) > 0) {
        printf("[Serveur] Requete recue: %s\n", buffer);

        /* Traiter la requete (convertir en majuscules) */
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] = (char)(buffer[i] - 'a' + 'A');
            }
        }

        /* Envoyer la reponse */
        char response[200];
        snprintf(response, sizeof(response), "Reponse: %s", buffer);
        write(fd_resp, response, strlen(response) + 1);

        printf("[Serveur] Reponse envoyee\n");
    }

    close(fd_req);
    close(fd_resp);

    /* Nettoyer */
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);

    printf("[Serveur] Arret\n");

    return 0;
}
