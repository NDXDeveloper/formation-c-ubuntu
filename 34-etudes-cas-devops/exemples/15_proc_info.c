/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Lecture de /proc (CPU, memoire, uptime)
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <string.h>

void afficher_info_cpu(void) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("fopen /proc/cpuinfo");
        return;
    }

    char ligne[256];
    int cpu_count = 0;

    printf("=== CPU ===\n");
    while (fgets(ligne, sizeof(ligne), fp) != NULL) {
        if (strncmp(ligne, "processor", 9) == 0) {
            cpu_count++;
        }
        if (strncmp(ligne, "model name", 10) == 0) {
            if (cpu_count == 1) {
                printf("%s", ligne);
            }
        }
    }

    printf("Nombre de CPUs: %d\n", cpu_count);
    fclose(fp);
}

void afficher_info_memoire(void) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen /proc/meminfo");
        return;
    }

    char ligne[256];
    long mem_total = 0, mem_libre = 0, mem_disponible = 0;

    while (fgets(ligne, sizeof(ligne), fp) != NULL) {
        if (strncmp(ligne, "MemTotal:", 9) == 0) {
            sscanf(ligne, "MemTotal: %ld kB", &mem_total);
        } else if (strncmp(ligne, "MemFree:", 8) == 0) {
            sscanf(ligne, "MemFree: %ld kB", &mem_libre);
        } else if (strncmp(ligne, "MemAvailable:", 13) == 0) {
            sscanf(ligne, "MemAvailable: %ld kB", &mem_disponible);
        }
    }

    printf("\n=== Memoire ===\n");
    printf("Mémoire totale: %ld MB\n", mem_total / 1024);
    printf("Mémoire libre: %ld MB\n", mem_libre / 1024);
    printf("Mémoire disponible: %ld MB\n", mem_disponible / 1024);
    printf("Mémoire utilisée: %ld MB\n", (mem_total - mem_disponible) / 1024);

    fclose(fp);
}

void afficher_uptime(void) {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        perror("fopen /proc/uptime");
        return;
    }

    double uptime, idle;
    if (fscanf(fp, "%lf %lf", &uptime, &idle) != 2) {
        fprintf(stderr, "Erreur lecture uptime\n");
        fclose(fp);
        return;
    }
    fclose(fp);

    int jours = (int)(uptime / 86400);
    int heures = (int)((uptime - (jours * 86400)) / 3600);
    int minutes = (int)((uptime - (jours * 86400) - (heures * 3600)) / 60);

    printf("\n=== Uptime ===\n");
    printf("Uptime: %d jours, %d heures, %d minutes\n", jours, heures, minutes);
}

int main(void) {
    afficher_info_cpu();
    afficher_info_memoire();
    afficher_uptime();
    return 0;
}
