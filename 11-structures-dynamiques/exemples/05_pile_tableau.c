/* ============================================================================
   Section 11.3 : Piles et Files - Pile avec tableau statique
   Description : Implementation d'une pile LIFO avec un tableau de taille fixe
   Fichier source : 03-piles-et-files.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct Stack {
    int data[MAX_SIZE];
    int top;
} Stack;

void init_stack(Stack* stack) {
    stack->top = -1;
}

bool is_empty(Stack* stack) {
    return stack->top == -1;
}

bool is_full(Stack* stack) {
    return stack->top == MAX_SIZE - 1;
}

void push(Stack* stack, int value) {
    if (is_full(stack)) {
        fprintf(stderr, "Erreur : pile pleine (overflow)\n");
        return;
    }
    stack->top++;
    stack->data[stack->top] = value;
    printf("Push : %d\n", value);
}

int pop(Stack* stack) {
    if (is_empty(stack)) {
        fprintf(stderr, "Erreur : pile vide (underflow)\n");
        exit(EXIT_FAILURE);
    }
    int value = stack->data[stack->top];
    stack->top--;
    printf("Pop : %d\n", value);
    return value;
}

int peek(Stack* stack) {
    if (is_empty(stack)) {
        fprintf(stderr, "Erreur : pile vide\n");
        exit(EXIT_FAILURE);
    }
    return stack->data[stack->top];
}

void print_stack(Stack* stack) {
    if (is_empty(stack)) {
        printf("Pile : [vide]\n");
        return;
    }
    printf("Pile (sommet -> base) : ");
    for (int i = stack->top; i >= 0; i--) {
        printf("%d ", stack->data[i]);
    }
    printf("\n");
}

int main(void) {
    Stack stack;
    init_stack(&stack);

    push(&stack, 10);
    push(&stack, 20);
    push(&stack, 30);

    print_stack(&stack);

    printf("Sommet : %d\n", peek(&stack));

    pop(&stack);
    pop(&stack);

    print_stack(&stack);

    push(&stack, 40);
    print_stack(&stack);

    return 0;
}
