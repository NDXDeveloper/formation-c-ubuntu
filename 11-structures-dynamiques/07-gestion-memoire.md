🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.7 Gestion mémoire des structures dynamiques

## Introduction

La **gestion de la mémoire** est l'une des responsabilités les plus importantes et les plus difficiles en C. Contrairement à des langages comme Python ou Java qui ont un *garbage collector* (ramasse-miettes), en C c'est **vous** qui devez gérer l'allocation et la libération de la mémoire.

### Pourquoi c'est important ?

```c
// Code apparemment innocent
for (int i = 0; i < 1000000; i++) {
    Node* node = create_node(i);
    // Oubli de free(node)
}
// ⚠️ 1 million de nœuds jamais libérés = FUITE MÉMOIRE massive !
```

**Conséquences des erreurs de gestion mémoire** :
- 🔴 **Fuites mémoire** : Votre programme consomme de plus en plus de RAM
- 💥 **Segmentation faults** : Crashs brutaux du programme
- 🐛 **Comportements imprévisibles** : Bugs difficiles à reproduire
- 🔐 **Failles de sécurité** : Vulnérabilités exploitables

### Le contrat de la mémoire dynamique

**Règle d'or** :
```
Pour chaque malloc() / calloc() / realloc()
Il DOIT y avoir exactement UN free() correspondant
```

---

## Rappel : Stack vs Heap

### Stack (Pile d'appels)

**Caractéristiques** :
- Allocation **automatique**
- Libération **automatique** (quand la fonction se termine)
- Taille **limitée** (quelques Mo généralement)
- Très **rapide**

```c
void function() {
    int x = 10;              // Alloué sur la stack
    char buffer[100];        // Alloué sur la stack
    // À la fin de la fonction : libération automatique
}
```

**Avantage** : Pas de gestion manuelle
**Inconvénient** : Taille limitée, durée de vie limitée à la fonction

### Heap (Tas)

**Caractéristiques** :
- Allocation **manuelle** (malloc, calloc, realloc)
- Libération **manuelle** (free)
- Taille **grande** (limité par la RAM)
- Plus **lent** que la stack

```c
void function() {
    int* ptr = (int*)malloc(sizeof(int) * 1000);  // Alloué sur le heap
    // ...
    free(ptr);  // DOIT être libéré manuellement !
}
```

**Avantage** : Grande taille, durée de vie contrôlée
**Inconvénient** : Gestion manuelle nécessaire

### Représentation visuelle

```
┌─────────────────────────────┐
│         STACK               │  ← Croît vers le bas
│  [variables locales]        │
│  [paramètres fonctions]     │
│  [adresses de retour]       │
├─────────────────────────────┤
│                             │
│         (espace libre)      │
│                             │
├─────────────────────────────┤
│         HEAP                │  ← Croît vers le haut
│  [malloc/calloc]            │
│  [structures dynamiques]    │
│  [grandes allocations]      │
└─────────────────────────────┘
```

---

## Cycle de vie de la mémoire dynamique

### Les 3 étapes essentielles

#### 1. Allocation

```c
// malloc : alloue de la mémoire non initialisée
int* ptr = (int*)malloc(sizeof(int) * 10);

// calloc : alloue ET initialise à zéro
int* ptr2 = (int*)calloc(10, sizeof(int));

// realloc : redimensionne une allocation existante
ptr = (int*)realloc(ptr, sizeof(int) * 20);
```

#### 2. Utilisation

```c
if (ptr != NULL) {
    for (int i = 0; i < 10; i++) {
        ptr[i] = i * 2;  // Utilisation normale
    }
}
```

#### 3. Libération

```c
free(ptr);      // Libère la mémoire
ptr = NULL;     // Bonne pratique : mettre à NULL après free
```

### Visualisation du cycle

```
[Allocation]
    ↓
malloc/calloc
    ↓
Pointeur valide → [Utilisation] → Accès aux données
    ↓
free()
    ↓
[Libération]
    ↓
Pointeur invalide (dangling pointer)
    ↓
Mettre à NULL
```

---

## Les erreurs classiques

### 1. Fuite mémoire (Memory Leak)

**Définition** : Mémoire allouée mais jamais libérée.

#### Exemple simple

```c
void leak_example() {
    int* ptr = (int*)malloc(sizeof(int) * 100);
    // ... utilisation ...
    // ⚠️ Oubli de free(ptr) !
}  // La mémoire reste allouée mais inaccessible

// Si appelé 1000 fois → 1000 blocs de mémoire perdus !
for (int i = 0; i < 1000; i++) {
    leak_example();
}
```

#### Exemple avec structures

```c
Node* create_leak() {
    Node* node = create_node(10);
    node->next = create_node(20);
    node->next->next = create_node(30);

    return node;  // On retourne seulement le premier nœud
}

// Utilisation
Node* temp = create_leak();
free(temp);  // ⚠️ Libère seulement le premier nœud !
             // Les nœuds 20 et 30 sont toujours alloués
```

**Correction** :

```c
void free_list_correctly(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;  // Sauvegarder le suivant
        free(current);               // Libérer le courant
        current = next;              // Avancer
    }
}
```

### 2. Double Free

**Définition** : Appeler `free()` deux fois sur le même pointeur.

```c
int* ptr = (int*)malloc(sizeof(int));
free(ptr);
free(ptr);  // ⚠️ DOUBLE FREE → Comportement indéfini / crash
```

**Conséquence** : Corruption de la mémoire, crash du programme.

**Prévention** :

```c
int* ptr = (int*)malloc(sizeof(int));
free(ptr);
ptr = NULL;  // ✅ Mettre à NULL après free

free(ptr);   // ✅ free(NULL) est sûr et ne fait rien
```

### 3. Use After Free (Utilisation après libération)

**Définition** : Utiliser un pointeur après avoir appelé `free()`.

```c
int* ptr = (int*)malloc(sizeof(int));
*ptr = 42;
free(ptr);

printf("%d\n", *ptr);  // ⚠️ Use after free !
*ptr = 100;            // ⚠️ Écriture dans mémoire libérée
```

**Conséquence** : Comportement imprévisible, corruption de données.

**Visualisation** :

```
Étape 1: Allocation
ptr → [Mémoire allouée]

Étape 2: Utilisation
ptr → [42] ✓

Étape 3: Libération
ptr → [Mémoire libérée] (mais ptr pointe toujours là !)

Étape 4: Use after free
ptr → [???] ⚠️ Contenu indéfini
```

**Prévention** :

```c
int* ptr = (int*)malloc(sizeof(int));
*ptr = 42;
free(ptr);
ptr = NULL;  // ✅ Empêche l'utilisation accidentelle

if (ptr != NULL) {
    *ptr = 100;  // Ne sera jamais exécuté
}
```

### 4. Dangling Pointer (Pointeur pendant)

**Définition** : Pointeur qui pointe vers une mémoire qui n'est plus valide.

#### Cas 1 : Après free

```c
int* ptr1 = (int*)malloc(sizeof(int));
int* ptr2 = ptr1;  // ptr2 pointe vers la même adresse

free(ptr1);
// ptr2 est maintenant un dangling pointer !
*ptr2 = 10;  // ⚠️ Erreur !
```

#### Cas 2 : Retour d'adresse locale

```c
int* return_local() {
    int x = 10;  // Variable locale (stack)
    return &x;   // ⚠️ Retourne l'adresse d'une variable locale !
}  // x est détruite à la fin de la fonction

int* ptr = return_local();
printf("%d\n", *ptr);  // ⚠️ Accès à mémoire invalide
```

**Correction** :

```c
int* return_heap() {
    int* x = (int*)malloc(sizeof(int));  // ✅ Allocation sur le heap
    *x = 10;
    return x;  // OK : la mémoire reste valide
}

int* ptr = return_heap();
printf("%d\n", *ptr);
free(ptr);  // Ne pas oublier de libérer !
```

### 5. Oubli de vérification de malloc

**Problème** : `malloc()` peut échouer et retourner `NULL`.

```c
int* ptr = (int*)malloc(sizeof(int) * 1000000000);  // Énorme allocation
*ptr = 42;  // ⚠️ Segfault si malloc a retourné NULL !
```

**Correction** :

```c
int* ptr = (int*)malloc(sizeof(int) * 1000000000);
if (ptr == NULL) {
    fprintf(stderr, "Erreur : allocation mémoire échouée\n");
    return -1;  // ou exit(EXIT_FAILURE);
}

*ptr = 42;  // ✅ Sûr maintenant
free(ptr);
```

### 6. Fuite lors d'une réallocation

```c
int* ptr = (int*)malloc(sizeof(int) * 10);

// ⚠️ MAUVAIS : Si realloc échoue, ptr perd l'adresse originale
ptr = (int*)realloc(ptr, sizeof(int) * 20);
```

**Correction** :

```c
int* ptr = (int*)malloc(sizeof(int) * 10);

int* temp = (int*)realloc(ptr, sizeof(int) * 20);
if (temp == NULL) {
    // realloc a échoué, mais ptr est toujours valide
    free(ptr);  // Libérer l'ancienne allocation
    return -1;
}
ptr = temp;  // ✅ Mise à jour seulement si succès
```

---

## Gestion mémoire des structures dynamiques

### Liste chaînée simple

#### Libération correcte

```c
void free_list(Node* head) {
    Node* current = head;

    while (current != NULL) {
        Node* next = current->next;  // ⚠️ Sauvegarder AVANT de libérer
        free(current);
        current = next;
    }
}
```

**Pourquoi sauvegarder `next` ?**

```c
// ❌ MAUVAIS
while (current != NULL) {
    free(current);
    current = current->next;  // ⚠️ Accès à mémoire libérée !
}

// ✅ BON
while (current != NULL) {
    Node* next = current->next;  // Sauvegarder d'abord
    free(current);
    current = next;  // Utiliser la copie sauvegardée
}
```

#### Suppression d'un nœud avec clés allouées

```c
typedef struct Node {
    char* key;     // Chaîne allouée dynamiquement
    int value;
    struct Node* next;
} Node;

void free_node(Node* node) {
    if (node != NULL) {
        free(node->key);   // ✅ Libérer la clé d'abord
        free(node);        // Puis le nœud
    }
}

void free_list_with_keys(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free_node(current);  // Libération complète
        current = next;
    }
}
```

### Liste doublement chaînée

```c
void free_double_list(DoublyNode* head) {
    DoublyNode* current = head;

    while (current != NULL) {
        DoublyNode* next = current->next;  // Sauvegarder le suivant

        // Si la donnée est allouée dynamiquement
        if (current->data != NULL) {
            free(current->data);
        }

        free(current);
        current = next;
    }
}
```

### Table de hachage

```c
void free_hash_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = table->buckets[i];

        while (current != NULL) {
            Node* next = current->next;

            // Libérer la clé (si allouée avec strdup)
            free(current->key);

            // Libérer le nœud
            free(current);

            current = next;
        }

        table->buckets[i] = NULL;  // Réinitialiser
    }
}
```

### Arbre binaire

```c
void free_tree(TreeNode* root) {
    if (root == NULL) {
        return;  // Cas de base
    }

    // ⚠️ IMPORTANT : Parcours postfixe (enfants avant parent)
    free_tree(root->left);   // Libérer sous-arbre gauche
    free_tree(root->right);  // Libérer sous-arbre droit

    // Si la donnée est allouée dynamiquement
    if (root->data != NULL) {
        free(root->data);
    }

    free(root);  // Libérer le nœud lui-même
}
```

**Pourquoi postfixe ?**

```c
// ❌ MAUVAIS : Préfixe
void free_tree_BAD(TreeNode* root) {
    free(root);              // Libère le nœud
    free_tree(root->left);   // ⚠️ Accès à mémoire libérée !
    free_tree(root->right);
}

// ✅ BON : Postfixe
void free_tree_GOOD(TreeNode* root) {
    free_tree(root->left);   // Libère les enfants d'abord
    free_tree(root->right);
    free(root);              // Puis le parent
}
```

### Pile et File

#### Pile avec tableau

```c
void free_stack_array(Stack* stack) {
    // Rien à faire si implémentation avec tableau statique
    // Le tableau est sur la stack, libération automatique
}
```

#### Pile avec liste chaînée

```c
void free_stack_list(StackList* stack) {
    while (!is_empty(stack)) {
        pop(stack);  // pop() fait déjà le free()
    }
}

// Ou directement
void free_stack_list_direct(StackList* stack) {
    Node* current = stack->top;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    stack->top = NULL;
}
```

---

## Patterns de gestion mémoire

### Pattern 1 : Initialisation et nettoyage

```c
typedef struct Database {
    HashTable* users;
    TreeNode* index;
    List* cache;
} Database;

// Initialisation
Database* db_create() {
    Database* db = (Database*)malloc(sizeof(Database));
    if (db == NULL) {
        return NULL;
    }

    db->users = create_hash_table();
    db->index = NULL;  // Arbre vide
    db->cache = create_list();

    return db;
}

// Nettoyage
void db_destroy(Database* db) {
    if (db == NULL) {
        return;
    }

    // Libérer chaque composant dans l'ordre inverse
    free_list(db->cache);
    free_tree(db->index);
    free_hash_table(db->users);

    free(db);
}
```

### Pattern 2 : Ownership (propriété)

**Principe** : Déterminer clairement qui est responsable de libérer la mémoire.

```c
// Option A : La fonction prend ownership
void list_add_takes_ownership(List* list, char* string) {
    // La liste devient propriétaire de 'string'
    // Elle devra faire free(string) plus tard
    Node* node = create_node_no_copy(string);
    add_node(list, node);
}

// Utilisation
char* str = strdup("Hello");
list_add_takes_ownership(&list, str);
// Ne PAS faire free(str) ici !

// Option B : La fonction fait une copie
void list_add_makes_copy(List* list, const char* string) {
    // La liste fait une copie de 'string'
    char* copy = strdup(string);
    Node* node = create_node_no_copy(copy);
    add_node(list, node);
}

// Utilisation
char* str = strdup("Hello");
list_add_makes_copy(&list, str);
free(str);  // On peut libérer, la liste a sa propre copie
```

**Convention** : Documenter clairement l'ownership dans les commentaires.

```c
/**
 * Ajoute une chaîne à la liste.
 * @param list La liste destination
 * @param string Chaîne à ajouter (la liste prend ownership)
 * @note L'appelant ne doit PAS libérer 'string' après l'appel
 */
void list_add_takes_ownership(List* list, char* string);
```

### Pattern 3 : Compteur de références

```c
typedef struct RefCounted {
    int ref_count;
    void* data;
} RefCounted;

RefCounted* rc_create(void* data) {
    RefCounted* rc = malloc(sizeof(RefCounted));
    rc->ref_count = 1;
    rc->data = data;
    return rc;
}

void rc_retain(RefCounted* rc) {
    if (rc != NULL) {
        rc->ref_count++;
    }
}

void rc_release(RefCounted* rc) {
    if (rc == NULL) {
        return;
    }

    rc->ref_count--;

    if (rc->ref_count == 0) {
        free(rc->data);
        free(rc);
    }
}
```

### Pattern 4 : RAII-like avec cleanup attribute (GNU)

```c
// Extension GCC : libération automatique
__attribute__((cleanup(cleanup_file))) FILE* f = fopen("file.txt", "r");
// À la sortie du scope, cleanup_file(f) est appelé automatiquement

void cleanup_file(FILE** fp) {
    if (*fp != NULL) {
        fclose(*fp);
    }
}

// Même principe pour malloc
__attribute__((cleanup(cleanup_ptr))) char* str = malloc(100);

void cleanup_ptr(void* ptr) {
    void** p = ptr;
    free(*p);
}
```

---

## Outils de détection

### Valgrind

**Le meilleur outil** pour détecter les problèmes de mémoire.

#### Installation

```bash
# Ubuntu/Debian
sudo apt-get install valgrind

# Fedora
sudo dnf install valgrind
```

#### Utilisation de base

```bash
# Compiler avec symboles de débogage
gcc -g -Wall program.c -o program

# Exécuter avec Valgrind
valgrind ./program

# Analyse détaillée des fuites
valgrind --leak-check=full ./program

# Avec toutes les options utiles
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program
```

#### Lecture des résultats

**Exemple de fuite** :

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 400 bytes in 1 blocks
==12345==   total heap usage: 2 allocs, 1 frees, 800 bytes allocated
==12345==
==12345== 400 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==12345==    by 0x400567: create_list (list.c:23)
==12345==    by 0x400623: main (main.c:10)
```

**Interprétation** :
- "definitely lost" → Fuite confirmée
- "400 bytes" → Taille de la fuite
- Trace de l'allocation (où le malloc a été fait)

**Exemple de use after free** :

```
==12345== Invalid write of size 4
==12345==    at 0x400567: main (main.c:15)
==12345==  Address 0x52050a0 is 0 bytes inside a block of size 4 free'd
==12345==    at 0x4C2BDEC: free (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==12345==    by 0x400554: main (main.c:12)
```

### AddressSanitizer (ASan)

Alternative rapide à Valgrind, intégré au compilateur.

```bash
# Compiler avec AddressSanitizer
gcc -g -fsanitize=address program.c -o program

# Exécuter normalement
./program
```

**Avantages** :
- Plus rapide que Valgrind
- Détection en temps réel
- Messages d'erreur clairs

**Exemple de sortie** :

```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x6020000000d0
READ of size 4 at 0x6020000000d0 thread T0
    #0 0x400567 in main main.c:15
    #1 0x7f8b4c92c082 in __libc_start_main
```

### Static Analysis : cppcheck

```bash
# Installation
sudo apt-get install cppcheck

# Analyse
cppcheck --enable=all program.c
```

---

## Bonnes pratiques

### 1. Toujours vérifier malloc/calloc/realloc

```c
✅ BON
int* ptr = (int*)malloc(sizeof(int) * n);
if (ptr == NULL) {
    fprintf(stderr, "Erreur allocation\n");
    return -1;
}
```

### 2. Mettre à NULL après free

```c
✅ BON
free(ptr);
ptr = NULL;  // Évite les double free et use after free
```

### 3. Libérer dans l'ordre inverse de l'allocation

```c
✅ BON
// Allocation
Database* db = malloc(...);
db->users = malloc(...);
db->cache = malloc(...);

// Libération (ordre inverse)
free(db->cache);
free(db->users);
free(db);
```

### 4. Utiliser sizeof avec le type, pas la taille

```c
❌ MAUVAIS
int* ptr = (int*)malloc(4 * n);  // Suppose sizeof(int) = 4

✅ BON
int* ptr = (int*)malloc(sizeof(int) * n);  // Portable
```

### 5. Ne jamais retourner l'adresse d'une variable locale

```c
❌ MAUVAIS
int* function() {
    int x = 10;
    return &x;  // Dangling pointer !
}

✅ BON
int* function() {
    int* x = malloc(sizeof(int));
    *x = 10;
    return x;  // OK
}
```

### 6. Libérer dans les cas d'erreur

```c
✅ BON
Node* create_complex_node() {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }

    node->data = malloc(sizeof(Data));
    if (node->data == NULL) {
        free(node);  // ✅ Libérer ce qui a été alloué
        return NULL;
    }

    node->name = strdup("example");
    if (node->name == NULL) {
        free(node->data);  // ✅ Libérer les deux allocations
        free(node);
        return NULL;
    }

    return node;
}
```

### 7. Documenter l'ownership

```c
/**
 * Crée un nouveau nœud.
 * @return Un nouveau nœud (l'appelant doit faire free())
 */
Node* create_node(int value);

/**
 * Ajoute un nœud à la liste.
 * @param node Nœud à ajouter (la liste prend ownership)
 */
void list_add(List* list, Node* node);
```

### 8. Utiliser des wrappers pour malloc/free

```c
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Erreur : allocation de %zu bytes échouée\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void safe_free(void** ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;  // Automatiquement mis à NULL
    }
}

// Utilisation
int* array = safe_malloc(sizeof(int) * 100);
safe_free((void**)&array);  // array devient NULL automatiquement
```

---

## Checklist de débogage mémoire

Quand vous avez un problème de mémoire :

### 1. Compilation avec flags de débogage

```bash
gcc -g -Wall -Wextra -fsanitize=address program.c -o program
```

### 2. Vérification systématique

- [ ] Chaque `malloc/calloc` est vérifié (≠ NULL)
- [ ] Chaque allocation a un `free` correspondant
- [ ] Pas de `free` multiple sur le même pointeur
- [ ] Pas d'utilisation après `free`
- [ ] Les pointeurs sont mis à NULL après `free`
- [ ] Les structures imbriquées sont libérées dans le bon ordre

### 3. Exécution des outils

```bash
# AddressSanitizer (rapide)
./program

# Valgrind (complet)
valgrind --leak-check=full --show-leak-kinds=all ./program

# Static analysis
cppcheck --enable=all program.c
```

### 4. Lecture des sorties

- Chercher "definitely lost" (fuites confirmées)
- Chercher "invalid read/write" (accès illégaux)
- Noter les numéros de lignes dans le code source

---

## Exemples complets

### Exemple 1 : Gestion correcte d'une liste

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* data;
    struct Node* next;
} Node;

Node* create_node(const char* data) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }

    node->data = strdup(data);  // Copie la chaîne
    if (node->data == NULL) {
        free(node);
        return NULL;
    }

    node->next = NULL;
    return node;
}

void free_node(Node* node) {
    if (node != NULL) {
        free(node->data);  // Libérer la chaîne d'abord
        free(node);        // Puis le nœud
    }
}

void free_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free_node(current);
        current = next;
    }
}

int main(void) {
    // Construction
    Node* head = create_node("Premier");
    head->next = create_node("Deuxième");
    head->next->next = create_node("Troisième");

    // Utilisation
    Node* current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }

    // Libération
    free_list(head);

    return 0;
}
```

**Test avec Valgrind** :
```bash
gcc -g program.c -o program
valgrind --leak-check=full ./program
```

**Résultat attendu** :
```
All heap blocks were freed -- no leaks are possible
```

### Exemple 2 : Table de hachage avec libération correcte

```c
typedef struct HashNode {
    char* key;
    int value;
    struct HashNode* next;
} HashNode;

typedef struct HashTable {
    HashNode* buckets[10];
} HashTable;

void init_hash_table(HashTable* table) {
    for (int i = 0; i < 10; i++) {
        table->buckets[i] = NULL;
    }
}

void insert(HashTable* table, const char* key, int value) {
    int index = hash(key) % 10;

    HashNode* node = malloc(sizeof(HashNode));
    if (node == NULL) {
        return;
    }

    node->key = strdup(key);
    if (node->key == NULL) {
        free(node);
        return;
    }

    node->value = value;
    node->next = table->buckets[index];
    table->buckets[index] = node;
}

void free_hash_table(HashTable* table) {
    for (int i = 0; i < 10; i++) {
        HashNode* current = table->buckets[i];

        while (current != NULL) {
            HashNode* next = current->next;
            free(current->key);   // Libérer la clé
            free(current);        // Libérer le nœud
            current = next;
        }

        table->buckets[i] = NULL;
    }
}

int main(void) {
    HashTable table;
    init_hash_table(&table);

    insert(&table, "Alice", 123);
    insert(&table, "Bob", 456);
    insert(&table, "Charlie", 789);

    // Utilisation...

    free_hash_table(&table);
    return 0;
}
```

---

## Récapitulatif

### Règles d'or

1. **Un malloc = un free** (ni plus, ni moins)
2. **Vérifier toujours** le retour de malloc/calloc/realloc
3. **Libérer dans l'ordre inverse** de l'allocation
4. **Mettre à NULL** après free
5. **Sauvegarder next** avant de libérer un nœud
6. **Parcours postfixe** pour libérer les arbres

### Erreurs à éviter

- ❌ Fuites mémoire (oubli de free)
- ❌ Double free
- ❌ Use after free
- ❌ Dangling pointers
- ❌ Oubli de vérification de malloc
- ❌ Mauvais ordre de libération

### Outils indispensables

- ✅ **Valgrind** : détection complète des problèmes
- ✅ **AddressSanitizer** : détection rapide en temps réel
- ✅ **Static analyzers** : cppcheck, clang-tidy
- ✅ **Flags de compilation** : `-Wall -Wextra -g`

### Commandes essentielles

```bash
# Compilation avec débogage
gcc -g -Wall -Wextra -fsanitize=address program.c -o program

# Valgrind complet
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program

# Static analysis
cppcheck --enable=all program.c
```

---

## Pour aller plus loin

### Techniques avancées

- **Smart pointers en C** : Implémentation de compteurs de références
- **Pools de mémoire** : Allocation en bloc pour performances
- **Custom allocators** : Gestionnaires mémoire spécialisés
- **Memory arenas** : Allocation/libération groupée

### Ressources

- **Livre** : "Expert C Programming" de Peter van der Linden
- **Documentation** : Valgrind User Manual
- **Article** : "Understanding glibc malloc" (Oracle)

### Projet pratique

Implémentez un détecteur de fuites mémoire simple en wrappant malloc/free :

```c
void* tracked_malloc(size_t size, const char* file, int line);
void tracked_free(void* ptr, const char* file, int line);

#define malloc(size) tracked_malloc(size, __FILE__, __LINE__)
#define free(ptr) tracked_free(ptr, __FILE__, __LINE__)
```

---

## Conclusion

La gestion de la mémoire en C est une responsabilité importante qui demande de la rigueur. Les erreurs de mémoire sont parmi les bugs les plus difficiles à déboguer, mais avec :

1. **Les bonnes pratiques** (vérifications, NULL après free, ordre de libération)
2. **Les bons outils** (Valgrind, ASan)
3. **La discipline** (toujours tester avec les outils)

Vous pouvez écrire du code C robuste et sans fuites mémoire.

**Rappelez-vous** : Un programme qui fonctionne mais qui fuit de la mémoire n'est pas un programme qui fonctionne. La gestion correcte de la mémoire fait partie intégrante de la correction d'un programme C.

---

*📚 Cette section conclut le chapitre 11 sur les structures de données dynamiques du Module 4 : Structures de Données et Modularité (Niveau Intermédiaire)*

*💡 Conseil final : Prenez l'habitude d'exécuter systématiquement Valgrind sur tous vos programmes, même ceux qui semblent fonctionner parfaitement !*

⏭️ [Organisation du code et Compilation modulaire](/12-organisation-code/README.md)
