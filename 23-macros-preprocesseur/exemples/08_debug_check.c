/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Macros de debug et de verification de pointeur NULL
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

#define CHECK_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "Erreur : pointeur NULL à %s:%d\n", \
                    __FILE__, __LINE__); \
            return -1; \
        } \
    } while(0)

int traiter_donnees(int *data, int taille) {
    CHECK_NULL(data);

    int somme = 0;
    for (int i = 0; i < taille; i++) {
        somme += data[i];
    }
    DEBUG_PRINT("Somme calculée: %d", somme);
    printf("Somme = %d\n", somme);
    return 0;
}

int main(void) {
    // Test avec données valides
    int donnees[] = {10, 20, 30, 40, 50};
    int ret = traiter_donnees(donnees, 5);
    printf("Retour (données valides) : %d\n", ret);

    // Test avec pointeur NULL
    ret = traiter_donnees(NULL, 0);
    printf("Retour (pointeur NULL) : %d\n", ret);

    return 0;
}
