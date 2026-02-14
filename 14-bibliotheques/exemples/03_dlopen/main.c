/* ============================================================================
   Section 02.3 : Resolution de symboles au runtime
   Description : Chargement dynamique avec dlopen/dlsym
   Fichier source : 02.3-resolution-symboles.md
   ============================================================================ */

#include <stdio.h>
#include <dlfcn.h>

int main(void) {
    void *handle;
    int (*addition)(int, int);
    char *error;

    /* 1. Charger la bibliotheque */
    handle = dlopen("./libmath.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Erreur dlopen: %s\n", dlerror());
        return 1;
    }

    /* 2. Effacer les anciennes erreurs */
    dlerror();

    /* 3. Obtenir l'adresse de la fonction */
    *(void **)(&addition) = dlsym(handle, "addition");

    /* 4. Verifier les erreurs */
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Erreur dlsym: %s\n", error);
        dlclose(handle);
        return 1;
    }

    /* 5. Utiliser la fonction */
    printf("5 + 3 = %d\n", addition(5, 3));

    /* 6. Decharger la bibliotheque */
    dlclose(handle);

    return 0;
}
