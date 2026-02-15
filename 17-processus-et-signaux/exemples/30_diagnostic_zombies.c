/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Programme de diagnostic - recherche de processus zombies
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void check_zombies(void) {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;
    int zombie_count = 0;

    if (!proc_dir) {
        perror("opendir");
        return;
    }

    printf("Recherche de processus zombies...\n\n");

    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
            continue;

        char path[512];
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);

        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        char line[512];
        if (fgets(line, sizeof(line), fp)) {
            char *state = strchr(line, ')');
            if (state && state[2] == 'Z') {
                printf("Zombie trouve: PID %s\n", entry->d_name);
                zombie_count++;
            }
        }

        fclose(fp);
    }

    closedir(proc_dir);

    printf("\nTotal de zombies: %d\n", zombie_count);
}

int main(void) {
    check_zombies();
    return 0;
}
