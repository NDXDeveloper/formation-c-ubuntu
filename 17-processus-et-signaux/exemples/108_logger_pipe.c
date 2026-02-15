/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Logger centralise - plusieurs workers envoient des logs
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

void worker(int id, int log_pipe) {
    char log_msg[100];

    for (int i = 0; i < 3; i++) {
        snprintf(log_msg, sizeof(log_msg),
                "[Worker %d] Message %d", id, i + 1);

        write(log_pipe, log_msg, strlen(log_msg) + 1);

        sleep(1);
    }
}

int main(void) {
    int log_pipe[2];
    pipe(log_pipe);

    /* Creer 3 workers */
    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            close(log_pipe[0]);
            worker(i + 1, log_pipe[1]);
            close(log_pipe[1]);
            return 0;
        }
    }

    /* Logger principal */
    close(log_pipe[1]);

    FILE *logfile = fopen("/tmp/application_test.log", "w");
    if (logfile == NULL) {
        perror("fopen");
        close(log_pipe[0]);
        while (wait(NULL) > 0) { /* vide */ }
        return 1;
    }

    char buffer[100];
    ssize_t n;

    printf("Logger actif, reception des messages...\n\n");

    while ((n = read(log_pipe[0], buffer, sizeof(buffer))) > 0) {
        time_t now = time(NULL);
        char timestr[64];
        ctime_r(&now, timestr);
        timestr[strlen(timestr) - 1] = '\0';  /* Retirer '\n' */

        fprintf(logfile, "[%s] %s\n", timestr, buffer);
        printf("[%s] %s\n", timestr, buffer);

        fflush(logfile);
    }

    fclose(logfile);
    close(log_pipe[0]);

    /* Attendre tous les enfants */
    while (wait(NULL) > 0) { /* vide */ }

    printf("\nLogs ecrits dans /tmp/application_test.log\n");

    return 0;
}
