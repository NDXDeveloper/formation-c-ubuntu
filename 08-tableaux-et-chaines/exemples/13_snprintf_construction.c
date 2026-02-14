/* ============================================================================
   Section 8.6 : Fonctions securisees
   Description : Construction progressive d'un message avec snprintf
   Fichier source : 06-fonctions-securisees.md
   ============================================================================ */

#include <stdio.h>

void construire_message_securise(char *buffer, size_t size) {
    int offset = 0;

    offset += snprintf(buffer + offset, size - offset, "Ligne 1\n");
    if (offset >= (int)size) return;

    offset += snprintf(buffer + offset, size - offset, "Ligne 2\n");
    if (offset >= (int)size) return;

    offset += snprintf(buffer + offset, size - offset, "Ligne 3\n");
}

int main(void) {
    char message[100];
    construire_message_securise(message, sizeof(message));
    printf("%s", message);
    return 0;
}
