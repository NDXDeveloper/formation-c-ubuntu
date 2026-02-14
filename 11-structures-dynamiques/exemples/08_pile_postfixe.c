/* ============================================================================
   Section 11.3 : Piles et Files - Application : notation polonaise inversee
   Description : Evaluation d'expressions postfixes (RPN) avec une pile
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
}

int pop(Stack* stack) {
    if (is_empty(stack)) {
        fprintf(stderr, "Erreur : pile vide (underflow)\n");
        exit(EXIT_FAILURE);
    }
    int value = stack->data[stack->top];
    stack->top--;
    return value;
}

int evaluate_postfix(const char* expression) {
    Stack stack;
    init_stack(&stack);

    for (int i = 0; expression[i] != '\0'; i++) {
        char ch = expression[i];

        if (ch >= '0' && ch <= '9') {
            push(&stack, ch - '0');
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            int b = pop(&stack);
            int a = pop(&stack);

            int result = 0;
            switch (ch) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = a / b; break;
                default: break;
            }

            push(&stack, result);
        }
    }

    return pop(&stack);
}

int main(void) {
    /* "34+" => 3 + 4 = 7 */
    printf("\"34+\" = %d (attendu: 7)\n", evaluate_postfix("34+"));

    /* "53*2+" => 5 * 3 + 2 = 17 */
    printf("\"53*2+\" = %d (attendu: 17)\n", evaluate_postfix("53*2+"));

    /* "82-" => 8 - 2 = 6 */
    printf("\"82-\" = %d (attendu: 6)\n", evaluate_postfix("82-"));

    /* "93/\" => 9 / 3 = 3 */
    printf("\"93/\" = %d (attendu: 3)\n", evaluate_postfix("93/"));

    return 0;
}
