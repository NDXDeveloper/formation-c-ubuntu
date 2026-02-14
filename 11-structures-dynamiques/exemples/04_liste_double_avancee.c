/* ============================================================================
   Section 11.2 : Listes doublement chainees - Operations avancees
   Description : Inversion, concatenation et acces par index optimise
   Fichier source : 02-listes-doublement-chainees.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct DoublyLinkedList {
    Node* head;
    Node* tail;
    size_t size;
} DoublyLinkedList;

Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : echec de l'allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = value;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

void init_list(DoublyLinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void insert_at_end(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);
    if (list->tail == NULL) {
        list->head = new_node;
        list->tail = new_node;
        list->size = 1;
        return;
    }
    new_node->prev = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
    list->size++;
}

void print_list_forward(DoublyLinkedList* list) {
    Node* current = list->head;
    printf("Liste (%zu elements) : ", list->size);
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" <-> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}

/* Inverser la liste en echangeant next et prev */
void reverse_list(DoublyLinkedList* list) {
    Node* current = list->head;
    Node* temp = NULL;

    /* Echanger head et tail */
    temp = list->head;
    list->head = list->tail;
    list->tail = temp;

    /* Echanger next et prev pour chaque noeud */
    while (current != NULL) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev;  /* Avancer (ancien next) */
    }
}

/* Concatener list2 a la fin de list1 */
void concatenate(DoublyLinkedList* list1, DoublyLinkedList* list2) {
    if (list1->tail == NULL) {
        list1->head = list2->head;
        list1->tail = list2->tail;
    } else if (list2->head != NULL) {
        list1->tail->next = list2->head;
        list2->head->prev = list1->tail;
        list1->tail = list2->tail;
    }

    list1->size += list2->size;

    /* Reinitialiser list2 (sans liberer les noeuds) */
    list2->head = NULL;
    list2->tail = NULL;
    list2->size = 0;
}

/* Obtenir le n-ieme element (optimise : part du cote le plus proche) */
Node* get_nth_node(DoublyLinkedList* list, size_t index) {
    if (index >= list->size) {
        return NULL;
    }

    Node* current;

    if (index < list->size / 2) {
        /* Plus proche du debut */
        current = list->head;
        for (size_t i = 0; i < index; i++) {
            current = current->next;
        }
    } else {
        /* Plus proche de la fin */
        current = list->tail;
        for (size_t i = list->size - 1; i > index; i--) {
            current = current->prev;
        }
    }

    return current;
}

void free_list(DoublyLinkedList* list) {
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
}

int main(void) {
    DoublyLinkedList list1;
    DoublyLinkedList list2;
    init_list(&list1);
    init_list(&list2);

    /* Construire list1 : 10 20 30 */
    for (int i = 1; i <= 3; i++) {
        insert_at_end(&list1, i * 10);
    }
    printf("=== Liste 1 ===\n");
    print_list_forward(&list1);

    /* Construire list2 : 40 50 60 */
    for (int i = 4; i <= 6; i++) {
        insert_at_end(&list2, i * 10);
    }
    printf("\n=== Liste 2 ===\n");
    print_list_forward(&list2);

    /* Concatenation */
    printf("\n=== Concatenation (list1 + list2) ===\n");
    concatenate(&list1, &list2);
    print_list_forward(&list1);
    printf("Liste 2 apres concatenation : ");
    print_list_forward(&list2);

    /* Acces par index */
    printf("\n=== Acces par index ===\n");
    for (size_t i = 0; i < list1.size; i++) {
        Node* n = get_nth_node(&list1, i);
        if (n != NULL) {
            printf("  Element[%zu] = %d\n", i, n->data);
        }
    }

    /* Inversion */
    printf("\n=== Inversion ===\n");
    printf("Avant : ");
    print_list_forward(&list1);
    reverse_list(&list1);
    printf("Apres : ");
    print_list_forward(&list1);

    /* Verification : head et tail corrects apres inversion */
    printf("\nHead = %d, Tail = %d\n",
           list1.head->data, list1.tail->data);

    free_list(&list1);
    return 0;
}
