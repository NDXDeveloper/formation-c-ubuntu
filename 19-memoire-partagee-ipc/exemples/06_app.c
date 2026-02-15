/* ============================================================================
   Section 19.3 : Message Queues System V
   Description : Application envoyant des logs vers le logger centralise
   Fichier source : 03-message-queues.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

struct log_msg {
    long mtype;
    pid_t sender_pid;
    time_t timestamp;
    char message[200];
};

void send_log(int msqid, long level, const char *message) {
    struct log_msg log;
    log.mtype = level;
    log.sender_pid = getpid();
    log.timestamp = time(NULL);
    strncpy(log.message, message, sizeof(log.message) - 1);
    log.message[sizeof(log.message) - 1] = '\0';

    msgsnd(msqid, &log, sizeof(log) - sizeof(long), 0);
}

int main(void) {
    key_t key = ftok("/tmp", 'L');
    int msqid = msgget(key, IPC_CREAT | 0666);

    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Application demarree (PID %d)\n", getpid());

    /* Simuler differents evenements */
    send_log(msqid, LOG_INFO, "Application demarree");
    send_log(msqid, LOG_DEBUG, "Initialisation des modules");
    send_log(msqid, LOG_WARNING, "Configuration manquante, utilisation des defauts");
    send_log(msqid, LOG_ERROR, "Echec de connexion a la base de donnees");
    send_log(msqid, LOG_INFO, "Tentative de reconnexion...");
    send_log(msqid, LOG_INFO, "Connexion etablie");
    send_log(msqid, LOG_DEBUG, "Traitement de 100 requetes");

    printf("Logs envoyes\n");

    return 0;
}
