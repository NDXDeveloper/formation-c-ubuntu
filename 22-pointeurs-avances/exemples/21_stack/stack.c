/* ============================================================================
   Section 22.4 : Pointeurs opaques
   Description : Implementation privee de la pile (stack) opaque
   Fichier source : 04-pointeurs-opaques.md
   ============================================================================ */

#include "stack.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 10

// Definition complete : privee
struct Stack {
    int *data;
    int size;
    int capacity;
};

Stack* stack_create(void) {
    Stack *s = malloc(sizeof(Stack));
    if (s == NULL) {
        return NULL;
    }

    s->data = malloc(INITIAL_CAPACITY * sizeof(int));
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    s->size = 0;
    s->capacity = INITIAL_CAPACITY;

    return s;
}

void stack_destroy(Stack *s) {
    if (s != NULL) {
        free(s->data);
        free(s);
    }
}

static bool stack_resize(Stack *s) {
    int new_capacity = s->capacity * 2;
    int *new_data = realloc(s->data, (size_t)new_capacity * sizeof(int));
    if (new_data == NULL) {
        return false;
    }

    s->data = new_data;
    s->capacity = new_capacity;
    return true;
}

bool stack_push(Stack *s, int value) {
    if (s == NULL) {
        return false;
    }

    if (s->size >= s->capacity) {
        if (!stack_resize(s)) {
            return false;
        }
    }

    s->data[s->size++] = value;
    return true;
}

bool stack_pop(Stack *s, int *value) {
    if (s == NULL || s->size == 0) {
        return false;
    }

    if (value != NULL) {
        *value = s->data[--s->size];
    } else {
        s->size--;
    }

    return true;
}

bool stack_peek(const Stack *s, int *value) {
    if (s == NULL || s->size == 0 || value == NULL) {
        return false;
    }

    *value = s->data[s->size - 1];
    return true;
}

bool stack_is_empty(const Stack *s) {
    return (s == NULL || s->size == 0);
}

int stack_size(const Stack *s) {
    return (s != NULL) ? s->size : 0;
}
