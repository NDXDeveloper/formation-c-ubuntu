/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Worker task queue - traite les taches (paire avec 123_scheduler)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define TASK_FIFO "/tmp/task_queue"

typedef struct {
    int task_id;
    char command[100];
} Task;

int main(void) {
    mkfifo(TASK_FIFO, 0666);

    printf("[Worker] Pret a traiter des taches\n");

    int fd = open(TASK_FIFO, O_RDONLY);

    if (fd == -1) {
        perror("open");
        unlink(TASK_FIFO);
        return 1;
    }

    Task task;

    while (read(fd, &task, sizeof(Task)) > 0) {
        printf("[Worker] Traitement tache #%d: %s\n",
               task.task_id, task.command);

        /* Simuler le traitement */
        sleep(2);

        printf("[Worker] Tache #%d terminee\n", task.task_id);
    }

    close(fd);
    unlink(TASK_FIFO);

    return 0;
}
