/* ============================================================================
   Section 19.3 : Message Queues System V
   Description : Serveur de priorites - traite les messages par priorite
   Fichier source : 03-message-queues.md
   ============================================================================ */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>

#define MSG_SIZE 100

/* Types de messages */
#define MSG_URGENT   1  /* Priorite haute */
#define MSG_NORMAL   2  /* Priorite normale */
#define MSG_LOW      3  /* Priorite basse */

struct message {
    long mtype;
    char text[MSG_SIZE];
};

int main(void) {
    key_t key = ftok("/tmp", 'P');
    int msqid = msgget(key, IPC_CREAT | 0666);
    struct message msg;
    int count = 0;

    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Serveur demarre (ID queue: %d)\n", msqid);
    printf("En attente de messages...\n\n");

    while (count < 5) {
        /* Recevoir les messages URGENTS en priorite (type <= 1) */
        ssize_t bytes = msgrcv(msqid, &msg, sizeof(msg.text), -1, IPC_NOWAIT);

        if (bytes != -1) {
            printf("[URGENT] %s\n", msg.text);
            count++;
            continue;
        }

        /* Puis les messages NORMAUX (type = 2) */
        bytes = msgrcv(msqid, &msg, sizeof(msg.text), MSG_NORMAL, IPC_NOWAIT);

        if (bytes != -1) {
            printf("[NORMAL] %s\n", msg.text);
            count++;
            continue;
        }

        /* Enfin les messages LOW (type = 3) */
        bytes = msgrcv(msqid, &msg, sizeof(msg.text), MSG_LOW, IPC_NOWAIT);

        if (bytes != -1) {
            printf("[LOW]    %s\n", msg.text);
            count++;
            continue;
        }

        /* Queue vide, attendre un peu */
        usleep(100000);  /* 100ms */
    }

    msgctl(msqid, IPC_RMID, NULL);
    printf("\nQueue supprimee.\n");
    return 0;
}
