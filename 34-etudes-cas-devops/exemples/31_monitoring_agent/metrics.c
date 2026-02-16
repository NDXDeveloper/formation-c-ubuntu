/* ============================================================================
   Section 34.3.2 : Export Prometheus
   Description : Collecte de metriques et generation format Prometheus
   Fichier source : 03.2-export-prometheus.md
   ============================================================================ */
#include "metrics.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>

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

static int read_cpu_stats(cpu_stats_t *stats) {
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) return -1;

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    int parsed = sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                        &stats->user, &stats->nice, &stats->system, &stats->idle,
                        &stats->iowait, &stats->irq, &stats->softirq, &stats->steal);

    fclose(fp);
    return (parsed < 8) ? -1 : 0;
}

static double calculate_cpu_usage(cpu_stats_t *prev, cpu_stats_t *curr) {
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

int collect_all_metrics(system_metrics_t *metrics) {
    cpu_stats_t prev_cpu, curr_cpu;
    struct sysinfo info;

    if (read_cpu_stats(&prev_cpu) != 0) return -1;
    sleep(1);
    if (read_cpu_stats(&curr_cpu) != 0) return -1;
    metrics->cpu_usage_percent = calculate_cpu_usage(&prev_cpu, &curr_cpu);

    if (sysinfo(&info) != 0) return -1;

    metrics->memory_total_kb = info.totalram / 1024;
    metrics->memory_available_kb = info.freeram / 1024;
    metrics->memory_used_kb = metrics->memory_total_kb - metrics->memory_available_kb;

    metrics->load_1min = info.loads[0] / 65536.0;
    metrics->load_5min = info.loads[1] / 65536.0;
    metrics->load_15min = info.loads[2] / 65536.0;

    return 0;
}

void generate_metrics(char *buffer, size_t buffer_size) {
    system_metrics_t metrics;
    int offset = 0;

    if (collect_all_metrics(&metrics) != 0) {
        snprintf(buffer, buffer_size, "# Error collecting metrics\n");
        return;
    }

    offset += snprintf(buffer + offset, buffer_size - (size_t)offset,
        "# HELP node_cpu_usage_percent CPU usage percentage\n"
        "# TYPE node_cpu_usage_percent gauge\n"
        "node_cpu_usage_percent %.2f\n\n",
        metrics.cpu_usage_percent);

    offset += snprintf(buffer + offset, buffer_size - (size_t)offset,
        "# HELP node_memory_total_bytes Total memory in bytes\n"
        "# TYPE node_memory_total_bytes gauge\n"
        "node_memory_total_bytes %llu\n\n",
        metrics.memory_total_kb * 1024ULL);

    offset += snprintf(buffer + offset, buffer_size - (size_t)offset,
        "# HELP node_memory_used_bytes Used memory in bytes\n"
        "# TYPE node_memory_used_bytes gauge\n"
        "node_memory_used_bytes %llu\n\n",
        metrics.memory_used_kb * 1024ULL);

    offset += snprintf(buffer + offset, buffer_size - (size_t)offset,
        "# HELP node_load1 1m load average\n"
        "# TYPE node_load1 gauge\n"
        "node_load1 %.2f\n\n",
        metrics.load_1min);

    offset += snprintf(buffer + offset, buffer_size - (size_t)offset,
        "# HELP node_load5 5m load average\n"
        "# TYPE node_load5 gauge\n"
        "node_load5 %.2f\n\n",
        metrics.load_5min);

    (void)offset;
    snprintf(buffer + offset, buffer_size - (size_t)offset,
        "# HELP node_load15 15m load average\n"
        "# TYPE node_load15 gauge\n"
        "node_load15 %.2f\n",
        metrics.load_15min);
}
