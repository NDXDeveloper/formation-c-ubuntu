/* ============================================================================
   Section 9.1 : Stack vs Heap
   Description : Comparaison directe Stack et Heap dans une meme fonction
   Fichier source : 01-stack-vs-heap.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exemple_complet(void) {
    /* ===== STACK ===== */
    int age = 25;
    char prenom[20] = "Alice";

    printf("Stack - Age: %d, Prenom: %s\n", age, prenom);

    /* ===== HEAP ===== */
    int longueur = 100;
    char* description = malloc(longueur * sizeof(char));

    if (description == NULL) {
        printf("Erreur d'allocation\n");
        return;
    }

    strcpy(description, "Ceci est une description sur le Heap");
    printf("Heap - Description: %s\n", description);

    free(description);
}

int main(void) {
    exemple_complet();
    return 0;
}
