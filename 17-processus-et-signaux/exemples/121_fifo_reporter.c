/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Reporter systeme - envoie des stats (paire avec 120_monitor)
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/monitor_fifo"

typedef struct {
    float cpu_usage;
    long memory_used;
    int process_count;
} SystemStats;

int main(void) {
    int fd = open(FIFO_PATH, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* Simuler des statistiques */
    for (int i = 0; i < 5; i++) {
        SystemStats stats = {
            .cpu_usage = 45.5f + (float)i * 2.0f,
            .memory_used = 4096 + i * 100,
            .process_count = 150 + i
        };

        printf("[Reporter] Envoi stats: CPU=%.1f%% MEM=%ldMB PROC=%d\n",
               (double)stats.cpu_usage, stats.memory_used, stats.process_count);

        write(fd, &stats, sizeof(SystemStats));
        sleep(2);
    }

    close(fd);

    return 0;
}
