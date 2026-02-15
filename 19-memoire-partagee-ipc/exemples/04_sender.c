/* ============================================================================
   Section 19.3 : Message Queues System V
   Description : Programme d'envoi de messages dans une queue
   Fichier source : 03-message-queues.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100

/* Structure du message */
struct message {
    long mtype;           /* Type (obligatoire, > 0) */
    char text[MSG_SIZE];  /* Contenu */
};

int main(void) {
    key_t key;
    int msqid;
    struct message msg;

    /* 1. Generer une cle unique */
    key = ftok("/tmp", 'M');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    /* 2. Creer ou obtenir la queue */
    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Queue de messages creee (ID: %d)\n", msqid);

    /* 3. Envoyer plusieurs messages */
    for (int i = 1; i <= 5; i++) {
        msg.mtype = (i + 1) % 2 + 1;  /* Types alternes : 1, 2, 1, 2, 1 */
        snprintf(msg.text, MSG_SIZE, "Message numero %d", i);

        printf("Envoi : type=%ld, texte=\"%s\"\n", msg.mtype, msg.text);

        /* Envoyer (taille = sizeof donnees, PAS sizeof structure) */
        if (msgsnd(msqid, &msg, sizeof(msg.text), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
    }

    printf("Tous les messages ont ete envoyes.\n");

    return 0;
}
