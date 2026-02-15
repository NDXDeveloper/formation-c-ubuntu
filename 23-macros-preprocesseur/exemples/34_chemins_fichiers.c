/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Chemins de fichiers multi-plateforme
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#ifdef _WIN32
    #define SEPARATEUR '\\'
    #define FICHIER_CONFIG "C:\\Program Files\\MonApp\\config.txt"
#else
    #define SEPARATEUR '/'
    #define FICHIER_CONFIG "/etc/monapp/config.txt"
#endif

int main(void) {
    printf("Séparateur de chemin: %c\n", SEPARATEUR);
    printf("Fichier de config: %s\n", FICHIER_CONFIG);

    return 0;
}
