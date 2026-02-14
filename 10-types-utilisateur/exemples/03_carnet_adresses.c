/* ============================================================================
   Section 10.1 : struct - Creation d'objets complexes
   Description : Carnet d'adresses simple avec structures
   Fichier source : 01-structures.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

#define MAX_CONTACTS 100

struct Contact {
    char nom[50];
    char telephone[15];
    char email[50];
};

void afficherContact(struct Contact c) {
    printf("Nom       : %s\n", c.nom);
    printf("Telephone : %s\n", c.telephone);
    printf("Email     : %s\n", c.email);
    printf("-------------------\n");
}

int main(void) {
    struct Contact carnet[MAX_CONTACTS];
    int nbContacts = 0;

    strcpy(carnet[nbContacts].nom, "Alice Dupont");
    strcpy(carnet[nbContacts].telephone, "06.12.34.56.78");
    strcpy(carnet[nbContacts].email, "alice@example.com");
    nbContacts++;

    strcpy(carnet[nbContacts].nom, "Bob Martin");
    strcpy(carnet[nbContacts].telephone, "06.98.76.54.32");
    strcpy(carnet[nbContacts].email, "bob@example.com");
    nbContacts++;

    printf("=== CARNET D'ADRESSES ===\n\n");
    for (int i = 0; i < nbContacts; i++) {
        afficherContact(carnet[i]);
    }

    return 0;
}
