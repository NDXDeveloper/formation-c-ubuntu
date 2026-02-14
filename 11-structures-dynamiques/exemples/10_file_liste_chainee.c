/* ============================================================================
   Section 11.3 : Piles et Files - File avec liste chainee
   Description : Implementation d'une file FIFO dynamique avec liste chainee
   Fichier source : 03-piles-et-files.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct QueueList {
    QueueNode* front;
    QueueNode* rear;
    size_t size;
} QueueList;

void init_queue_list(QueueList* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

bool is_empty_queue_list(QueueList* queue) {
    return queue->front == NULL;
}

void enqueue_list(QueueList* queue, int value) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = value;
    new_node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    queue->size++;
    printf("Enqueue : %d\n", value);
}

int dequeue_list(QueueList* queue) {
    if (queue->front == NULL) {
        fprintf(stderr, "Erreur : file vide\n");
        exit(EXIT_FAILURE);
    }

    QueueNode* temp = queue->front;
    int value = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;
    printf("Dequeue : %d\n", value);
    return value;
}

void print_queue(QueueList* queue) {
    if (is_empty_queue_list(queue)) {
        printf("File : [vide]\n");
        return;
    }
    printf("File (front -> rear) : ");
    QueueNode* current = queue->front;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("(taille: %zu)\n", queue->size);
}

void free_queue_list(QueueList* queue) {
    QueueNode* current = queue->front;
    while (current != NULL) {
        QueueNode* next = current->next;
        free(current);
        current = next;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

int main(void) {
    QueueList queue;
    init_queue_list(&queue);

    printf("=== Enqueue ===\n");
    enqueue_list(&queue, 10);
    enqueue_list(&queue, 20);
    enqueue_list(&queue, 30);
    print_queue(&queue);

    printf("\n=== Dequeue ===\n");
    dequeue_list(&queue);
    print_queue(&queue);

    printf("\n=== Enqueue supplementaire ===\n");
    enqueue_list(&queue, 40);
    enqueue_list(&queue, 50);
    print_queue(&queue);

    printf("\n=== Vider la file un par un ===\n");
    while (!is_empty_queue_list(&queue)) {
        dequeue_list(&queue);
    }
    print_queue(&queue);

    free_queue_list(&queue);
    return 0;
}
