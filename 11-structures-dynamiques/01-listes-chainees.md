🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.1 Listes chaînées simples

## Introduction

Une **liste chaînée simple** (ou *singly linked list* en anglais) est une structure de données dynamique fondamentale en programmation. Contrairement aux tableaux qui stockent les éléments de manière contiguë en mémoire, une liste chaînée est constituée d'éléments (appelés **nœuds**) dispersés en mémoire et reliés entre eux par des **pointeurs**.

### Pourquoi apprendre les listes chaînées ?

- **Taille dynamique** : La liste peut grandir ou rétrécir selon les besoins, sans nécessiter de réallocation comme avec `realloc()` pour les tableaux
- **Insertion/Suppression efficaces** : Ajouter ou retirer un élément au début ou au milieu est rapide (O(1) ou O(n) selon la position)
- **Base pour d'autres structures** : Comprendre les listes chaînées est essentiel pour maîtriser les piles, files, arbres, graphes, etc.
- **Exercice de pointeurs** : C'est l'occasion parfaite de consolider votre compréhension des pointeurs

---

## Concept de base

### Structure d'un nœud

Chaque élément de la liste chaînée est un **nœud** (*node*) qui contient :
1. **Une donnée** : La valeur que l'on souhaite stocker (un entier, une chaîne, une structure complexe, etc.)
2. **Un pointeur vers le nœud suivant** : L'adresse mémoire du prochain élément de la liste

```c
struct Node {
    int data;              // Donnée stockée (ici un entier)
    struct Node* next;     // Pointeur vers le nœud suivant
};
```

### Représentation visuelle

Imaginons une liste contenant les valeurs `10`, `20`, `30` :

```
[HEAD] --> [10|•] --> [20|•] --> [30|•] --> NULL
            ^          ^          ^
            |          |          |
         nœud 1     nœud 2     nœud 3
```

- **HEAD** : Un pointeur vers le premier nœud de la liste (l'entrée)
- **data** : La valeur stockée (représentée par le nombre)
- **next** : Le pointeur vers le nœud suivant (représenté par `•`)
- **NULL** : Le dernier nœud pointe vers `NULL` pour indiquer la fin de la liste

---

## Déclaration et initialisation

### Créer un type alias pour simplifier

Pour éviter d'écrire `struct Node` à chaque fois, on utilise souvent `typedef` :

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;
```

**Note importante** : On doit écrire `struct Node* next;` et non `Node* next;` car le type `Node` n'existe pas encore au moment de la déclaration.

### Initialiser une liste vide

Une liste vide est représentée par un pointeur `head` qui vaut `NULL` :

```c
Node* head = NULL;  // Liste vide
```

---

## Opérations de base

### 1. Créer un nouveau nœud

Fonction utilitaire pour allouer et initialiser un nœud :

```c
#include <stdlib.h>
#include <stdio.h>

Node* create_node(int value) {
    // Allocation dynamique d'un nœud
    Node* new_node = (Node*)malloc(sizeof(Node));

    // Vérification de l'allocation
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : échec de l'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Initialisation des champs
    new_node->data = value;
    new_node->next = NULL;  // Par défaut, le nœud ne pointe vers rien

    return new_node;
}
```

**Points clés** :
- On utilise `malloc()` pour allouer la mémoire sur le tas (*heap*)
- Toujours vérifier si `malloc()` a réussi (retourne `NULL` en cas d'échec)
- Le nœud est initialisé avec `next = NULL` (il sera modifié lors de l'insertion)

---

### 2. Insérer au début de la liste

C'est l'opération la plus simple et la plus rapide (complexité O(1)).

```c
void insert_at_beginning(Node** head_ref, int value) {
    // Créer un nouveau nœud
    Node* new_node = create_node(value);

    // Le nouveau nœud pointe vers l'ancien premier élément
    new_node->next = *head_ref;

    // Le head pointe maintenant vers le nouveau nœud
    *head_ref = new_node;
}
```

**Explication détaillée** :

Avant l'insertion (liste contient `20` et `30`) :
```
[HEAD] --> [20|•] --> [30|•] --> NULL
```

On veut insérer `10` au début :
```c
insert_at_beginning(&head, 10);
```

Étapes :
1. Créer un nouveau nœud avec `data = 10`
2. Faire pointer `new_node->next` vers l'ancien premier élément (`20`)
3. Mettre à jour `head` pour qu'il pointe vers `new_node`

Après l'insertion :
```
[HEAD] --> [10|•] --> [20|•] --> [30|•] --> NULL
```

**Pourquoi `Node** head_ref` ?**
- On veut modifier la valeur de `head` lui-même (pas juste le nœud pointé)
- On passe donc un **pointeur vers le pointeur** (double pointeur)
- Cela permet de mettre à jour `head` dans la fonction appelante

---

### 3. Insérer à la fin de la liste

Plus complexe car il faut parcourir toute la liste (complexité O(n)).

```c
void insert_at_end(Node** head_ref, int value) {
    // Créer le nouveau nœud
    Node* new_node = create_node(value);

    // Cas particulier : liste vide
    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }

    // Parcourir jusqu'au dernier nœud
    Node* current = *head_ref;
    while (current->next != NULL) {
        current = current->next;
    }

    // Accrocher le nouveau nœud à la fin
    current->next = new_node;
}
```

**Détail du parcours** :

Liste initiale : `[10] --> [20] --> [30] --> NULL`

On veut ajouter `40` :
```c
insert_at_end(&head, 40);
```

1. `current` commence à `head` (nœud `10`)
2. Boucle : `current->next != NULL` ? Oui → `current = current->next` (passe à `20`)
3. Boucle : `current->next != NULL` ? Oui → `current = current->next` (passe à `30`)
4. Boucle : `current->next != NULL` ? Non → sortie de la boucle
5. `current` pointe maintenant vers le dernier nœud (`30`)
6. On fait `current->next = new_node` pour accrocher `40`

Résultat : `[10] --> [20] --> [30] --> [40] --> NULL`

---

### 4. Afficher la liste

Pour vérifier le contenu de notre liste :

```c
void print_list(Node* head) {
    Node* current = head;

    printf("Liste : ");
    while (current != NULL) {
        printf("%d --> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}
```

**Exemple de sortie** :
```
Liste : 10 --> 20 --> 30 --> NULL
```

**Points importants** :
- On passe une copie du pointeur `head` (pas besoin de `**` car on ne modifie pas la liste)
- La boucle continue tant qu'on n'atteint pas `NULL`
- On avance avec `current = current->next`

---

### 5. Rechercher un élément

Vérifier si une valeur existe dans la liste :

```c
int search(Node* head, int value) {
    Node* current = head;

    while (current != NULL) {
        if (current->data == value) {
            return 1;  // Trouvé
        }
        current = current->next;
    }

    return 0;  // Non trouvé
}
```

**Utilisation** :
```c
if (search(head, 20)) {
    printf("La valeur 20 est dans la liste\n");
} else {
    printf("La valeur 20 n'est pas dans la liste\n");
}
```

---

### 6. Supprimer un nœud par valeur

Opération délicate qui nécessite de gérer plusieurs cas :

```c
void delete_node(Node** head_ref, int value) {
    // Cas 1 : Liste vide
    if (*head_ref == NULL) {
        printf("La liste est vide, rien à supprimer\n");
        return;
    }

    Node* current = *head_ref;

    // Cas 2 : Le nœud à supprimer est le premier
    if (current->data == value) {
        *head_ref = current->next;  // Déplacer head
        free(current);              // Libérer la mémoire
        printf("Nœud avec valeur %d supprimé\n", value);
        return;
    }

    // Cas 3 : Le nœud est ailleurs dans la liste
    Node* previous = NULL;
    while (current != NULL && current->data != value) {
        previous = current;
        current = current->next;
    }

    // Nœud non trouvé
    if (current == NULL) {
        printf("Valeur %d non trouvée\n", value);
        return;
    }

    // Nœud trouvé : rebrancher les liens
    previous->next = current->next;
    free(current);
    printf("Nœud avec valeur %d supprimé\n", value);
}
```

**Explication visuelle** :

Supprimer `20` dans : `[10] --> [20] --> [30] --> NULL`

1. `previous` pointe vers `10`
2. `current` pointe vers `20` (trouvé !)
3. On fait `previous->next = current->next` pour "sauter" le nœud `20`
4. On libère la mémoire de `current` avec `free()`

Résultat : `[10] --> [30] --> NULL`

**⚠️ Piège fréquent** : Oublier de `free()` le nœud supprimé provoque une **fuite mémoire** !

---

### 7. Libérer toute la liste

**CRUCIAL** : Toujours libérer la mémoire allouée pour éviter les fuites.

```c
void free_list(Node** head_ref) {
    Node* current = *head_ref;
    Node* next_node;

    while (current != NULL) {
        next_node = current->next;  // Sauvegarder le pointeur suivant
        free(current);              // Libérer le nœud actuel
        current = next_node;        // Avancer au suivant
    }

    *head_ref = NULL;  // Réinitialiser head
    printf("Liste libérée\n");
}
```

**Pourquoi sauvegarder `next_node` ?**
- Si on fait `free(current)` directement, on ne peut plus accéder à `current->next` !
- On doit sauvegarder le pointeur suivant **avant** de libérer

---

## Exemple complet

Voici un programme qui utilise toutes ces opérations :

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Prototypes des fonctions (voir implémentations ci-dessus)
Node* create_node(int value);
void insert_at_beginning(Node** head_ref, int value);
void insert_at_end(Node** head_ref, int value);
void print_list(Node* head);
int search(Node* head, int value);
void delete_node(Node** head_ref, int value);
void free_list(Node** head_ref);

int main(void) {
    Node* head = NULL;  // Liste vide

    printf("=== Insertion au début ===\n");
    insert_at_beginning(&head, 30);
    insert_at_beginning(&head, 20);
    insert_at_beginning(&head, 10);
    print_list(head);  // Affiche : 10 --> 20 --> 30 --> NULL

    printf("\n=== Insertion à la fin ===\n");
    insert_at_end(&head, 40);
    insert_at_end(&head, 50);
    print_list(head);  // Affiche : 10 --> 20 --> 30 --> 40 --> 50 --> NULL

    printf("\n=== Recherche ===\n");
    if (search(head, 30)) {
        printf("30 trouvé dans la liste\n");
    }
    if (!search(head, 100)) {
        printf("100 non trouvé dans la liste\n");
    }

    printf("\n=== Suppression ===\n");
    delete_node(&head, 20);
    print_list(head);  // Affiche : 10 --> 30 --> 40 --> 50 --> NULL

    delete_node(&head, 10);  // Suppression du premier
    print_list(head);  // Affiche : 30 --> 40 --> 50 --> NULL

    printf("\n=== Libération de la mémoire ===\n");
    free_list(&head);
    print_list(head);  // Affiche : Liste : NULL

    return 0;
}

// ... (implémentations des fonctions ici)
```

---

## Avantages et inconvénients

### ✅ Avantages

1. **Taille dynamique** : Pas besoin de connaître la taille à l'avance
2. **Insertion/Suppression au début** : O(1) - très rapide
3. **Pas de déplacement d'éléments** : Contrairement aux tableaux, pas de décalage nécessaire
4. **Utilisation efficace de la mémoire** : Seulement la mémoire nécessaire est allouée

### ❌ Inconvénients

1. **Accès séquentiel uniquement** : Pas d'accès direct par index (O(n) pour accéder au i-ème élément)
2. **Overhead mémoire** : Chaque nœud nécessite un pointeur supplémentaire
3. **Fragmentation** : Les nœuds sont dispersés en mémoire (mauvais pour le cache CPU)
4. **Complexité** : Plus difficile à implémenter que les tableaux

---

## Comparaison avec les tableaux

| Opération | Tableau | Liste chaînée |
|-----------|---------|---------------|
| Accès par index | O(1) | O(n) |
| Insertion au début | O(n) | O(1) |
| Insertion à la fin | O(1) | O(n) * |
| Suppression au début | O(n) | O(1) |
| Recherche | O(n) | O(n) |
| Mémoire contiguë | Oui | Non |

\* O(1) si on maintient un pointeur vers le dernier élément (*tail*)

---

## Erreurs courantes et pièges

### 1. Oublier de vérifier `malloc()`

❌ **Mauvais** :
```c
Node* new_node = (Node*)malloc(sizeof(Node));
new_node->data = 10;  // Segfault si malloc a échoué !
```

✅ **Bon** :
```c
Node* new_node = (Node*)malloc(sizeof(Node));
if (new_node == NULL) {
    perror("malloc");
    return;
}
new_node->data = 10;
```

### 2. Perdre la référence au head

❌ **Mauvais** :
```c
void insert_at_beginning(Node* head, int value) {
    Node* new_node = create_node(value);
    new_node->next = head;
    head = new_node;  // ⚠️ Modifie seulement la copie locale !
}
```

✅ **Bon** :
```c
void insert_at_beginning(Node** head_ref, int value) {
    Node* new_node = create_node(value);
    new_node->next = *head_ref;
    *head_ref = new_node;  // ✅ Modifie le vrai pointeur
}
```

### 3. Fuites mémoire

❌ **Mauvais** :
```c
// Supprimer un nœud sans free()
previous->next = current->next;
// Oubli de : free(current);
```

✅ **Bon** :
```c
previous->next = current->next;
free(current);  // ✅ Libérer la mémoire
```

### 4. Déréférencer NULL

❌ **Mauvais** :
```c
void print_list(Node* head) {
    while (head->next != NULL) {  // ⚠️ Crash si head == NULL
        printf("%d\n", head->data);
        head = head->next;
    }
}
```

✅ **Bon** :
```c
void print_list(Node* head) {
    while (head != NULL) {  // ✅ Vérifier head, pas head->next
        printf("%d\n", head->data);
        head = head->next;
    }
}
```

---

## Variantes et optimisations

### Maintenir un pointeur vers la fin (tail)

Pour rendre l'insertion à la fin en O(1) :

```c
typedef struct LinkedList {
    Node* head;
    Node* tail;
    size_t size;
} LinkedList;

void init_list(LinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void insert_at_end_optimized(LinkedList* list, int value) {
    Node* new_node = create_node(value);

    if (list->head == NULL) {
        // Liste vide
        list->head = new_node;
        list->tail = new_node;
    } else {
        // Accrocher directement au tail
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;
}
```

---

## Bonnes pratiques

1. **Toujours initialiser les pointeurs** : `Node* head = NULL;`
2. **Vérifier les allocations** : Tester le retour de `malloc()`
3. **Libérer la mémoire** : Appeler `free_list()` avant la fin du programme
4. **Documenter les fonctions** : Indiquer si elles modifient la liste, si elles allouent de la mémoire, etc.
5. **Utiliser Valgrind** : Pour détecter les fuites mémoire

```bash
# Compiler avec les symboles de débogage
gcc -g -Wall -Wextra liste_chainee.c -o liste_chainee

# Exécuter avec Valgrind
valgrind --leak-check=full ./liste_chainee
```

---

## Récapitulatif

Les listes chaînées simples sont une structure de données dynamique essentielle où :
- Chaque **nœud** contient une donnée et un pointeur vers le suivant
- Le **head** pointe vers le premier nœud
- Le dernier nœud pointe vers **NULL**

**Opérations clés** :
- Insertion au début : O(1)
- Insertion à la fin : O(n) [ou O(1) avec tail]
- Recherche : O(n)
- Suppression : O(1) à O(n) selon la position

**Points critiques** :
- Toujours passer `Node**` pour modifier le head
- Libérer la mémoire avec `free()` pour éviter les fuites
- Gérer les cas limites (liste vide, suppression du premier élément, etc.)

---

## Pour aller plus loin

Dans les prochaines sections, nous verrons :
- **11.2 Listes doublement chaînées** : Nœuds avec pointeurs vers le précédent ET le suivant
- **11.3 Piles et Files** : Structures LIFO et FIFO basées sur les listes
- **11.4 Tables de hachage** : Combinaison de tableaux et de listes chaînées

**Conseil** : Pratiquez en implémentant vous-même toutes ces fonctions sans regarder le code. C'est le meilleur moyen de maîtriser les pointeurs en C !

---


⏭️ [Listes doublement chaînées](/11-structures-dynamiques/02-listes-doublement-chainees.md)
