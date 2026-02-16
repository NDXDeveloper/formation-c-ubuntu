/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Formatage de durees lisibles (jours, heures, minutes, secondes)
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>

void format_duree(long secondes, char *buffer, size_t size) {
    long jours = secondes / 86400;
    long heures = (secondes % 86400) / 3600;
    long minutes = (secondes % 3600) / 60;
    long secs = secondes % 60;

    if (jours > 0) {
        snprintf(buffer, size, "%ldj %ldh %ldm %lds", jours, heures, minutes, secs);
    } else if (heures > 0) {
        snprintf(buffer, size, "%ldh %ldm %lds", heures, minutes, secs);
    } else if (minutes > 0) {
        snprintf(buffer, size, "%ldm %lds", minutes, secs);
    } else {
        snprintf(buffer, size, "%lds", secs);
    }
}

int main(void) {
    long durees[] = {45, 125, 3665, 90125, 259265};
    char buffer[64];

    for (int i = 0; i < 5; i++) {
        format_duree(durees[i], buffer, sizeof(buffer));
        printf("%7lds = %s\n", durees[i], buffer);
    }

    return 0;
}
