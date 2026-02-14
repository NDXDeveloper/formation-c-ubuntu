/* ============================================================================
   Section 11.3 : Piles et Files - Application : parentheses equilibrees
   Description : Verification de parentheses/crochets/accolades avec une pile
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

bool is_balanced(const char* expression) {
    Stack stack;
    init_stack(&stack);

    for (int i = 0; expression[i] != '\0'; i++) {
        char ch = expression[i];

        if (ch == '(' || ch == '[' || ch == '{') {
            push(&stack, ch);
        }
        else if (ch == ')' || ch == ']' || ch == '}') {
            if (is_empty(&stack)) {
                return false;
            }

            char top = (char)pop(&stack);

            if ((ch == ')' && top != '(') ||
                (ch == ']' && top != '[') ||
                (ch == '}' && top != '{')) {
                return false;
            }
        }
    }

    return is_empty(&stack);
}

int main(void) {
    const char* tests[] = {
        "(())",
        "({[]})",
        "(()",
        "({)}"
    };
    const bool expected[] = { true, true, false, false };

    for (int i = 0; i < 4; i++) {
        bool result = is_balanced(tests[i]);
        printf("\"%s\" -> %s %s\n",
               tests[i],
               result ? "equilibre" : "desequilibre",
               result == expected[i] ? "(OK)" : "(ERREUR)");
    }

    return 0;
}
