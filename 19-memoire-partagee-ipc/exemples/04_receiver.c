/* ============================================================================
   Section 19.3 : Message Queues System V
   Description : Programme de reception de messages depuis une queue
   Fichier source : 03-message-queues.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100

struct message {
    long mtype;
    char text[MSG_SIZE];
};

int main(void) {
    key_t key;
    int msqid;
    struct message msg;

    /* 1. Obtenir la cle (meme que sender) */
    key = ftok("/tmp", 'M');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    /* 2. Obtenir la queue existante */
    msqid = msgget(key, 0666);
    if (msqid == -1) {
        perror("msgget (la queue existe-t-elle ?)");
        exit(1);
    }

    printf("Connexion a la queue (ID: %d)\n", msqid);

    /* 3. Recevoir tous les messages */
    while (1) {
        /* Recevoir n'importe quel type (msgtyp = 0) */
        ssize_t bytes = msgrcv(msqid, &msg, sizeof(msg.text), 0, IPC_NOWAIT);

        if (bytes == -1) {
            /* Plus de messages (queue vide) */
            break;
        }

        printf("Recu : type=%ld, texte=\"%s\"\n", msg.mtype, msg.text);
    }

    printf("Tous les messages ont ete recus.\n");

    /* 4. Supprimer la queue */
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl IPC_RMID");
    }

    return 0;
}
