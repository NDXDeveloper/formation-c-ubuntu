/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Separateur de chemin portable Windows/Unix
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>

// Définition du séparateur de chemin
#ifdef _WIN32
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STR "\\"
#else
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STR "/"
#endif

void afficher_chemin(const char* dossier, const char* fichier) {
    printf("%s%c%s\n", dossier, PATH_SEPARATOR, fichier);
}

int main(void) {
    // Fonctionne sur toutes les plateformes
    afficher_chemin("Documents", "rapport.txt");

#ifdef _WIN32
    printf("Chemin Windows : C:\\Users\\Alice\\Documents\\fichier.txt\n");
#else
    printf("Chemin Unix : /home/alice/documents/fichier.txt\n");
#endif

    return 0;
}
