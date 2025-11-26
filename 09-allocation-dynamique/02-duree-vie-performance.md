🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.2 Durée de vie et performance

## Introduction

Dans la section précédente (9.1), nous avons vu les différences fondamentales entre la Stack et le Heap. Maintenant, nous allons approfondir deux aspects cruciaux qui influencent vos décisions de conception : **la durée de vie des données** et **les performances**.

Comprendre ces concepts vous permettra de :
- Choisir la bonne stratégie d'allocation pour chaque situation
- Éviter les bugs subtils liés à la durée de vie des variables
- Optimiser les performances de vos programmes
- Anticiper les coûts cachés de l'allocation dynamique

---

## Partie 1 : Durée de vie des variables (Lifetime)

### Qu'est-ce que la durée de vie ?

La **durée de vie** (ou *lifetime*) d'une variable désigne la période pendant laquelle cette variable existe en mémoire et peut être utilisée dans votre programme.

```
┌───────────────────────────────────────────────────────────┐
│  Durée de vie = De la création à la destruction           │
│                                                           │
│  Création ─────────[Variable existe]────────► Destruction │
└───────────────────────────────────────────────────────────┘
```

---

## Durée de vie sur la Stack

### Principe de base

Les variables sur la Stack ont une durée de vie **automatique** et **limitée à leur portée** (scope).

### Règle fondamentale

> **Une variable locale Stack est détruite automatiquement dès que son bloc de code se termine.**

### Exemple détaillé

```c
#include <stdio.h>

void fonction_interne() {
    int b = 20;
    printf("Dans fonction_interne : b = %d\n", b);
    // b existe UNIQUEMENT ici
} // ← b est DÉTRUITE automatiquement ici

void fonction_externe() {
    int a = 10;
    printf("Dans fonction_externe : a = %d\n", a);

    fonction_interne();

    // Ici, b n'existe plus !
    // printf("%d", b); // ❌ ERREUR : b n'existe pas dans ce scope

    printf("Retour dans fonction_externe : a = %d\n", a);
} // ← a est DÉTRUITE automatiquement ici

int main() {
    printf("Début du programme\n");
    fonction_externe();
    // Ici, ni a ni b n'existent plus !
    printf("Fin du programme\n");
    return 0;
}
```

### Visualisation temporelle

```
Ligne de temps du programme :
════════════════════════════════════════════════════════

main() débute
│
├─ fonction_externe() appelée
│  │
│  ├─ Variable 'a' CRÉÉE (Stack)
│  │
│  ├─ fonction_interne() appelée
│  │  │
│  │  ├─ Variable 'b' CRÉÉE (Stack)
│  │  │  [a et b existent toutes les deux]
│  │  │
│  │  └─ fonction_interne() se termine
│  │     'b' est DÉTRUITE automatiquement ✗
│  │
│  │  [seule 'a' existe encore]
│  │
│  └─ fonction_externe() se termine
│     'a' est DÉTRUITE automatiquement ✗
│
└─ main() se termine
   [aucune variable locale n'existe plus]
```

### Portée et blocs

La durée de vie est liée aux **blocs de code** délimités par `{ }` :

```c
int main() {
    int x = 5;  // x existe dans tout le main()

    {
        int y = 10;  // y existe UNIQUEMENT dans ce bloc
        printf("x = %d, y = %d\n", x, y);  // ✅ Les deux accessibles
    } // ← y est DÉTRUITE ici

    printf("x = %d\n", x);  // ✅ x existe toujours
    // printf("y = %d\n", y);  // ❌ ERREUR : y n'existe plus

    return 0;
}
```

### Boucles et durée de vie

```c
for (int i = 0; i < 5; i++) {
    int temporaire = i * 2;
    printf("%d ", temporaire);
    // temporaire est DÉTRUITE à chaque itération
    // et RECRÉÉE à l'itération suivante
}
// i et temporaire n'existent plus ici
```

---

## Durée de vie sur le Heap

### Principe de base

Les variables sur le Heap ont une durée de vie **manuelle** et **contrôlée par le programmeur**.

### Règle fondamentale

> **Une variable Heap existe tant que vous ne l'avez pas explicitement libérée avec `free()`.**

### Exemple : Persistance au-delà des fonctions

```c
#include <stdio.h>
#include <stdlib.h>

// Cette fonction crée des données qui SURVIVENT à son exécution
int* creer_donnees() {
    int* ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = 42;

    printf("Dans creer_donnees : données créées à l'adresse %p\n", (void*)ptr);
    return ptr;
    // ← La fonction se termine, mais les données sur le Heap persistent !
}

int main() {
    printf("=== Début du main ===\n");

    int* mes_donnees = creer_donnees();

    printf("Dans main : les données existent toujours !\n");
    printf("Valeur : %d à l'adresse %p\n", *mes_donnees, (void*)mes_donnees);

    // Les données sont toujours valides, même après la fin de creer_donnees()
    *mes_donnees = 100;
    printf("Nouvelle valeur : %d\n", *mes_donnees);

    // ⚠️ IMPORTANT : Libération manuelle nécessaire
    free(mes_donnees);

    printf("=== Fin du main ===\n");
    return 0;
}
```

### Visualisation temporelle

```
Ligne de temps du programme :
════════════════════════════════════════════════════════

main() débute
│
├─ creer_donnees() appelée
│  │
│  ├─ malloc() alloue sur le HEAP ─────┐
│  │                                   │ [Mémoire Heap allouée]
│  └─ creer_donnees() se termine       │
│     (retourne le pointeur)           │
│                                      │
│  [La mémoire Heap EXISTE TOUJOURS] ◄─┘
│
├─ Utilisation dans main()
│  Modification de la valeur
│  [La mémoire Heap est toujours là]
│
├─ free() appelé ─────────────────┐
│                                 │
│  [Mémoire Heap LIBÉRÉE] ◄───────┘
│
└─ main() se termine
```

---

## Comparaison des durées de vie

### Tableau récapitulatif

| Aspect | Stack | Heap |
|--------|-------|------|
| **Création** | Automatique à l'entrée du bloc | Manuelle avec `malloc()` |
| **Destruction** | Automatique à la sortie du bloc | Manuelle avec `free()` |
| **Durée** | Limitée au scope | Jusqu'à `free()` ou fin du programme |
| **Contrôle** | Compilateur | Programmeur |
| **Risque d'oubli** | Aucun | Fuite mémoire si oubli de `free()` |
| **Persistance entre fonctions** | ❌ Non | ✅ Oui |

### Exemple comparatif complet

```c
#include <stdio.h>
#include <stdlib.h>

void demonstration_duree_vie() {
    // ===== STACK =====
    int stack_var = 100;  // Créée automatiquement
    printf("Variable Stack : %d\n", stack_var);
    // stack_var sera détruite à la fin de cette fonction

    // ===== HEAP =====
    int* heap_var = malloc(sizeof(int));  // Créée manuellement
    if (heap_var != NULL) {
        *heap_var = 200;
        printf("Variable Heap : %d\n", *heap_var);
    }
    // heap_var (le pointeur) sera détruit
    // MAIS la mémoire allouée sur le Heap persiste !

    // ⚠️ Si on ne fait pas free() ici, on a une FUITE MÉMOIRE
    free(heap_var);  // Libération manuelle
}

int main() {
    demonstration_duree_vie();
    // Après cette ligne :
    // - stack_var n'existe plus (Stack automatique)
    // - heap_var (pointeur) n'existe plus (Stack automatique)
    // - La mémoire Heap a été libérée avec free() ✓

    return 0;
}
```

---

## Problèmes liés à la durée de vie

### ❌ Problème 1 : Dangling Pointer (pointeur pendant)

Tenter d'utiliser une adresse Stack après la fin de sa durée de vie.

```c
int* fonction_dangereuse() {
    int x = 42;       // x est sur la Stack
    return &x;        // ❌ ERREUR : on retourne l'adresse d'une variable locale
}  // x est DÉTRUITE ici

int main() {
    int* ptr = fonction_dangereuse();
    // ptr pointe vers une zone mémoire qui n'est plus valide !
    printf("%d\n", *ptr);  // ⚠️ COMPORTEMENT INDÉFINI
    return 0;
}
```

**Pourquoi c'est dangereux ?**

```
État de la Stack :

Pendant fonction_dangereuse() :
┌──────────────┐
│  x = 42      │ ◄─── ptr pointe ici
└──────────────┘

Après fonction_dangereuse() :
┌──────────────┐
│  (données    │ ◄─── ptr pointe toujours ici, mais c'est invalide !
│   écrasées)  │      Cette zone peut être réutilisée à tout moment
└──────────────┘
```

**Solution correcte : Utiliser le Heap**

```c
int* fonction_correcte() {
    int* x = malloc(sizeof(int));  // x pointe vers le Heap
    if (x != NULL) {
        *x = 42;
    }
    return x;  // ✅ OK : la mémoire Heap persiste
}

int main() {
    int* ptr = fonction_correcte();
    if (ptr != NULL) {
        printf("%d\n", *ptr);  // ✅ Valide
        free(ptr);             // N'oubliez pas de libérer !
    }
    return 0;
}
```

### ❌ Problème 2 : Use After Free

Utiliser une mémoire Heap après l'avoir libérée.

```c
int* ptr = malloc(sizeof(int));
*ptr = 42;

free(ptr);  // Libération de la mémoire

printf("%d\n", *ptr);  // ❌ ERREUR : use after free
*ptr = 100;            // ❌ ERREUR : écriture dans mémoire libérée
```

### ❌ Problème 3 : Memory Leak (fuite mémoire)

Oublier de libérer la mémoire Heap.

```c
void fonction_avec_fuite() {
    int* data = malloc(1000 * sizeof(int));
    // ... utilisation ...

    // ❌ Oubli de free(data) !
}  // Le pointeur est détruit, mais la mémoire Heap reste allouée

int main() {
    for (int i = 0; i < 1000000; i++) {
        fonction_avec_fuite();  // ⚠️ Fuite de 4 MB à chaque itération !
    }
    // Le programme a perdu 4 GB de mémoire !
    return 0;
}
```

---

## Partie 2 : Performance

### Introduction aux performances

La performance ne se limite pas à la vitesse d'exécution. Elle englobe :
- **Temps d'allocation** : combien de temps pour obtenir de la mémoire ?
- **Temps d'accès** : combien de temps pour lire/écrire ?
- **Overhead mémoire** : combien de mémoire supplémentaire est utilisée ?
- **Impact sur le cache** : les données sont-elles bien organisées ?

---

## Performance de la Stack

### Vitesse d'allocation

L'allocation sur la Stack est **extrêmement rapide** car elle se résume à **modifier un seul registre** (le Stack Pointer).

```c
int x = 10;  // Stack : ~1 nanoseconde
```

**Pourquoi c'est si rapide ?**

```
Allocation Stack (simplifié) :

Avant :
┌──────────────┐
│  (autre var) │
└──────────────┘
     ↑
     SP (Stack Pointer)

Après (allouer 4 octets pour int) :
┌──────────────┐
│  x = 10      │ ← Nouvelle variable
├──────────────┤
│  (autre var) │
└──────────────┘
     ↑
     SP (décrémenté de 4)

C'est une simple opération : SP = SP - 4
```

### Libération instantanée

La libération est tout aussi rapide :

```c
{
    int x = 10;
    int y = 20;
    // ...
}  // Libération : SP = SP + 8 (instantané !)
```

### Localité mémoire et cache

Les variables Stack sont **contiguës** en mémoire, ce qui optimise l'utilisation du cache CPU.

```
Stack en mémoire :
┌────────────────┐ ← Adresse haute
│  Variable 4    │
├────────────────┤
│  Variable 3    │  ← Ces variables sont proches
├────────────────┤     Elles tiennent dans une ligne de cache
│  Variable 2    │  ← Le CPU les charge ensemble
├────────────────┤
│  Variable 1    │
└────────────────┘

Résultat : Accès ULTRA-RAPIDE (hit de cache)
```

### Overhead mémoire

**Zéro overhead** : Chaque variable occupe exactement sa taille.

```c
int x;      // 4 octets utilisés
char c;     // 1 octet utilisé
double d;   // 8 octets utilisés
```

---

## Performance du Heap

### Vitesse d'allocation

L'allocation sur le Heap est **beaucoup plus lente** car elle nécessite :
1. Rechercher un bloc libre de taille suffisante
2. Mettre à jour les structures internes de l'allocateur
3. Gérer la fragmentation
4. Potentiellement demander plus de mémoire à l'OS

```c
int* x = malloc(sizeof(int));  // Heap : ~100-1000 nanosecondes
```

**Pourquoi c'est plus lent ?**

```
Allocation Heap (simplifié) :

1. malloc() cherche dans la free list :
   ┌─────────┬──────────┬─────────┐
   │ Bloc A  │ Bloc B   │ Bloc C  │
   │ (libre) │ (utilisé)│ (libre) │
   └─────────┴──────────┴─────────┘

2. Trouve un bloc assez grand (Bloc A)

3. Découpe le bloc si nécessaire :
   ┌───┬─────┬──────────┬─────────┐
   │ X │reste│ Bloc B   │ Bloc C  │
   │ 4B│libre│ (utilisé)│ (libre) │
   └───┴─────┴──────────┴─────────┘

4. Met à jour les métadonnées
5. Retourne le pointeur

Beaucoup d'opérations comparé à la Stack !
```

### Overhead mémoire

L'allocateur doit stocker des **métadonnées** pour chaque bloc :

```c
int* ptr = malloc(sizeof(int));  // Demande 4 octets

// En réalité, l'allocateur utilise plus de mémoire :
┌──────────────────────┐
│  Métadonnées (8-16B) │ ← Taille du bloc, état, liens
├──────────────────────┤
│  Vos données (4B)    │ ← int
├──────────────────────┤
│  Padding (0-12B)     │ ← Alignement mémoire
└──────────────────────┘

Total réel : 12-32 octets au lieu de 4 !
```

**Overhead typique : 200-400%** pour les petites allocations.

### Fragmentation

Avec le temps, le Heap peut se fragmenter :

```
Heap au début :
┌─────────────────────────────────────┐
│         [Espace libre continu]      │
└─────────────────────────────────────┘

Après plusieurs malloc/free :
┌───┬─────┬───┬─────────┬───┬─────────┐
│ A │libre│ B │ libre   │ C │ libre   │
└───┴─────┴───┴─────────┴───┴─────────┘
      ↑        ↑             ↑
   Petits trous inutilisables = Fragmentation

Problème : On ne peut plus allouer un gros bloc même si
la somme des trous libres est suffisante !
```

### Localité mémoire

Les allocations Heap peuvent être **dispersées** en mémoire :

```
Mémoire physique :
┌────────────────┐
│  ... autres... │
├────────────────┤
│  [Objet C]     │ ← 3e malloc
├────────────────┤
│  ... autres... │
├────────────────┤
│  [Objet A]     │ ← 1e malloc
├────────────────┤
│  ... autres... │
├────────────────┤
│  [Objet B]     │ ← 2e malloc
└────────────────┘

Résultat : Cache misses fréquents = Lenteur
```

---

## Comparaison des performances

### Tableau de comparaison

| Métrique | Stack | Heap | Ratio |
|----------|-------|------|-------|
| **Temps d'allocation** | ~1 ns | ~100-1000 ns | **100-1000x plus lent** |
| **Temps de libération** | ~1 ns | ~100-1000 ns | **100-1000x plus lent** |
| **Overhead mémoire** | 0% | 200-400% (petits blocs) | **Significatif** |
| **Localité cache** | Excellente | Variable | **Meilleure sur Stack** |
| **Fragmentation** | Aucune | Possible | **Risque sur Heap** |
| **Prédictibilité** | Parfaite | Variable | **Meilleure sur Stack** |

### Benchmark concret

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 1000000

void benchmark_stack() {
    clock_t start = clock();

    for (int i = 0; i < ITERATIONS; i++) {
        int x = 42;  // Allocation Stack
        (void)x;     // Éviter l'optimisation du compilateur
    }

    clock_t end = clock();
    double temps = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Stack : %f secondes pour %d allocations\n", temps, ITERATIONS);
}

void benchmark_heap() {
    clock_t start = clock();

    for (int i = 0; i < ITERATIONS; i++) {
        int* x = malloc(sizeof(int));  // Allocation Heap
        if (x != NULL) {
            *x = 42;
            free(x);  // Libération Heap
        }
    }

    clock_t end = clock();
    double temps = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Heap  : %f secondes pour %d allocations\n", temps, ITERATIONS);
}

int main() {
    printf("=== Benchmark Stack vs Heap ===\n");
    benchmark_stack();
    benchmark_heap();
    return 0;
}
```

**Résultats typiques :**
```
=== Benchmark Stack vs Heap ===
Stack : 0.001 secondes pour 1000000 allocations
Heap  : 0.150 secondes pour 1000000 allocations

→ Le Heap est environ 150x plus lent dans ce cas
```

---

## Impact du cache CPU

### Hiérarchie mémoire

```
Vitesse décroissante ↓          Taille croissante ↓

┌───────────────┐
│ Registres CPU │  < 1 ns       ~1 KB
├───────────────┤
│ Cache L1      │  ~1-2 ns      32-64 KB
├───────────────┤
│ Cache L2      │  ~3-10 ns     256 KB - 1 MB
├───────────────┤
│ Cache L3      │  ~10-20 ns    8-32 MB
├───────────────┤
│ RAM (DRAM)    │  ~100 ns      8-64 GB
├───────────────┤
│ SSD           │  ~50-100 µs   500 GB - 2 TB
├───────────────┤
│ HDD           │  ~10 ms       1-10 TB
└───────────────┘
```

### Stack et cache : Amis naturels

Les variables Stack sont généralement dans le cache :

```c
void fonction_rapide() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    // Ces 4 variables sont probablement dans le cache L1
    int resultat = a + b + c + d;  // Accès ultra-rapide !
}
```

### Heap et cache : Relation compliquée

```c
void fonction_lente() {
    int* a = malloc(sizeof(int)); *a = 1;
    int* b = malloc(sizeof(int)); *b = 2;
    int* c = malloc(sizeof(int)); *c = 3;
    int* d = malloc(sizeof(int)); *d = 4;

    // Ces pointeurs peuvent être dispersés en mémoire
    int resultat = *a + *b + *c + *d;  // Potentiels cache misses

    free(a); free(b); free(c); free(d);
}
```

### Améliorer la localité Heap

**Solution : Allouer en un seul bloc**

```c
// ❌ Mauvais : Allocations multiples
int* tableau[100];
for (int i = 0; i < 100; i++) {
    tableau[i] = malloc(sizeof(int));  // 100 zones dispersées !
}

// ✅ Bon : Allocation unique
int* tableau = malloc(100 * sizeof(int));  // Une seule zone contiguë
```

---

## Stratégies d'optimisation

### 1. Préférez la Stack quand c'est possible

```c
// ✅ Rapide
void calcul_rapide(int n) {
    int resultats[100];  // Stack
    // ...
}

// ❌ Plus lent (si n <= 100)
void calcul_lent(int n) {
    int* resultats = malloc(n * sizeof(int));  // Heap
    // ...
    free(resultats);
}
```

### 2. Réduisez les allocations Heap

```c
// ❌ Lent : Allocation dans la boucle
for (int i = 0; i < 1000; i++) {
    char* buffer = malloc(256);
    // ... traitement ...
    free(buffer);
}

// ✅ Rapide : Allocation une seule fois
char* buffer = malloc(256);
for (int i = 0; i < 1000; i++) {
    // ... traitement ...
}
free(buffer);
```

### 3. Réutilisez la mémoire

```c
// ❌ Lent : Allocations multiples
void traiter_donnees(int n_operations) {
    for (int i = 0; i < n_operations; i++) {
        int* temp = malloc(1000 * sizeof(int));
        // ... traitement ...
        free(temp);
    }
}

// ✅ Rapide : Réutilisation du buffer
void traiter_donnees_optimise(int n_operations) {
    int* temp = malloc(1000 * sizeof(int));
    for (int i = 0; i < n_operations; i++) {
        // ... traitement avec le même buffer ...
    }
    free(temp);
}
```

### 4. Utilisez des pools de mémoire

Pour des allocations fréquentes de même taille :

```c
// Concept de memory pool (simplifié)
typedef struct {
    void* memory;
    size_t block_size;
    size_t num_blocks;
    // ... métadonnées ...
} MemoryPool;

// Pré-allouer un grand bloc
MemoryPool* pool = create_pool(sizeof(int), 1000);

// Allocation rapide depuis le pool (pas de malloc)
int* x = pool_alloc(pool);
int* y = pool_alloc(pool);

// Libération rapide vers le pool (pas de free)
pool_free(pool, x);
pool_free(pool, y);

// Nettoyage final
destroy_pool(pool);
```

---

## Cas d'usage et recommandations

### Utilisez la Stack pour :

✅ **Performances critiques**
```c
void traitement_temps_reel() {
    int buffer[1024];  // Rapide, prévisible
    // ...
}
```

✅ **Petites données temporaires**
```c
void calcul() {
    double temp1, temp2, resultat;  // Variables temporaires
    // ...
}
```

✅ **Tableaux de taille connue et raisonnable**
```c
void process_image_tile() {
    uint8_t pixel_buffer[256];  // Taille fixe, petite
    // ...
}
```

### Utilisez le Heap pour :

✅ **Données de taille inconnue à la compilation**
```c
char* load_file(const char* filename) {
    size_t size = get_file_size(filename);
    char* content = malloc(size);  // Taille connue au runtime
    // ...
    return content;
}
```

✅ **Grandes structures de données**
```c
// Structure de 10 MB : trop grand pour la Stack !
Image* img = malloc(sizeof(Image) + 10 * 1024 * 1024);
```

✅ **Données qui doivent survivre entre fonctions**
```c
Node* create_node(int value) {
    Node* node = malloc(sizeof(Node));
    node->value = value;
    return node;  // Persiste après la fonction
}
```

---

## Mesurer les performances de votre code

### Outils de profilage

```bash
# Valgrind : Analyse mémoire complète
valgrind --tool=massif ./mon_programme

# Perf : Profilage CPU et cache
perf stat -e cache-misses,cache-references ./mon_programme

# Time : Temps d'exécution simple
time ./mon_programme
```

### Profiler avec time

```c
#include <time.h>

clock_t start = clock();

// Code à mesurer
for (int i = 0; i < 1000000; i++) {
    // ...
}

clock_t end = clock();
double temps_cpu = ((double)(end - start)) / CLOCKS_PER_SEC;
printf("Temps CPU : %f secondes\n", temps_cpu);
```

---

## Récapitulatif

### Points clés : Durée de vie

1. **Stack** : Durée de vie automatique, limitée au scope
2. **Heap** : Durée de vie manuelle, jusqu'à `free()`
3. Ne jamais retourner l'adresse d'une variable locale Stack
4. Toujours libérer la mémoire Heap pour éviter les fuites

### Points clés : Performance

1. **Stack** : ~100-1000x plus rapide que le Heap
2. **Stack** : Zéro overhead, excellente localité cache
3. **Heap** : Overhead mémoire 200-400% pour petits blocs
4. **Heap** : Risque de fragmentation et cache misses
5. Préférez la Stack quand c'est possible
6. Réduisez le nombre d'allocations Heap
7. Réutilisez les buffers Heap quand possible

### Règles de décision

```
Taille connue à la compilation ?
                           │
                ┌──────────┴──────────┐
                │                     │
               OUI                   NON
                │                     │
                ↓                     ↓
         Taille < 1 KB ?         ┌────────┐
                │                │  HEAP  │
        ┌───────┴───────┐        └────────┘
        │               │
       OUI             NON
        │               │
        ↓               ↓
  Persistance ?   Persistance ?
        │               │
   ┌────┴────┐     ┌────┴────┐
   │         │     │         │
  OUI       NON   OUI       NON
   │         │     │         │
   ↓         ↓     ↓         ↓
┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐
│ HEAP │ │STACK │ │ HEAP │ │STACK*│
└──────┘ └──────┘ └──────┘ └──────┘
                             * Attention
                             au stack
                             overflow !
```

---

## Pour aller plus loin

### Lectures recommandées

- **Section 9.3** : Les fonctions `malloc`, `calloc`, `realloc` et `free` (détails pratiques)
- **Section 9.5** : Fuites mémoire : causes et prévention
- **Module 5, Section 15.4** : Détection de fuites avec Valgrind
- **Section 27.3** : Profiling avancé et optimisation

### Man pages essentielles

```bash
man 3 malloc   # Allocation dynamique
man 3 free     # Libération mémoire
man ulimit     # Limites ressources (taille Stack)
```

---

⏭️ [Les fonctions malloc, calloc, realloc et free](/09-allocation-dynamique/03-fonctions-allocation.md)
