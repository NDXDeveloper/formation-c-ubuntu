🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 11.4 Tables de hachage

## Introduction

Une **table de hachage** (*hash table* ou *hash map* en anglais) est une structure de données qui permet d'associer des **clés** à des **valeurs** et d'y accéder extrêmement rapidement. C'est l'une des structures de données les plus utilisées en informatique moderne.

### Analogie : Un annuaire téléphonique intelligent

Imaginez un annuaire classique :
- **Recherche séquentielle** : Parcourir page par page jusqu'à trouver "Martin" → Lent (O(n))
- **Recherche par index** : Aller directement à la section "M" → Plus rapide

Une table de hachage fait encore mieux : elle calcule **instantanément** où chercher !

**Exemple concret** :
- Clé : `"Alice"` → Valeur : `06 12 34 56 78`
- Clé : `"Bob"` → Valeur : `06 98 76 54 32`

La table de hachage calcule où stocker ces informations pour les retrouver en **O(1)** (temps constant) !

---

## Concept fondamental

### Le principe de base

Une table de hachage utilise une **fonction de hachage** pour transformer une clé en un **index** dans un tableau.

```
Clé → [Fonction de hachage] → Index → Valeur stockée
```

**Exemple simplifié** :

```c
Clé : "Alice"
Fonction de hachage : somme des codes ASCII % taille_tableau
Calcul : (65 + 108 + 105 + 99 + 101) % 10 = 478 % 10 = 8
Index : 8
Action : Stocker "Alice" → "06 12 34 56 78" à l'index 8
```

### Représentation visuelle

```
Table de hachage (taille = 10)

Index   Contenu
  0     [ vide ]
  1     [ vide ]
  2     ["Bob" → "06 98 76 54 32"]
  3     [ vide ]
  4     [ vide ]
  5     ["Charlie" → "06 11 22 33 44"]
  6     [ vide ]
  7     [ vide ]
  8     ["Alice" → "06 12 34 56 78"]
  9     [ vide ]
```

---

## Fonctions de hachage

Une **fonction de hachage** prend une clé et retourne un index dans le tableau.

### Propriétés d'une bonne fonction de hachage

1. **Déterministe** : Même clé → toujours le même index
2. **Uniforme** : Distribue les clés de manière équitable
3. **Rapide** : Calcul en O(1)
4. **Minimise les collisions** : Différentes clés → différents index (idéalement)

### Fonction de hachage simple pour entiers

```c
unsigned int hash_int(int key, int table_size) {
    return abs(key) % table_size;
}
```

**Exemple** :
```c
hash_int(42, 10) = 42 % 10 = 2     // Index : 2
hash_int(157, 10) = 157 % 10 = 7   // Index : 7
```

### Fonction de hachage pour chaînes de caractères

#### Méthode 1 : Somme des caractères (simple mais médiocre)

```c
unsigned int hash_string_simple(const char* key, int table_size) {
    unsigned int hash = 0;

    for (int i = 0; key[i] != '\0'; i++) {
        hash += key[i];  // Somme des codes ASCII
    }

    return hash % table_size;
}
```

**Problème** : "abc" et "bac" donnent le même hash (collision) !

#### Méthode 2 : djb2 (très populaire)

```c
unsigned int hash_djb2(const char* key, int table_size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % table_size;
}
```

**Avantage** : Distribution très uniforme, peu de collisions.

#### Méthode 3 : Polynomiale (académique)

```c
unsigned int hash_polynomial(const char* key, int table_size) {
    unsigned int hash = 0;
    const int PRIME = 31;  // Nombre premier

    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * PRIME + key[i];
    }

    return hash % table_size;
}
```

**Principe** : Chaque caractère contribue de manière pondérée par sa position.

---

## Le problème des collisions

### Qu'est-ce qu'une collision ?

Une **collision** se produit quand deux clés différentes ont le même hash.

```
hash("Alice") = 8
hash("Zoe") = 8  ← Collision !
```

**Inévitable** : Avec un espace de clés infini et un tableau fini, les collisions sont mathématiquement garanties (principe du pigeonnier).

### Solutions principales

Il existe deux grandes familles de solutions :

1. **Chaînage** (*chaining*) : Utiliser des listes chaînées
2. **Adressage ouvert** (*open addressing*) : Chercher une autre case

---

## Solution 1 : Chaînage (Chaining)

### Principe

Chaque case du tableau contient une **liste chaînée** de tous les éléments ayant le même hash.

### Représentation visuelle

```
Table de hachage avec chaînage

Index   Liste chaînée
  0     NULL
  1     NULL
  2     ["Bob"|•] → ["David"|•] → NULL  (collision : même index 2)
  3     NULL
  4     NULL
  5     ["Charlie"|•] → NULL
  6     NULL
  7     NULL
  8     ["Alice"|•] → ["Zoe"|•] → NULL  (collision : même index 8)
  9     NULL
```

### Structure de données

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 10

// Nœud pour la liste chaînée
typedef struct HashNode {
    char* key;              // Clé (chaîne de caractères)
    int value;              // Valeur associée (ici un entier)
    struct HashNode* next;  // Pointeur vers le nœud suivant
} HashNode;

// Table de hachage
typedef struct HashTable {
    HashNode* buckets[TABLE_SIZE];  // Tableau de pointeurs vers les listes
    size_t size;                     // Nombre d'éléments total
} HashTable;
```

**Vocabulaire** : Chaque case est appelée un **bucket** (seau).

### Initialisation

```c
void init_hash_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;  // Toutes les listes sont vides
    }
    table->size = 0;
}
```

### Fonction de hachage

```c
unsigned int hash(const char* key) {
    unsigned long hash_value = 5381;  // djb2
    int c;

    while ((c = *key++)) {
        hash_value = ((hash_value << 5) + hash_value) + c;
    }

    return hash_value % TABLE_SIZE;
}
```

### Insertion

```c
void hash_table_insert(HashTable* table, const char* key, int value) {
    // 1. Calculer l'index
    unsigned int index = hash(key);

    // 2. Vérifier si la clé existe déjà (mise à jour)
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Clé trouvée, mettre à jour la valeur
            current->value = value;
            return;
        }
        current = current->next;
    }

    // 3. Clé non trouvée, créer un nouveau nœud
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    new_node->key = strdup(key);  // Copier la clé
    new_node->value = value;

    // 4. Insertion au début de la liste (plus simple)
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->size++;
}
```

**Note** : `strdup()` alloue de la mémoire et copie la chaîne.

### Recherche

```c
bool hash_table_search(HashTable* table, const char* key, int* value_out) {
    // 1. Calculer l'index
    unsigned int index = hash(key);

    // 2. Parcourir la liste chaînée à cet index
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Clé trouvée !
            if (value_out != NULL) {
                *value_out = current->value;
            }
            return true;
        }
        current = current->next;
    }

    // Clé non trouvée
    return false;
}
```

**Utilisation** :
```c
int phone;
if (hash_table_search(&table, "Alice", &phone)) {
    printf("Téléphone d'Alice : %d\n", phone);
} else {
    printf("Alice non trouvée\n");
}
```

### Suppression

```c
bool hash_table_delete(HashTable* table, const char* key) {
    // 1. Calculer l'index
    unsigned int index = hash(key);

    // 2. Parcourir la liste
    HashNode* current = table->buckets[index];
    HashNode* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Nœud trouvé

            if (previous == NULL) {
                // C'est le premier nœud
                table->buckets[index] = current->next;
            } else {
                // Nœud au milieu ou à la fin
                previous->next = current->next;
            }

            // Libérer la mémoire
            free(current->key);
            free(current);
            table->size--;

            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;  // Clé non trouvée
}
```

### Affichage

```c
void hash_table_print(HashTable* table) {
    printf("=== Table de hachage ===\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Bucket %d: ", i);

        HashNode* current = table->buckets[i];
        if (current == NULL) {
            printf("[vide]\n");
        } else {
            while (current != NULL) {
                printf("[\"%s\" → %d]", current->key, current->value);
                if (current->next != NULL) {
                    printf(" → ");
                }
                current = current->next;
            }
            printf("\n");
        }
    }

    printf("Taille totale : %zu éléments\n", table->size);
}
```

### Libération de la mémoire

```c
void hash_table_free(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];

        while (current != NULL) {
            HashNode* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }

        table->buckets[i] = NULL;
    }

    table->size = 0;
}
```

### Exemple complet

```c
int main(void) {
    HashTable table;
    init_hash_table(&table);

    // Insertion
    hash_table_insert(&table, "Alice", 123456);
    hash_table_insert(&table, "Bob", 789012);
    hash_table_insert(&table, "Charlie", 345678);
    hash_table_insert(&table, "Zoe", 901234);  // Pourrait collisionner

    // Affichage
    hash_table_print(&table);

    // Recherche
    int value;
    if (hash_table_search(&table, "Alice", &value)) {
        printf("\nAlice trouvée : %d\n", value);
    }

    if (!hash_table_search(&table, "David", &value)) {
        printf("David non trouvé\n");
    }

    // Mise à jour
    hash_table_insert(&table, "Alice", 999999);
    printf("\nAprès mise à jour :\n");
    hash_table_search(&table, "Alice", &value);
    printf("Alice : %d\n", value);

    // Suppression
    hash_table_delete(&table, "Bob");
    printf("\nAprès suppression de Bob :\n");
    hash_table_print(&table);

    // Libération
    hash_table_free(&table);

    return 0;
}
```

---

## Solution 2 : Adressage ouvert (Open Addressing)

### Principe

Au lieu d'utiliser des listes chaînées, on stocke tous les éléments **directement dans le tableau**. En cas de collision, on cherche une autre case libre selon une **stratégie de sondage**.

### Méthode 1 : Sondage linéaire (Linear Probing)

En cas de collision à l'index `i`, on essaie `i+1`, puis `i+2`, etc.

#### Représentation visuelle

```
Insertion de "Alice" (hash = 8), "Bob" (hash = 2), "Zoe" (hash = 8)

Index   Avant               Après Zoe (collision en 8)
  0     [ vide ]            [ vide ]
  1     [ vide ]            [ vide ]
  2     ["Bob"]             ["Bob"]
  3     [ vide ]            [ vide ]
  4     [ vide ]            [ vide ]
  5     [ vide ]            [ vide ]
  6     [ vide ]            [ vide ]
  7     [ vide ]            [ vide ]
  8     ["Alice"]           ["Alice"]
  9     [ vide ]            ["Zoe"]  ← Case suivante disponible
```

#### Structure de données

```c
#define TABLE_SIZE 10

typedef struct HashEntry {
    char* key;
    int value;
    bool is_occupied;  // Indicateur d'occupation
    bool is_deleted;   // Marqueur de suppression (tombstone)
} HashEntry;

typedef struct HashTableOpen {
    HashEntry entries[TABLE_SIZE];
    size_t size;
} HashTableOpen;
```

**Note** : On a besoin de `is_deleted` pour gérer les suppressions correctement.

#### Initialisation

```c
void init_hash_table_open(HashTableOpen* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i].key = NULL;
        table->entries[i].is_occupied = false;
        table->entries[i].is_deleted = false;
    }
    table->size = 0;
}
```

#### Insertion avec sondage linéaire

```c
bool hash_table_open_insert(HashTableOpen* table, const char* key, int value) {
    if (table->size >= TABLE_SIZE) {
        fprintf(stderr, "Erreur : table pleine\n");
        return false;
    }

    unsigned int index = hash(key);
    unsigned int original_index = index;

    // Sondage linéaire
    while (table->entries[index].is_occupied &&
           !table->entries[index].is_deleted &&
           strcmp(table->entries[index].key, key) != 0) {

        index = (index + 1) % TABLE_SIZE;  // Passer à la case suivante

        // Si on revient au point de départ, la table est pleine
        if (index == original_index) {
            fprintf(stderr, "Erreur : table pleine (boucle complète)\n");
            return false;
        }
    }

    // Cas 1 : Mise à jour d'une clé existante
    if (table->entries[index].is_occupied &&
        strcmp(table->entries[index].key, key) == 0) {
        table->entries[index].value = value;
        return true;
    }

    // Cas 2 : Insertion dans une case libre ou supprimée
    if (table->entries[index].key != NULL) {
        free(table->entries[index].key);
    }

    table->entries[index].key = strdup(key);
    table->entries[index].value = value;
    table->entries[index].is_occupied = true;
    table->entries[index].is_deleted = false;

    table->size++;
    return true;
}
```

#### Recherche avec sondage linéaire

```c
bool hash_table_open_search(HashTableOpen* table, const char* key, int* value_out) {
    unsigned int index = hash(key);
    unsigned int original_index = index;

    while (table->entries[index].is_occupied ||
           table->entries[index].is_deleted) {

        if (table->entries[index].is_occupied &&
            !table->entries[index].is_deleted &&
            strcmp(table->entries[index].key, key) == 0) {
            // Clé trouvée
            if (value_out != NULL) {
                *value_out = table->entries[index].value;
            }
            return true;
        }

        index = (index + 1) % TABLE_SIZE;

        // Éviter une boucle infinie
        if (index == original_index) {
            break;
        }
    }

    return false;  // Clé non trouvée
}
```

#### Suppression (lazy deletion)

```c
bool hash_table_open_delete(HashTableOpen* table, const char* key) {
    unsigned int index = hash(key);
    unsigned int original_index = index;

    while (table->entries[index].is_occupied ||
           table->entries[index].is_deleted) {

        if (table->entries[index].is_occupied &&
            strcmp(table->entries[index].key, key) == 0) {
            // Marqueur de suppression (tombstone)
            table->entries[index].is_deleted = true;
            table->size--;
            return true;
        }

        index = (index + 1) % TABLE_SIZE;

        if (index == original_index) {
            break;
        }
    }

    return false;
}
```

**Note importante** : On utilise un **tombstone** (marqueur de suppression) au lieu de vraiment supprimer. Sinon, la recherche pourrait s'arrêter prématurément.

### Méthode 2 : Sondage quadratique (Quadratic Probing)

Au lieu de `i+1, i+2, i+3...`, on essaie `i+1², i+2², i+3²...`

```c
unsigned int probe_index = (index + attempt * attempt) % TABLE_SIZE;
```

**Avantage** : Réduit le clustering (groupement d'éléments).

### Méthode 3 : Double hachage (Double Hashing)

On utilise une **seconde fonction de hachage** pour calculer le pas.

```c
unsigned int hash2(const char* key) {
    // Doit retourner un nombre non nul
    return 1 + (hash(key) % (TABLE_SIZE - 1));
}

// Dans la boucle de sondage
index = (original_index + attempt * hash2(key)) % TABLE_SIZE;
```

**Avantage** : Meilleure distribution, moins de collisions secondaires.

---

## Comparaison des méthodes

| Aspect | Chaînage | Adressage ouvert |
|--------|----------|------------------|
| **Structure** | Tableau + Listes | Tableau uniquement |
| **Mémoire** | Overhead des pointeurs | Compacte |
| **Performance worst-case** | O(n) si tout dans 1 bucket | O(n) si beaucoup de collisions |
| **Taille max** | Illimitée (si mémoire) | Limitée par TABLE_SIZE |
| **Cache CPU** | Moins bon (listes dispersées) | Meilleur (données contiguës) |
| **Suppression** | Simple | Complexe (tombstones) |
| **Facteur de charge** | Peut dépasser 1 | Doit rester < 1 |
| **Complexité implémentation** | Moyenne | Plus complexe |

**Facteur de charge** : λ = nombre d'éléments / taille du tableau

---

## Redimensionnement dynamique

Quand la table devient trop pleine (λ > 0.7), les performances se dégradent. Solution : **redimensionner** !

### Stratégie de redimensionnement

```c
void hash_table_resize(HashTable* table, int new_size) {
    // 1. Créer une nouvelle table plus grande
    HashNode** new_buckets = (HashNode**)calloc(new_size, sizeof(HashNode*));
    if (new_buckets == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire\n");
        return;
    }

    int old_size = TABLE_SIZE;
    HashNode** old_buckets = table->buckets;

    // 2. Mettre à jour la taille
    // (dans une vraie implémentation, TABLE_SIZE serait un champ de la structure)

    // 3. Ré-hasher tous les éléments
    for (int i = 0; i < old_size; i++) {
        HashNode* current = old_buckets[i];

        while (current != NULL) {
            HashNode* next = current->next;

            // Recalculer l'index avec la nouvelle taille
            unsigned int new_index = hash(current->key) % new_size;

            // Insertion dans la nouvelle table
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    // 4. Libérer l'ancien tableau
    free(old_buckets);

    // Note : dans une vraie implémentation, on mettrait à jour table->buckets
}
```

**Quand redimensionner ?**
- Agrandir : λ > 0.7 (typiquement doubler la taille)
- Rétrécir : λ < 0.25 (diviser par deux)

**Complexité** : O(n) pour le redimensionnement, mais **amorti** sur de nombreuses opérations.

---

## Applications pratiques

### 1. Dictionnaire de mots

```c
// Compter la fréquence des mots
HashTable word_count;
init_hash_table(&word_count);

char* text = "hello world hello";
char* word = strtok(text, " ");

while (word != NULL) {
    int count = 0;
    hash_table_search(&word_count, word, &count);
    hash_table_insert(&word_count, word, count + 1);

    word = strtok(NULL, " ");
}
```

### 2. Cache LRU simplifié

```c
typedef struct Cache {
    HashTable table;
    // + structure pour gérer l'ordre LRU (liste doublement chaînée)
} Cache;

int cache_get(Cache* cache, const char* key) {
    int value;
    if (hash_table_search(&cache->table, key, &value)) {
        // Déplacer en tête (LRU)
        return value;
    }
    return -1;  // Cache miss
}
```

### 3. Détection de doublons

```c
bool has_duplicates(int arr[], int size) {
    HashTable seen;
    init_hash_table(&seen);

    for (int i = 0; i < size; i++) {
        char key[20];
        sprintf(key, "%d", arr[i]);

        if (hash_table_search(&seen, key, NULL)) {
            return true;  // Doublon trouvé
        }

        hash_table_insert(&seen, key, 1);
    }

    hash_table_free(&seen);
    return false;
}
```

### 4. Implémentation de sets (ensembles)

```c
typedef HashTable Set;

void set_add(Set* set, const char* element) {
    hash_table_insert(set, element, 1);  // La valeur n'a pas d'importance
}

bool set_contains(Set* set, const char* element) {
    return hash_table_search(set, element, NULL);
}

void set_remove(Set* set, const char* element) {
    hash_table_delete(set, element);
}
```

---

## Complexité des opérations

### Cas moyen (bonne fonction de hachage)

| Opération | Chaînage | Adressage ouvert |
|-----------|----------|------------------|
| Insertion | **O(1)** | **O(1)** |
| Recherche | **O(1)** | **O(1)** |
| Suppression | **O(1)** | **O(1)** |

### Pire cas (toutes les clés dans le même bucket)

| Opération | Chaînage | Adressage ouvert |
|-----------|----------|------------------|
| Insertion | O(n) | O(n) |
| Recherche | O(n) | O(n) |
| Suppression | O(n) | O(n) |

**Note** : Le cas moyen O(1) est garanti si :
- Bonne fonction de hachage
- Facteur de charge λ < 0.7
- Distribution uniforme des clés

---

## Comparaison avec d'autres structures

| Structure | Recherche | Insertion | Suppression | Ordre | Mémoire |
|-----------|-----------|-----------|-------------|-------|---------|
| **Tableau** | O(n) | O(1) fin / O(n) début | O(n) | ✅ Naturel | Faible |
| **Liste chaînée** | O(n) | O(1) début | O(1) si nœud connu | ✅ Insertion | Moyenne |
| **Table de hachage** | **O(1)** | **O(1)** | **O(1)** | ❌ Non ordonné | Moyenne/Haute |
| **Arbre binaire de recherche** | O(log n) | O(log n) | O(log n) | ✅ Trié | Moyenne |

**Quand utiliser une table de hachage ?**
- ✅ Besoin d'accès très rapide par clé
- ✅ Pas besoin d'ordre trié
- ✅ Recherche, insertion, suppression fréquentes
- ❌ Si besoin de parcourir dans l'ordre
- ❌ Si mémoire très limitée

---

## Erreurs courantes et pièges

### 1. Mauvaise fonction de hachage

❌ **Mauvais** :
```c
// Retourne toujours 0 !
unsigned int bad_hash(const char* key, int size) {
    return 0;  // ⚠️ Toutes les clés dans le bucket 0
}
```

✅ **Bon** :
```c
unsigned int good_hash(const char* key, int size) {
    // Utiliser djb2 ou une autre fonction éprouvée
    return hash_djb2(key, size);
}
```

### 2. Oublier de copier les clés

❌ **Mauvais** :
```c
new_node->key = key;  // ⚠️ Pointeur vers une chaîne temporaire !
```

✅ **Bon** :
```c
new_node->key = strdup(key);  // ✅ Copie indépendante
```

### 3. Fuites mémoire

❌ **Mauvais** :
```c
// Oublier de libérer les clés
free(node);
```

✅ **Bon** :
```c
free(node->key);  // ✅ Libérer la clé d'abord
free(node);
```

### 4. Ne pas gérer les collisions

❌ **Mauvais** :
```c
table[index] = value;  // ⚠️ Écrase l'ancienne valeur !
```

✅ **Bon** :
```c
// Utiliser chaînage ou adressage ouvert
insert_with_chaining(table, key, value);
```

### 5. Facteur de charge trop élevé

❌ **Mauvais** :
```c
// Continuer à insérer sans redimensionner
// λ = 0.95 → Performances catastrophiques
```

✅ **Bon** :
```c
if ((float)table->size / TABLE_SIZE > 0.7) {
    hash_table_resize(table, TABLE_SIZE * 2);
}
```

### 6. Adressage ouvert : oublier les tombstones

❌ **Mauvais** :
```c
// Suppression sans marqueur
table->entries[index].is_occupied = false;
// ⚠️ La recherche s'arrêtera prématurément !
```

✅ **Bon** :
```c
table->entries[index].is_deleted = true;  // ✅ Tombstone
```

---

## Optimisations avancées

### 1. Utiliser des nombres premiers pour la taille

```c
// Meilleure distribution avec une taille première
#define TABLE_SIZE 101  // Premier, pas 100
```

### 2. Hachage parfait (Perfect Hashing)

Si l'ensemble de clés est **connu à l'avance**, on peut construire une fonction de hachage sans collision.

```c
// Pour {"Alice", "Bob", "Charlie"}
// Trouver une fonction h telle que :
// h("Alice") = 0, h("Bob") = 1, h("Charlie") = 2
```

**Cas d'usage** : Mots-clés de langages de programmation, commandes shell.

### 3. Bloom filters (pour l'appartenance)

Structure probabiliste pour tester si un élément est **peut-être** présent ou **certainement absent**.

---

## Variantes et structures connexes

### 1. HashMap vs HashSet

**HashMap** : Stocke des paires clé-valeur
```c
hash_table_insert(&map, "Alice", 123);
```

**HashSet** : Stocke uniquement des clés (valeur = dummy)
```c
hash_set_add(&set, "Alice");  // Valeur = 1 (ignorée)
```

### 2. Multimap

Permet plusieurs valeurs pour une même clé.

```c
// "Alice" → [123, 456, 789]
multimap_insert(&mmap, "Alice", 123);
multimap_insert(&mmap, "Alice", 456);
```

### 3. Ordered Hash Table (Hybrid)

Combine table de hachage + liste doublement chaînée pour garder l'ordre d'insertion.

**Cas d'usage** : Cache LRU, JSON (préserver l'ordre des clés).

---

## Bonnes pratiques

1. **Choisir une bonne taille** : Nombre premier, pas trop petit (TABLE_SIZE ≥ 10)
2. **Utiliser une fonction de hachage éprouvée** : djb2, FNV-1a, MurmurHash
3. **Gérer le facteur de charge** : Redimensionner si λ > 0.7
4. **Libérer la mémoire** : Ne pas oublier les clés copiées avec `strdup()`
5. **Tester les collisions** : Vérifier le comportement avec des clés similaires
6. **Documenter les choix** : Fonction de hachage, méthode de collision, etc.

### Commandes pour tester

```bash
# Compilation
gcc -g -Wall -Wextra hash_table.c -o hash_table

# Vérification mémoire
valgrind --leak-check=full ./hash_table

# Avec sanitizers
gcc -g -fsanitize=address hash_table.c -o hash_table
./hash_table
```

---

## Récapitulatif

### Concept clé

Une table de hachage transforme une clé en un index via une **fonction de hachage** pour un accès **O(1)**.

```
Clé → [Hash] → Index → Valeur
```

### Opérations principales

- **insert(key, value)** : Ajouter ou mettre à jour
- **search(key)** : Récupérer la valeur
- **delete(key)** : Supprimer une entrée

### Gestion des collisions

**Chaînage** :
- Tableau de listes chaînées
- Simple à implémenter
- Pas de limite de taille

**Adressage ouvert** :
- Tout dans le tableau
- Sondage linéaire / quadratique / double hachage
- Meilleur pour le cache CPU

### Complexité

- **Cas moyen** : O(1) pour toutes les opérations ✅
- **Pire cas** : O(n) si toutes les clés collisionnent
- **Condition** : Bonne fonction de hachage + λ < 0.7

### Avantages

- ✅ Accès ultra-rapide (O(1))
- ✅ Flexible (tout type de clé)
- ✅ Implémentation de dictionnaires, sets, caches

### Inconvénients

- ❌ Pas d'ordre
- ❌ Overhead mémoire
- ❌ Performance dégradée si λ élevé
- ❌ Fonction de hachage critique

---

## Pour aller plus loin

Dans les prochaines sections, nous verrons :
- **11.5 Arbres binaires** : Structures hiérarchiques avec ordre
- **11.6 Choix de la structure de données** : Guide de décision
- **Structures avancées** : Tries, B-trees, Skip lists

**Ressources** :
- [Hash Table Wikipedia](https://en.wikipedia.org/wiki/Hash_table)
- Livre : "Introduction to Algorithms" (CLRS) - Chapitre 11
- Visualisation : [visualgo.net/en/hashtable](https://visualgo.net/en/hashtable)

**Projet suggéré** : Implémentez un mini-dictionnaire de mots avec comptage de fréquences à partir d'un fichier texte en utilisant une table de hachage !

---


⏭️ [Arbres binaires](/11-structures-dynamiques/05-arbres-binaires.md)
