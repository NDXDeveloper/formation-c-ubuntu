/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Remonter la chaine de parente jusqu'a systemd via /proc
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 256

/* Fonction pour lire le nom du processus depuis /proc */
void get_process_name(pid_t pid, char *name, size_t len) {
    char path[64];
    char cmdline[MAX_CMD_LEN];
    FILE *fp;

    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    fp = fopen(path, "r");
    if (fp == NULL) {
        snprintf(name, len, "???");
        return;
    }

    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
        /* Extraire juste le nom du programme */
        char *base = strrchr(cmdline, '/');
        if (base != NULL) {
            snprintf(name, len, "%s", base + 1);
        } else {
            snprintf(name, len, "%s", cmdline);
        }
    } else {
        snprintf(name, len, "???");
    }

    fclose(fp);
}

/* Fonction pour obtenir le PPID depuis /proc */
pid_t get_parent_pid(pid_t pid) {
    char path[64];
    char line[256];
    FILE *fp;
    pid_t ppid = 0;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line, "PPid:\t%d", &ppid);
            break;
        }
    }

    fclose(fp);
    return ppid;
}

int main(void) {
    pid_t current_pid = getpid();
    char process_name[MAX_CMD_LEN];
    int level = 0;

    printf("Chaine de parente (du processus actuel vers systemd):\n");
    printf("========================================================\n\n");

    while (current_pid > 0) {
        get_process_name(current_pid, process_name, sizeof(process_name));

        /* Indentation selon le niveau */
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        printf("-- PID %d: %s", current_pid, process_name);

        if (current_pid == 1) {
            printf(" [INIT/SYSTEMD - racine de l'arbre]\n");
            break;
        }

        printf("\n");

        /* Remonter au parent */
        pid_t parent = get_parent_pid(current_pid);

        if (parent == 0 || parent == current_pid) {
            break;
        }

        current_pid = parent;
        level++;
    }

    return 0;
}
