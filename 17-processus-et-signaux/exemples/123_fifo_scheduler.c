/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Scheduler task queue - ajoute des taches (paire avec 122_worker)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TASK_FIFO "/tmp/task_queue"

typedef struct {
    int task_id;
    char command[100];
} Task;

int main(void) {
    int fd = open(TASK_FIFO, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *commands[] = {
        "backup_database",
        "generate_report",
        "send_emails",
        "cleanup_logs",
        NULL
    };

    for (int i = 0; commands[i] != NULL; i++) {
        Task task = {
            .task_id = i + 1,
            .command = {0}
        };
        strncpy(task.command, commands[i], sizeof(task.command) - 1);

        printf("[Scheduler] Ajout tache #%d: %s\n",
               task.task_id, task.command);

        write(fd, &task, sizeof(Task));
    }

    close(fd);

    return 0;
}
