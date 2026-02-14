/* ============================================================================
   Section 06 : Creation d'une API propre
   Description : Interface publique de la file d'attente (type opaque)
   Fichier source : 06-creation-api-propre.md
   ============================================================================ */

#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>

/* Type opaque - l'utilisateur ne voit pas la structure interne */
typedef struct queue queue_t;

/* Creation et destruction */
queue_t* queue_create(void);
void queue_destroy(queue_t *q);

/* Operations */
bool queue_push(queue_t *q, int value);
bool queue_pop(queue_t *q, int *value);
bool queue_peek(const queue_t *q, int *value);
size_t queue_size(const queue_t *q);
bool queue_is_empty(const queue_t *q);

#endif /* QUEUE_H */
