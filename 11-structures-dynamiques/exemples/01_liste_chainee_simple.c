/* ============================================================================
   Section 11.1 : Listes chainees simples
   Description : Exemple complet combinant creation, insertion, recherche,
                 suppression et liberation d'une liste chainee simple
   Fichier source : 01-listes-chainees.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

/* Creer un nouveau noeud */
Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : echec de l'allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = value;
    new_node->next = NULL;
    return new_node;
}

/* Inserer au debut de la liste */
void insert_at_beginning(Node** head_ref, int value) {
    Node* new_node = create_node(value);
    new_node->next = *head_ref;
    *head_ref = new_node;
}

/* Inserer a la fin de la liste */
void insert_at_end(Node** head_ref, int value) {
    Node* new_node = create_node(value);
    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }
    Node* current = *head_ref;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

/* Afficher la liste */
void print_list(Node* head) {
    Node* current = head;
    printf("Liste : ");
    while (current != NULL) {
        printf("%d --> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

/* Rechercher un element */
int search(Node* head, int value) {
    Node* current = head;
    while (current != NULL) {
        if (current->data == value) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/* Supprimer un noeud par valeur */
void delete_node(Node** head_ref, int value) {
    if (*head_ref == NULL) {
        printf("La liste est vide, rien a supprimer\n");
        return;
    }

    Node* current = *head_ref;

    if (current->data == value) {
        *head_ref = current->next;
        free(current);
        printf("Noeud avec valeur %d supprime\n", value);
        return;
    }

    Node* previous = NULL;
    while (current != NULL && current->data != value) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Valeur %d non trouvee\n", value);
        return;
    }

    previous->next = current->next;
    free(current);
    printf("Noeud avec valeur %d supprime\n", value);
}

/* Liberer toute la liste */
void free_list(Node** head_ref) {
    Node* current = *head_ref;
    Node* next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    *head_ref = NULL;
    printf("Liste liberee\n");
}

int main(void) {
    Node* head = NULL;

    printf("=== Insertion au debut ===\n");
    insert_at_beginning(&head, 30);
    insert_at_beginning(&head, 20);
    insert_at_beginning(&head, 10);
    print_list(head);

    printf("\n=== Insertion a la fin ===\n");
    insert_at_end(&head, 40);
    insert_at_end(&head, 50);
    print_list(head);

    printf("\n=== Recherche ===\n");
    if (search(head, 30)) {
        printf("30 trouve dans la liste\n");
    }
    if (!search(head, 100)) {
        printf("100 non trouve dans la liste\n");
    }

    printf("\n=== Suppression ===\n");
    delete_node(&head, 20);
    print_list(head);

    delete_node(&head, 10);
    print_list(head);

    printf("\n=== Liberation de la memoire ===\n");
    free_list(&head);
    print_list(head);

    return 0;
}
