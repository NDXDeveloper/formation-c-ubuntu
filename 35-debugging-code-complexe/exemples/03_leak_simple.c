/* ============================================================================
   Section 35.2 : Resolution des fuites memoire
   Description : Bug intentionnel - fuite memoire simple (allocation sans free)
   Fichier source : 02-resolution-memory-leaks.md
   ============================================================================ */
#include <stdlib.h>
#include <stdio.h>

void process_request(void) {
    char *buffer = malloc(1024);
    /* Traitement... */
    /* Bug : pas de free(buffer) */
    (void)buffer;
}

int main(void) {
    printf("Allocation de 10 buffers sans liberation...\n");
    for (int i = 0; i < 10; i++) {
        process_request();  /* Fuit 1024 octets a chaque appel */
    }
    printf("Termine (10240 octets fuites)\n");
    return 0;
}
