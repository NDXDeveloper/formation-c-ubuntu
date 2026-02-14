/* ============================================================================
   Section 9.2 : Duree de vie et performance
   Description : Persistance des donnees Heap au-dela des fonctions
   Fichier source : 02-duree-vie-performance.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int* creer_donnees(void) {
    int* ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = 42;

    printf("Dans creer_donnees : donnees creees a l'adresse %p\n", (void*)ptr);
    return ptr;
}

int main(void) {
    printf("=== Debut du main ===\n");

    int* mes_donnees = creer_donnees();

    printf("Dans main : les donnees existent toujours !\n");
    printf("Valeur : %d a l'adresse %p\n", *mes_donnees, (void*)mes_donnees);

    *mes_donnees = 100;
    printf("Nouvelle valeur : %d\n", *mes_donnees);

    free(mes_donnees);

    printf("=== Fin du main ===\n");
    return 0;
}
