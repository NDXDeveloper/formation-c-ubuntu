/* ============================================================================
   Section 11.5 : Arbres binaires
   Description : Arbre binaire de recherche (BST) complet avec insertion,
                 recherche, parcours, proprietes, suppression et affichage
   Fichier source : 05-arbres-binaires.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/* ---- Structure de l'arbre ---- */

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

/* ---- File pour parcours en largeur ---- */

typedef struct QueueNode {
    TreeNode* tree_node;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

void enqueue(Queue* q, TreeNode* node) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    new_node->tree_node = node;
    new_node->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

TreeNode* dequeue(Queue* q) {
    if (q->front == NULL) {
        return NULL;
    }
    QueueNode* temp = q->front;
    TreeNode* tree_node = temp->tree_node;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return tree_node;
}

bool queue_is_empty(Queue* q) {
    return q->front == NULL;
}

/* ---- Operations de base du BST ---- */

TreeNode* create_node(int value) {
    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = value;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

TreeNode* insert(TreeNode* root, int value) {
    if (root == NULL) {
        return create_node(value);
    }
    if (value < root->data) {
        root->left = insert(root->left, value);
    } else if (value > root->data) {
        root->right = insert(root->right, value);
    }
    return root;
}

TreeNode* search(TreeNode* root, int value) {
    if (root == NULL || root->data == value) {
        return root;
    }
    if (value < root->data) {
        return search(root->left, value);
    }
    return search(root->right, value);
}

TreeNode* find_min(TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

TreeNode* find_max(TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->right != NULL) {
        root = root->right;
    }
    return root;
}

/* ---- Parcours ---- */

void preorder(TreeNode* root) {
    if (root == NULL) return;
    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}

void inorder(TreeNode* root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

void postorder(TreeNode* root) {
    if (root == NULL) return;
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}

void level_order(TreeNode* root) {
    if (root == NULL) return;

    Queue q = {NULL, NULL};
    enqueue(&q, root);

    while (!queue_is_empty(&q)) {
        TreeNode* current = dequeue(&q);
        printf("%d ", current->data);
        if (current->left != NULL) {
            enqueue(&q, current->left);
        }
        if (current->right != NULL) {
            enqueue(&q, current->right);
        }
    }
}

/* ---- Proprietes ---- */

int height(TreeNode* root) {
    if (root == NULL) {
        return -1;
    }
    int left_height = height(root->left);
    int right_height = height(root->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

int count_nodes(TreeNode* root) {
    if (root == NULL) return 0;
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

int count_leaves(TreeNode* root) {
    if (root == NULL) return 0;
    if (root->left == NULL && root->right == NULL) return 1;
    return count_leaves(root->left) + count_leaves(root->right);
}

bool is_bst_helper(TreeNode* root, int min, int max) {
    if (root == NULL) return true;
    if (root->data <= min || root->data >= max) return false;
    return is_bst_helper(root->left, min, root->data) &&
           is_bst_helper(root->right, root->data, max);
}

bool is_bst(TreeNode* root) {
    return is_bst_helper(root, INT_MIN, INT_MAX);
}

/* ---- Affichage visuel (rotation 90 degres) ---- */

void print_tree(TreeNode* root, int space) {
    if (root == NULL) return;
    space += 5;
    print_tree(root->right, space);
    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }
    printf("%d\n", root->data);
    print_tree(root->left, space);
}

/* ---- Suppression ---- */

TreeNode* delete_node(TreeNode* root, int value) {
    if (root == NULL) return NULL;

    if (value < root->data) {
        root->left = delete_node(root->left, value);
    } else if (value > root->data) {
        root->right = delete_node(root->right, value);
    } else {
        /* Cas 1 : Feuille */
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        /* Cas 2a : Un seul enfant (a droite) */
        else if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        }
        /* Cas 2b : Un seul enfant (a gauche) */
        else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }
        /* Cas 3 : Deux enfants */
        else {
            TreeNode* successor = find_min(root->right);
            root->data = successor->data;
            root->right = delete_node(root->right, successor->data);
        }
    }
    return root;
}

/* ---- Liberation memoire (parcours postfixe) ---- */

void free_tree(TreeNode* root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ---- Programme principal ---- */

int main(void) {
    TreeNode* root = NULL;

    printf("=== Construction de l'arbre ===\n");
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 70);
    root = insert(root, 20);
    root = insert(root, 40);
    root = insert(root, 80);
    root = insert(root, 10);
    printf("Arbre cree avec 7 noeuds\n\n");

    printf("=== Parcours ===\n");
    printf("Prefixe  : "); preorder(root); printf("\n");
    printf("Infixe   : "); inorder(root); printf("\n");
    printf("Postfixe : "); postorder(root); printf("\n");
    printf("Largeur  : "); level_order(root); printf("\n\n");

    printf("=== Proprietes ===\n");
    printf("Hauteur : %d\n", height(root));
    printf("Nombre de noeuds : %d\n", count_nodes(root));
    printf("Nombre de feuilles : %d\n", count_leaves(root));
    printf("Est un BST valide ? %s\n\n", is_bst(root) ? "Oui" : "Non");

    printf("=== Recherche ===\n");
    TreeNode* found = search(root, 40);
    if (found) {
        printf("40 trouve dans l'arbre\n");
    }
    found = search(root, 100);
    if (found == NULL) {
        printf("100 non trouve dans l'arbre\n\n");
    }

    printf("=== Min/Max ===\n");
    TreeNode* min_node = find_min(root);
    TreeNode* max_node = find_max(root);
    printf("Minimum : %d\n", min_node->data);
    printf("Maximum : %d\n\n", max_node->data);

    printf("=== Affichage visuel ===");
    print_tree(root, 0);
    printf("\n");

    printf("=== Suppression ===\n");
    printf("Suppression de 20 (feuille)...\n");
    root = delete_node(root, 20);
    printf("Infixe apres suppression : ");
    inorder(root);
    printf("\n\n");

    printf("Suppression de 30 (deux enfants)...\n");
    root = delete_node(root, 30);
    printf("Infixe apres suppression : ");
    inorder(root);
    printf("\n\n");

    printf("=== Liberation de la memoire ===\n");
    free_tree(root);
    printf("Memoire liberee\n");

    return 0;
}
