/* ============================================================================
   Section 17.2 : Les PIDs et hierarchie
   Description : Lister les processus enfants via /proc
   Fichier source : 02-pids-hierarchie.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void list_children(pid_t parent_pid) {
    DIR *proc_dir;
    struct dirent *entry;
    char path[512];
    char line[256];
    FILE *fp;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir /proc");
        return;
    }

    printf("Enfants du processus %d:\n", parent_pid);

    while ((entry = readdir(proc_dir)) != NULL) {
        /* Ignorer les entrees non numeriques */
        if (entry->d_name[0] < '0' || entry->d_name[0] > '9') {
            continue;
        }

        /* Construire le chemin vers /proc/PID/status */
        snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

        fp = fopen(path, "r");
        if (fp == NULL) {
            continue;
        }

        /* Chercher la ligne PPid */
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "PPid:", 5) == 0) {
                int ppid;
                sscanf(line, "PPid:\t%d", &ppid);

                if (ppid == parent_pid) {
                    printf("  -- PID %s\n", entry->d_name);
                }
                break;
            }
        }

        fclose(fp);
    }

    closedir(proc_dir);
}

int main(void) {
    pid_t my_pid = getpid();

    printf("Mon PID: %d\n", my_pid);
    printf("Creation de 3 processus enfants...\n\n");

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            printf("  Enfant %d cree avec PID %d\n", i + 1, getpid());
            sleep(5);  /* Rester actif */
            return 0;
        }
    }

    sleep(1);  /* Laisser les enfants s'initialiser */
    printf("\n");

    list_children(my_pid);

    sleep(5);  /* Attendre que les enfants terminent */

    return 0;
}
