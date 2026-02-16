🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 35.5 Performance debugging

## Introduction

Le **performance debugging** (débogage de performance) consiste à identifier et corriger les parties d'un programme qui le rendent lent. Contrairement aux bugs classiques qui causent des crashs ou des résultats incorrects, les problèmes de performance font que votre programme fonctionne... mais trop lentement.

### Pourquoi la performance est-elle critique ?

En production, la performance n'est pas un "nice-to-have", c'est une **exigence fonctionnelle** :

- **Expérience utilisateur** : Une API qui met 5 secondes à répondre est inutilisable
- **Coûts** : CPU et RAM se paient, l'optimisation réduit les factures cloud
- **Scalabilité** : Un serveur qui traite 100 req/s coûte 10x moins qu'un serveur qui traite 10 req/s
- **Énergie** : Code efficace = moins de consommation électrique
- **Compétitivité** : Les utilisateurs abandonnent les applications lentes

**Exemple concret** :
- Amazon a découvert que **100ms de latence supplémentaire** = **1% de ventes en moins**
- Google a constaté que **500ms de délai** = **20% de recherches en moins**

### Les symptômes de problèmes de performance

| Symptôme | Cause probable |
|----------|----------------|
| CPU à 100% | Calculs intensifs, boucle infinie, algorithme inefficace |
| Beaucoup de page faults | Accès mémoire inefficaces, cache misses |
| I/O élevé | Accès disque/réseau excessifs, pas de cache |
| Temps de réponse croissant | Fuite mémoire, fragmentation |
| Blocages intermittents | Contention sur les locks, deadlocks temporaires |

---

## Méthodologie de performance debugging

### Les 4 phases

#### Phase 1 : Mesurer (Measure)

**Règle d'or** : "You can't optimize what you can't measure"

Avant d'optimiser quoi que ce soit, vous devez **mesurer** où le temps est réellement passé. L'intuition est souvent **fausse**.

```c
// ❌ MAL : Optimiser sans mesurer
// "Je pense que cette fonction est lente, je vais l'optimiser"

// ✅ BIEN : Mesurer d'abord
// 1. Profiler le code
// 2. Identifier les hotspots réels
// 3. Optimiser seulement les bottlenecks
```

#### Phase 2 : Analyser (Analyze)

Comprendre **pourquoi** le code est lent :
- Algorithme inefficace ? (O(n²) au lieu de O(n))
- Cache misses ?
- Allocation mémoire excessive ?
- I/O bloquante ?

#### Phase 3 : Optimiser (Optimize)

Appliquer les corrections ciblées sur les vrais problèmes.

#### Phase 4 : Vérifier (Verify)

Mesurer à nouveau pour confirmer l'amélioration.

**Cycle d'optimisation** :
```
Mesurer → Analyser → Optimiser → Vérifier → (Répéter si nécessaire)
```

---

## Outils de profiling

### 1. time : Mesure simple

L'outil le plus basique mais très utile pour commencer.

```bash
# Mesurer le temps d'exécution
time ./myapp

# Sortie typique:
real    0m5.234s   # Temps total (wall-clock time)  
user    0m4.567s   # Temps CPU en mode utilisateur  
sys     0m0.234s   # Temps CPU en mode noyau  
```

**Interprétation** :
- `real` >> `user + sys` : Programme passe du temps en attente (I/O, sleep)
- `user` >> `sys` : Programme CPU-bound (calculs intensifs)
- `sys` élevé : Beaucoup d'appels système

### 2. gprof : Profiling basique

**gprof** analyse où le temps CPU est dépensé.

#### Utilisation

```bash
# 1. Compiler avec profiling
gcc -pg -o myapp myapp.c

# 2. Exécuter (génère gmon.out)
./myapp

# 3. Analyser
gprof myapp gmon.out > analysis.txt
```

#### Exemple de sortie

```
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total
 time   seconds   seconds    calls  ms/call  ms/call  name
 60.00      0.30     0.30   100000     0.00     0.00  slow_function
 30.00      0.45     0.15        1   150.00   450.00  process_data
 10.00      0.50     0.05     5000     0.01     0.01  helper_function
```

**Interprétation** :
- `slow_function` prend **60% du temps CPU** → Principal hotspot
- Appelée 100 000 fois → Optimiser ici aura le plus d'impact
- `process_data` : 30% du temps, mais appelée une seule fois

#### Call graph

```
index % time    self  children    called     name
[1]     90.0    0.30      0.15  100000         slow_function [1]
                0.15      0.00  100000/100000      helper_function [3]
-----------------------------------------------
[2]     10.0    0.05      0.00   5000         helper_function [3]
```

Montre quelle fonction appelle quelle autre.

**Limitations de gprof** :
- Ne profile que le temps CPU (pas les I/O)
- Sampling (peut rater des fonctions rapides)
- Overhead non négligeable

### 3. Valgrind Callgrind : Profiling détaillé

**Callgrind** donne un profil très détaillé des appels de fonctions et du cache.

#### Utilisation

```bash
# 1. Exécuter avec callgrind
valgrind --tool=callgrind ./myapp

# Génère callgrind.out.12345

# 2. Analyser en ligne de commande
callgrind_annotate callgrind.out.12345

# 3. Visualiser graphiquement (recommandé)
kcachegrind callgrind.out.12345
```

#### Exemple de sortie

```
--------------------------------------------------------------------------------
Ir          I1mr  ILmr  Dr          D1mr  DLmr  Dw          D1mw  DLmw
--------------------------------------------------------------------------------
12,345,678  1,234   234  4,567,890   456    45  2,345,678   234    23  PROGRAM TOTALS

--------------------------------------------------------------------------------
Ir          I1mr  ILmr  Dr          D1mr  DLmr  Dw          D1mw  DLmw  file:function
--------------------------------------------------------------------------------
7,234,567     100    10  2,234,567   234    23  1,234,567   100    10  myapp.c:slow_function
2,345,678     234    23    987,654   123    12    567,890    56     6  myapp.c:process_data
```

**Légende** :
- `Ir` : Instructions exécutées
- `I1mr` / `ILmr` : Instruction cache misses (L1/Last level)
- `Dr` / `Dw` : Lectures/Écritures de données
- `D1mr` / `DLmr` : Data cache misses

**Avantages** :
- Très précis (compte chaque instruction)
- Détecte les cache misses
- Visualisation graphique avec kcachegrind

**Inconvénients** :
- Très lent (20-50x)
- Ne pas utiliser pour mesurer le temps réel

### 4. perf : L'outil professionnel

**perf** est l'outil de référence sur Linux, utilisant les compteurs matériels du CPU.

#### Installation

```bash
sudo apt install linux-tools-generic linux-tools-$(uname -r)
```

#### Utilisation basique

```bash
# Profiler un programme
perf record ./myapp

# Générer un rapport
perf report

# Profiler avec call graph
perf record -g ./myapp  
perf report -g  
```

#### Exemple de sortie

```
# Overhead  Command  Shared Object       Symbol
# ........  .......  ..................  .............................
#
    60.34%  myapp    myapp               [.] slow_function
    25.67%  myapp    libc-2.31.so        [.] memcpy
    10.45%  myapp    myapp               [.] process_data
     3.54%  myapp    libc-2.31.so        [.] malloc
```

**Interprétation** :
- 60% du temps dans `slow_function`
- 25% dans `memcpy` (copie de mémoire coûteuse)
- 10% dans `process_data`

#### Mesurer des événements spécifiques

```bash
# Cache misses
perf stat -e cache-misses,cache-references ./myapp

# Branch mispredictions
perf stat -e branches,branch-misses ./myapp

# Instructions par cycle
perf stat -e instructions,cycles ./myapp

# Exemple de sortie:
Performance counter stats for './myapp':

    234,567,890      cache-references
     12,345,678      cache-misses              #    5.26% of all cache refs
    567,890,123      branches
      8,765,432      branch-misses             #    1.54% of all branches
  2,345,678,901      instructions              #    1.23  insn per cycle
  1,901,234,567      cycles

       1.234567890 seconds time elapsed
```

**Interprétation** :
- **5.26% de cache misses** : Bon (< 10%)
- **1.54% de branch mispredictions** : Excellent (< 5%)
- **1.23 instructions/cycle** : Moyen (moderne CPU peut faire 2-4)

#### Flamegraphs

Visualisation puissante des hotspots.

```bash
# Enregistrer avec call stacks
perf record -F 99 -g ./myapp

# Générer un flamegraph
perf script | stackcollapse-perf.pl | flamegraph.pl > flamegraph.svg

# Ouvrir dans un navigateur
firefox flamegraph.svg
```

Un flamegraph montre visuellement où le temps est passé, avec la largeur proportionnelle au temps CPU.

---

## Mesure de temps précise dans le code

### clock_gettime : Recommandé

```c
#include <time.h>
#include <stdio.h>

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    double start = get_time();

    // Code à mesurer
    for (int i = 0; i < 1000000; i++) {
        // ...
    }

    double end = get_time();
    printf("Elapsed: %.6f seconds\n", end - start);

    return 0;
}
```

**Avantages** :
- Précision nanoseconde
- Monotone (ne recule jamais, même si l'heure système change)

### RDTSC : Pour micro-benchmarks

Lecture directe du compteur de cycles CPU (x86).

```c
#include <x86intrin.h>
#include <stdio.h>

static inline uint64_t rdtsc() {
    return __rdtsc();
}

int main() {
    uint64_t start = rdtsc();

    // Code à mesurer (très court)
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }

    uint64_t end = rdtsc();
    printf("Cycles: %lu\n", end - start);

    return 0;
}
```

**Attention** :
- Sensible aux changements de fréquence CPU
- Utiliser uniquement pour micro-benchmarks
- Nécessite de désactiver le frequency scaling

---

## Identification des bottlenecks

### CPU-bound vs I/O-bound

#### Test simple

```bash
# Si le programme utilise 100% d'un cœur → CPU-bound
top -p $(pidof myapp)

# Si le programme attend souvent → I/O-bound
strace -c ./myapp
```

**Exemple strace** :

```
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 89.45    0.123456          12     10000           read
  5.67    0.007812           3      2500           write
  2.34    0.003234          32       100           open
```

89% du temps en `read` → Programme I/O-bound (lecture disque/réseau)

### Cas pratique : Parser de logs

**Code initial** (lent) :

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    char ip[16];
    int status;
    long bytes;
} LogEntry;

void parse_log_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    char line[1024];
    LogEntry entry;
    int count = 0;

    while (fgets(line, sizeof(line), f)) {
        // Parse chaque ligne
        sscanf(line, "%s - - [%*[^]]] \"%*s %*s %*s\" %d %ld",
               entry.ip, &entry.status, &entry.bytes);
        count++;
    }

    fclose(f);
    printf("Parsed %d lines\n", count);
}
```

**Profiling avec perf** :

```bash
$ perf record ./parser access.log
$ perf report

# Overhead  Command  Shared Object  Symbol
    45.23%  parser   libc.so        [.] __GI__IO_getc
    23.45%  parser   libc.so        [.] _IO_getc
    15.67%  parser   libc.so        [.] sscanf
     8.90%  parser   parser         [.] parse_log_file
```

**Diagnostic** :
- 68% du temps dans `fgets` (lecture caractère par caractère)
- 15% dans `sscanf` (parsing inefficace)

**Optimisation** :

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void parse_log_file_optimized(const char *filename) {
    // Memory-map le fichier entier
    int fd = open(filename, O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);

    char *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    char *line = data;
    char *end = data + sb.st_size;
    int count = 0;

    while (line < end) {
        // Trouver la fin de ligne (parsing manuel, plus rapide que sscanf)
        char *newline = memchr(line, '\n', end - line);
        if (!newline) break;

        // Parse rapide (extraction directe sans sscanf)
        // ...

        count++;
        line = newline + 1;
    }

    munmap(data, sb.st_size);
    close(fd);
    printf("Parsed %d lines\n", count);
}
```

**Résultat** : 10x plus rapide
- `fgets` → `mmap` : Accès mémoire au lieu de syscalls répétés
- `sscanf` → parsing manuel : Élimination de l'overhead de parsing générique

---

## Cas pratiques d'optimisation

### Cas 1 : Allocation excessive

**Symptôme** : Programme lent avec beaucoup de temps dans `malloc`/`free`.

**Code problématique** :

```c
void process_items(Item *items, int count) {
    for (int i = 0; i < count; i++) {
        char *buffer = malloc(1024);  // ❌ Allocation dans la boucle

        process_item(&items[i], buffer);

        free(buffer);
    }
}
```

**Profiling avec perf** :

```
30.45%  malloc
25.67%  free
20.34%  process_item
```

55% du temps dans malloc/free !

**Solution** : Réutiliser le buffer

```c
void process_items_optimized(Item *items, int count) {
    char *buffer = malloc(1024);  // ✅ Une seule allocation

    for (int i = 0; i < count; i++) {
        process_item(&items[i], buffer);
    }

    free(buffer);
}
```

**Résultat** : 3x plus rapide

### Cas 2 : Cache misses

**Symptôme** : Beaucoup de cache misses avec perf.

**Code problématique** :

```c
typedef struct {
    int id;
    char padding[124];  // Structure de 132 bytes
    int value;
} Data;

int sum_values(Data *array, int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += array[i].value;  // ❌ Accède à value (offset 128)
    }
    return sum;
}
```

**Profiling** :

```bash
$ perf stat -e cache-misses ./program
  12,345,678  cache-misses  # 15% de cache miss rate ❌
```

**Problème** : Structure mal alignée, accès mémoire espacés.

**Solution 1** : Structure compacte

```c
typedef struct {
    int value;      // ✅ value au début
    int id;
    char padding[120];
} Data;
```

**Solution 2** : Séparation des données (SoA - Structure of Arrays)

```c
// Au lieu de : Array of Structures (AoS)
Data array[10000];

// Utiliser : Structure of Arrays (SoA)
typedef struct {
    int *values;  // Array contigu
    int *ids;
    int count;
} DataSet;

int sum_values_soa(DataSet *dataset) {
    int sum = 0;
    for (int i = 0; i < dataset->count; i++) {
        sum += dataset->values[i];  // ✅ Accès contigu, cache-friendly
    }
    return sum;
}
```

**Résultat** :

```bash
$ perf stat -e cache-misses ./program_optimized
  1,234,567  cache-misses  # 1.5% de cache miss rate ✅ (10x mieux)
```

### Cas 3 : Algorithme inefficace

**Symptôme** : Le code est "correct" mais horriblement lent sur de gros volumes.

**Code problématique** :

```c
// Trouver les doublons dans un tableau
int find_duplicates(int *array, int size) {
    int count = 0;

    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {  // ❌ O(n²)
            if (array[i] == array[j]) {
                count++;
                break;
            }
        }
    }

    return count;
}
```

**Performance** :
- 1 000 éléments : 0.01s
- 10 000 éléments : 1s
- 100 000 éléments : 100s ❌

**Solution** : Algorithme plus efficace

```c
#include <stdlib.h>

int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int find_duplicates_optimized(int *array, int size) {
    // Trier d'abord O(n log n)
    int *sorted = malloc(size * sizeof(int));
    memcpy(sorted, array, size * sizeof(int));
    qsort(sorted, size, sizeof(int), compare_ints);

    // Compter les doublons O(n)
    int count = 0;
    for (int i = 1; i < size; i++) {
        if (sorted[i] == sorted[i-1]) {
            count++;
        }
    }

    free(sorted);
    return count;
}
```

**Performance** : O(n log n) au lieu de O(n²)
- 100 000 éléments : 0.05s ✅ (2000x plus rapide)

### Cas 4 : I/O non bufferisé

**Symptôme** : Beaucoup d'appels système `write` avec strace.

**Code problématique** :

```c
void write_log(const char *message) {
    int fd = open("log.txt", O_WRONLY | O_APPEND);
    write(fd, message, strlen(message));  // ❌ Syscall à chaque message
    close(fd);
}

int main() {
    for (int i = 0; i < 10000; i++) {
        write_log("Event occurred\n");  // 10000 syscalls !
    }
}
```

**Profiling avec strace** :

```bash
$ strace -c ./program
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 95.67    1.234567         123     10000           write
  2.34    0.030234           3     10000           open
  1.99    0.025678           2     10000           close
```

95% du temps dans les syscalls !

**Solution** : Buffer les écritures

```c
FILE *log_file = NULL;

void open_log() {
    log_file = fopen("log.txt", "a");
    setvbuf(log_file, NULL, _IOFBF, 8192);  // Buffer de 8 KB
}

void write_log_buffered(const char *message) {
    fputs(message, log_file);  // ✅ Bufferisé, syscall seulement quand buffer plein
}

void close_log() {
    fclose(log_file);  // Flush automatique
}

int main() {
    open_log();
    for (int i = 0; i < 10000; i++) {
        write_log_buffered("Event occurred\n");
    }
    close_log();
}
```

**Résultat** : 50x plus rapide (10000 syscalls → ~20 syscalls)

---

## Optimisations courantes

### 1. Éviter les copies inutiles

```c
// ❌ MAL : Copie de structure
void process_data(Data value) {  // Passage par valeur
    // ...
}

// ✅ BIEN : Passage par référence
void process_data(const Data *value) {  // Passage par pointeur
    // ...
}
```

### 2. Précalculer les valeurs

```c
// ❌ MAL : Calcul répété
for (int i = 0; i < 1000; i++) {
    double result = expensive_calculation() * array[i];
}

// ✅ BIEN : Calcul une seule fois
double factor = expensive_calculation();  
for (int i = 0; i < 1000; i++) {  
    double result = factor * array[i];
}
```

### 3. Loop unrolling (déroulement de boucle)

```c
// ❌ Normal
for (int i = 0; i < 1000; i++) {
    sum += array[i];
}

// ✅ Unrolled (le compilateur peut le faire avec -O3)
for (int i = 0; i < 1000; i += 4) {
    sum += array[i];
    sum += array[i+1];
    sum += array[i+2];
    sum += array[i+3];
}
```

### 4. Branch prediction friendly

```c
// ❌ MAL : Branches imprévisibles
for (int i = 0; i < size; i++) {
    if (array[i] % 2 == 0) {  // 50/50 unpredictable
        process_even(array[i]);
    } else {
        process_odd(array[i]);
    }
}

// ✅ MIEUX : Séparer les cas
int even_count = 0, odd_count = 0;  
int *even = malloc(size * sizeof(int));  
int *odd = malloc(size * sizeof(int));  

for (int i = 0; i < size; i++) {
    if (array[i] % 2 == 0) {
        even[even_count++] = array[i];
    } else {
        odd[odd_count++] = array[i];
    }
}

// Traiter par lots (moins de branches)
for (int i = 0; i < even_count; i++) {
    process_even(even[i]);
}
for (int i = 0; i < odd_count; i++) {
    process_odd(odd[i]);
}
```

### 5. Utiliser les fonctions optimisées

```c
// ❌ Lent : Boucle manuelle
void copy_array(int *dest, const int *src, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

// ✅ Rapide : Fonction optimisée (SIMD, etc.)
memcpy(dest, src, size * sizeof(int));
```

---

## Flags de compilation d'optimisation

### Niveaux d'optimisation GCC

```bash
# Aucune optimisation (debug)
gcc -O0 -g -o myapp myapp.c

# Optimisations de base
gcc -O1 -o myapp myapp.c

# Optimisations standard (recommandé)
gcc -O2 -o myapp myapp.c

# Optimisations agressives
gcc -O3 -o myapp myapp.c

# Optimisation pour la taille
gcc -Os -o myapp myapp.c
```

### Optimisations spécifiques

```bash
# Optimiser pour l'architecture native
gcc -O3 -march=native -o myapp myapp.c

# Link-Time Optimization (LTO)
gcc -O3 -flto -o myapp myapp.c

# Profile-Guided Optimization (PGO)
# 1. Compiler avec instrumentation
gcc -O3 -fprofile-generate -o myapp myapp.c

# 2. Exécuter avec données représentatives
./myapp < typical_input.txt

# 3. Recompiler avec les profils
gcc -O3 -fprofile-use -o myapp myapp.c
```

### Comparaison d'impact

```bash
# Test avec un programme réel
$ gcc -O0 -o app_O0 myapp.c
$ gcc -O2 -o app_O2 myapp.c
$ gcc -O3 -o app_O3 myapp.c

$ time ./app_O0
real    0m5.234s

$ time ./app_O2
real    0m1.456s  # 3.6x plus rapide

$ time ./app_O3
real    0m1.123s  # 4.7x plus rapide
```

---

## Benchmarking rigoureux

### Éviter les pièges

#### Piège 1 : Optimisation du code mort

```c
// ❌ Le compilateur peut éliminer le code
int benchmark_bad() {
    int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    // sum n'est jamais utilisé → optimisé away
    return 0;
}

// ✅ Forcer l'utilisation du résultat
int benchmark_good() {
    int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    // Empêcher l'optimisation avec un volatile ou un side-effect
    volatile int result = sum;
    return result;
}
```

#### Piège 2 : Cache warming

```c
// ❌ Première itération inclut le cache cold
for (int run = 0; run < 10; run++) {
    start = get_time();
    process_data();
    end = get_time();
    printf("Run %d: %f\n", run, end - start);
}

// ✅ Exclure les premières itérations
process_data();  // Warmup  
process_data();  // Warmup  

for (int run = 0; run < 10; run++) {
    start = get_time();
    process_data();
    end = get_time();
    printf("Run %d: %f\n", run, end - start);
}
```

#### Piège 3 : Variabilité système

```c
// ✅ Benchmark statistiquement valide
#define ITERATIONS 100

double times[ITERATIONS];

for (int i = 0; i < ITERATIONS; i++) {
    double start = get_time();
    process_data();
    double end = get_time();
    times[i] = end - start;
}

// Calculer médiane et percentiles
qsort(times, ITERATIONS, sizeof(double), compare_double);  
printf("Median: %f\n", times[ITERATIONS/2]);  
printf("P95: %f\n", times[(int)(ITERATIONS * 0.95)]);  
printf("P99: %f\n", times[(int)(ITERATIONS * 0.99)]);  
```

### Micro-benchmark robuste

```c
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define WARMUP_RUNS 10
#define BENCHMARK_RUNS 1000

double benchmark_function(void (*func)(void)) {
    double *times = malloc(BENCHMARK_RUNS * sizeof(double));

    // Warmup
    for (int i = 0; i < WARMUP_RUNS; i++) {
        func();
    }

    // Benchmark
    for (int i = 0; i < BENCHMARK_RUNS; i++) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        func();

        clock_gettime(CLOCK_MONOTONIC, &end);
        times[i] = (end.tv_sec - start.tv_sec) +
                   (end.tv_nsec - start.tv_nsec) / 1e9;
    }

    // Calculer la médiane
    qsort(times, BENCHMARK_RUNS, sizeof(double), compare_double);
    double median = times[BENCHMARK_RUNS / 2];

    free(times);
    return median;
}
```

---

## Outils système

### htop : Monitoring en temps réel

```bash
htop -p $(pidof myapp)
```

Permet de voir :
- Utilisation CPU par thread
- Mémoire RSS
- I/O disk et network

### iotop : Monitoring I/O

```bash
sudo iotop -o  # Seulement les processus avec I/O
```

Identifie les processus qui font beaucoup d'I/O disque.

### nethogs : Monitoring réseau

```bash
sudo nethogs
```

Identifie les processus qui consomment de la bande passante.

---

## Checklist de performance debugging

### Phase de diagnostic

- [ ] Mesurer le temps d'exécution global (`time`)
- [ ] Identifier si CPU-bound ou I/O-bound
- [ ] Profiler avec perf ou gprof
- [ ] Identifier les 2-3 fonctions les plus coûteuses (hotspots)
- [ ] Mesurer les cache misses avec perf
- [ ] Vérifier les allocations mémoire excessives

### Phase d'analyse

- [ ] L'algorithme est-il optimal ? (complexité O)
- [ ] Les structures de données sont-elles adaptées ?
- [ ] Y a-t-il des copies inutiles ?
- [ ] Les I/O sont-elles bufferisées ?
- [ ] Les accès mémoire sont-ils cache-friendly ?

### Phase d'optimisation

- [ ] Optimiser les hotspots identifiés (loi de Amdahl)
- [ ] Compiler avec `-O2` ou `-O3`
- [ ] Considérer `-march=native` si pas de portabilité
- [ ] Utiliser les fonctions optimisées (memcpy, etc.)
- [ ] Benchmark avant/après chaque optimisation

### Phase de validation

- [ ] Mesurer l'amélioration de performance
- [ ] Vérifier que les résultats sont toujours corrects
- [ ] Tester avec des données réalistes
- [ ] Valider en conditions de charge (stress test)

---

## Cas d'étude : Optimisation d'un serveur HTTP

### Contexte

Un serveur HTTP traite 100 req/s. Objectif : atteindre 1000 req/s.

### Diagnostic initial

```bash
$ perf record -g ./http_server
$ perf report

# Overhead  Command  Symbol
    45.23%  http     [.] read_request
    23.45%  http     [.] parse_http_header
    15.67%  http     [.] malloc
     8.90%  http     [.] send_response
```

**Hotspots** :
1. `read_request` : 45% (I/O)
2. `parse_http_header` : 23% (parsing)
3. `malloc` : 15% (allocations)

### Optimisation 1 : Buffer pooling

**Avant** :

```c
void handle_request(int socket) {
    char *buffer = malloc(4096);  // ❌ Allocation par requête
    read(socket, buffer, 4096);
    // ... traitement ...
    free(buffer);
}
```

**Après** :

```c
#define POOL_SIZE 100
char buffer_pool[POOL_SIZE][4096];  
int pool_available[POOL_SIZE] = {1, 1, ...};  
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;  

char* get_buffer() {
    pthread_mutex_lock(&pool_mutex);
    for (int i = 0; i < POOL_SIZE; i++) {
        if (pool_available[i]) {
            pool_available[i] = 0;
            pthread_mutex_unlock(&pool_mutex);
            return buffer_pool[i];
        }
    }
    pthread_mutex_unlock(&pool_mutex);
    return NULL;
}

void handle_request(int socket) {
    char *buffer = get_buffer();  // ✅ Pool
    read(socket, buffer, 4096);
    // ... traitement ...
    release_buffer(buffer);
}
```

**Résultat** : 200 req/s (+100%)

### Optimisation 2 : Parser zéro-copie

**Avant** :

```c
char* parse_header(const char *request) {
    char *header = malloc(256);
    sscanf(request, "GET %s HTTP", header);  // ❌ Copie + parsing lent
    return header;
}
```

**Après** :

```c
typedef struct {
    const char *start;
    size_t length;
} StringView;

StringView parse_header(const char *request) {
    StringView view;
    view.start = request + 4;  // Skip "GET "

    const char *space = strchr(view.start, ' ');
    view.length = space - view.start;

    return view;  // ✅ Pas de malloc, pas de copie
}
```

**Résultat** : 500 req/s (+250%)

### Optimisation 3 : epoll au lieu de threads

**Avant** : 1 thread par connexion (coûteux en ressources)

**Après** : Event loop avec epoll (1 thread gère des milliers de connexions)

```c
int epoll_fd = epoll_create1(0);

struct epoll_event event;  
event.events = EPOLLIN;  
event.data.fd = listen_socket;  
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_socket, &event);  

while (1) {
    struct epoll_event events[MAX_EVENTS];
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
        if (events[i].data.fd == listen_socket) {
            int client = accept(listen_socket, NULL, NULL);
            // Ajouter à epoll
        } else {
            // Traiter la requête (non-blocking)
            handle_request_nonblocking(events[i].data.fd);
        }
    }
}
```

**Résultat** : 1200 req/s (+1200%) 🎉

---

## Résumé

Le performance debugging est un processus **méthodique** et **basé sur les données**. Points essentiels :

### Outils indispensables
- **perf** : Profiling moderne avec compteurs matériels
- **Valgrind Callgrind** : Analyse détaillée et cache misses
- **gprof** : Profiling basique mais accessible
- **time/strace** : Diagnostic initial rapide

### Méthodologie
1. **Mesurer** : Profiler avant d'optimiser
2. **Analyser** : Comprendre les hotspots
3. **Optimiser** : Cibler les 20% qui causent 80% du problème
4. **Vérifier** : Mesurer l'amélioration

### Optimisations efficaces
- Algorithmes : O(n²) → O(n log n) = gains énormes
- Réduire allocations : Pool, réutilisation de buffers
- Cache-friendly : Accès mémoire contigus (SoA)
- I/O : Bufferiser, batch, async
- Compilation : `-O2`, `-O3`, `-march=native`

### Règles d'or
- **Mesurer, ne pas deviner** : L'intuition est souvent fausse
- **Optimiser les hotspots** : 20% du code = 80% du temps
- **Benchmark** : Valider chaque optimisation
- **Ne pas sur-optimiser** : Équilibre performance/maintenabilité

**La performance n'est pas une réflexion après-coup, c'est une exigence dès la conception.**

---


⏭️ [Références essentielles](/annexes/references-essentielles/README.md)
