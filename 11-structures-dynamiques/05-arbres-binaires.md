🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.5 Arbres binaires

## Introduction

Un **arbre binaire** est une structure de données hiérarchique où chaque élément (appelé **nœud**) peut avoir au maximum **deux enfants** : un enfant gauche et un enfant droit. C'est une généralisation des listes chaînées qui permet une organisation hiérarchique des données.

### Analogie : Un arbre généalogique

Imaginez un arbre généalogique simplifié :
- Chaque personne (nœud) peut avoir **au maximum 2 enfants**
- On part d'un **ancêtre** (la racine)
- Les personnes sans enfants sont des **feuilles**

```
         Grand-père (racine)
            /         \
        Père          Oncle
       /    \           \
    Vous   Sœur      Cousin (feuilles)
```

### Différence avec les structures linéaires

| Structure | Organisation | Accès |
|-----------|-------------|-------|
| **Tableau** | Linéaire | Index direct |
| **Liste chaînée** | Linéaire | Séquentiel |
| **Arbre binaire** | **Hiérarchique** | Récursif |

---

## Terminologie de base

### Structure d'un nœud

```c
typedef struct TreeNode {
    int data;                    // Donnée stockée
    struct TreeNode* left;       // Enfant gauche
    struct TreeNode* right;      // Enfant droit
} TreeNode;
```

### Représentation visuelle

```
           [50]           ← Racine (root)
          /    \
       [30]    [70]       ← Nœuds internes
       /  \      \
    [20] [40]   [80]      ← [20], [40], [80] sont des feuilles
```

### Vocabulaire essentiel

**Racine (root)** : Le nœud au sommet de l'arbre (ici `50`)

**Nœud parent** : Un nœud qui a des enfants
- `50` est le parent de `30` et `70`

**Nœud enfant** : Un nœud pointé par un parent
- `30` et `70` sont les enfants de `50`

**Feuille (leaf)** : Un nœud sans enfants
- `20`, `40`, `80` sont des feuilles

**Nœud interne** : Un nœud qui a au moins un enfant
- `50`, `30`, `70` sont des nœuds internes

**Sous-arbre** : Un arbre contenu dans un arbre plus grand
- Sous-arbre gauche de `50` : `[30, 20, 40]`
- Sous-arbre droit de `50` : `[70, 80]`

**Hauteur** : Distance maximale de la racine à une feuille
- Hauteur de l'arbre ci-dessus : 2

**Profondeur** : Distance d'un nœud à la racine
- Profondeur de `20` : 2

**Niveau** : Ensemble des nœuds à la même profondeur
- Niveau 0 : `[50]`
- Niveau 1 : `[30, 70]`
- Niveau 2 : `[20, 40, 80]`

---

## Arbre Binaire de Recherche (BST)

### Définition

Un **Arbre Binaire de Recherche** (Binary Search Tree) est un arbre binaire avec une propriété spéciale :

**Propriété BST** : Pour chaque nœud :
- Tous les nœuds du **sous-arbre gauche** ont des valeurs **plus petites**
- Tous les nœuds du **sous-arbre droit** ont des valeurs **plus grandes**

```
           [50]
          /    \
       [30]    [70]       ← 30 < 50 < 70 ✓
       /  \      \
    [20] [40]   [80]      ← 20 < 30 < 40 ✓  et  70 < 80 ✓
```

**Vérification** :
- Sous-arbre gauche de 50 : `{20, 30, 40}` < 50 ✓
- Sous-arbre droit de 50 : `{70, 80}` > 50 ✓

### Contre-exemple (ce n'est PAS un BST)

```
           [50]
          /    \
       [30]    [70]
       /  \      \
    [60] [40]   [80]      ← 60 > 50 mais dans le sous-arbre gauche ✗
```

---

## Opérations de base

### 1. Créer un nœud

```c
#include <stdio.h>
#include <stdlib.h>

TreeNode* create_node(int value) {
    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));

    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = value;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}
```

### 2. Insertion dans un BST

La clé est de respecter la propriété BST : plus petit à gauche, plus grand à droite.

```c
TreeNode* insert(TreeNode* root, int value) {
    // Cas de base : arbre vide ou position trouvée
    if (root == NULL) {
        return create_node(value);
    }

    // Décider d'aller à gauche ou à droite
    if (value < root->data) {
        // Insérer dans le sous-arbre gauche
        root->left = insert(root->left, value);
    }
    else if (value > root->data) {
        // Insérer dans le sous-arbre droit
        root->right = insert(root->right, value);
    }
    // Si value == root->data, on ne fait rien (pas de doublons)

    return root;
}
```

**Explication étape par étape** :

Insérer `40` dans l'arbre :
```
       [50]
      /    \
   [30]    [70]
```

1. `40 < 50` → Aller à gauche
2. `40 > 30` → Aller à droite de 30
3. Position trouvée (NULL) → Créer le nœud

Résultat :
```
       [50]
      /    \
   [30]    [70]
      \
     [40]
```

**Utilisation** :
```c
TreeNode* root = NULL;  // Arbre vide

root = insert(root, 50);
root = insert(root, 30);
root = insert(root, 70);
root = insert(root, 20);
root = insert(root, 40);
root = insert(root, 80);
```

### 3. Recherche dans un BST

Grâce à la propriété BST, la recherche est très efficace.

```c
TreeNode* search(TreeNode* root, int value) {
    // Cas de base : arbre vide ou valeur trouvée
    if (root == NULL || root->data == value) {
        return root;
    }

    // Décider d'aller à gauche ou à droite
    if (value < root->data) {
        return search(root->left, value);   // Chercher à gauche
    } else {
        return search(root->right, value);  // Chercher à droite
    }
}
```

**Version itérative** (sans récursion) :
```c
TreeNode* search_iterative(TreeNode* root, int value) {
    TreeNode* current = root;

    while (current != NULL && current->data != value) {
        if (value < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return current;  // NULL si non trouvé
}
```

**Utilisation** :
```c
TreeNode* found = search(root, 40);
if (found != NULL) {
    printf("Valeur %d trouvée\n", found->data);
} else {
    printf("Valeur non trouvée\n");
}
```

**Complexité** : O(h) où h est la hauteur de l'arbre
- Arbre équilibré : O(log n)
- Arbre dégénéré (liste) : O(n)

### 4. Trouver le minimum et le maximum

**Minimum** : Le nœud le plus à gauche
```c
TreeNode* find_min(TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }

    // Descendre à gauche jusqu'à la fin
    while (root->left != NULL) {
        root = root->left;
    }

    return root;
}
```

**Maximum** : Le nœud le plus à droite
```c
TreeNode* find_max(TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }

    // Descendre à droite jusqu'à la fin
    while (root->right != NULL) {
        root = root->right;
    }

    return root;
}
```

**Exemple** :
```
       [50]
      /    \
   [30]    [70]
   /          \
[20]         [80]

Minimum : 20 (le plus à gauche)
Maximum : 80 (le plus à droite)
```

---

## Parcours d'arbres (Tree Traversal)

Il existe plusieurs façons de visiter tous les nœuds d'un arbre. Les trois principales sont basées sur l'ordre de visite du nœud par rapport à ses enfants.

### Arbre d'exemple

```
       [50]
      /    \
   [30]    [70]
   /  \      \
[20] [40]   [80]
```

### 1. Parcours Préfixe (Preorder) : Racine-Gauche-Droit

**Ordre** : Visite le nœud **avant** ses enfants

```c
void preorder(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    printf("%d ", root->data);        // 1. Visiter la racine
    preorder(root->left);             // 2. Parcourir le sous-arbre gauche
    preorder(root->right);            // 3. Parcourir le sous-arbre droit
}
```

**Résultat** : `50 30 20 40 70 80`

**Mnémonique** : On "traite" le nœud **pré**-maturément (avant les enfants)

**Cas d'usage** : Copier un arbre, préfixe d'expressions arithmétiques

### 2. Parcours Infixe (Inorder) : Gauche-Racine-Droit

**Ordre** : Visite le nœud **entre** ses enfants

```c
void inorder(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    inorder(root->left);              // 1. Parcourir le sous-arbre gauche
    printf("%d ", root->data);        // 2. Visiter la racine
    inorder(root->right);             // 3. Parcourir le sous-arbre droit
}
```

**Résultat** : `20 30 40 50 70 80`

**Propriété magique pour BST** : Le parcours infixe donne les éléments **dans l'ordre croissant** ! ✨

**Cas d'usage** : Obtenir les éléments triés d'un BST

### 3. Parcours Postfixe (Postorder) : Gauche-Droit-Racine

**Ordre** : Visite le nœud **après** ses enfants

```c
void postorder(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    postorder(root->left);            // 1. Parcourir le sous-arbre gauche
    postorder(root->right);           // 2. Parcourir le sous-arbre droit
    printf("%d ", root->data);        // 3. Visiter la racine
}
```

**Résultat** : `20 40 30 80 70 50`

**Mnémonique** : On "traite" le nœud **post**-mortem (après les enfants)

**Cas d'usage** : Libérer la mémoire d'un arbre, évaluation d'expressions postfixes

### 4. Parcours en Largeur (Level Order / BFS)

**Ordre** : Visite les nœuds niveau par niveau (de haut en bas, de gauche à droite)

```c
#include <stdbool.h>

// Utilisation d'une file (voir section 11.3)
typedef struct QueueNode {
    TreeNode* tree_node;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

// Fonctions de base de la file (simplifiées)
void enqueue(Queue* q, TreeNode* node) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
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

bool is_empty(Queue* q) {
    return q->front == NULL;
}

// Parcours en largeur
void level_order(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    Queue q = {NULL, NULL};
    enqueue(&q, root);

    while (!is_empty(&q)) {
        TreeNode* current = dequeue(&q);
        printf("%d ", current->data);

        // Ajouter les enfants à la file
        if (current->left != NULL) {
            enqueue(&q, current->left);
        }
        if (current->right != NULL) {
            enqueue(&q, current->right);
        }
    }
}
```

**Résultat** : `50 30 70 20 40 80`

**Explication** :
1. File : `[50]` → Visite 50, ajoute 30 et 70
2. File : `[30, 70]` → Visite 30, ajoute 20 et 40
3. File : `[70, 20, 40]` → Visite 70, ajoute 80
4. File : `[20, 40, 80]` → Visite 20, 40, 80

**Cas d'usage** : Plus court chemin, affichage niveau par niveau

---

## Suppression dans un BST

La suppression est l'opération la plus complexe. Il faut gérer **3 cas** :

### Cas 1 : Nœud feuille (sans enfants)

Simplement supprimer le nœud.

```
       [50]                    [50]
      /    \                  /    \
   [30]    [70]    →       [30]    [70]
   /  \                    /
[20] [40]               [20]

Supprimer 40 (feuille)
```

### Cas 2 : Nœud avec un seul enfant

Remplacer le nœud par son enfant.

```
       [50]                    [50]
      /    \                  /    \
   [30]    [70]    →       [20]    [70]
   /          \                       \
[20]         [80]                    [80]

Supprimer 30 (un seul enfant : 20)
```

### Cas 3 : Nœud avec deux enfants

**Stratégie** : Remplacer par le **successeur** (le plus petit du sous-arbre droit) ou le **prédécesseur** (le plus grand du sous-arbre gauche).

```
       [50]                    [70]
      /    \                  /    \
   [30]    [70]    →       [30]    [80]
   /  \      \             /  \
[20] [40]   [80]        [20] [40]

Supprimer 50 (deux enfants)
Remplacer par 70 (successeur = min du sous-arbre droit)
```

### Implémentation complète

```c
TreeNode* delete_node(TreeNode* root, int value) {
    if (root == NULL) {
        return NULL;
    }

    // Chercher le nœud à supprimer
    if (value < root->data) {
        root->left = delete_node(root->left, value);
    }
    else if (value > root->data) {
        root->right = delete_node(root->right, value);
    }
    else {
        // Nœud trouvé ! Gérer les 3 cas

        // Cas 1 : Feuille (pas d'enfants)
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }

        // Cas 2a : Un seul enfant (à droite)
        else if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        }

        // Cas 2b : Un seul enfant (à gauche)
        else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        // Cas 3 : Deux enfants
        else {
            // Trouver le successeur (min du sous-arbre droit)
            TreeNode* successor = find_min(root->right);

            // Copier la valeur du successeur
            root->data = successor->data;

            // Supprimer le successeur
            root->right = delete_node(root->right, successor->data);
        }
    }

    return root;
}
```

---

## Calcul de propriétés

### Hauteur de l'arbre

```c
int height(TreeNode* root) {
    if (root == NULL) {
        return -1;  // Convention : hauteur d'un arbre vide = -1
    }

    int left_height = height(root->left);
    int right_height = height(root->right);

    return 1 + (left_height > right_height ? left_height : right_height);
}
```

**Exemple** :
```
       [50]           Hauteur = 2
      /    \
   [30]    [70]       Hauteur = 1
   /  \      \
[20] [40]   [80]      Hauteur = 0 (feuilles)
```

### Nombre de nœuds

```c
int count_nodes(TreeNode* root) {
    if (root == NULL) {
        return 0;
    }

    return 1 + count_nodes(root->left) + count_nodes(root->right);
}
```

### Nombre de feuilles

```c
int count_leaves(TreeNode* root) {
    if (root == NULL) {
        return 0;
    }

    // C'est une feuille si pas d'enfants
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }

    return count_leaves(root->left) + count_leaves(root->right);
}
```

### Vérifier si c'est un BST valide

```c
bool is_bst_helper(TreeNode* root, int min, int max) {
    if (root == NULL) {
        return true;
    }

    // Vérifier la contrainte
    if (root->data <= min || root->data >= max) {
        return false;
    }

    // Vérifier récursivement les sous-arbres
    return is_bst_helper(root->left, min, root->data) &&
           is_bst_helper(root->right, root->data, max);
}

bool is_bst(TreeNode* root) {
    return is_bst_helper(root, INT_MIN, INT_MAX);
}
```

---

## Affichage visuel d'un arbre

### Affichage simple (rotation de 90°)

```c
void print_tree(TreeNode* root, int space) {
    if (root == NULL) {
        return;
    }

    // Augmenter l'espacement
    space += 5;

    // Afficher le sous-arbre droit en premier
    print_tree(root->right, space);

    // Afficher le nœud actuel
    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }
    printf("%d\n", root->data);

    // Afficher le sous-arbre gauche
    print_tree(root->left, space);
}

// Utilisation
print_tree(root, 0);
```

**Résultat** :
```
          80
     70
50
          40
     30
          20
```

### Affichage par niveau avec indentation

```c
void print_level_order_pretty(TreeNode* root) {
    if (root == NULL) {
        printf("Arbre vide\n");
        return;
    }

    Queue q = {NULL, NULL};
    enqueue(&q, root);
    enqueue(&q, NULL);  // Marqueur de fin de niveau

    int level = 0;
    printf("Niveau %d: ", level);

    while (!is_empty(&q)) {
        TreeNode* current = dequeue(&q);

        if (current == NULL) {
            printf("\n");
            if (!is_empty(&q)) {
                level++;
                printf("Niveau %d: ", level);
                enqueue(&q, NULL);
            }
        } else {
            printf("%d ", current->data);

            if (current->left != NULL) {
                enqueue(&q, current->left);
            }
            if (current->right != NULL) {
                enqueue(&q, current->right);
            }
        }
    }
}
```

---

## Libération de la mémoire

**Crucial** : Utiliser un parcours **postfixe** pour libérer tous les nœuds.

```c
void free_tree(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    // Libérer d'abord les sous-arbres (postfixe)
    free_tree(root->left);
    free_tree(root->right);

    // Puis le nœud lui-même
    free(root);
}
```

**Pourquoi postfixe ?**
- Si on libère le nœud avant ses enfants, on perd les pointeurs `left` et `right` !

---

## Exemple complet

```c
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// ... (toutes les fonctions définies ci-dessus)

int main(void) {
    TreeNode* root = NULL;

    // Construction de l'arbre
    printf("=== Construction de l'arbre ===\n");
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 70);
    root = insert(root, 20);
    root = insert(root, 40);
    root = insert(root, 80);
    root = insert(root, 10);

    printf("Arbre créé avec 7 nœuds\n\n");

    // Parcours
    printf("=== Parcours ===\n");
    printf("Préfixe  : "); preorder(root); printf("\n");
    printf("Infixe   : "); inorder(root); printf("\n");
    printf("Postfixe : "); postorder(root); printf("\n");
    printf("Largeur  : "); level_order(root); printf("\n\n");

    // Propriétés
    printf("=== Propriétés ===\n");
    printf("Hauteur : %d\n", height(root));
    printf("Nombre de nœuds : %d\n", count_nodes(root));
    printf("Nombre de feuilles : %d\n", count_leaves(root));
    printf("Est un BST valide ? %s\n\n", is_bst(root) ? "Oui" : "Non");

    // Recherche
    printf("=== Recherche ===\n");
    TreeNode* found = search(root, 40);
    if (found) {
        printf("40 trouvé dans l'arbre\n");
    }

    found = search(root, 100);
    if (found == NULL) {
        printf("100 non trouvé dans l'arbre\n\n");
    }

    // Min et Max
    printf("=== Min/Max ===\n");
    TreeNode* min = find_min(root);
    TreeNode* max = find_max(root);
    printf("Minimum : %d\n", min->data);
    printf("Maximum : %d\n\n", max->data);

    // Affichage visuel
    printf("=== Affichage visuel ===\n");
    print_tree(root, 0);
    printf("\n");

    // Suppression
    printf("=== Suppression ===\n");
    printf("Suppression de 20 (feuille)...\n");
    root = delete_node(root, 20);
    printf("Infixe après suppression : ");
    inorder(root);
    printf("\n\n");

    printf("Suppression de 30 (deux enfants)...\n");
    root = delete_node(root, 30);
    printf("Infixe après suppression : ");
    inorder(root);
    printf("\n\n");

    // Libération
    printf("=== Libération de la mémoire ===\n");
    free_tree(root);
    printf("Mémoire libérée\n");

    return 0;
}
```

**Sortie attendue** :
```
=== Construction de l'arbre ===
Arbre créé avec 7 nœuds

=== Parcours ===
Préfixe  : 50 30 20 10 40 70 80
Infixe   : 10 20 30 40 50 70 80
Postfixe : 10 20 40 30 80 70 50
Largeur  : 50 30 70 20 40 80 10

=== Propriétés ===
Hauteur : 3
Nombre de nœuds : 7
Nombre de feuilles : 3
Est un BST valide ? Oui

...
```

---

## Complexité des opérations

| Opération | Cas moyen (arbre équilibré) | Pire cas (arbre dégénéré) |
|-----------|----------------------------|---------------------------|
| Recherche | **O(log n)** | O(n) |
| Insertion | **O(log n)** | O(n) |
| Suppression | **O(log n)** | O(n) |
| Min/Max | **O(log n)** | O(n) |
| Parcours | O(n) | O(n) |

### Arbre équilibré vs dégénéré

**Arbre équilibré** (hauteur = log n) :
```
       [50]
      /    \
   [30]    [70]     Hauteur = 2
   /  \    /  \
[20][40][60][80]
```

**Arbre dégénéré** (hauteur = n) :
```
[10]
   \
   [20]
      \
      [30]          Hauteur = 4 (comme une liste !)
         \
         [40]
            \
            [50]
```

**Solution** : Utiliser des arbres auto-équilibrés (AVL, Red-Black Trees) pour garantir O(log n).

---

## Comparaison avec d'autres structures

| Structure | Recherche | Insertion | Suppression | Ordre | Espace |
|-----------|-----------|-----------|-------------|-------|--------|
| **Tableau trié** | O(log n) binaire | O(n) | O(n) | ✅ | O(n) |
| **Liste chaînée** | O(n) | O(1) début | O(1) si nœud connu | ❌ | O(n) |
| **Table de hachage** | **O(1)** | **O(1)** | **O(1)** | ❌ | O(n) |
| **BST (équilibré)** | O(log n) | O(log n) | O(log n) | ✅ | O(n) |

**Quand utiliser un BST ?**
- ✅ Besoin d'ordre trié (min, max, successeur)
- ✅ Recherche dichotomique efficace
- ✅ Parcours dans l'ordre
- ❌ Si accès constant nécessaire (→ table de hachage)
- ❌ Si insertions/suppressions fréquentes au milieu (→ liste)

---

## Applications pratiques

### 1. Dictionnaire ordonné

```c
// Stocker des mots dans l'ordre alphabétique
TreeNode* dictionary = NULL;
dictionary = insert(dictionary, hash("apple"));
dictionary = insert(dictionary, hash("banana"));
dictionary = insert(dictionary, hash("cherry"));

// Afficher dans l'ordre alphabétique
inorder(dictionary);  // apple, banana, cherry
```

### 2. Intervalles de valeurs

```c
// Trouver toutes les valeurs dans [min, max]
void range_query(TreeNode* root, int min, int max) {
    if (root == NULL) {
        return;
    }

    if (root->data > min) {
        range_query(root->left, min, max);
    }

    if (root->data >= min && root->data <= max) {
        printf("%d ", root->data);
    }

    if (root->data < max) {
        range_query(root->right, min, max);
    }
}

// Utilisation
range_query(root, 25, 55);  // Valeurs entre 25 et 55
```

### 3. Arbre de décision

```c
typedef struct DecisionNode {
    char* question;
    struct DecisionNode* yes;  // Branche "oui"
    struct DecisionNode* no;   // Branche "non"
} DecisionNode;

// Exemple : diagnostic médical, jeu des 20 questions
```

### 4. Arbre d'expression arithmétique

```c
// Représenter (3 + 4) * 5
//       [*]
//      /   \
//   [+]     [5]
//   / \
// [3] [4]

int evaluate_expression(TreeNode* root) {
    if (root->left == NULL && root->right == NULL) {
        return root->data;  // Feuille = nombre
    }

    int left_val = evaluate_expression(root->left);
    int right_val = evaluate_expression(root->right);

    // root->data contient l'opérateur
    switch(root->data) {
        case '+': return left_val + right_val;
        case '-': return left_val - right_val;
        case '*': return left_val * right_val;
        case '/': return left_val / right_val;
    }

    return 0;
}
```

---

## Erreurs courantes et pièges

### 1. Oublier le cas de base dans la récursion

❌ **Mauvais** :
```c
void inorder(TreeNode* root) {
    inorder(root->left);      // ⚠️ Segfault si root == NULL !
    printf("%d ", root->data);
    inorder(root->right);
}
```

✅ **Bon** :
```c
void inorder(TreeNode* root) {
    if (root == NULL) {       // ✅ Toujours vérifier NULL
        return;
    }
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}
```

### 2. Ne pas retourner la nouvelle racine après insertion

❌ **Mauvais** :
```c
void insert_BAD(TreeNode* root, int value) {
    if (root == NULL) {
        root = create_node(value);  // ⚠️ Modification locale !
        return;
    }
    // ...
}

// Utilisation
insert_BAD(root, 10);  // root reste NULL !
```

✅ **Bon** :
```c
TreeNode* insert_GOOD(TreeNode* root, int value) {
    if (root == NULL) {
        return create_node(value);  // ✅ Retourner le nouveau nœud
    }
    // ...
    return root;
}

// Utilisation
root = insert_GOOD(root, 10);  // ✅ Mettre à jour root
```

### 3. Ordre de libération incorrect

❌ **Mauvais** :
```c
void free_tree_BAD(TreeNode* root) {
    free(root);               // ⚠️ On perd root->left et root->right !
    free_tree_BAD(root->left);
    free_tree_BAD(root->right);
}
```

✅ **Bon** :
```c
void free_tree_GOOD(TreeNode* root) {
    if (root == NULL) return;
    free_tree_GOOD(root->left);   // ✅ Libérer les enfants d'abord
    free_tree_GOOD(root->right);
    free(root);
}
```

### 4. Recherche dans un arbre non-BST

❌ **Mauvais** :
```c
// Utiliser la recherche BST sur un arbre quelconque
TreeNode* result = search(non_bst_tree, 40);  // ⚠️ Résultat incorrect !
```

✅ **Bon** :
```c
// Pour un arbre binaire général, parcourir tout l'arbre
TreeNode* search_general(TreeNode* root, int value) {
    if (root == NULL) return NULL;
    if (root->data == value) return root;

    TreeNode* left_result = search_general(root->left, value);
    if (left_result != NULL) return left_result;

    return search_general(root->right, value);
}
```

### 5. Débordement de pile avec récursion profonde

```c
// Arbre très déséquilibré → récursion profonde
// Solution : Utiliser des versions itératives avec une pile explicite
```

---

## Variantes d'arbres binaires

### 1. Arbre binaire complet

Tous les niveaux sont remplis, sauf peut-être le dernier (rempli de gauche à droite).

```
       [1]
      /   \
    [2]   [3]
    / \   /
  [4][5][6]        ✓ Complet
```

### 2. Arbre binaire parfait

Tous les niveaux sont complètement remplis.

```
       [1]
      /   \
    [2]   [3]
    / \   / \
  [4][5][6][7]     ✓ Parfait
```

### 3. Arbres équilibrés

**AVL Tree** : Différence de hauteur entre sous-arbres ≤ 1

**Red-Black Tree** : Propriétés de coloration garantissant l'équilibrage

Ces structures garantissent O(log n) dans tous les cas.

### 4. B-Tree

Arbre avec plus de 2 enfants par nœud, utilisé pour les bases de données et systèmes de fichiers.

---

## Bonnes pratiques

1. **Toujours vérifier NULL** avant de déréférencer
2. **Retourner la nouvelle racine** dans les fonctions d'insertion/suppression
3. **Utiliser un parcours postfixe** pour libérer la mémoire
4. **Tester les cas limites** : arbre vide, un seul nœud, arbre dégénéré
5. **Visualiser l'arbre** avec des fonctions d'affichage
6. **Valgrind** pour vérifier les fuites mémoire

```bash
# Compilation
gcc -g -Wall -Wextra binary_tree.c -o binary_tree

# Vérification mémoire
valgrind --leak-check=full ./binary_tree

# Avec sanitizers
gcc -g -fsanitize=address binary_tree.c -o binary_tree
./binary_tree
```

---

## Récapitulatif

### Structure

Un arbre binaire est une structure hiérarchique où chaque nœud a au maximum 2 enfants.

```c
typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;
```

### BST (Arbre Binaire de Recherche)

**Propriété** : Gauche < Racine < Droit

**Avantage** : Recherche efficace en O(log n) si équilibré

### Opérations principales

- **Insertion** : Récursive, respecter la propriété BST
- **Recherche** : Comparer et aller à gauche/droite
- **Suppression** : 3 cas (feuille, 1 enfant, 2 enfants)

### Parcours

- **Préfixe** : Racine-Gauche-Droit
- **Infixe** : Gauche-Racine-Droit (ordre croissant pour BST)
- **Postfixe** : Gauche-Droit-Racine (libération mémoire)
- **Largeur** : Niveau par niveau (BFS)

### Complexité

| Opération | Équilibré | Dégénéré |
|-----------|-----------|----------|
| Recherche | O(log n) | O(n) |
| Insertion | O(log n) | O(n) |
| Suppression | O(log n) | O(n) |

### Applications

- Dictionnaires ordonnés
- Bases de données (indexation)
- Arbres de décision
- Expressions arithmétiques
- Systèmes de fichiers

---

## Pour aller plus loin

### Structures avancées

- **AVL Trees** : Auto-équilibrage avec rotations
- **Red-Black Trees** : Équilibrage par coloration (utilisés dans `std::map` C++)
- **B-Trees** : Arbres multi-branches (bases de données)
- **Tries** : Arbres pour chaînes de caractères
- **Segment Trees** : Requêtes d'intervalles

### Ressources

- Livre : "Introduction to Algorithms" (CLRS) - Chapitre 12
- Visualisation : [visualgo.net/en/bst](https://visualgo.net/en/bst)
- VisuAlgo pour animations interactives

**Projet suggéré** : Implémentez un système de gestion de contacts avec recherche par nom (BST) permettant l'ajout, la recherche, la suppression et l'affichage dans l'ordre alphabétique !

---


⏭️ [Choix de la structure de données](/11-structures-dynamiques/06-choix-structure.md)
