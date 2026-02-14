/* ============================================================================
   Section 11.3 : Piles et Files - Pile avec liste chainee
   Description : Implementation d'une pile LIFO dynamique avec liste chainee
   Fichier source : 03-piles-et-files.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct StackList {
    Node* top;
    size_t size;
} StackList;

void init_stack_list(StackList* stack) {
    stack->top = NULL;
    stack->size = 0;
}

bool is_empty_list(StackList* stack) {
    return stack->top == NULL;
}

void push_list(StackList* stack, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = value;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
    printf("Push : %d\n", value);
}

int pop_list(StackList* stack) {
    if (is_empty_list(stack)) {
        fprintf(stderr, "Erreur : pile vide\n");
        exit(EXIT_FAILURE);
    }
    Node* temp = stack->top;
    int value = temp->data;
    stack->top = stack->top->next;
    free(temp);
    stack->size--;
    printf("Pop : %d\n", value);
    return value;
}

void print_stack(StackList* stack) {
    if (is_empty_list(stack)) {
        printf("Pile : [vide]\n");
        return;
    }
    printf("Pile (sommet -> base) : ");
    Node* current = stack->top;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

void free_stack_list(StackList* stack) {
    Node* current = stack->top;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    stack->top = NULL;
    stack->size = 0;
}

int main(void) {
    StackList stack;
    init_stack_list(&stack);

    push_list(&stack, 10);
    push_list(&stack, 20);
    push_list(&stack, 30);

    print_stack(&stack);
    printf("Taille : %zu\n", stack.size);

    pop_list(&stack);
    pop_list(&stack);

    print_stack(&stack);

    push_list(&stack, 40);
    push_list(&stack, 50);
    print_stack(&stack);

    printf("\nLiberation de la pile\n");
    free_stack_list(&stack);
    print_stack(&stack);

    return 0;
}
