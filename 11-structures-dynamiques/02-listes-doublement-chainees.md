🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.2 Listes doublement chaînées

## Introduction

Une **liste doublement chaînée** (*doubly linked list* en anglais) est une évolution de la liste chaînée simple où chaque nœud possède **deux pointeurs** : un vers le nœud **suivant** et un vers le nœud **précédent**. Cette bidirectionnalité offre une plus grande flexibilité pour naviguer dans la liste.

### Pourquoi utiliser des listes doublement chaînées ?

Bien que plus complexes que les listes simples, elles offrent des avantages significatifs :

- **Navigation bidirectionnelle** : On peut parcourir la liste dans les deux sens (avant ↔ arrière)
- **Suppression simplifiée** : Plus besoin de garder un pointeur vers le nœud précédent
- **Insertion avant un nœud** : Devient triviale avec l'accès au prédécesseur
- **Implémentation de files double-ended** : Ajout/retrait efficace aux deux extrémités

**Cas d'usage typiques** :
- Historique de navigation (navigateur web : boutons précédent/suivant)
- Éditeurs de texte (undo/redo)
- Playlists musicales avec navigation avant/arrière
- Implémentation de caches LRU (*Least Recently Used*)

---

## Concept de base

### Structure d'un nœud

Contrairement à la liste simple, chaque nœud contient maintenant **trois éléments** :

```c
struct Node {
    int data;              // Donnée stockée
    struct Node* next;     // Pointeur vers le nœud suivant
    struct Node* prev;     // Pointeur vers le nœud précédent (nouveau !)
};
```

### Représentation visuelle

Liste doublement chaînée contenant `10`, `20`, `30` :

```
NULL <-- [•|10|•] <--> [•|20|•] <--> [•|30|•] --> NULL
         ^   ^          ^   ^          ^   ^
         |   |          |   |          |   |
       prev data next  prev data next prev data next

[HEAD] --------^
                                           [TAIL] ---^
```

**Légende** :
- `<-->` : Lien bidirectionnel (double flèche)
- Le premier nœud a `prev = NULL`
- Le dernier nœud a `next = NULL`
- **HEAD** : Pointeur vers le premier nœud
- **TAIL** : Pointeur optionnel vers le dernier nœud (recommandé pour l'efficacité)

---

## Déclaration et initialisation

### Créer un type alias

```c
typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
} Node;
```

### Structure de liste complète (recommandée)

Pour gérer efficacement une liste doublement chaînée, on utilise souvent une structure englobante :

```c
typedef struct DoublyLinkedList {
    Node* head;      // Premier nœud
    Node* tail;      // Dernier nœud (optimise les insertions en fin)
    size_t size;     // Nombre d'éléments (optionnel mais utile)
} DoublyLinkedList;
```

### Initialiser une liste vide

```c
void init_list(DoublyLinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}
```

**Utilisation** :
```c
DoublyLinkedList my_list;  
init_list(&my_list);  
```

---

## Opérations de base

### 1. Créer un nouveau nœud

```c
#include <stdlib.h>
#include <stdio.h>

Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL) {
        fprintf(stderr, "Erreur : échec de l'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = value;
    new_node->next = NULL;
    new_node->prev = NULL;  // Initialiser aussi prev à NULL

    return new_node;
}
```

**Différence avec la liste simple** : On doit également initialiser le pointeur `prev`.

---

### 2. Insérer au début de la liste

Cette opération reste très efficace (O(1)), mais nécessite de gérer les deux pointeurs.

```c
void insert_at_beginning(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);

    // Cas 1 : Liste vide
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        list->size = 1;
        return;
    }

    // Cas 2 : Liste non vide
    new_node->next = list->head;   // Nouveau nœud pointe vers l'ancien premier
    list->head->prev = new_node;   // Ancien premier pointe en arrière vers nouveau
    list->head = new_node;         // Mettre à jour le head

    list->size++;
}
```

**Explication visuelle** :

Avant insertion (liste contient `20` et `30`) :
```
NULL <-- [•|20|•] <--> [•|30|•] --> NULL
         ^
      [HEAD]
```

On insère `10` au début :

**Étape 1** : `new_node->next = list->head;`
```
         [•|10|•]
              |
              v
NULL <-- [•|20|•] <--> [•|30|•] --> NULL
```

**Étape 2** : `list->head->prev = new_node;`
```
         [•|10|•]
              |
              v
         [•|20|•] <--> [•|30|•] --> NULL
              ^
              |
         [•|10|•]
```

**Étape 3** : `list->head = new_node;`
```
NULL <-- [•|10|•] <--> [•|20|•] <--> [•|30|•] --> NULL
         ^
      [HEAD]
```

---

### 3. Insérer à la fin de la liste

Grâce au pointeur `tail`, cette opération est maintenant en O(1) !

```c
void insert_at_end(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);

    // Cas 1 : Liste vide
    if (list->tail == NULL) {
        list->head = new_node;
        list->tail = new_node;
        list->size = 1;
        return;
    }

    // Cas 2 : Liste non vide
    new_node->prev = list->tail;   // Nouveau nœud pointe en arrière vers l'ancien dernier
    list->tail->next = new_node;   // Ancien dernier pointe vers le nouveau
    list->tail = new_node;         // Mettre à jour le tail

    list->size++;
}
```

**Avantage majeur** : Pas besoin de parcourir toute la liste comme avec une liste simple !

---

### 4. Insérer après un nœud spécifique

Une opération très utile, grandement simplifiée par le pointeur `prev`.

```c
void insert_after(Node* node, int value) {
    if (node == NULL) {
        fprintf(stderr, "Erreur : nœud NULL\n");
        return;
    }

    Node* new_node = create_node(value);

    new_node->next = node->next;   // Nouveau pointe vers le suivant de node
    new_node->prev = node;         // Nouveau pointe en arrière vers node

    if (node->next != NULL) {
        node->next->prev = new_node;  // Le suivant pointe en arrière vers nouveau
    }

    node->next = new_node;         // node pointe vers nouveau
}
```

**Schéma** :

Avant : `[node|20|•] <--> [•|30|•]`  
Après insertion de `25` : `[node|20|•] <--> [•|25|•] <--> [•|30|•]`  

**Note importante** : Si `node` est le dernier élément, il faut aussi mettre à jour `list->tail` !

---

### 5. Insérer avant un nœud spécifique

**Nouveau** : Impossible efficacement avec une liste simple, trivial avec une liste double !

```c
void insert_before(DoublyLinkedList* list, Node* node, int value) {
    if (node == NULL) {
        fprintf(stderr, "Erreur : nœud NULL\n");
        return;
    }

    Node* new_node = create_node(value);

    new_node->prev = node->prev;   // Nouveau pointe en arrière vers le précédent de node
    new_node->next = node;         // Nouveau pointe vers node

    if (node->prev != NULL) {
        node->prev->next = new_node;  // Le précédent pointe vers nouveau
    } else {
        // node était le premier, mettre à jour head
        list->head = new_node;
    }

    node->prev = new_node;         // node pointe en arrière vers nouveau

    list->size++;
}
```

---

### 6. Afficher la liste (avant et arrière)

Un des grands avantages : on peut parcourir dans les deux sens !

#### Parcours avant (HEAD → TAIL)

```c
void print_list_forward(DoublyLinkedList* list) {
    Node* current = list->head;

    printf("Liste (avant) : ");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" <-> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}
```

#### Parcours arrière (TAIL → HEAD)

```c
void print_list_backward(DoublyLinkedList* list) {
    Node* current = list->tail;

    printf("Liste (arrière) : ");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->prev != NULL) {
            printf(" <-> ");
        }
        current = current->prev;
    }
    printf(" -> NULL\n");
}
```

**Exemple de sortie** :
```
Liste (avant) : 10 <-> 20 <-> 30 -> NULL  
Liste (arrière) : 30 <-> 20 <-> 10 -> NULL  
```

---

### 7. Rechercher un élément

Similaire à la liste simple, mais on peut optimiser en cherchant depuis le début ou la fin selon le contexte :

```c
Node* search(DoublyLinkedList* list, int value) {
    Node* current = list->head;

    while (current != NULL) {
        if (current->data == value) {
            return current;  // Retourner le nœud trouvé
        }
        current = current->next;
    }

    return NULL;  // Non trouvé
}
```

**Utilisation** :
```c
Node* found = search(&my_list, 20);  
if (found != NULL) {  
    printf("Valeur 20 trouvée : %d\n", found->data);
}
```

---

### 8. Supprimer un nœud

**Avantage majeur** : Plus besoin de garder un pointeur vers le précédent !

```c
void delete_node(DoublyLinkedList* list, Node* node) {
    if (node == NULL) {
        return;
    }

    // Cas 1 : Nœud est le premier (head)
    if (node->prev == NULL) {
        list->head = node->next;
    } else {
        // Rebrancher le précédent vers le suivant
        node->prev->next = node->next;
    }

    // Cas 2 : Nœud est le dernier (tail)
    if (node->next == NULL) {
        list->tail = node->prev;
    } else {
        // Rebrancher le suivant vers le précédent
        node->next->prev = node->prev;
    }

    // Libérer la mémoire
    free(node);
    list->size--;
}
```

**Explication visuelle** :

Supprimer `20` dans : `[10] <--> [20] <--> [30]`

1. `node->prev->next = node->next;` : `[10]` pointe vers `[30]`
2. `node->next->prev = node->prev;` : `[30]` pointe vers `[10]`
3. `free(node);` : Libérer la mémoire de `[20]`

Résultat : `[10] <--> [30]`

**Comparaison avec liste simple** :
```c
// Liste simple : Besoin de parcourir pour trouver le précédent
void delete_node_simple(Node** head, int value) {
    Node* current = *head;
    Node* previous = NULL;  // ⚠️ Nécessaire !
    // ... parcours pour trouver previous ...
}

// Liste double : Accès direct au précédent !
void delete_node_double(DoublyLinkedList* list, Node* node) {
    if (node->prev != NULL) {
        node->prev->next = node->next;  // ✅ Accès direct !
    }
    // ...
}
```

---

### 9. Supprimer par valeur

Version qui combine recherche et suppression :

```c
int delete_by_value(DoublyLinkedList* list, int value) {
    Node* node = search(list, value);

    if (node == NULL) {
        return 0;  // Non trouvé
    }

    delete_node(list, node);
    return 1;  // Supprimé avec succès
}
```

---

### 10. Libérer toute la liste

Similaire à la liste simple, mais on peut parcourir dans n'importe quel sens :

```c
void free_list(DoublyLinkedList* list) {
    Node* current = list->head;
    Node* next_node;

    while (current != NULL) {
        next_node = current->next;  // Sauvegarder le suivant
        free(current);              // Libérer le nœud actuel
        current = next_node;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    printf("Liste libérée\n");
}
```

---

## Exemple complet

Voici un programme de démonstration complet :

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct DoublyLinkedList {
    Node* head;
    Node* tail;
    size_t size;
} DoublyLinkedList;

// Prototypes
Node* create_node(int value);  
void init_list(DoublyLinkedList* list);  
void insert_at_beginning(DoublyLinkedList* list, int value);  
void insert_at_end(DoublyLinkedList* list, int value);  
void print_list_forward(DoublyLinkedList* list);  
void print_list_backward(DoublyLinkedList* list);  
Node* search(DoublyLinkedList* list, int value);  
void delete_node(DoublyLinkedList* list, Node* node);  
int delete_by_value(DoublyLinkedList* list, int value);  
void free_list(DoublyLinkedList* list);  

int main(void) {
    DoublyLinkedList list;
    init_list(&list);

    printf("=== Insertion au début ===\n");
    insert_at_beginning(&list, 30);
    insert_at_beginning(&list, 20);
    insert_at_beginning(&list, 10);
    print_list_forward(&list);
    // Affiche : 10 <-> 20 <-> 30 -> NULL

    printf("\n=== Insertion à la fin ===\n");
    insert_at_end(&list, 40);
    insert_at_end(&list, 50);
    print_list_forward(&list);
    // Affiche : 10 <-> 20 <-> 30 <-> 40 <-> 50 -> NULL

    printf("\n=== Parcours arrière ===\n");
    print_list_backward(&list);
    // Affiche : 50 <-> 40 <-> 30 <-> 20 <-> 10 -> NULL

    printf("\n=== Recherche ===\n");
    Node* found = search(&list, 30);
    if (found != NULL) {
        printf("Valeur 30 trouvée : %d\n", found->data);
        printf("  Précédent : %d\n", found->prev ? found->prev->data : -1);
        printf("  Suivant : %d\n", found->next ? found->next->data : -1);
    }

    printf("\n=== Suppression de 20 ===\n");
    delete_by_value(&list, 20);
    print_list_forward(&list);
    // Affiche : 10 <-> 30 <-> 40 <-> 50 -> NULL

    printf("\n=== Suppression du premier (10) ===\n");
    delete_by_value(&list, 10);
    print_list_forward(&list);
    // Affiche : 30 <-> 40 <-> 50 -> NULL

    printf("\n=== Suppression du dernier (50) ===\n");
    delete_by_value(&list, 50);
    print_list_forward(&list);
    // Affiche : 30 <-> 40 -> NULL

    printf("\n=== Taille de la liste : %zu ===\n", list.size);

    printf("\n=== Libération ===\n");
    free_list(&list);
    print_list_forward(&list);
    // Affiche : Liste (avant) :  -> NULL

    return 0;
}

// ... (implémentations des fonctions comme ci-dessus)
```

---

## Avantages et inconvénients

### ✅ Avantages par rapport aux listes simples

1. **Navigation bidirectionnelle** : Parcours avant et arrière
2. **Suppression simplifiée** : Pas besoin de garder le nœud précédent
3. **Insertion avant** : Triviale avec l'accès à `prev`
4. **Opérations en O(1) aux deux extrémités** : Avec head et tail
5. **Symétrie** : Les algorithmes sont souvent plus élégants

### ❌ Inconvénients

1. **Overhead mémoire** : Deux pointeurs par nœud au lieu d'un (+50% de mémoire pour les pointeurs)
2. **Complexité accrue** : Plus de pointeurs à gérer (risque d'erreurs)
3. **Maintenance** : Plus de liens à mettre à jour lors des modifications
4. **Cache CPU** : Structure plus lourde, moins favorable au cache

---

## Comparaison liste simple vs liste double

| Critère | Liste simple | Liste double |
|---------|--------------|--------------|
| **Mémoire par nœud** | `data + 1 pointeur` | `data + 2 pointeurs` |
| **Parcours avant** | O(n) | O(n) |
| **Parcours arrière** | ❌ Impossible | ✅ O(n) |
| **Insertion au début** | O(1) | O(1) |
| **Insertion à la fin** | O(n) ou O(1)* | O(1) avec tail |
| **Suppression d'un nœud connu** | O(n) | O(1) ✅ |
| **Complexité d'implémentation** | Simple | Moyenne |

\* O(1) si on maintient un pointeur tail

---

## Erreurs courantes et pièges

### 1. Oublier de mettre à jour les deux liens

❌ **Mauvais** :
```c
void insert_at_beginning_BAD(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);
    new_node->next = list->head;
    list->head = new_node;
    // ⚠️ Oubli : list->head->prev = new_node;
}
```

Conséquence : La liste est **cassée**, le parcours arrière échouera !

✅ **Bon** :
```c
void insert_at_beginning_GOOD(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);
    new_node->next = list->head;

    if (list->head != NULL) {
        list->head->prev = new_node;  // ✅ Ne pas oublier !
    }

    list->head = new_node;
}
```

### 2. Ne pas gérer les cas limites

❌ **Mauvais** :
```c
void delete_node_BAD(Node* node) {
    node->prev->next = node->next;  // ⚠️ Segfault si node est le premier !
    node->next->prev = node->prev;  // ⚠️ Segfault si node est le dernier !
    free(node);
}
```

✅ **Bon** :
```c
void delete_node_GOOD(DoublyLinkedList* list, Node* node) {
    if (node->prev != NULL) {
        node->prev->next = node->next;  // ✅ Vérification
    } else {
        list->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;  // ✅ Vérification
    } else {
        list->tail = node->prev;
    }

    free(node);
}
```

### 3. Créer des cycles accidentellement

❌ **Mauvais** :
```c
// Si on oublie de mettre prev/next à NULL lors de la création
Node* new_node = malloc(sizeof(Node));  
new_node->data = 10;  
// ⚠️ prev et next contiennent des valeurs aléatoires (garbage) !
```

✅ **Bon** :
```c
Node* new_node = create_node(10);  // Initialise prev et next à NULL
```

### 4. Oublier de mettre à jour tail

❌ **Mauvais** :
```c
void insert_at_end_BAD(DoublyLinkedList* list, int value) {
    Node* new_node = create_node(value);
    list->tail->next = new_node;
    new_node->prev = list->tail;
    // ⚠️ Oubli : list->tail = new_node;
}
```

Conséquence : `tail` pointe vers l'avant-dernier élément !

---

## Opérations avancées

### 1. Inverser une liste doublement chaînée

Très élégant avec une liste double : on échange simplement `next` et `prev` !

```c
void reverse_list(DoublyLinkedList* list) {
    Node* current = list->head;
    Node* temp = NULL;

    // Échanger head et tail
    temp = list->head;
    list->head = list->tail;
    list->tail = temp;

    // Parcourir et échanger next et prev pour chaque nœud
    while (current != NULL) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;

        current = current->prev;  // Avancer (qui était next avant l'échange)
    }
}
```

**Avant** : `10 <-> 20 <-> 30`  
**Après** : `30 <-> 20 <-> 10`  

### 2. Concaténer deux listes

```c
void concatenate(DoublyLinkedList* list1, DoublyLinkedList* list2) {
    if (list1->tail == NULL) {
        // list1 est vide, copier list2
        list1->head = list2->head;
        list1->tail = list2->tail;
    } else if (list2->head != NULL) {
        // Relier les deux listes
        list1->tail->next = list2->head;
        list2->head->prev = list1->tail;
        list1->tail = list2->tail;
    }

    list1->size += list2->size;

    // Réinitialiser list2 (sans libérer les nœuds)
    list2->head = NULL;
    list2->tail = NULL;
    list2->size = 0;
}
```

### 3. Obtenir le n-ième élément

```c
Node* get_nth_node(DoublyLinkedList* list, size_t index) {
    if (index >= list->size) {
        return NULL;
    }

    Node* current;

    // Optimisation : partir du début ou de la fin selon l'index
    if (index < list->size / 2) {
        // Plus proche du début
        current = list->head;
        for (size_t i = 0; i < index; i++) {
            current = current->next;
        }
    } else {
        // Plus proche de la fin
        current = list->tail;
        for (size_t i = list->size - 1; i > index; i--) {
            current = current->prev;
        }
    }

    return current;
}
```

**Avantage** : On peut optimiser en partant du côté le plus proche !

---

## Cas d'usage pratiques

### 1. Implémentation d'un historique de navigation

```c
typedef struct BrowserHistory {
    DoublyLinkedList pages;
    Node* current_page;
} BrowserHistory;

void visit_page(BrowserHistory* history, const char* url) {
    // Supprimer les pages "forward" si on visite une nouvelle page
    if (history->current_page != NULL && history->current_page->next != NULL) {
        // TODO : Libérer les nœuds après current_page
    }

    // Ajouter la nouvelle page
    insert_at_end(&history->pages, hash(url));  // Simplification
    history->current_page = history->pages.tail;
}

void go_back(BrowserHistory* history) {
    if (history->current_page != NULL && history->current_page->prev != NULL) {
        history->current_page = history->current_page->prev;
    }
}

void go_forward(BrowserHistory* history) {
    if (history->current_page != NULL && history->current_page->next != NULL) {
        history->current_page = history->current_page->next;
    }
}
```

### 2. Cache LRU (Least Recently Used)

Les listes doublement chaînées sont parfaites pour implémenter un cache LRU :
- Les éléments récemment utilisés sont déplacés en tête
- Les éléments non utilisés se retrouvent en queue
- Suppression facile du dernier élément (le moins récemment utilisé)

---

## Optimisation et variantes

### Liste doublement chaînée circulaire

Le dernier nœud pointe vers le premier, et vice-versa :

```
    +----------------------------------+
    |                                  v
[•|10|•] <--> [•|20|•] <--> [•|30|•]
    ^                                  |
    +----------------------------------+
```

**Avantages** :
- Pas besoin de vérifier `NULL`
- Parcours circulaire naturel (playlists en boucle)

**Inconvénient** :
- Plus complexe à gérer (risque de boucles infinies)

---

## Bonnes pratiques

1. **Toujours mettre à jour les deux liens** : `next` ET `prev`
2. **Vérifier les cas limites** : Liste vide, premier nœud, dernier nœud
3. **Utiliser une structure englobante** : `DoublyLinkedList` avec `head`, `tail`, `size`
4. **Maintenir tail** : Pour des opérations O(1) aux deux extrémités
5. **Tests rigoureux** : Tester l'insertion/suppression au début, milieu, fin
6. **Valgrind** : Détecter les fuites et les accès invalides

```bash
# Compilation
gcc -g -Wall -Wextra -Werror liste_double.c -o liste_double

# Vérification mémoire
valgrind --leak-check=full ./liste_double

# Avec AddressSanitizer
gcc -g -fsanitize=address liste_double.c -o liste_double
./liste_double
```

---

## Complexité des opérations

| Opération | Complexité | Notes |
|-----------|------------|-------|
| Insertion au début | O(1) | Avec head |
| Insertion à la fin | O(1) | Avec tail |
| Insertion après nœud | O(1) | Si on a le pointeur |
| Recherche | O(n) | Parcours nécessaire |
| Accès au n-ième | O(n) | Optimisable depuis les 2 bouts |
| Suppression (nœud connu) | O(1) ✅ | **Avantage majeur** |
| Suppression (par valeur) | O(n) | Recherche + O(1) suppression |
| Parcours avant | O(n) | |
| Parcours arrière | O(n) | **Nouveau** |

---

## Récapitulatif

Les listes doublement chaînées sont une évolution puissante des listes simples :

**Structure** :
- Chaque nœud a un pointeur `next` ET un pointeur `prev`
- Navigation bidirectionnelle possible
- Souvent accompagnée d'un pointeur `tail` pour l'efficacité

**Avantages clés** :
- ✅ Suppression en O(1) si on a le pointeur vers le nœud  
- ✅ Insertion avant/après un nœud en O(1)  
- ✅ Parcours dans les deux sens  
- ✅ Opérations aux deux extrémités en O(1)

**Prix à payer** :
- ❌ +50% de mémoire pour les pointeurs  
- ❌ Plus complexe à implémenter correctement  
- ❌ Plus de liens à maintenir

**Quand l'utiliser ?**
- Navigation bidirectionnelle nécessaire (historique, playlist)
- Suppressions fréquentes de nœuds connus
- Implémentation de caches LRU
- Files double-ended (deque)

**Quand préférer les listes simples ?**
- Mémoire limitée
- Navigation unidirectionnelle suffisante
- Simplicité prioritaire

---

## Pour aller plus loin

Dans les prochaines sections, nous verrons :
- **11.3 Piles et Files** : Structures LIFO/FIFO basées sur des listes
- **11.4 Tables de hachage** : Gestion des collisions avec des listes chaînées
- **11.5 Arbres binaires** : Introduction aux structures arborescentes

**Conseil pratique** : Implémentez un système de gestion de tâches avec undo/redo en utilisant une liste doublement chaînée. C'est un excellent exercice pour maîtriser tous les cas limites !

---


⏭️ [Piles et Files](/11-structures-dynamiques/03-piles-et-files.md)
