🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.3 Piles et Files

## Introduction

Les **piles** (*stacks*) et les **files** (*queues*) sont deux structures de données abstraites fondamentales qui imposent un ordre strict d'accès aux éléments. Contrairement aux listes où on peut accéder à n'importe quel élément, ces structures limitent intentionnellement les opérations pour garantir un comportement spécifique.

### Analogies du monde réel

**Pile** : Imaginez une pile d'assiettes
- On ajoute une assiette **sur le dessus**
- On retire une assiette **depuis le dessus**
- On ne peut pas retirer une assiette du milieu sans tout démonter

**File** : Imaginez une file d'attente à la caisse
- Les gens arrivent **par la fin** de la file
- Les gens sont servis **depuis le début** de la file
- Le premier arrivé est le premier servi

---

## Partie 1 : Les Piles (Stacks)

### Concept : LIFO (Last In, First Out)

Une **pile** suit le principe **LIFO** : Le **dernier élément ajouté** est le **premier à être retiré**.

**Représentation visuelle** :

```
Opération push(10), push(20), push(30)

    |     |         |     |         |     |         | 30  |  ← Sommet (top)
    |     |         |     |         | 20  |         | 20  |
    |     |    →    | 10  |    →    | 10  |    →    | 10  |
    +-----+         +-----+         +-----+         +-----+
     Vide            push 10         push 20         push 30

Opération pop()

    | 30  |         |     |
    | 20  |    →    | 20  |  ← Nouveau sommet
    | 10  |         | 10  |
    +-----+         +-----+
  Avant pop()     Après pop() (30 retiré)
```

### Opérations principales

Une pile supporte principalement **4 opérations** :

1. **push(element)** : Ajouter un élément au sommet
2. **pop()** : Retirer et retourner l'élément du sommet
3. **peek() / top()** : Consulter l'élément du sommet sans le retirer
4. **is_empty()** : Vérifier si la pile est vide

---

### Implémentation 1 : Pile avec tableau statique

La méthode la plus simple pour les débutants.

#### Structure de données

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct Stack {
    int data[MAX_SIZE];    // Tableau pour stocker les éléments
    int top;               // Index du sommet (-1 si vide)
} Stack;
```

**Convention** : `top = -1` signifie que la pile est vide.

#### Initialisation

```c
void init_stack(Stack* stack) {
    stack->top = -1;  // Pile vide
}
```

#### Vérifier si vide ou pleine

```c
bool is_empty(Stack* stack) {
    return stack->top == -1;
}

bool is_full(Stack* stack) {
    return stack->top == MAX_SIZE - 1;
}
```

#### Push : Ajouter un élément

```c
void push(Stack* stack, int value) {
    // Vérifier si la pile est pleine
    if (is_full(stack)) {
        fprintf(stderr, "Erreur : pile pleine (overflow)\n");
        return;
    }

    // Incrémenter top et ajouter l'élément
    stack->top++;
    stack->data[stack->top] = value;

    printf("Push : %d\n", value);
}
```

**Explication** :
- Si `top = -1`, après `top++` on a `top = 0`, et on place l'élément à `data[0]`
- Si `top = 2`, après `top++` on a `top = 3`, et on place l'élément à `data[3]`

#### Pop : Retirer un élément

```c
int pop(Stack* stack) {
    // Vérifier si la pile est vide
    if (is_empty(stack)) {
        fprintf(stderr, "Erreur : pile vide (underflow)\n");
        exit(EXIT_FAILURE);
    }

    // Récupérer l'élément et décrémenter top
    int value = stack->data[stack->top];
    stack->top--;

    printf("Pop : %d\n", value);
    return value;
}
```

**Note** : On ne supprime pas physiquement l'élément, on décrémente juste `top`. L'ancienne valeur est "oubliée".

#### Peek : Consulter le sommet

```c
int peek(Stack* stack) {
    if (is_empty(stack)) {
        fprintf(stderr, "Erreur : pile vide\n");
        exit(EXIT_FAILURE);
    }

    return stack->data[stack->top];
}
```

#### Afficher la pile

```c
void print_stack(Stack* stack) {
    if (is_empty(stack)) {
        printf("Pile : [vide]\n");
        return;
    }

    printf("Pile (sommet → base) : ");
    for (int i = stack->top; i >= 0; i--) {
        printf("%d ", stack->data[i]);
    }
    printf("\n");
}
```

#### Exemple d'utilisation

```c
int main(void) {
    Stack stack;
    init_stack(&stack);

    push(&stack, 10);    // Pile : 10
    push(&stack, 20);    // Pile : 20 10
    push(&stack, 30);    // Pile : 30 20 10

    print_stack(&stack); // Affiche : 30 20 10

    printf("Sommet : %d\n", peek(&stack));  // Affiche : 30

    pop(&stack);         // Retire 30
    pop(&stack);         // Retire 20

    print_stack(&stack); // Affiche : 10

    push(&stack, 40);    // Pile : 40 10
    print_stack(&stack); // Affiche : 40 10

    return 0;
}
```

**Sortie** :
```
Push : 10
Push : 20
Push : 30
Pile (sommet → base) : 30 20 10
Sommet : 30
Pop : 30
Pop : 20
Pile (sommet → base) : 10
Push : 40
Pile (sommet → base) : 40 10
```

---

### Implémentation 2 : Pile avec liste chaînée

Pour une pile **dynamique** (taille illimitée, limitée seulement par la mémoire).

#### Structure de données

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct StackList {
    Node* top;     // Pointeur vers le sommet
    size_t size;   // Nombre d'éléments (optionnel)
} StackList;
```

**Astuce** : Le sommet de la pile correspond au **début** de la liste chaînée.

#### Initialisation

```c
void init_stack_list(StackList* stack) {
    stack->top = NULL;
    stack->size = 0;
}
```

#### Vérifier si vide

```c
bool is_empty_list(StackList* stack) {
    return stack->top == NULL;
}
```

#### Push avec liste chaînée

```c
void push_list(StackList* stack, int value) {
    // Créer un nouveau nœud
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = value;
    new_node->next = stack->top;  // Nouveau nœud pointe vers l'ancien sommet
    stack->top = new_node;        // Mettre à jour le sommet
    stack->size++;

    printf("Push : %d\n", value);
}
```

**Explication visuelle** :

Avant `push(30)` :
```
[top] → [20] → [10] → NULL
```

Après `push(30)` :
```
[top] → [30] → [20] → [10] → NULL
```

**C'est exactement** l'opération `insert_at_beginning` d'une liste chaînée !

#### Pop avec liste chaînée

```c
int pop_list(StackList* stack) {
    if (is_empty_list(stack)) {
        fprintf(stderr, "Erreur : pile vide\n");
        exit(EXIT_FAILURE);
    }

    Node* temp = stack->top;           // Sauvegarder le sommet
    int value = temp->data;            // Récupérer la valeur
    stack->top = stack->top->next;     // Déplacer top vers le suivant
    free(temp);                        // Libérer l'ancien sommet
    stack->size--;

    printf("Pop : %d\n", value);
    return value;
}
```

#### Libérer la pile

```c
void free_stack_list(StackList* stack) {
    Node* current = stack->top;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    stack->top = NULL;
    stack->size = 0;
}
```

---

### Applications pratiques des piles

#### 1. Vérification de parenthèses équilibrées

```c
bool is_balanced(const char* expression) {
    Stack stack;
    init_stack(&stack);

    for (int i = 0; expression[i] != '\0'; i++) {
        char ch = expression[i];

        if (ch == '(' || ch == '[' || ch == '{') {
            push(&stack, ch);
        }
        else if (ch == ')' || ch == ']' || ch == '}') {
            if (is_empty(&stack)) {
                return false;  // Fermante sans ouvrante
            }

            char top = pop(&stack);

            // Vérifier la correspondance
            if ((ch == ')' && top != '(') ||
                (ch == ']' && top != '[') ||
                (ch == '}' && top != '{')) {
                return false;  // Mauvaise correspondance
            }
        }
    }

    return is_empty(&stack);  // Doit être vide à la fin
}

// Utilisation
int main(void) {
    printf("%d\n", is_balanced("(())")); // 1 (vrai)
    printf("%d\n", is_balanced("({[]})")); // 1 (vrai)
    printf("%d\n", is_balanced("(()")); // 0 (faux)
    printf("%d\n", is_balanced("({)}"));  // 0 (faux)
    return 0;
}
```

#### 2. Évaluation d'expressions postfixes (Notation Polonaise Inversée)

Expression **infixe** : `3 + 4`
Expression **postfixe** : `3 4 +`

```c
int evaluate_postfix(const char* expression) {
    Stack stack;
    init_stack(&stack);

    for (int i = 0; expression[i] != '\0'; i++) {
        char ch = expression[i];

        if (ch >= '0' && ch <= '9') {
            // C'est un chiffre
            push(&stack, ch - '0');
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            // C'est un opérateur
            int b = pop(&stack);
            int a = pop(&stack);

            int result;
            switch(ch) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = a / b; break;
            }

            push(&stack, result);
        }
    }

    return pop(&stack);  // Le résultat final
}

// Utilisation
int main(void) {
    printf("%d\n", evaluate_postfix("34+")); // 3 + 4 = 7
    printf("%d\n", evaluate_postfix("53*2+")); // 5 * 3 + 2 = 17
    return 0;
}
```

#### 3. Fonction d'annulation (Undo)

```c
typedef struct TextEditor {
    Stack history;  // Pile pour les actions
} TextEditor;

void type_text(TextEditor* editor, const char* text) {
    // Sauvegarder l'action dans l'historique
    push(&editor->history, hash(text));  // Simplification
}

void undo(TextEditor* editor) {
    if (!is_empty(&editor->history)) {
        int last_action = pop(&editor->history);
        printf("Annulation de l'action %d\n", last_action);
    }
}
```

---

## Partie 2 : Les Files (Queues)

### Concept : FIFO (First In, First Out)

Une **file** suit le principe **FIFO** : Le **premier élément ajouté** est le **premier à être retiré**.

**Représentation visuelle** :

```
Opération enqueue(10), enqueue(20), enqueue(30)

    Front                           Front
      ↓                               ↓
    +-----+         +----+----+      +----+----+----+
    | 10  |    →    | 10 | 20 |  →   | 10 | 20 | 30 |
    +-----+         +----+----+      +----+----+----+
      Rear            Rear                   Rear
                                               ↑

Opération dequeue()

    Front                Front
      ↓                    ↓
    +----+----+----+     +----+----+
    | 10 | 20 | 30 |  →  | 20 | 30 |
    +----+----+----+     +----+----+
           Rear                Rear
                                 ↑
    (10 est retiré)
```

### Opérations principales

1. **enqueue(element)** : Ajouter un élément à la fin (*rear*)
2. **dequeue()** : Retirer et retourner l'élément du début (*front*)
3. **front() / peek()** : Consulter l'élément du début sans le retirer
4. **is_empty()** : Vérifier si la file est vide

---

### Implémentation 1 : File avec tableau circulaire

Un tableau circulaire évite le décalage des éléments.

#### Problème avec un tableau simple

Si on utilise un tableau linéaire :
```
Enqueue : [10, 20, 30, _, _, _]
           ^front    ^rear

Dequeue : [_, 20, 30, _, _, _]  ← Gaspillage d'espace !
              ^front  ^rear
```

**Solution** : Utiliser un **tableau circulaire** où les indices "bouclent".

#### Structure de données

```c
#define QUEUE_SIZE 100

typedef struct Queue {
    int data[QUEUE_SIZE];
    int front;    // Index du premier élément
    int rear;     // Index du dernier élément
    int size;     // Nombre d'éléments actuels
} Queue;
```

#### Initialisation

```c
void init_queue(Queue* queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}
```

#### Vérifier si vide ou pleine

```c
bool is_empty_queue(Queue* queue) {
    return queue->size == 0;
}

bool is_full_queue(Queue* queue) {
    return queue->size == QUEUE_SIZE;
}
```

#### Enqueue : Ajouter un élément

```c
void enqueue(Queue* queue, int value) {
    if (is_full_queue(queue)) {
        fprintf(stderr, "Erreur : file pleine\n");
        return;
    }

    // Incrémenter rear de façon circulaire
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->data[queue->rear] = value;
    queue->size++;

    printf("Enqueue : %d\n", value);
}
```

**Explication du modulo** :
- Si `rear = 4` et `QUEUE_SIZE = 5`, alors `(4 + 1) % 5 = 0` → On revient au début !

#### Dequeue : Retirer un élément

```c
int dequeue(Queue* queue) {
    if (is_empty_queue(queue)) {
        fprintf(stderr, "Erreur : file vide\n");
        exit(EXIT_FAILURE);
    }

    int value = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;  // Circulaire
    queue->size--;

    printf("Dequeue : %d\n", value);
    return value;
}
```

#### Front : Consulter le premier élément

```c
int front(Queue* queue) {
    if (is_empty_queue(queue)) {
        fprintf(stderr, "Erreur : file vide\n");
        exit(EXIT_FAILURE);
    }

    return queue->data[queue->front];
}
```

#### Afficher la file

```c
void print_queue(Queue* queue) {
    if (is_empty_queue(queue)) {
        printf("File : [vide]\n");
        return;
    }

    printf("File (front → rear) : ");
    int index = queue->front;
    for (int i = 0; i < queue->size; i++) {
        printf("%d ", queue->data[index]);
        index = (index + 1) % QUEUE_SIZE;  // Circulaire
    }
    printf("\n");
}
```

#### Exemple d'utilisation

```c
int main(void) {
    Queue queue;
    init_queue(&queue);

    enqueue(&queue, 10);  // File : 10
    enqueue(&queue, 20);  // File : 10 20
    enqueue(&queue, 30);  // File : 10 20 30

    print_queue(&queue);  // Affiche : 10 20 30

    dequeue(&queue);      // Retire 10
    print_queue(&queue);  // Affiche : 20 30

    enqueue(&queue, 40);  // File : 20 30 40
    print_queue(&queue);  // Affiche : 20 30 40

    return 0;
}
```

---

### Implémentation 2 : File avec liste chaînée

Plus simple conceptuellement, sans limite de taille.

#### Structure de données

```c
typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct QueueList {
    QueueNode* front;  // Premier élément (on retire ici)
    QueueNode* rear;   // Dernier élément (on ajoute ici)
    size_t size;
} QueueList;
```

**Astuce** :
- `enqueue` = insertion à la fin (comme `insert_at_end`)
- `dequeue` = suppression au début (comme `delete_at_beginning`)

#### Initialisation

```c
void init_queue_list(QueueList* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}
```

#### Enqueue avec liste chaînée

```c
void enqueue_list(QueueList* queue, int value) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = value;
    new_node->next = NULL;

    // Cas 1 : File vide
    if (queue->rear == NULL) {
        queue->front = new_node;
        queue->rear = new_node;
    }
    // Cas 2 : File non vide
    else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    queue->size++;
    printf("Enqueue : %d\n", value);
}
```

**Explication visuelle** :

Avant `enqueue(30)` :
```
[front] → [10] → [20] → NULL
                  ↑
                [rear]
```

Après `enqueue(30)` :
```
[front] → [10] → [20] → [30] → NULL
                         ↑
                       [rear]
```

#### Dequeue avec liste chaînée

```c
int dequeue_list(QueueList* queue) {
    if (queue->front == NULL) {
        fprintf(stderr, "Erreur : file vide\n");
        exit(EXIT_FAILURE);
    }

    QueueNode* temp = queue->front;
    int value = temp->data;

    queue->front = queue->front->next;

    // Si la file devient vide, mettre rear à NULL aussi
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;

    printf("Dequeue : %d\n", value);
    return value;
}
```

#### Libérer la file

```c
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
```

---

### Applications pratiques des files

#### 1. Gestion de tâches (Task scheduler)

```c
typedef struct Task {
    int id;
    char description[100];
} Task;

typedef struct TaskQueue {
    QueueList queue;
} TaskQueue;

void add_task(TaskQueue* tq, int id, const char* desc) {
    // Créer et ajouter une tâche à la file
    enqueue_list(&tq->queue, id);
    printf("Tâche ajoutée : %s\n", desc);
}

void process_next_task(TaskQueue* tq) {
    if (!is_empty_queue(&tq->queue)) {
        int task_id = dequeue_list(&tq->queue);
        printf("Traitement de la tâche %d\n", task_id);
    }
}
```

#### 2. Buffer de messages

```c
typedef struct MessageBuffer {
    Queue buffer;
} MessageBuffer;

void send_message(MessageBuffer* mb, int message) {
    enqueue(&mb->buffer, message);
}

int receive_message(MessageBuffer* mb) {
    return dequeue(&mb->buffer);
}
```

#### 3. Algorithme de parcours BFS (Breadth-First Search)

Les files sont essentielles pour parcourir des graphes en largeur :

```c
void bfs(Graph* graph, int start) {
    QueueList queue;
    init_queue_list(&queue);
    bool visited[MAX_NODES] = {false};

    enqueue_list(&queue, start);
    visited[start] = true;

    while (!is_empty_queue_list(&queue)) {
        int node = dequeue_list(&queue);
        printf("Visite : %d\n", node);

        // Ajouter tous les voisins non visités
        for (int i = 0; i < graph->num_neighbors[node]; i++) {
            int neighbor = graph->neighbors[node][i];
            if (!visited[neighbor]) {
                enqueue_list(&queue, neighbor);
                visited[neighbor] = true;
            }
        }
    }

    free_queue_list(&queue);
}
```

---

## Comparaison Pile vs File

| Aspect | Pile (Stack) | File (Queue) |
|--------|--------------|--------------|
| **Principe** | LIFO | FIFO |
| **Ajout** | push (au sommet) | enqueue (à la fin) |
| **Retrait** | pop (du sommet) | dequeue (du début) |
| **Analogie** | Pile d'assiettes | File d'attente |
| **Usage typique** | Undo/Redo, parenthèses, récursion | Tâches, BFS, buffers |
| **Complexité** | O(1) pour toutes les ops | O(1) pour toutes les ops |

---

## Comparaison des implémentations

### Tableau vs Liste chaînée

#### Pour les Piles

| Critère | Tableau | Liste chaînée |
|---------|---------|---------------|
| **Taille** | Fixe (MAX_SIZE) | Dynamique |
| **Mémoire** | Pré-allouée | Allocation à la demande |
| **Simplicité** | Plus simple | Plus complexe |
| **Overhead** | Faible | +1 pointeur par élément |
| **Débordement** | Possible | Seulement si mémoire épuisée |

#### Pour les Files

| Critère | Tableau circulaire | Liste chaînée |
|---------|-------------------|---------------|
| **Taille** | Fixe | Dynamique |
| **Complexité** | Modulo (%) | Simple |
| **Cache CPU** | Meilleur | Moins bon |
| **Overhead** | Faible | +1 pointeur par élément |

---

## Erreurs courantes et pièges

### Pile

#### 1. Oublier de vérifier si vide avant pop

❌ **Mauvais** :
```c
int value = pop(&stack);  // ⚠️ Crash si vide !
```

✅ **Bon** :
```c
if (!is_empty(&stack)) {
    int value = pop(&stack);
}
```

#### 2. Débordement de pile (stack overflow)

❌ **Mauvais** :
```c
for (int i = 0; i < 1000; i++) {
    push(&stack, i);  // ⚠️ Dépasse MAX_SIZE !
}
```

✅ **Bon** :
```c
for (int i = 0; i < 1000; i++) {
    if (!is_full(&stack)) {
        push(&stack, i);
    } else {
        fprintf(stderr, "Pile pleine\n");
        break;
    }
}
```

### File

#### 1. Oublier le modulo dans le tableau circulaire

❌ **Mauvais** :
```c
queue->rear = queue->rear + 1;  // ⚠️ Déborde du tableau !
```

✅ **Bon** :
```c
queue->rear = (queue->rear + 1) % QUEUE_SIZE;  // ✅ Circulaire
```

#### 2. Ne pas mettre rear à NULL quand la file se vide

❌ **Mauvais** :
```c
int dequeue_BAD(QueueList* queue) {
    QueueNode* temp = queue->front;
    int value = temp->data;
    queue->front = queue->front->next;
    free(temp);
    // ⚠️ Si la file devient vide, rear pointe vers un nœud libéré !
    return value;
}
```

✅ **Bon** :
```c
int dequeue_GOOD(QueueList* queue) {
    QueueNode* temp = queue->front;
    int value = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;  // ✅ Mettre à jour rear
    }

    free(temp);
    return value;
}
```

---

## Variantes avancées

### 1. Deque (Double-Ended Queue)

Une **deque** permet d'ajouter/retirer aux **deux extrémités** :

```c
typedef struct Deque {
    Node* front;
    Node* rear;
    size_t size;
} Deque;

// Opérations possibles :
void push_front(Deque* dq, int value);   // Ajouter au début
void push_back(Deque* dq, int value);    // Ajouter à la fin
int pop_front(Deque* dq);                // Retirer du début
int pop_back(Deque* dq);                 // Retirer de la fin
```

**Implémentation idéale** : Liste doublement chaînée

### 2. File de priorité (Priority Queue)

Les éléments sont retirés selon leur **priorité**, pas leur ordre d'arrivée.

```c
typedef struct PriorityQueue {
    // Implémentation typique : tas binaire (binary heap)
    int data[MAX_SIZE];
    int priorities[MAX_SIZE];
    int size;
} PriorityQueue;
```

**Cas d'usage** : Planification de tâches, algorithme de Dijkstra

### 3. Pile avec minimum en O(1)

Garder trace du minimum actuel :

```c
typedef struct MinStack {
    Stack values;      // Pile des valeurs
    Stack minimums;    // Pile des minimums
} MinStack;

void push_min_stack(MinStack* stack, int value) {
    push(&stack->values, value);

    if (is_empty(&stack->minimums) || value <= peek(&stack->minimums)) {
        push(&stack->minimums, value);
    }
}

int get_min(MinStack* stack) {
    return peek(&stack->minimums);  // O(1) !
}
```

---

## Complexité des opérations

### Pile

| Opération | Tableau | Liste chaînée |
|-----------|---------|---------------|
| push | O(1) | O(1) |
| pop | O(1) | O(1) |
| peek | O(1) | O(1) |
| is_empty | O(1) | O(1) |

### File

| Opération | Tableau circulaire | Liste chaînée |
|-----------|-------------------|---------------|
| enqueue | O(1) | O(1) |
| dequeue | O(1) | O(1) |
| front | O(1) | O(1) |
| is_empty | O(1) | O(1) |

**Toutes les opérations sont en temps constant** → Très efficace !

---

## Bonnes pratiques

1. **Toujours vérifier si vide** avant pop/dequeue
2. **Gérer les débordements** (tableau) ou les échecs d'allocation (liste)
3. **Libérer la mémoire** pour les implémentations avec listes
4. **Choisir la bonne implémentation** :
   - Tableau si taille max connue et petite
   - Liste chaînée si taille imprévisible ou grande
5. **Documenter les conventions** : top = -1 pour pile vide, etc.
6. **Tester les cas limites** : pile/file vide, pleine, un seul élément

---

## Récapitulatif

### Piles (LIFO)

**Concept** : Dernier entré, premier sorti (comme une pile d'assiettes)

**Opérations** : push, pop, peek, is_empty

**Implémentations** :
- Tableau : Simple, taille fixe
- Liste chaînée : Dynamique, insertion au début = push

**Applications** :
- Vérification de parenthèses
- Évaluation d'expressions postfixes
- Undo/Redo
- Pile d'appels (call stack)

### Files (FIFO)

**Concept** : Premier entré, premier sorti (comme une file d'attente)

**Opérations** : enqueue, dequeue, front, is_empty

**Implémentations** :
- Tableau circulaire : Efficace, utilise le modulo
- Liste chaînée : Simple, maintenir front et rear

**Applications** :
- Gestion de tâches
- Buffers de messages
- Algorithme BFS
- Serveurs (traitement de requêtes)

**Complexité** : Toutes les opérations en **O(1)** ✅

---

## Pour aller plus loin

Dans les prochaines sections, nous verrons :
- **11.4 Tables de hachage** : Accès ultra-rapide avec des clés
- **11.5 Arbres binaires** : Structures hiérarchiques
- **Structures avancées** : AVL, B-trees, graphes

**Conseil** : Implémentez un évaluateur d'expressions arithmétiques complet avec deux piles (une pour les opérateurs, une pour les opérandes). C'est un excellent exercice pour maîtriser les piles !

---


⏭️ [Tables de hachage](/11-structures-dynamiques/04-tables-hachage.md)
