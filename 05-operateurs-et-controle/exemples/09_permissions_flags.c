/* ============================================================================
   Section 5.3 : Operateurs bit-a-bit
   Description : Gestion de permissions avec OR et AND (flags)
   Fichier source : 03-operateurs-bitwise.md
   ============================================================================ */
#include <stdio.h>

#define LECTURE   0x01
#define ECRITURE  0x02
#define EXECUTION 0x04

int main(void) {
    unsigned int permissions = 0;

    /* Activer la lecture et l'execution */
    permissions = permissions | LECTURE | EXECUTION;

    printf("Permissions: %u\n", permissions);

    if (permissions & LECTURE) {
        printf("Lecture autorisee\n");
    }
    if (permissions & ECRITURE) {
        printf("Ecriture autorisee\n");
    } else {
        printf("Ecriture non autorisee\n");
    }
    if (permissions & EXECUTION) {
        printf("Execution autorisee\n");
    }

    return 0;
}
