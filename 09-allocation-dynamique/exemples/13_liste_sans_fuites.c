/* ============================================================================
   Section 9.5 : Fuites memoire : causes et prevention
   Description : Liste chainee complete sans fuites memoire
   Fichier source : 05-fuites-memoire.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    size_t count;
} Liste;

Liste* creer_liste(void) {
    Liste* liste = malloc(sizeof(Liste));
    if (liste == NULL) return NULL;

    liste->head = NULL;
    liste->count = 0;

    return liste;
}

int ajouter(Liste* liste, const char* data) {
    if (liste == NULL || data == NULL) return 0;

    Node* nouveau = malloc(sizeof(Node));
    if (nouveau == NULL) return 0;

    nouveau->data = malloc(strlen(data) + 1);
    if (nouveau->data == NULL) {
        free(nouveau);
        return 0;
    }

    strcpy(nouveau->data, data);
    nouveau->next = liste->head;
    liste->head = nouveau;
    liste->count++;

    return 1;
}

void afficher(const Liste* liste) {
    if (liste == NULL) return;

    printf("Liste (%zu elements) : ", liste->count);

    Node* courant = liste->head;
    while (courant != NULL) {
        printf("%s ", courant->data);
        courant = courant->next;
    }
    printf("\n");
}

void liberer_liste(Liste* liste) {
    if (liste == NULL) return;

    Node* courant = liste->head;
    Node* suivant;

    while (courant != NULL) {
        suivant = courant->next;
        free(courant->data);
        free(courant);
        courant = suivant;
    }

    free(liste);
}

int main(void) {
    Liste* ma_liste = creer_liste();
    if (ma_liste == NULL) {
        fprintf(stderr, "Erreur de creation\n");
        return 1;
    }

    if (!ajouter(ma_liste, "Premier")) goto cleanup;
    if (!ajouter(ma_liste, "Deuxieme")) goto cleanup;
    if (!ajouter(ma_liste, "Troisieme")) goto cleanup;

    afficher(ma_liste);

cleanup:
    liberer_liste(ma_liste);

    printf("Programme termine sans fuite\n");
    return 0;
}
