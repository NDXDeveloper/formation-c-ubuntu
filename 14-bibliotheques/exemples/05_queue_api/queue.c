/* ============================================================================
   Section 06 : Creation d'une API propre
   Description : Implementation privee de la file d'attente
   Fichier source : 06-creation-api-propre.md
   ============================================================================ */

#include "queue.h"
#include <stdlib.h>

/* Structure interne (invisible pour l'utilisateur) */
typedef struct node {
    int value;
    struct node *next;
} node_t;

struct queue {
    node_t *head;
    node_t *tail;
    size_t size;
};

queue_t* queue_create(void) {
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

bool queue_push(queue_t *q, int value) {
    if (!q) return false;

    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return false;

    new_node->value = value;
    new_node->next = NULL;

    if (q->tail) {
        q->tail->next = new_node;
    } else {
        q->head = new_node;
    }
    q->tail = new_node;
    q->size++;

    return true;
}

bool queue_pop(queue_t *q, int *value) {
    if (!q || !q->head || !value) return false;

    node_t *old_head = q->head;
    *value = old_head->value;

    q->head = old_head->next;
    if (!q->head) {
        q->tail = NULL;
    }

    free(old_head);
    q->size--;

    return true;
}

bool queue_peek(const queue_t *q, int *value) {
    if (!q || !q->head || !value) return false;
    *value = q->head->value;
    return true;
}

size_t queue_size(const queue_t *q) {
    return q ? q->size : 0;
}

bool queue_is_empty(const queue_t *q) {
    return !q || q->head == NULL;
}

void queue_destroy(queue_t *q) {
    if (!q) return;

    while (q->head) {
        node_t *temp = q->head;
        q->head = q->head->next;
        free(temp);
    }

    free(q);
}
