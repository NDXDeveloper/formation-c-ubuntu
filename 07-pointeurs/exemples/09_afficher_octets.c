/* ============================================================================
   Section 7.4 : Pointeurs void* et casting
   Description : Fonction générique afficher_octets avec void*
   Fichier source : 04-pointeurs-void.md
   ============================================================================ */

#include <stdio.h>

void afficher_octets(void *ptr, size_t taille) {
    unsigned char *bytes = (unsigned char*)ptr;

    printf("Contenu en octets : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

int main(void) {
    int nombre = 0x12345678;
    float decimal = 3.14f;

    printf("Representation d'un int (0x12345678) :\n");
    afficher_octets(&nombre, sizeof(nombre));

    printf("\nRepresentation d'un float (3.14f) :\n");
    afficher_octets(&decimal, sizeof(decimal));

    return 0;
}
