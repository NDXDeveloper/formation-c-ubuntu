/* ============================================================================
   Section 5.3 : Operateurs bit-a-bit
   Description : Gestion de flags d'etat avec |= &= ^= (operations combinees)
   Fichier source : 03-operateurs-bitwise.md
   ============================================================================ */
#include <stdio.h>

#define STATUS_READY      (1 << 0)
#define STATUS_BUSY       (1 << 1)
#define STATUS_ERROR      (1 << 2)
#define STATUS_CONNECTED  (1 << 3)

int main(void) {
    unsigned int status = 0;

    status |= STATUS_READY;
    status |= STATUS_CONNECTED;

    printf("Status initial : 0x%02X\n", status);

    if (status & STATUS_READY) {
        printf("Systeme pret\n");
    }

    if (status & STATUS_BUSY) {
        printf("Systeme occupe\n");
    } else {
        printf("Systeme libre\n");
    }

    status |= STATUS_BUSY;
    printf("Status apres activation BUSY : 0x%02X\n", status);

    status &= ~STATUS_READY;
    printf("Status apres desactivation READY : 0x%02X\n", status);

    status ^= STATUS_ERROR;
    printf("Status apres inversion ERROR : 0x%02X\n", status);

    return 0;
}
