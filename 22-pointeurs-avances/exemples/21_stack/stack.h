/* ============================================================================
   Section 22.4 : Pointeurs opaques
   Description : Interface publique d'une pile (stack) opaque
   Fichier source : 04-pointeurs-opaques.md
   ============================================================================ */

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

// Declaration forward : type opaque
typedef struct Stack Stack;

// API publique
Stack* stack_create(void);
void stack_destroy(Stack *s);
bool stack_push(Stack *s, int value);
bool stack_pop(Stack *s, int *value);
bool stack_peek(const Stack *s, int *value);
bool stack_is_empty(const Stack *s);
int stack_size(const Stack *s);

#endif
