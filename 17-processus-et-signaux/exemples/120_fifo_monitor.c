/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Monitor systeme - recoit des stats (paire avec 121_reporter)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/monitor_fifo"

typedef struct {
    float cpu_usage;
    long memory_used;
    int process_count;
} SystemStats;

int main(void) {
    mkfifo(FIFO_PATH, 0666);

    printf("Monitor demarre. En attente de statistiques...\n");

    int fd = open(FIFO_PATH, O_RDONLY);

    if (fd == -1) {
        perror("open");
        unlink(FIFO_PATH);
        return 1;
    }

    SystemStats stats;

    while (read(fd, &stats, sizeof(SystemStats)) > 0) {
        printf("=== Statistiques Systeme ===\n");
        printf("CPU:       %.1f%%\n", (double)stats.cpu_usage);
        printf("Memoire:   %ld MB\n", stats.memory_used);
        printf("Processus: %d\n", stats.process_count);
        printf("============================\n\n");
    }

    close(fd);
    unlink(FIFO_PATH);

    return 0;
}
