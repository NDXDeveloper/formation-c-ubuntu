/* ============================================================================
   Section 34.3.1 : Collecte de metriques
   Description : Collecte de metriques CPU et memoire depuis /proc
   Fichier source : 03.1-collecte-metriques.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* --- CPU stats --- */

typedef struct {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
} cpu_stats_t;

int read_cpu_stats(cpu_stats_t *stats) {
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Erreur ouverture /proc/stat");
        return -1;
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    int parsed = sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                        &stats->user, &stats->nice, &stats->system,
                        &stats->idle, &stats->iowait, &stats->irq,
                        &stats->softirq, &stats->steal);

    fclose(fp);
    return (parsed < 8) ? -1 : 0;
}

double calculate_cpu_usage(cpu_stats_t *prev, cpu_stats_t *curr) {
    unsigned long long prev_idle = prev->idle + prev->iowait;
    unsigned long long curr_idle = curr->idle + curr->iowait;

    unsigned long long prev_total = prev->user + prev->nice + prev->system +
                                    prev->idle + prev->iowait + prev->irq +
                                    prev->softirq + prev->steal;
    unsigned long long curr_total = curr->user + curr->nice + curr->system +
                                    curr->idle + curr->iowait + curr->irq +
                                    curr->softirq + curr->steal;

    unsigned long long total_diff = curr_total - prev_total;
    unsigned long long idle_diff = curr_idle - prev_idle;

    if (total_diff == 0) return 0.0;
    return 100.0 * (double)(total_diff - idle_diff) / (double)total_diff;
}

/* --- Memory stats --- */

typedef struct {
    unsigned long total_kb;
    unsigned long free_kb;
    unsigned long available_kb;
    unsigned long buffers_kb;
    unsigned long cached_kb;
    unsigned long swap_total_kb;
    unsigned long swap_free_kb;
} memory_stats_t;

int read_memory_stats(memory_stats_t *stats) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("Erreur ouverture /proc/meminfo");
        return -1;
    }

    char line[256];
    memset(stats, 0, sizeof(memory_stats_t));

    while (fgets(line, sizeof(line), fp) != NULL) {
        char key[64];
        unsigned long value;

        if (sscanf(line, "%63s %lu kB", key, &value) == 2) {
            if (strcmp(key, "MemTotal:") == 0) stats->total_kb = value;
            else if (strcmp(key, "MemFree:") == 0) stats->free_kb = value;
            else if (strcmp(key, "MemAvailable:") == 0) stats->available_kb = value;
            else if (strcmp(key, "Buffers:") == 0) stats->buffers_kb = value;
            else if (strcmp(key, "Cached:") == 0) stats->cached_kb = value;
            else if (strcmp(key, "SwapTotal:") == 0) stats->swap_total_kb = value;
            else if (strcmp(key, "SwapFree:") == 0) stats->swap_free_kb = value;
        }
    }

    fclose(fp);
    return 0;
}

/* --- Main --- */

int main(void) {
    printf("=== Collecte de metriques systeme ===\n\n");

    /* CPU */
    printf("Mesure CPU (1 seconde)...\n");
    cpu_stats_t prev_stats, curr_stats;
    if (read_cpu_stats(&prev_stats) == 0) {
        sleep(1);
        if (read_cpu_stats(&curr_stats) == 0) {
            double cpu_usage = calculate_cpu_usage(&prev_stats, &curr_stats);
            printf("Utilisation CPU: %.2f%%\n", cpu_usage);
        }
    }

    /* Memoire */
    printf("\n");
    memory_stats_t mem;
    if (read_memory_stats(&mem) == 0) {
        unsigned long used_kb = mem.total_kb - mem.free_kb -
                                mem.buffers_kb - mem.cached_kb;
        double mem_pct = 100.0 * (double)used_kb / (double)mem.total_kb;

        printf("Memoire Totale:     %lu MB\n", mem.total_kb / 1024);
        printf("Memoire Utilisee:   %lu MB (%.2f%%)\n", used_kb / 1024, mem_pct);
        printf("Memoire Disponible: %lu MB\n", mem.available_kb / 1024);

        if (mem.swap_total_kb > 0) {
            unsigned long swap_used = mem.swap_total_kb - mem.swap_free_kb;
            double swap_pct = 100.0 * (double)swap_used / (double)mem.swap_total_kb;
            printf("Swap Total:         %lu MB\n", mem.swap_total_kb / 1024);
            printf("Swap Utilise:       %lu MB (%.2f%%)\n", swap_used / 1024, swap_pct);
        }
    }

    return 0;
}
