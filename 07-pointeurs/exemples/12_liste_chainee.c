/* ============================================================================
   Section 7.5 : Pointeurs NULL et dangling pointers
   Description : Liste chainée sûre avec gestion NULL
   Fichier source : 05-pointeurs-null.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *creer_node(int valeur) {
    Node *node = malloc(sizeof(Node));
    if (node) {
        node->data = valeur;
        node->next = NULL;
    }
    return node;
}

void ajouter(Node **head, int valeur) {
    if (head == NULL) return;

    Node *nouveau = creer_node(valeur);
    if (nouveau == NULL) return;

    if (*head == NULL) {
        *head = nouveau;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = nouveau;
    }
}

void afficher(Node *head) {
    Node *current = head;
    printf("Liste : ");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

void liberer_liste(Node **head) {
    if (head == NULL) return;

    Node *current = *head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

int main(void) {
    Node *liste = NULL;

    ajouter(&liste, 10);
    ajouter(&liste, 20);
    ajouter(&liste, 30);

    afficher(liste);

    liberer_liste(&liste);

    if (liste == NULL) {
        printf("Liste correctement liberee\n");
    }

    return 0;
}
