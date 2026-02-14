/* ============================================================================
   Section 11.3 : Piles et Files - File avec tableau circulaire
   Description : Implementation d'une file FIFO avec tableau circulaire (modulo)
   Fichier source : 03-piles-et-files.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUEUE_SIZE 100

typedef struct Queue {
    int data[QUEUE_SIZE];
    int front;
    int rear;
    int size;
} Queue;

void init_queue(Queue* queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

bool is_empty_queue(Queue* queue) {
    return queue->size == 0;
}

bool is_full_queue(Queue* queue) {
    return queue->size == QUEUE_SIZE;
}

void enqueue(Queue* queue, int value) {
    if (is_full_queue(queue)) {
        fprintf(stderr, "Erreur : file pleine\n");
        return;
    }
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->data[queue->rear] = value;
    queue->size++;
    printf("Enqueue : %d\n", value);
}

int dequeue(Queue* queue) {
    if (is_empty_queue(queue)) {
        fprintf(stderr, "Erreur : file vide\n");
        exit(EXIT_FAILURE);
    }
    int value = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->size--;
    printf("Dequeue : %d\n", value);
    return value;
}

int front(Queue* queue) {
    if (is_empty_queue(queue)) {
        fprintf(stderr, "Erreur : file vide\n");
        exit(EXIT_FAILURE);
    }
    return queue->data[queue->front];
}

void print_queue(Queue* queue) {
    if (is_empty_queue(queue)) {
        printf("File : [vide]\n");
        return;
    }
    printf("File (front -> rear) : ");
    int index = queue->front;
    for (int i = 0; i < queue->size; i++) {
        printf("%d ", queue->data[index]);
        index = (index + 1) % QUEUE_SIZE;
    }
    printf("\n");
}

int main(void) {
    Queue queue;
    init_queue(&queue);

    enqueue(&queue, 10);
    enqueue(&queue, 20);
    enqueue(&queue, 30);

    print_queue(&queue);

    printf("Front : %d\n", front(&queue));

    dequeue(&queue);
    print_queue(&queue);

    enqueue(&queue, 40);
    print_queue(&queue);

    return 0;
}
