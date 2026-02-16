/* ============================================================================
   Section 34.3.2 : Export Prometheus
   Description : Header pour le module de metriques
   Fichier source : 03.2-export-prometheus.md
   ============================================================================ */
#ifndef METRICS_H
#define METRICS_H

#include <stddef.h>

typedef struct {
    double cpu_usage_percent;
    unsigned long long memory_total_kb;
    unsigned long long memory_used_kb;
    unsigned long long memory_available_kb;
    double load_1min;
    double load_5min;
    double load_15min;
} system_metrics_t;

int collect_all_metrics(system_metrics_t *metrics);
void generate_metrics(char *buffer, size_t buffer_size);

#endif
