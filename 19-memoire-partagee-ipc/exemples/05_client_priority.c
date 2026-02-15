/* ============================================================================
   Section 19.3 : Message Queues System V
   Description : Client envoyant des messages avec differentes priorites
   Fichier source : 03-message-queues.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100
#define MSG_URGENT 1
#define MSG_NORMAL 2
#define MSG_LOW 3

struct message {
    long mtype;
    char text[MSG_SIZE];
};

void send_message(int msqid, long type, const char *text) {
    struct message msg;
    msg.mtype = type;
    strncpy(msg.text, text, MSG_SIZE);
    msgsnd(msqid, &msg, sizeof(msg.text), 0);
}

int main(void) {
    key_t key = ftok("/tmp", 'P');
    int msqid = msgget(key, IPC_CREAT | 0666);

    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    /* Envoyer dans un ordre mixte */
    send_message(msqid, MSG_LOW, "Tache de nettoyage");
    send_message(msqid, MSG_NORMAL, "Traiter la commande #123");
    send_message(msqid, MSG_URGENT, "ALERTE SECURITE !");
    send_message(msqid, MSG_LOW, "Archiver les logs");
    send_message(msqid, MSG_URGENT, "Crash du serveur DB !");

    printf("Messages envoyes avec differentes priorites\n");

    return 0;
}
