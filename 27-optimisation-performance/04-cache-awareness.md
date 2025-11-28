🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.4 Cache awareness

## Introduction

Le **cache awareness** (conscience du cache) est l'art d'écrire du code qui exploite efficacement la **hiérarchie mémoire** du processeur. C'est l'un des facteurs les plus importants pour les performances modernes, et pourtant souvent négligé par les débutants.

### Pourquoi le cache est-il si important ?

**Fait surprenant :** Sur un processeur moderne, accéder à la RAM est **200 à 300 fois plus lent** qu'accéder au cache L1 !

```
Latences typiques (ordres de grandeur) :
- Registre CPU      : 0 cycles    (instantané)
- Cache L1          : 4 cycles    (~1 nanoseconde)
- Cache L2          : 12 cycles   (~3 nanosecondes)
- Cache L3          : 40 cycles   (~10 nanosecondes)
- RAM               : 200 cycles  (~60 nanosecondes)
- SSD               : 50,000 ns   (50 microsecondes)
- Disque dur        : 5,000,000 ns (5 millisecondes)
```

**Conséquence :** Un programme qui fait beaucoup de cache misses (défauts de cache) peut être **10 à 100 fois plus lent** qu'un programme cache-friendly !

### Analogie : La bibliothèque

Imaginez que vous travaillez sur un projet :

- **Registres CPU** = Votre bureau (livres directement sous vos yeux)
- **Cache L1** = Étagère à portée de main (2 secondes pour prendre un livre)
- **Cache L2** = Bibliothèque de votre bureau (10 secondes)
- **Cache L3** = Bibliothèque de l'étage (30 secondes)
- **RAM** = Bibliothèque municipale (1 heure aller-retour)
- **Disque** = Bibliothèque nationale (1 journée)

Si vous devez aller à la bibliothèque nationale pour chaque page que vous lisez, votre travail sera **très lent**. Si vous ramenez tous les livres nécessaires sur votre bureau une seule fois, vous serez **beaucoup plus efficace**.

C'est exactement le principe du cache !

---

## La hiérarchie mémoire

### Architecture typique (processeur moderne)

```
┌──────────────────────────────────────────┐
│              CPU Core                    │
│  ┌───────────────────────────────────┐   │
│  │        Registres (64 bits)        │   │  ← Plus rapide, plus petit
│  │        ~100 bytes                 │   │
│  └───────────────────────────────────┘   │
│                   ↓↑                     │
│  ┌───────────────────────────────────┐   │
│  │     Cache L1 (données + instr)    │   │
│  │     32 KB / 32 KB                 │   │
│  └───────────────────────────────────┘   │
│                   ↓↑                     │
│  ┌───────────────────────────────────┐   │
│  │         Cache L2 (unifié)         │   │
│  │           256 KB - 512 KB         │   │
│  └───────────────────────────────────┘   │
└──────────────────┬┬──────────────────────┘
                   ↓↑
┌──────────────────────────────────────────┐
│    Cache L3 (partagé entre cores)        │
│         8 MB - 64 MB                     │
└──────────────────┬┬──────────────────────┘
                   ↓↑
┌──────────────────────────────────────────┐
│             RAM (DRAM)                   │
│         8 GB - 128 GB                    │
└──────────────────┬┬──────────────────────┘
                   ↓↑
┌──────────────────────────────────────────┐
│           Stockage (SSD/HDD)             │
│          500 GB - 4 TB                   │  ← Plus lent, plus grand
└──────────────────────────────────────────┘
```

**Principe fondamental :** Plus c'est rapide, plus c'est petit et cher.

### Vérifier la configuration de votre CPU

```bash
# Voir les informations de cache
lscpu | grep cache

# Exemple de sortie :
# L1d cache:           32K
# L1i cache:           32K
# L2 cache:            256K
# L3 cache:            8192K

# Ou avec getconf
getconf -a | grep CACHE
```

---

## Comment fonctionne le cache ?

### Lignes de cache (Cache Lines)

Le cache ne stocke pas les données **octet par octet**, mais par **blocs** appelés **lignes de cache** (cache lines).

**Taille typique d'une ligne de cache :** 64 bytes (octets)

**Principe :** Quand vous lisez un seul byte en mémoire, le CPU charge toute la ligne de cache (64 bytes) contenant ce byte.

```c
// Exemple
int tableau[16];  // 16 * 4 = 64 bytes = 1 ligne de cache

// Quand vous accédez à tableau[0]
int x = tableau[0];  // Le CPU charge tableau[0] à tableau[15] en une fois !
```

**Conséquence :** Accéder à `tableau[1]` juste après `tableau[0]` est **gratuit** (déjà dans le cache) !

### Cache hit vs Cache miss

- **Cache hit** : La donnée demandée est déjà dans le cache → Accès rapide ✅
- **Cache miss** : La donnée n'est pas dans le cache → Il faut aller la chercher en RAM → Accès lent ❌

### Types de cache miss

1. **Compulsory miss (Cold miss)** : Premier accès à une donnée (inévitable)
2. **Capacity miss** : Le cache est plein, des données anciennes sont évincées
3. **Conflict miss** : Collision dans le cache (architecture associative)

---

## Localité spatiale et temporelle

### Localité temporelle (Temporal Locality)

**Principe :** Si vous accédez à une donnée maintenant, vous êtes susceptible de la réutiliser **bientôt**.

**Exemple :**
```c
int somme = 0;
for (int i = 0; i < 1000; i++) {
    somme += i;  // 'somme' est réutilisée à chaque itération
}
```

La variable `somme` reste dans un registre CPU ou le cache L1, ce qui est très rapide.

### Localité spatiale (Spatial Locality)

**Principe :** Si vous accédez à une donnée, vous êtes susceptible d'accéder aux données **voisines** bientôt.

**Exemple cache-friendly :**
```c
int tableau[1000];

// BON : Accès séquentiel (localité spatiale)
for (int i = 0; i < 1000; i++) {
    tableau[i] = i;
}
```

Les éléments contigus en mémoire sont chargés ensemble dans les lignes de cache.

**Exemple cache-hostile :**
```c
// MAUVAIS : Accès aléatoire
for (int i = 0; i < 1000; i++) {
    int index = rand() % 1000;
    tableau[index] = i;
}
```

Chaque accès peut provoquer un cache miss.

---

## Exemples de code : Impact du cache

### Exemple 1 : Parcours de tableau (row-major vs column-major)

**Contexte :** Tableau 2D (matrice) en C

```c
#define TAILLE 1000
int matrice[TAILLE][TAILLE];  // 1000x1000 = 1 million d'entiers
```

**En mémoire**, C stocke les tableaux 2D en **row-major order** (par ligne) :

```
matrice[0][0], matrice[0][1], matrice[0][2], ..., matrice[0][999],
matrice[1][0], matrice[1][1], ...
```

#### Parcours cache-friendly (par ligne)

```c
// ✅ BON : Parcours séquentiel (row-major)
for (int i = 0; i < TAILLE; i++) {
    for (int j = 0; j < TAILLE; j++) {
        matrice[i][j] = 0;  // Accès continu en mémoire
    }
}
```

**Explication :** On accède à `matrice[i][0]`, `matrice[i][1]`, `matrice[i][2]`... qui sont contigus en mémoire. Le cache précharge les données voisines, donc chaque accès est un cache hit (après le premier).

#### Parcours cache-hostile (par colonne)

```c
// ❌ MAUVAIS : Parcours par colonne (column-major)
for (int j = 0; j < TAILLE; j++) {
    for (int i = 0; i < TAILLE; i++) {
        matrice[i][j] = 0;  // Accès non continu (stride de 1000)
    }
}
```

**Explication :** On accède à `matrice[0][j]`, `matrice[1][j]`, `matrice[2][j]`... Ces éléments sont espacés de 1000 entiers (4000 bytes) en mémoire. Chaque accès peut provoquer un cache miss.

#### Mesure de performance

```c
// test_cache.c
#include <stdio.h>
#include <time.h>

#define TAILLE 1000

int matrice[TAILLE][TAILLE];

void parcours_ligne() {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            matrice[i][j] = 0;
        }
    }
}

void parcours_colonne() {
    for (int j = 0; j < TAILLE; j++) {
        for (int i = 0; i < TAILLE; i++) {
            matrice[i][j] = 0;
        }
    }
}

int main() {
    clock_t debut, fin;

    // Test parcours par ligne
    debut = clock();
    parcours_ligne();
    fin = clock();
    printf("Parcours ligne:   %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    // Test parcours par colonne
    debut = clock();
    parcours_colonne();
    fin = clock();
    printf("Parcours colonne: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    return 0;
}
```

**Compilation et exécution :**
```bash
gcc -O2 test_cache.c -o test_cache
./test_cache
```

**Résultats typiques :**
```
Parcours ligne:   1.2 ms
Parcours colonne: 8.5 ms  ← 7x plus lent !
```

**Conclusion :** Le parcours par colonne est **7 fois plus lent** à cause des cache misses !

### Exemple 2 : Structure de données et alignement

#### Structure mal alignée (cache-hostile)

```c
struct Point {
    int x;          // 4 bytes
    char valide;    // 1 byte
    int y;          // 4 bytes
    char couleur;   // 1 byte
};  // Taille réelle : 16 bytes (à cause du padding)

struct Point points[1000];

// Parcourir tous les x
for (int i = 0; i < 1000; i++) {
    int valeur = points[i].x;  // Les 'x' ne sont pas contigus
}
```

**Problème :** Les valeurs `x` sont espacées de 16 bytes (taille de la structure). Si vous voulez juste les `x`, vous chargez aussi les autres champs inutilement.

#### Structure réorganisée (cache-friendly)

**Option 1 : Réorganiser les champs**
```c
struct Point {
    int x;          // 4 bytes
    int y;          // 4 bytes
    char valide;    // 1 byte
    char couleur;   // 1 byte
    // padding: 2 bytes
};  // Taille : 12 bytes (plus compact)
```

**Option 2 : Structure of Arrays (SoA)**
```c
// Plutôt que Array of Structures (AoS)
struct Points {
    int x[1000];       // Tous les x ensemble
    int y[1000];       // Tous les y ensemble
    char valide[1000];
    char couleur[1000];
};

struct Points points;

// Parcourir tous les x : accès séquentiel !
for (int i = 0; i < 1000; i++) {
    int valeur = points.x[i];  // ✅ Contigus en mémoire
}
```

**Avantages SoA :**
- ✅ Meilleure utilisation du cache (données du même type ensemble)
- ✅ Vectorisation SIMD facilitée
- ✅ Moins de padding gaspillé

**Inconvénients SoA :**
- ❌ Moins intuitif
- ❌ Plus difficile à passer en paramètre
- ❌ Accès à un "point complet" plus complexe

---

## Techniques pour optimiser l'utilisation du cache

### 1. Parcourir les tableaux séquentiellement

**✅ BON :**
```c
for (int i = 0; i < N; i++) {
    tableau[i] = valeur;
}
```

**❌ MAUVAIS :**
```c
for (int i = 0; i < N; i += 8) {  // Stride = 8
    tableau[i] = valeur;
}
```

### 2. Utiliser des boucles avec un stride de 1

**Stride** = distance entre deux accès successifs

```c
// Stride = 1 (optimal)
for (int i = 0; i < N; i++) {
    tableau[i] = 0;
}

// Stride = 2 (moins bon)
for (int i = 0; i < N; i += 2) {
    tableau[i] = 0;
}

// Stride = 1000 (très mauvais si TAILLE_CACHE < 4000 bytes)
for (int i = 0; i < 1000; i++) {
    matrice[i][0] = 0;
}
```

**Règle d'or :** Plus le stride est grand, plus il y a de cache misses.

### 3. Regrouper les données fréquemment utilisées ensemble

**❌ MAUVAIS :**
```c
struct Personne {
    char nom[100];         // 100 bytes, rarement utilisé
    int age;               // 4 bytes, souvent utilisé
    char adresse[200];     // 200 bytes, rarement utilisé
    int salaire;           // 4 bytes, souvent utilisé
};
```

Si vous ne faites que lire `age` et `salaire`, vous chargez aussi `nom` et `adresse` dans le cache, gaspillant de l'espace.

**✅ BON : Séparer les données chaudes et froides**
```c
struct PersonneHot {     // "Hot data" = fréquemment utilisé
    int age;
    int salaire;
};

struct PersonneCold {    // "Cold data" = rarement utilisé
    char nom[100];
    char adresse[200];
};
```

### 4. Bloquer les boucles (Loop Tiling / Blocking)

**Technique avancée** : Diviser une grosse boucle en petits blocs qui tiennent dans le cache.

**Exemple : Multiplication de matrices**

```c
// ❌ Algorithme naïf
for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
        for (int k = 0; k < N; k++) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }
}
```

Pour de grandes matrices (N=1000), cet algorithme fait beaucoup de cache misses car il parcourt `B` par colonne.

**✅ Avec blocking (tiling)**

```c
#define BLOCK_SIZE 32  // Taille de bloc qui tient dans le cache

for (int ii = 0; ii < N; ii += BLOCK_SIZE) {
    for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
        for (int kk = 0; kk < N; kk += BLOCK_SIZE) {
            // Traiter un bloc de BLOCK_SIZE x BLOCK_SIZE
            for (int i = ii; i < ii + BLOCK_SIZE; i++) {
                for (int j = jj; j < jj + BLOCK_SIZE; j++) {
                    for (int k = kk; k < kk + BLOCK_SIZE; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
    }
}
```

Les petits blocs tiennent dans le cache L1, ce qui améliore drastiquement les performances.

**Amélioration typique :** 2-5x plus rapide sur de grandes matrices !

### 5. Préchargement de données (Prefetching)

**Préchargement manuel (hint au CPU)**

```c
#include <xmmintrin.h>  // Pour _mm_prefetch

for (int i = 0; i < N - 8; i++) {
    _mm_prefetch(&tableau[i + 8], _MM_HINT_T0);  // Précharger 8 éléments à l'avance
    // Traitement de tableau[i]
}
```

**Note :** Les CPUs modernes font déjà du prefetching automatique pour les accès séquentiels. Cette technique est rarement nécessaire en C classique.

### 6. Aligner les données en mémoire

```c
// Forcer l'alignement sur 64 bytes (taille d'une ligne de cache)
struct __attribute__((aligned(64))) DonneesAlignees {
    int valeurs[16];
};
```

**Avantages :**
- ✅ Évite qu'une structure chevauche deux lignes de cache
- ✅ Améliore les performances de certaines instructions SIMD

---

## False Sharing (Faux partage)

### Problème spécifique au multi-threading

Le **false sharing** se produit quand deux threads modifient des variables différentes qui se trouvent sur la **même ligne de cache**.

**Exemple problématique :**

```c
// Structure partagée entre threads
struct Compteurs {
    int compteur_thread1;  // 4 bytes
    int compteur_thread2;  // 4 bytes (sur la même ligne de cache !)
} compteurs;

// Thread 1
void* thread1_func(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        compteurs.compteur_thread1++;  // Modifie ligne de cache
    }
    return NULL;
}

// Thread 2
void* thread2_func(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        compteurs.compteur_thread2++;  // Modifie MÊME ligne de cache
    }
    return NULL;
}
```

**Pourquoi c'est un problème ?**

1. Thread 1 modifie `compteur_thread1`
2. La ligne de cache entière (contenant aussi `compteur_thread2`) est marquée "modifiée"
3. Thread 2 veut modifier `compteur_thread2`
4. Le cache de Thread 2 doit invalider sa copie et recharger la ligne depuis le cache de Thread 1
5. **Ping-pong de lignes de cache entre les threads** → Performance catastrophique

### Solution : Padding pour séparer les données

```c
#define CACHE_LINE_SIZE 64

struct Compteurs {
    int compteur_thread1;
    char padding1[CACHE_LINE_SIZE - sizeof(int)];  // Remplissage

    int compteur_thread2;
    char padding2[CACHE_LINE_SIZE - sizeof(int)];
};
```

Maintenant `compteur_thread1` et `compteur_thread2` sont sur des **lignes de cache différentes**, donc pas de false sharing !

**Amélioration typique :** 10-100x plus rapide sur du code massivement parallèle.

**Alternative moderne (C11) :**

```c
#include <stdalign.h>

struct Compteurs {
    alignas(64) int compteur_thread1;
    alignas(64) int compteur_thread2;
};
```

---

## Mesurer l'efficacité du cache

### Avec perf (Linux)

```bash
# Compiler le programme
gcc -O2 -g test_cache.c -o test_cache

# Mesurer les cache misses
perf stat -e cache-references,cache-misses ./test_cache

# Exemple de sortie :
#    4,123,456      cache-references
#      234,567      cache-misses              #    5.69% of all cache refs
```

**Interprétation :**
- **Cache miss rate < 5%** : Excellent ✅
- **Cache miss rate 5-10%** : Bon
- **Cache miss rate 10-20%** : Acceptable
- **Cache miss rate > 20%** : Problématique ❌

### Avec Valgrind Cachegrind

```bash
valgrind --tool=cachegrind ./test_cache

# Sortie exemple :
# I   refs:      10,000,000
# I1  misses:        12,345  (0.12%)
# LLi misses:         1,234  (0.01%)
#
# D   refs:       5,000,000  (3,000,000 rd + 2,000,000 wr)
# D1  misses:       456,789  (9.14%)
# LLd misses:        45,678  (0.91%)
```

**Légende :**
- **I** = Instructions (code)
- **D** = Données
- **I1/D1** = Cache L1
- **LL** = Last Level cache (L3)

**Analyse :**
- **D1 misses = 9.14%** → Taux de défaut du cache L1 pour les données
- Si ce taux est élevé, optimiser l'accès aux données

### Comparer deux versions du code

```bash
# Version cache-hostile
valgrind --tool=cachegrind ./test_cache_bad > rapport_bad.txt

# Version cache-friendly
valgrind --tool=cachegrind ./test_cache_good > rapport_good.txt

# Comparer
cg_diff rapport_bad.txt rapport_good.txt
```

---

## Cas d'étude : Somme de tableau

### Version 1 : Naïve

```c
// sum_naive.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 10000000  // 10 millions

int main() {
    int *tableau = malloc(TAILLE * sizeof(int));

    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = i;
    }

    clock_t debut = clock();

    long long somme = 0;
    for (int i = 0; i < TAILLE; i++) {
        somme += tableau[i];
    }

    clock_t fin = clock();

    printf("Somme: %lld\n", somme);
    printf("Temps: %.3f ms\n", (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    free(tableau);
    return 0;
}
```

**Performance :**
```bash
gcc -O2 sum_naive.c -o sum_naive
./sum_naive
# Temps: 15.2 ms
```

### Version 2 : Avec déroulage de boucle (loop unrolling)

```c
// sum_unroll.c
long long somme = 0;
int i;

// Traiter 4 éléments à la fois
for (i = 0; i < TAILLE - 3; i += 4) {
    somme += tableau[i];
    somme += tableau[i + 1];
    somme += tableau[i + 2];
    somme += tableau[i + 3];
}

// Reste
for (; i < TAILLE; i++) {
    somme += tableau[i];
}
```

**Performance :**
```bash
gcc -O2 sum_unroll.c -o sum_unroll
./sum_unroll
# Temps: 12.8 ms  ← 15% plus rapide
```

**Pourquoi ?** Moins de sauts conditionnels, meilleure utilisation des registres, parallélisme au niveau instruction.

### Version 3 : Avec plusieurs accumulateurs

```c
// sum_multi_acc.c
long long somme1 = 0, somme2 = 0, somme3 = 0, somme4 = 0;
int i;

// 4 accumulateurs indépendants
for (i = 0; i < TAILLE - 3; i += 4) {
    somme1 += tableau[i];
    somme2 += tableau[i + 1];
    somme3 += tableau[i + 2];
    somme4 += tableau[i + 3];
}

long long somme = somme1 + somme2 + somme3 + somme4;

// Reste
for (; i < TAILLE; i++) {
    somme += tableau[i];
}
```

**Performance :**
```bash
gcc -O2 sum_multi_acc.c -o sum_multi_acc
./sum_multi_acc
# Temps: 8.5 ms  ← 44% plus rapide que la version naïve !
```

**Pourquoi ?** Les 4 accumulateurs peuvent être calculés **en parallèle** par le CPU (instruction-level parallelism). Pas de dépendance entre les calculs.

**Mesure des cache misses :**
```bash
perf stat -e cache-references,cache-misses ./sum_naive
perf stat -e cache-references,cache-misses ./sum_multi_acc

# Les deux versions ont le même taux de cache misses (accès séquentiel)
# Mais la version multi-accumulateurs est plus rapide grâce au parallélisme
```

---

## Structures de données cache-friendly

### Listes chaînées vs Tableaux

**Liste chaînée :**
```c
struct Node {
    int valeur;
    struct Node *suivant;  // Pointeur vers le nœud suivant
};
```

**Problème :** Les nœuds peuvent être **éparpillés en mémoire** (à cause de malloc). Parcourir la liste provoque beaucoup de cache misses.

**Tableau dynamique :**
```c
int *tableau = malloc(1000 * sizeof(int));
```

**Avantage :** Éléments **contigus en mémoire**. Parcours séquentiel très cache-friendly.

**Règle générale :** Préférez les tableaux aux listes chaînées pour les performances (sauf si insertions/suppressions fréquentes).

### Pool d'allocation

Si vous devez absolument utiliser des listes chaînées, allouez les nœuds dans un **pool** :

```c
#define POOL_SIZE 10000

struct Node pool[POOL_SIZE];  // Nœuds contigus !
int pool_index = 0;

struct Node* allouer_node() {
    if (pool_index < POOL_SIZE) {
        return &pool[pool_index++];
    }
    return NULL;
}
```

Les nœuds étant contigus, le parcours est plus cache-friendly.

---

## Recommandations pratiques

### ✅ Faire

1. **Parcourir les tableaux séquentiellement** (stride = 1)
2. **Utiliser des tableaux plutôt que des listes chaînées** quand possible
3. **Regrouper les données chaudes ensemble** (hot/cold data separation)
4. **Aligner les structures critiques sur des lignes de cache**
5. **Éviter le false sharing dans le code multi-threadé**
6. **Utiliser Structure of Arrays (SoA)** pour les gros datasets
7. **Blocker les boucles** (loop tiling) pour de grosses matrices
8. **Mesurer avec perf ou Valgrind** avant d'optimiser

### ❌ Éviter

1. **Accès aléatoires en mémoire**
2. **Parcours par colonne de matrices en C**
3. **Structures avec beaucoup de padding inutile**
4. **Variables partagées entre threads sur la même ligne de cache**
5. **Listes chaînées pour de grands datasets**
6. **Sauts de pointeurs indirects**
7. **Stride trop grand dans les boucles**

---

## Checklist : Code cache-friendly

Avant d'écrire du code critique en performance :

- ✅ Les données fréquemment accédées sont-elles contiguës ?
- ✅ Les boucles parcourent-elles les tableaux séquentiellement ?
- ✅ Les structures sont-elles compactes (peu de padding) ?
- ✅ Les données chaudes sont-elles séparées des données froides ?
- ✅ Y a-t-il du false sharing dans le code multi-threadé ?
- ✅ Puis-je utiliser un tableau au lieu d'une liste chaînée ?

Après avoir écrit le code :

- ✅ Ai-je mesuré le taux de cache misses avec perf ?
- ✅ Ai-je comparé avec une version alternative ?
- ✅ L'optimisation a-t-elle vraiment amélioré les performances ?

---

## Outils pour analyser le cache

### perf (Linux)

```bash
# Cache miss rate global
perf stat -e cache-references,cache-misses ./programme

# Cache L1, L2, L3 détaillé
perf stat -e L1-dcache-loads,L1-dcache-load-misses \
          -e LLC-loads,LLC-load-misses ./programme

# Profiler les cache misses par fonction
perf record -e cache-misses ./programme
perf report
```

### Valgrind Cachegrind

```bash
# Profiling détaillé du cache
valgrind --tool=cachegrind --cache-sim=yes ./programme

# Voir le rapport
cg_annotate cachegrind.out.<pid>

# Visualisation avec KCachegrind
kcachegrind cachegrind.out.<pid>
```

### Intel VTune (propriétaire mais puissant)

Pour les processeurs Intel, VTune offre une analyse très détaillée du cache, des pipelines CPU, etc.

---

## Mythes et réalités

### ❌ Mythe : "Les compilateurs optimisent tout automatiquement"

**Réalité :** Les compilateurs font un excellent travail, mais ils ne peuvent pas :
- Changer l'ordre de parcours d'un tableau multidimensionnel
- Transformer Array of Structures en Structure of Arrays
- Deviner que vous voulez du loop tiling

**Votre responsabilité :** Structurer le code de manière cache-friendly.

### ❌ Mythe : "L'optimisation cache n'est utile que pour les supercalculateurs"

**Réalité :** Même sur un smartphone ou un Raspberry Pi, le cache a un impact énorme. Les principes restent les mêmes.

### ✅ Réalité : "L'optimisation prématurée est mauvaise, mais la conception cache-aware dès le départ est bonne"

Écrire du code séquentiel et compact dès le début ne coûte rien et évite des refactorisations coûteuses plus tard.

---

## Pour aller plus loin

### Lectures recommandées

- **"What Every Programmer Should Know About Memory"** par Ulrich Drepper (PDF gratuit, référence absolue)
- **"Computer Architecture: A Quantitative Approach"** par Hennessy & Patterson
- **"Systems Performance"** par Brendan Gregg (chapitre sur la mémoire et le cache)

### Ressources en ligne

- **Gallery of Processor Cache Effects** : http://igoro.com/archive/gallery-of-processor-cache-effects/
- **Cache-Oblivious Algorithms** : https://en.wikipedia.org/wiki/Cache-oblivious_algorithm
- **Intel Optimization Manual** : Documentation détaillée sur l'architecture x86

### Commandes utiles

```bash
# Afficher la hiérarchie mémoire
lstopo        # Nécessite hwloc : sudo apt install hwloc

# Infos détaillées CPU
cat /sys/devices/system/cpu/cpu0/cache/index*/size
cat /sys/devices/system/cpu/cpu0/cache/index*/level
cat /sys/devices/system/cpu/cpu0/cache/index*/type

# Benchmark mémoire
sysbench memory run
```

---

## Résumé

Le cache est la clé des performances modernes :

1. ✅ **Principe fondamental** : Accéder au cache est 100-300x plus rapide que la RAM
2. ✅ **Localité spatiale** : Données voisines en mémoire → chargées ensemble
3. ✅ **Localité temporelle** : Données récemment utilisées → restent dans le cache
4. ✅ **Lignes de cache** : 64 bytes chargés à la fois
5. ✅ **Parcours séquentiel** : Toujours préférer stride = 1
6. ✅ **False sharing** : Attention au multi-threading !
7. ✅ **Mesurer** : Utiliser perf et Valgrind pour identifier les problèmes

**Citation :**

> "Memory is the new disk, disk is the new tape" — Jim Gray

Les accès mémoire sont devenus le goulot d'étranglement principal. Écrire du code cache-aware n'est plus optionnel pour les applications critiques en performance !

---

*Prochaine section : 27.5 Branch prediction*

⏭️ [Branch prediction](/27-optimisation-performance/05-branch-prediction.md)
