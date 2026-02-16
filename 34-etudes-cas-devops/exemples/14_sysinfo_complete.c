/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Outil complet d'information systeme (uname, memoire, disque)
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <string.h>

void afficher_separateur(void) {
    printf("========================================\n");
}

void info_systeme(void) {
    struct utsname uts;
    if (uname(&uts) == 0) {
        printf("Système: %s %s\n", uts.sysname, uts.release);
        printf("Machine: %s\n", uts.machine);
        printf("Hostname: %s\n", uts.nodename);
    }
}

void info_disque(void) {
    struct statvfs stat;

    if (statvfs("/", &stat) == 0) {
        unsigned long total = (stat.f_blocks * stat.f_frsize) / (1024 * 1024 * 1024);
        unsigned long libre = (stat.f_bfree * stat.f_frsize) / (1024 * 1024 * 1024);
        unsigned long utilise = total - libre;
        int pourcentage = 0;
        if (total > 0) {
            pourcentage = (int)((utilise * 100) / total);
        }

        printf("Espace disque /:\n");
        printf("  Total: %lu GB\n", total);
        printf("  Utilisé: %lu GB (%d%%)\n", utilise, pourcentage);
        printf("  Libre: %lu GB\n", libre);
    }
}

void info_memoire(void) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char ligne[256];
        long total = 0, disponible = 0;

        while (fgets(ligne, sizeof(ligne), fp)) {
            if (strncmp(ligne, "MemTotal:", 9) == 0) {
                sscanf(ligne, "MemTotal: %ld kB", &total);
            } else if (strncmp(ligne, "MemAvailable:", 13) == 0) {
                sscanf(ligne, "MemAvailable: %ld kB", &disponible);
            }
        }
        fclose(fp);

        long utilise = total - disponible;
        int pourcentage = 0;
        if (total > 0) {
            pourcentage = (int)((utilise * 100) / total);
        }

        printf("Mémoire RAM:\n");
        printf("  Total: %ld MB\n", total / 1024);
        printf("  Utilisée: %ld MB (%d%%)\n", utilise / 1024, pourcentage);
        printf("  Disponible: %ld MB\n", disponible / 1024);
    }
}

void info_utilisateur(void) {
    char *user = getenv("USER");
    char *home = getenv("HOME");

    printf("Utilisateur: %s\n", user ? user : "inconnu");
    printf("Répertoire home: %s\n", home ? home : "inconnu");
    printf("PID: %d\n", getpid());
}

int main(void) {
    printf("\n");
    afficher_separateur();
    printf("  INFORMATIONS SYSTÈME\n");
    afficher_separateur();
    printf("\n");

    info_systeme();
    printf("\n");

    info_utilisateur();
    printf("\n");

    info_memoire();
    printf("\n");

    info_disque();
    printf("\n");

    afficher_separateur();

    return 0;
}
