🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.6 Choix de la structure de données

## Introduction

Choisir la **bonne structure de données** est l'une des décisions les plus importantes en programmation. Une mauvaise structure peut transformer un algorithme rapide en cauchemar de performance, tandis qu'une structure bien choisie peut simplifier considérablement votre code.

### L'importance du bon choix

**Analogie** : Choisir une structure de données, c'est comme choisir un outil :
- Vous n'utilisez pas un marteau pour visser
- Vous n'utilisez pas une cuillère pour couper du pain
- Chaque outil a son cas d'usage optimal

**Impact sur la performance** :

```c
// Chercher un élément dans un tableau de 1 million d'éléments
// Tableau non trié : 1 million de comparaisons
// Table de hachage : 1 seule opération ! (1 million de fois plus rapide)
```

---

## Récapitulatif des structures vues

### 1. Tableaux

**Description** : Séquence d'éléments contigus en mémoire, taille fixe.

```c
int arr[100];  // Tableau statique de 100 entiers
```

**Caractéristiques** :
- ✅ Accès direct par index en O(1)
- ✅ Cache-friendly (mémoire contiguë)
- ✅ Simple à utiliser
- ❌ Taille fixe
- ❌ Insertion/suppression coûteuses (décalages)

### 2. Listes chaînées simples

**Description** : Nœuds dispersés en mémoire, reliés par des pointeurs `next`.

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;
```

**Caractéristiques** :
- ✅ Taille dynamique
- ✅ Insertion/suppression au début en O(1)
- ✅ Pas de réallocation nécessaire
- ❌ Accès séquentiel uniquement O(n)
- ❌ Overhead mémoire (pointeurs)
- ❌ Pas cache-friendly

### 3. Listes doublement chaînées

**Description** : Nœuds avec deux pointeurs `prev` et `next`.

```c
typedef struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
} Node;
```

**Caractéristiques** :
- ✅ Navigation bidirectionnelle
- ✅ Suppression en O(1) si on a le nœud
- ✅ Insertion avant/après un nœud en O(1)
- ❌ Overhead mémoire double (2 pointeurs)
- ❌ Plus complexe à gérer

### 4. Piles (LIFO)

**Description** : Accès restreint, dernier entré = premier sorti.

```c
push(10);   // Ajouter
pop();      // Retirer le dernier
```

**Caractéristiques** :
- ✅ Opérations en O(1)
- ✅ Parfait pour l'ordre inversé
- ✅ Simple et efficace
- ❌ Accès limité (seulement le sommet)

### 5. Files (FIFO)

**Description** : Accès restreint, premier entré = premier sorti.

```c
enqueue(10);   // Ajouter à la fin
dequeue();     // Retirer du début
```

**Caractéristiques** :
- ✅ Opérations en O(1)
- ✅ Ordre naturel préservé
- ✅ Idéal pour les files d'attente
- ❌ Accès limité (début et fin seulement)

### 6. Tables de hachage

**Description** : Paires clé-valeur avec accès ultra-rapide.

```c
hash_table_insert(&table, "Alice", 12345);
hash_table_search(&table, "Alice", &value);
```

**Caractéristiques** :
- ✅ Recherche/insertion/suppression en O(1) moyen
- ✅ Très flexible (tout type de clé)
- ✅ Idéal pour les dictionnaires
- ❌ Pas d'ordre
- ❌ Overhead mémoire
- ❌ Performances dépendent de la fonction de hachage

### 7. Arbres binaires de recherche (BST)

**Description** : Structure hiérarchique avec propriété d'ordre.

```c
root = insert(root, 50);
found = search(root, 30);
```

**Caractéristiques** :
- ✅ Recherche/insertion/suppression en O(log n) si équilibré
- ✅ Maintient l'ordre trié
- ✅ Min/Max/Successeur faciles
- ❌ Performances O(n) si déséquilibré
- ❌ Plus complexe qu'un tableau ou une liste

---

## Tableau comparatif général

| Structure | Accès | Recherche | Insertion début | Insertion fin | Suppression | Ordre | Mémoire |
|-----------|-------|-----------|----------------|---------------|-------------|-------|---------|
| **Tableau** | **O(1)** | O(n) | O(n) | O(1) | O(n) | ✅ | Faible |
| **Liste simple** | O(n) | O(n) | **O(1)** | O(n)* | O(n) | ✅ | Moyenne |
| **Liste double** | O(n) | O(n) | **O(1)** | **O(1)** | **O(1)** | ✅ | Moyenne+ |
| **Pile** | O(1) sommet | N/A | **O(1)** | N/A | **O(1)** | LIFO | Faible |
| **File** | O(1) front | N/A | N/A | **O(1)** | **O(1)** | FIFO | Faible |
| **Table hash** | **O(1)** | **O(1)** | **O(1)** | **O(1)** | **O(1)** | ❌ | Moyenne+ |
| **BST (éq.)** | O(log n) | O(log n) | O(log n) | O(log n) | O(log n) | ✅ | Moyenne |

\* O(1) si on maintient un pointeur `tail`

**Légende** :
- **Gras** = Meilleure performance pour cette opération
- Ordre : Structure préserve un ordre naturel/trié
- N/A : Opération non applicable ou non pertinente

---

## Critères de choix

### 1. Opérations principales

**Question** : Quelles opérations vais-je faire le plus souvent ?

#### Si vous faites principalement de l'**accès par index**
→ **Tableau** (O(1))

```c
// Accès direct
int value = array[42];
```

#### Si vous faites principalement des **insertions/suppressions au début**
→ **Liste chaînée simple** (O(1)) ou **Pile** (LIFO)

```c
// Insertion rapide au début
insert_at_beginning(&list, 10);
```

#### Si vous faites principalement de la **recherche par clé**
→ **Table de hachage** (O(1))

```c
// Recherche ultra-rapide
hash_table_search(&table, "Alice", &value);
```

#### Si vous devez maintenir l'**ordre trié**
→ **Arbre binaire de recherche** (O(log n))

```c
// Toujours trié
inorder(root);  // Affiche dans l'ordre croissant
```

### 2. Contraintes mémoire

**Question** : Quelle est ma contrainte de mémoire ?

| Contrainte | Structure recommandée |
|------------|----------------------|
| **Très limitée** | Tableau (pas de pointeurs) |
| **Moyenne** | Liste simple, Pile, File |
| **Flexible** | Table de hachage, Liste double |

**Overhead par élément** :
- Tableau : 0 byte (juste la donnée)
- Liste simple : 1 pointeur (8 bytes sur 64-bit)
- Liste double : 2 pointeurs (16 bytes sur 64-bit)
- Table de hachage : 1-2 pointeurs + overhead du tableau

### 3. Taille des données

**Question** : Combien d'éléments vais-je stocker ?

#### Petite taille (< 100 éléments)
→ **Tableau simple** (même recherche linéaire est rapide)

```c
// Pour 50 éléments, recherche linéaire = 50 comparaisons max
// Négligeable sur un CPU moderne
```

#### Taille moyenne (100 - 10,000)
→ **Selon les opérations** (voir critère 1)

#### Grande taille (> 10,000)
→ **Structure avec O(1) ou O(log n)** impératif
- Table de hachage pour recherche par clé
- BST pour ordre trié
- Éviter les listes pour la recherche

### 4. Ordre et parcours

**Question** : Dois-je parcourir dans un ordre particulier ?

| Besoin | Structure |
|--------|-----------|
| **Ordre d'insertion** | Liste chaînée |
| **Ordre inverse d'insertion** | Pile |
| **Ordre d'arrivée (FIFO)** | File |
| **Ordre trié** | BST (parcours infixe) |
| **Pas d'ordre** | Table de hachage |

### 5. Prévisibilité de la taille

**Question** : Connais-je la taille à l'avance ?

| Situation | Structure |
|-----------|-----------|
| **Taille connue et fixe** | Tableau statique |
| **Taille inconnue** | Liste chaînée, Table hash |
| **Taille variable** | Structures dynamiques |

---

## Arbres de décision

### Arbre 1 : Choix rapide

```
┌─ Besoin d'accès par index ?
│   └─ OUI → TABLEAU
│   └─ NON → Continuer
│
├─ Besoin de recherche ultra-rapide par clé ?
│   └─ OUI → TABLE DE HACHAGE
│   └─ NON → Continuer
│
├─ Besoin d'ordre LIFO (pile d'assiettes) ?
│   └─ OUI → PILE
│   └─ NON → Continuer
│
├─ Besoin d'ordre FIFO (file d'attente) ?
│   └─ OUI → FILE
│   └─ NON → Continuer
│
├─ Besoin de maintenir l'ordre trié ?
│   └─ OUI → ARBRE BINAIRE DE RECHERCHE
│   └─ NON → Continuer
│
└─ Insertions/suppressions fréquentes + parcours ?
    └─ OUI → LISTE CHAÎNÉE (double si navigation 2 sens)
    └─ NON → TABLEAU (par défaut)
```

### Arbre 2 : Performance prioritaire

```
┌─ Quelle est l'opération la plus fréquente ?
│
├─ Accès par position → TABLEAU
│
├─ Recherche par clé
│   ├─ Sans ordre nécessaire → TABLE DE HACHAGE
│   └─ Avec ordre trié → BST
│
├─ Insertion/Suppression
│   ├─ Au début uniquement → LISTE SIMPLE ou PILE
│   ├─ Aux deux bouts → LISTE DOUBLE ou FILE
│   └─ N'importe où → LISTE DOUBLE
│
└─ Parcours dans un ordre spécifique
    ├─ LIFO → PILE
    ├─ FIFO → FILE
    └─ Trié → BST
```

---

## Cas d'usage concrets

### Scénario 1 : Gestionnaire de tâches (TODO list)

**Besoins** :
- Ajouter des tâches
- Marquer comme complétées
- Supprimer des tâches
- Parcourir toutes les tâches

**Analyse** :
- Pas besoin d'accès par index
- Insertions/suppressions fréquentes
- Parcours occasionnel
- Taille variable

**Choix** : **Liste doublement chaînée**

**Raison** : Suppression facile de n'importe quelle tâche en O(1) si on a le nœud.

```c
typedef struct Task {
    int id;
    char description[100];
    bool completed;
    struct Task* prev;
    struct Task* next;
} Task;

// Suppression simple
void delete_task(TaskList* list, Task* task) {
    if (task->prev) task->prev->next = task->next;
    if (task->next) task->next->prev = task->prev;
    free(task);
}
```

---

### Scénario 2 : Cache LRU (Least Recently Used)

**Besoins** :
- Recherche rapide par clé
- Savoir quel élément est le moins récemment utilisé
- Déplacer un élément en tête lors d'un accès

**Analyse** :
- Recherche O(1) nécessaire
- Maintien d'un ordre (récence)
- Suppression du dernier élément

**Choix** : **Table de hachage + Liste doublement chaînée**

**Raison** : Combinaison des deux structures
- Hash table : recherche rapide
- Liste double : maintien de l'ordre LRU

```c
typedef struct LRUCache {
    HashTable hash_table;      // Clé → Pointeur vers nœud
    DoublyLinkedList list;     // Ordre de récence
    int capacity;
} LRUCache;

// Accès : déplacer en tête
void cache_get(LRUCache* cache, const char* key) {
    Node* node = hash_table_find(cache->hash_table, key);
    if (node) {
        move_to_front(&cache->list, node);  // O(1) avec liste double
    }
}
```

---

### Scénario 3 : Annuaire téléphonique

**Besoins** :
- Rechercher un numéro par nom
- Ajouter/supprimer des contacts
- Afficher tous les contacts (éventuellement triés)

**Analyse** :
- Recherche par clé (nom)
- Insertions/suppressions occasionnelles
- Affichage trié serait un plus

**Choix** : **Table de hachage** (recherche) ou **BST** (recherche + ordre)

#### Option 1 : Table de hachage (si vitesse prioritaire)
```c
HashTable phonebook;
hash_table_insert(&phonebook, "Alice", 123456);
hash_table_search(&phonebook, "Alice", &number);  // O(1)
```

#### Option 2 : BST (si ordre alphabétique important)
```c
TreeNode* phonebook = NULL;
phonebook = insert(phonebook, hash("Alice"), 123456);
inorder(phonebook);  // Affiche dans l'ordre alphabétique
```

---

### Scénario 4 : Historique de navigation web

**Besoins** :
- Ajouter une page visitée
- Bouton "Précédent"
- Bouton "Suivant"
- Limite de taille

**Analyse** :
- Navigation bidirectionnelle
- LIFO pour "Précédent"
- Taille limitée

**Choix** : **Liste doublement chaînée** avec pointeur courant

```c
typedef struct BrowserHistory {
    DoublyLinkedList pages;
    Node* current_page;
} BrowserHistory;

void go_back(BrowserHistory* history) {
    if (history->current_page->prev) {
        history->current_page = history->current_page->prev;
    }
}

void go_forward(BrowserHistory* history) {
    if (history->current_page->next) {
        history->current_page = history->current_page->next;
    }
}
```

---

### Scénario 5 : File d'attente d'impression

**Besoins** :
- Ajouter des documents à imprimer
- Imprimer dans l'ordre d'arrivée
- Annuler une impression

**Analyse** :
- FIFO strict
- Insertions en fin
- Suppression au début

**Choix** : **File (Queue)**

```c
Queue print_queue;
init_queue(&print_queue);

enqueue(&print_queue, document1);  // Ajouter
enqueue(&print_queue, document2);

Document* next = dequeue(&print_queue);  // Imprimer le premier
```

---

### Scénario 6 : Vérification de parenthèses dans un éditeur

**Besoins** :
- Vérifier que chaque `(` a son `)`
- Vérifier que chaque `{` a son `}`
- Ordre d'ouverture/fermeture correct

**Analyse** :
- LIFO (dernière ouverte = première fermée)
- Pas besoin de stocker longtemps

**Choix** : **Pile**

```c
bool check_parentheses(const char* code) {
    Stack stack;
    init_stack(&stack);

    for (int i = 0; code[i]; i++) {
        if (code[i] == '(' || code[i] == '{') {
            push(&stack, code[i]);
        } else if (code[i] == ')' || code[i] == '}') {
            if (is_empty(&stack)) return false;
            char open = pop(&stack);
            if (!matches(open, code[i])) return false;
        }
    }

    return is_empty(&stack);
}
```

---

### Scénario 7 : Système de classement (leaderboard)

**Besoins** :
- Ajouter des scores
- Trouver le top N
- Mise à jour des scores
- Ordre décroissant des scores

**Analyse** :
- Maintien d'un ordre
- Insertions/suppressions avec ordre
- Trouver min/max rapide

**Choix** : **Arbre binaire de recherche** (ou heap pour top K)

```c
TreeNode* leaderboard = NULL;

void add_score(TreeNode** leaderboard, const char* player, int score) {
    *leaderboard = insert(*leaderboard, score, player);
}

void print_top_10(TreeNode* leaderboard) {
    reverse_inorder_limited(leaderboard, 10);  // 10 meilleurs
}
```

---

### Scénario 8 : Matrice sparse (peu d'éléments non-nuls)

**Besoins** :
- Stocker une matrice 1000x1000 avec 95% de zéros
- Économiser la mémoire
- Accès aux éléments non-nuls

**Analyse** :
- Tableau classique gaspille mémoire
- Besoin d'accès par coordonnées (ligne, colonne)

**Choix** : **Table de hachage** avec clé composée

```c
// Clé = "ligne,colonne"
char key[50];
sprintf(key, "%d,%d", row, col);
hash_table_insert(&sparse_matrix, key, value);

// Accès
int value;
if (hash_table_search(&sparse_matrix, "10,25", &value)) {
    // Élément trouvé
} else {
    // Implicitement zéro
    value = 0;
}
```

---

## Erreurs courantes de choix

### Erreur 1 : Utiliser une liste pour de la recherche fréquente

❌ **Mauvais** :
```c
// Rechercher dans une liste de 10,000 éléments
Node* found = search_list(list, target);  // O(n) = 10,000 comparaisons !
```

✅ **Bon** :
```c
// Utiliser une table de hachage
int value;
bool found = hash_table_search(&table, key, &value);  // O(1)
```

**Leçon** : Liste ≠ recherche rapide. Utiliser hash table ou BST.

---

### Erreur 2 : Utiliser un tableau pour des insertions/suppressions fréquentes

❌ **Mauvais** :
```c
// Insérer au début d'un tableau
void insert_beginning(int arr[], int* size, int value) {
    // Décaler tous les éléments → O(n)
    for (int i = *size; i > 0; i--) {
        arr[i] = arr[i-1];
    }
    arr[0] = value;
    (*size)++;
}
```

✅ **Bon** :
```c
// Utiliser une liste chaînée
insert_at_beginning(&list, value);  // O(1)
```

**Leçon** : Tableau ≠ insertions/suppressions efficaces. Utiliser liste.

---

### Erreur 3 : Utiliser une table de hachage quand l'ordre est important

❌ **Mauvais** :
```c
// Afficher les éléments dans l'ordre trié
print_sorted(hash_table);  // ⚠️ Pas d'ordre dans une hash table !
```

✅ **Bon** :
```c
// Utiliser un BST
inorder(bst);  // Affiche dans l'ordre trié
```

**Leçon** : Hash table ≠ ordre. Utiliser BST ou liste.

---

### Erreur 4 : Sur-optimiser prématurément

❌ **Mauvais** :
```c
// Utiliser une structure complexe pour 10 éléments
TreeNode* contacts = NULL;  // Overkill pour 10 contacts
```

✅ **Bon** :
```c
// Un simple tableau suffit
Contact contacts[10];
// Recherche linéaire = 10 comparaisons max (négligeable)
```

**Leçon** : "Premature optimization is the root of all evil" - Donald Knuth

---

## Guide de décision rapide

### Je dois faire quoi principalement ?

| Tâche principale | Structure recommandée |
|------------------|----------------------|
| **Accéder par index** | Tableau |
| **Rechercher par clé** | Table de hachage |
| **Maintenir ordre trié** | BST |
| **Insérer/supprimer au début** | Liste simple / Pile |
| **Insérer/supprimer aux 2 bouts** | Liste double / File |
| **Parcourir dans l'ordre d'insertion** | Liste |
| **Dernier entré, premier sorti** | Pile |
| **Premier entré, premier sorti** | File |

### Quelle est ma priorité ?

| Priorité | Structure recommandée |
|----------|----------------------|
| **Vitesse absolue** | Table de hachage |
| **Mémoire minimale** | Tableau |
| **Simplicité** | Tableau ou Liste simple |
| **Flexibilité** | Liste double |
| **Ordre + Recherche** | BST |

---

## Combinaisons de structures

Parfois, la meilleure solution est de **combiner** plusieurs structures !

### Exemple 1 : Cache LRU

**Combinaison** : Hash Table + Liste doublement chaînée

```c
typedef struct LRUCache {
    HashTable table;     // Clé → Nœud (O(1) recherche)
    DoublyList list;     // Ordre LRU (O(1) déplacement)
} LRUCache;
```

**Pourquoi** :
- Hash table : recherche rapide
- Liste double : maintien de l'ordre

### Exemple 2 : Graphe

**Combinaison** : Tableau + Listes chaînées

```c
typedef struct Graph {
    Node* adjacency_lists[MAX_VERTICES];  // Tableau de listes
    int num_vertices;
} Graph;
```

**Pourquoi** :
- Tableau : accès direct aux sommets
- Listes : voisins de chaque sommet

### Exemple 3 : Index de base de données

**Combinaison** : BST + Pointeurs vers données

```c
typedef struct Index {
    TreeNode* tree;      // BST pour recherche triée
    Record* data;        // Tableau des enregistrements réels
} Index;
```

**Pourquoi** :
- BST : recherche O(log n) avec ordre
- Tableau : stockage compact des données

---

## Checklist de décision

Avant de choisir une structure, posez-vous ces questions :

### 1. Opérations
- [ ] Quelle opération sera la plus fréquente ?
- [ ] Ai-je besoin d'accès aléatoire ou séquentiel ?
- [ ] Les insertions/suppressions sont-elles fréquentes ?

### 2. Données
- [ ] Quelle est la taille approximative des données ?
- [ ] La taille est-elle connue à l'avance ?
- [ ] Les données évoluent-elles beaucoup ?

### 3. Contraintes
- [ ] Ai-je des contraintes mémoire strictes ?
- [ ] La vitesse est-elle critique ?
- [ ] La simplicité d'implémentation est-elle importante ?

### 4. Ordre
- [ ] Dois-je maintenir un ordre particulier ?
- [ ] L'ordre d'insertion doit-il être préservé ?
- [ ] Ai-je besoin d'un tri ?

### 5. Cas particuliers
- [ ] Ai-je besoin de LIFO ou FIFO ?
- [ ] Dois-je rechercher par clé ?
- [ ] Navigation bidirectionnelle nécessaire ?

---

## Évolution et refactoring

**Important** : Vous pouvez commencer simple et évoluer !

### Approche progressive

**Phase 1 : Prototype**
```c
// Commencer simple
Contact contacts[10];  // Tableau statique
```

**Phase 2 : Croissance**
```c
// Passer à une structure dynamique
DoublyLinkedList contacts;  // Pour plus de flexibilité
```

**Phase 3 : Optimisation**
```c
// Optimiser si nécessaire
HashTable contacts;  // Pour recherche rapide
```

**Règle d'or** : Ne pas optimiser avant d'avoir un problème de performance mesuré.

---

## Cas particuliers et exceptions

### Petits datasets (< 100 éléments)

Pour de très petits ensembles, **un tableau simple** est souvent le meilleur choix, même pour de la recherche :
- Cache CPU très efficace
- Overhead des structures complexes non justifié
- Code plus simple

```c
// Pour 20 contacts, recherche linéaire est OK
Contact contacts[20];
for (int i = 0; i < size; i++) {
    if (strcmp(contacts[i].name, "Alice") == 0) {
        // Trouvé en quelques cycles CPU
    }
}
```

### Données majoritairement en lecture

Si vous lisez beaucoup mais modifiez rarement :
- **Tableau trié** + recherche binaire peut être optimal
- O(log n) pour la recherche
- Tri une fois au début

```c
// Trier une fois
qsort(array, size, sizeof(int), compare);

// Recherche binaire ensuite
int* found = bsearch(&key, array, size, sizeof(int), compare);
```

---

## Récapitulatif

### Règles générales

1. **Commencer simple** : Tableau ou liste suffit souvent
2. **Mesurer avant d'optimiser** : Profiler le code réel
3. **Privilégier la clarté** : Code maintenable > micro-optimisations
4. **Choisir selon le cas d'usage** : Pas de solution universelle

### Choix rapides

| Besoin | Structure |
|--------|-----------|
| Accès index | **Tableau** |
| Recherche clé | **Table hash** |
| Ordre trié | **BST** |
| LIFO | **Pile** |
| FIFO | **File** |
| Insertion début | **Liste simple** |
| Navigation 2 sens | **Liste double** |

### Ne jamais oublier

- ✅ **Performance** : Analyser les opérations critiques
- ✅ **Mémoire** : Considérer l'overhead
- ✅ **Simplicité** : Code facile à maintenir
- ✅ **Évolutivité** : Peut-on changer facilement ?

- ❌ **Sur-optimisation** : KISS (Keep It Simple, Stupid)
- ❌ **Mauvais choix initial** : Bien analyser avant de coder
- ❌ **Ignorer le profiling** : Mesurer, ne pas deviner

---

## Pour aller plus loin

### Ressources complémentaires

- **Livre** : "Introduction to Algorithms" (CLRS) - Analyse de complexité
- **Site** : [Big-O Cheat Sheet](https://www.bigocheatsheet.com/) - Comparaison visuelle
- **Pratique** : Implémenter chaque structure au moins une fois

### Structures avancées à explorer

Une fois les bases maîtrisées, explorez :
- **AVL / Red-Black Trees** : BST auto-équilibrés
- **Heaps** : Files de priorité
- **Tries** : Arbres pour chaînes de caractères
- **Bloom Filters** : Test d'appartenance probabiliste
- **Skip Lists** : Alternative aux arbres équilibrés

### Exercice mental

Pour chaque projet, demandez-vous :
> "Pourquoi est-ce que j'utilise cette structure et pas une autre ?"

Si vous ne pouvez pas répondre clairement, réfléchissez à votre choix !

---

## Conclusion

Choisir la bonne structure de données est un art qui s'apprend avec l'expérience. Les points clés à retenir :

1. **Analysez vos besoins** avant de coder
2. **Connaissez les forces et faiblesses** de chaque structure
3. **Commencez simple** et optimisez si nécessaire
4. **Mesurez les performances** réelles
5. **N'ayez pas peur de changer** si le choix initial était mauvais

Avec la pratique, le choix deviendra intuitif. En cas de doute, privilégiez la **simplicité** et l'**expressivité** du code. Un code clair avec une structure "pas optimale" vaut mieux qu'un code illisible avec la "structure parfaite".

**Rappelez-vous** : Knuth disait "Premature optimization is the root of all evil", mais il complétait aussi par "Yet we should not pass up our opportunities in that critical 3%". Trouvez le bon équilibre !

---

*💡 Conseil final : Implémentez un petit projet utilisant au moins 3 structures différentes pour comprendre leurs interactions !*

⏭️ [Gestion mémoire des structures dynamiques](/11-structures-dynamiques/07-gestion-memoire.md)
