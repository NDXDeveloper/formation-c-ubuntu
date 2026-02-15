/* ============================================================================
   Section 18.4 : Threads detaches vs joinable
   Description : Serveur simple avec threads detaches pour chaque client
   Fichier source : 04-detaches-vs-joinable.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int client_id;
    char requete[100];
} ClientData;

void *traiter_requete(void *arg) {
    pthread_detach(pthread_self());

    ClientData *client = (ClientData *)arg;

    printf("[Client %d] Requete recue: %s\n",
           client->client_id, client->requete);

    sleep(2);

    printf("[Client %d] Requete traitee\n", client->client_id);

    free(client);
    return NULL;
}

int main(void) {
    printf("Serveur demarre\n");

    for (int i = 1; i <= 5; i++) {
        ClientData *client = malloc(sizeof(ClientData));
        if (client == NULL) {
            fprintf(stderr, "Erreur malloc\n");
            continue;
        }
        client->client_id = i;
        snprintf(client->requete, sizeof(client->requete),
                "GET /data/%d", i);

        pthread_t tid;
        pthread_create(&tid, NULL, traiter_requete, client);

        sleep(1);
    }

    printf("En attente de terminaison des requetes...\n");
    sleep(5);

    printf("Serveur arrete\n");
    return 0;
}
