// Ligne 1100-1125 : readdir listing
#include <dirent.h>
#include <stdio.h>

int main() {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s", entry->d_name);

        if (entry->d_type == DT_DIR)
            printf(" [DIR]");
        else if (entry->d_type == DT_REG)
            printf(" [FILE]");

        printf("\n");
    }

    closedir(dir);
    return 0;
}
