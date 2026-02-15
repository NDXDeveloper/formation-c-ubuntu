/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Event logger - recoit et journalise des evenements
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define EVENT_FIFO "/tmp/event_log"

typedef struct {
    time_t timestamp;
    int severity;  /* 0=INFO, 1=WARNING, 2=ERROR */
    char message[200];
} Event;

int main(void) {
    mkfifo(EVENT_FIFO, 0666);

    FILE *logfile = fopen("/tmp/events_test.log", "a");
    if (logfile == NULL) {
        perror("fopen");
        unlink(EVENT_FIFO);
        return 1;
    }

    printf("Event Logger demarre\n");

    int fd = open(EVENT_FIFO, O_RDONLY);

    if (fd == -1) {
        perror("open");
        fclose(logfile);
        unlink(EVENT_FIFO);
        return 1;
    }

    Event event;
    const char *severity_str[] = {"INFO", "WARNING", "ERROR"};

    while (read(fd, &event, sizeof(Event)) > 0) {
        char timestr[26];
        ctime_r(&event.timestamp, timestr);
        timestr[strlen(timestr) - 1] = '\0';  /* Retirer '\n' */

        if (event.severity >= 0 && event.severity <= 2) {
            printf("[%s] [%s] %s\n",
                   timestr,
                   severity_str[event.severity],
                   event.message);

            fprintf(logfile, "[%s] [%s] %s\n",
                    timestr,
                    severity_str[event.severity],
                    event.message);

            fflush(logfile);
        }
    }

    fclose(logfile);
    close(fd);
    unlink(EVENT_FIFO);

    return 0;
}
