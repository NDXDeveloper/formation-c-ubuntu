/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Formatage de tailles de fichiers lisibles (Ko, Mo, Go, To)
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>
#include <stdint.h>

void format_taille(uint64_t octets, char *buffer, size_t buffer_size) {
    const char *unites[] = {"o", "Ko", "Mo", "Go", "To"};
    int unite_index = 0;
    double taille = (double)octets;

    while (taille >= 1024.0 && unite_index < 4) {
        taille /= 1024.0;
        unite_index++;
    }

    if (unite_index == 0) {
        snprintf(buffer, buffer_size, "%lu %s", (unsigned long)octets, unites[unite_index]);
    } else {
        snprintf(buffer, buffer_size, "%.2f %s", taille, unites[unite_index]);
    }
}

int main(void) {
    uint64_t tailles[] = {512, 1024, 1048576, 1073741824, 1099511627776ULL};
    char buffer[32];

    for (int i = 0; i < 5; i++) {
        format_taille(tailles[i], buffer, sizeof(buffer));
        printf("%15lu octets = %s\n", (unsigned long)tailles[i], buffer);
    }

    return 0;
}
