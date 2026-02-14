/* ============================================================================
   Section 11.2 : Listes doublement chainees
   Description : Exemple complet avec insertion, parcours bidirectionnel,
                 recherche, suppression et liberation
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

/* Creer un nouveau noeud */
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

/* Initialiser la liste */
void init_list(DoublyLinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/* Inserer au debut */
void insert_at_beginning(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        list->size = 1;
        return;
    }

    new_node->next = list->head;
    list->head->prev = new_node;
    list->head = new_node;
    list->size++;
}

/* Inserer a la fin (O(1) grace au pointeur tail) */
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

/* Parcours avant (HEAD -> TAIL) */
void print_list_forward(DoublyLinkedList* list) {
    Node* current = list->head;
    printf("Liste (avant) : ");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" <-> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}

/* Parcours arriere (TAIL -> HEAD) */
void print_list_backward(DoublyLinkedList* list) {
    Node* current = list->tail;
    printf("Liste (arriere) : ");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->prev != NULL) {
            printf(" <-> ");
        }
        current = current->prev;
    }
    printf(" -> NULL\n");
}

/* Rechercher un element */
Node* search(DoublyLinkedList* list, int value) {
    Node* current = list->head;
    while (current != NULL) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* Supprimer un noeud */
void delete_node(DoublyLinkedList* list, Node* node) {
    if (node == NULL) {
        return;
    }

    if (node->prev == NULL) {
        list->head = node->next;
    } else {
        node->prev->next = node->next;
    }

    if (node->next == NULL) {
        list->tail = node->prev;
    } else {
        node->next->prev = node->prev;
    }

    free(node);
    list->size--;
}

/* Supprimer par valeur */
int delete_by_value(DoublyLinkedList* list, int value) {
    Node* node = search(list, value);
    if (node == NULL) {
        return 0;
    }
    delete_node(list, node);
    return 1;
}

/* Liberer toute la liste */
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
    printf("Liste liberee\n");
}

int main(void) {
    DoublyLinkedList list;
    init_list(&list);

    printf("=== Insertion au debut ===\n");
    insert_at_beginning(&list, 30);
    insert_at_beginning(&list, 20);
    insert_at_beginning(&list, 10);
    print_list_forward(&list);

    printf("\n=== Insertion a la fin ===\n");
    insert_at_end(&list, 40);
    insert_at_end(&list, 50);
    print_list_forward(&list);

    printf("\n=== Parcours arriere ===\n");
    print_list_backward(&list);

    printf("\n=== Recherche ===\n");
    Node* found = search(&list, 30);
    if (found != NULL) {
        printf("Valeur 30 trouvee : %d\n", found->data);
        printf("  Precedent : %d\n", found->prev ? found->prev->data : -1);
        printf("  Suivant : %d\n", found->next ? found->next->data : -1);
    }

    printf("\n=== Suppression de 20 ===\n");
    delete_by_value(&list, 20);
    print_list_forward(&list);

    printf("\n=== Suppression du premier (10) ===\n");
    delete_by_value(&list, 10);
    print_list_forward(&list);

    printf("\n=== Suppression du dernier (50) ===\n");
    delete_by_value(&list, 50);
    print_list_forward(&list);

    printf("\n=== Taille de la liste : %zu ===\n", list.size);

    printf("\n=== Liberation ===\n");
    free_list(&list);
    print_list_forward(&list);

    return 0;
}
