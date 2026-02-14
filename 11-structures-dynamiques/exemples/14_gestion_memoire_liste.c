/* ============================================================================
   Section 11.7 : Gestion memoire des structures dynamiques
   Description : Liste chainee avec chaines allouees dynamiquement,
                 liberation correcte et verification avec Valgrind
   Fichier source : 07-gestion-memoire.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L  /* Pour strdup() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* data;          /* Chaine allouee dynamiquement */
    struct Node* next;
} Node;

Node* create_node(const char* data) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }

    node->data = strdup(data);  /* Copie la chaine */
    if (node->data == NULL) {
        free(node);
        return NULL;
    }

    node->next = NULL;
    return node;
}

void free_node(Node* node) {
    if (node != NULL) {
        free(node->data);  /* Liberer la chaine d'abord */
        free(node);        /* Puis le noeud */
    }
}

void free_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free_node(current);
        current = next;
    }
}

void print_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("  -> \"%s\"\n", current->data);
        current = current->next;
    }
}

int main(void) {
    /* Construction */
    Node* head = create_node("Premier");
    if (head == NULL) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }
    head->next = create_node("Deuxieme");
    head->next->next = create_node("Troisieme");

    /* Utilisation */
    printf("Liste :\n");
    print_list(head);

    /* Liberation */
    printf("\nLiberation de la liste...\n");
    free_list(head);
    printf("Memoire liberee correctement.\n");
    printf("(Verifier avec : valgrind --leak-check=full ./14_gestion_memoire_liste)\n");

    return 0;
}
