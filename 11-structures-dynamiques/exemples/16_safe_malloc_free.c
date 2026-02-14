/* ============================================================================
   Section 11.7 : Gestion memoire des structures dynamiques
   Description : Wrappers safe_malloc/safe_free pour securiser les allocations
                 et eviter les fuites memoire
   Fichier source : 07-gestion-memoire.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

/* Wrapper securise pour malloc : arrete le programme si echec */
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Erreur : allocation de %zu bytes echouee\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/* Wrapper securise pour free : met automatiquement le pointeur a NULL */
void safe_free(void** ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

/* Demonstration avec une structure simple */
typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* create_node(int value) {
    Node* node = safe_malloc(sizeof(Node));
    node->data = value;
    node->next = NULL;
    return node;
}

int main(void) {
    printf("=== Demonstration safe_malloc / safe_free ===\n\n");

    /* Allocation securisee */
    int* array = safe_malloc(sizeof(int) * 5);
    for (int i = 0; i < 5; i++) {
        array[i] = (i + 1) * 10;
    }

    printf("Tableau : ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    /* Liberation securisee */
    printf("\nAvant safe_free : array = %p\n", (void*)array);
    safe_free((void**)&array);
    printf("Apres safe_free : array = %p (NULL)\n", (void*)array);

    /* Double free est maintenant sans danger */
    safe_free((void**)&array);  /* Ne fait rien car array == NULL */
    printf("Double safe_free : pas de crash !\n");

    /* Avec une liste chainee */
    printf("\n=== Liste chainee avec safe_malloc ===\n");
    Node* head = create_node(10);
    head->next = create_node(20);
    head->next->next = create_node(30);

    Node* current = head;
    printf("Liste : ");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");

    /* Liberation de la liste */
    current = head;
    while (current != NULL) {
        Node* next = current->next;
        safe_free((void**)&current);
        current = next;
    }
    printf("Liste liberee.\n");

    return 0;
}
