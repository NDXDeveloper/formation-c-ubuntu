/* ============================================================================
   Section 16.1 : Descripteurs de fichiers
   Description : Affichage des limites de descripteurs de fichiers
   Fichier source : 01-descripteurs-fichiers.md
   ============================================================================ */

#include <sys/resource.h>
#include <stdio.h>

int main(void) {
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("getrlimit");
        return 1;
    }

    printf("Limite souple : %lu\n", (unsigned long)limit.rlim_cur);
    printf("Limite dure   : %lu\n", (unsigned long)limit.rlim_max);

    return 0;
}
