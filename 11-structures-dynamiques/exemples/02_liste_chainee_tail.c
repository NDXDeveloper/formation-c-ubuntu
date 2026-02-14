/* ============================================================================
   Section 11.1 : Listes chainees simples - Variantes et optimisations
   Description : Liste chainee avec pointeur tail pour insertion en O(1) a la fin
   Fichier source : 01-listes-chainees.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
    Node* tail;
    size_t size;
} LinkedList;

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

/* Initialiser la liste */
void init_list(LinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/* Insertion a la fin en O(1) grace au pointeur tail */
void insert_at_end_optimized(LinkedList* list, int value) {
    Node* new_node = create_node(value);

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;
}

/* Afficher la liste */
void print_list(LinkedList* list) {
    Node* current = list->head;
    printf("Liste (%zu elements) : ", list->size);
    while (current != NULL) {
        printf("%d --> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

/* Liberer toute la liste */
void free_list(LinkedList* list) {
    Node* current = list->head;
    Node* next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    printf("Liste liberee\n");
}

int main(void) {
    LinkedList list;
    init_list(&list);

    printf("=== Insertion a la fin optimisee (O(1)) ===\n");
    insert_at_end_optimized(&list, 10);
    insert_at_end_optimized(&list, 20);
    insert_at_end_optimized(&list, 30);
    insert_at_end_optimized(&list, 40);
    insert_at_end_optimized(&list, 50);
    print_list(&list);

    printf("\nHead = %d, Tail = %d\n", list.head->data, list.tail->data);

    printf("\n=== Liberation ===\n");
    free_list(&list);

    return 0;
}
