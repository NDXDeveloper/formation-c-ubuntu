/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Capture de sortie de commandes avec popen()
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

void executer_et_lire(const char *commande) {
    FILE *fp = popen(commande, "r");
    if (fp == NULL) {
        perror("popen");
        return;
    }

    char ligne[256];
    printf("Sortie de '%s':\n", commande);

    while (fgets(ligne, sizeof(ligne), fp) != NULL) {
        printf("  %s", ligne);
    }

    int status = pclose(fp);
    if (status != 0) {
        fprintf(stderr, "Commande echouee avec code: %d\n", status);
    }
}

int main(void) {
    // Compter les processus
    executer_et_lire("ps -e --no-headers | wc -l");
    printf("\n");

    // Uptime
    executer_et_lire("uptime");

    return 0;
}
