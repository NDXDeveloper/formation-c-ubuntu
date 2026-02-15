/* ============================================================================
   Section 19.3 : Message Queues System V
   Description : Serveur de logging centralise avec filtrage par niveau
   Fichier source : 03-message-queues.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

struct log_msg {
    long mtype;        /* Niveau de log */
    pid_t sender_pid;
    time_t timestamp;
    char message[200];
};

const char* log_level_str(long level) {
    switch (level) {
        case LOG_ERROR: return "ERROR";
        case LOG_WARNING: return "WARN ";
        case LOG_INFO: return "INFO ";
        case LOG_DEBUG: return "DEBUG";
        default: return "?????";
    }
}

int main(int argc, char *argv[]) {
    key_t key = ftok("/tmp", 'L');
    int msqid = msgget(key, IPC_CREAT | 0666);
    struct log_msg log;
    int count = 0;

    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    /* Recuperer le niveau minimum a afficher (argument) */
    long min_level = (argc > 1) ? atoi(argv[1]) : LOG_INFO;

    printf("Logger demarre (niveau min: %s)\n", log_level_str(min_level));
    printf("Listening...\n\n");

    /* Lire les messages disponibles (non-bloquant pour test) */
    while (count < 20) {
        /* msgtyp negatif = prend le premier message de type <= abs(msgtyp) */
        ssize_t bytes = msgrcv(msqid, &log, sizeof(log) - sizeof(long),
                               -min_level, IPC_NOWAIT);

        if (bytes == -1) {
            if (errno == ENOMSG) {
                break;  /* Plus de messages */
            }
            perror("msgrcv");
            break;
        }

        /* Formater et afficher */
        char time_str[26];
        ctime_r(&log.timestamp, time_str);
        time_str[24] = '\0';  /* Retirer le \n */

        printf("[%s] [%s] [PID %d] %s\n",
               time_str,
               log_level_str(log.mtype),
               log.sender_pid,
               log.message);

        fflush(stdout);
        count++;
    }

    msgctl(msqid, IPC_RMID, NULL);
    printf("\nQueue supprimee. %d messages traites.\n", count);
    return 0;
}
