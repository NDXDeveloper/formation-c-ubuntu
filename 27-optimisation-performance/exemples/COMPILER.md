# Chapitre 27 - Optimisation et performance : Compilation des exemples

## Flags standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17
```

La plupart des exemples sont compilés avec `-O2` pour que les benchmarks soient significatifs.

---

## Section 27.3 : Profiling (01-03)

### 01_test_perf.c
- **Section** : 27.3 - Profiling (gprof)
- **Description** : Programme de test pour profiling gprof - fonctions lentes, rapides et intermédiaires
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 01_test_perf.c -o 01_test_perf`
- **Profiling** : `gcc -pg -O2 01_test_perf.c -o 01_test_perf_pg && ./01_test_perf_pg && gprof 01_test_perf_pg gmon.out`
- **Sortie attendue** :
```
Debut du programme
Fin du programme
```

### 02_test_perf2.c
- **Section** : 27.3 - Profiling (perf)
- **Description** : Calcul intensif et allocations mémoire pour identifier les hotspots avec perf
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 02_test_perf2.c -o 02_test_perf2`
- **Profiling** : `perf record ./02_test_perf2 && perf report`
- **Sortie attendue** :
```
Debut du profiling
Total calcul : 4.99985e+12
Fin du profiling
```

### 03_sort_benchmark.c
- **Section** : 27.3 - Profiling (workflow complet)
- **Description** : Bubble sort O(n²) vs qsort O(n log n) - démonstration d'optimisation guidée par profiling
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 03_sort_benchmark.c -o 03_sort_benchmark`
- **Note** : Le bubble sort est volontairement lent (~5s sur 50000 éléments)
- **Sortie attendue** :
```
Bubble sort (50000 elements) : ~5.9 s
qsort       (50000 elements) : ~0.005 s
Ratio : ~1200x plus rapide avec qsort
```

---

## Section 27.4 : Cache awareness (04-05)

### 04_test_cache.c
- **Section** : 27.4 - Cache awareness
- **Description** : Parcours ligne (cache-friendly) vs colonne (cache-hostile) d'une matrice 1000×1000
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 04_test_cache.c -o 04_test_cache`
- **Sortie attendue** :
```
Parcours ligne   (100x) : ~16 ms
Parcours colonne (100x) : ~177 ms
Ratio : colonne ~10x plus lent
```

### 05_sum_versions.c
- **Section** : 27.4 - Cache awareness (cas d'étude)
- **Description** : 3 versions de somme de tableau (naïve, loop unrolling, multi-accumulateurs ILP)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 05_sum_versions.c -o 05_sum_versions`
- **Sortie attendue** :
```
Naive          : ~3 ms   (somme = 49999995000000)
Loop unrolling : ~0.4 ms (somme = 49999995000000)
Multi-accum.   : ~0.3 ms (somme = 49999995000000)
```

---

## Section 27.5 : Branch prediction (06-07)

### 06_test_branch.c
- **Section** : 27.5 - Branch prediction
- **Description** : Condition prévisible (tableau trié) vs imprévisible (aléatoire)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 06_test_branch.c -o 06_test_branch`
- **Note** : La différence de performance est mieux visible avec `perf stat -e branches,branch-misses`
- **Sortie attendue** :
```
Condition previsible (trie) :
  Somme : 957521390
  Temps : ~4 ms
Condition imprevisible (aleatoire) :
  Somme : 957521390
  Temps : ~4 ms
```

### 07_benchmark_branches.c
- **Section** : 27.5 - Branch prediction
- **Description** : Branches prévisibles (pattern i%2) vs imprévisibles (pattern pseudo-aléatoire)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 07_benchmark_branches.c -o 07_benchmark_branches`
- **Profiling** : `perf stat -e branches,branch-misses ./07_benchmark_branches`
- **Sortie attendue** :
```
=== Branches previsibles ===
  Somme (previsible)   : ...
  Temps : ~97 ms
=== Branches imprevisibles ===
  Somme (imprevisible) : ...
  Temps : ~93 ms
```

---

## Section 27.6 : Optimisations algorithmiques (08-09)

### 08_test_recherche.c
- **Section** : 27.6 - Optimisations algorithmiques
- **Description** : Recherche linéaire O(n) vs dichotomique O(log n) sur 1 million d'éléments
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 08_test_recherche.c -o 08_test_recherche`
- **Sortie attendue** :
```
Recherche lineaire      : ~619 ms  (index = 999999)
Recherche dichotomique  : ~0.0 ms  (index = 999999)
```

### 09_test_tri.c
- **Section** : 27.6 - Optimisations algorithmiques
- **Description** : Tri à bulles O(n²) vs quicksort O(n log n) sur 10000 éléments
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 09_test_tri.c -o 09_test_tri`
- **Sortie attendue** :
```
Tri a bulles : ~221 ms
Quicksort    : ~0.5 ms
Resultats identiques : oui
```

---

## Section 27.7 : Vectorisation et SIMD (10-12)

### 10_add_arrays.c
- **Section** : 27.7 - Vectorisation (auto-vectorisation)
- **Description** : Addition de 10M floats - compiler avec -O2 vs -O3 -march=native pour voir la vectorisation automatique
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 10_add_arrays.c -o 10_add_arrays`
- **Vectorisation** : `gcc -O3 -march=native -fopt-info-vec-optimized 10_add_arrays.c -o 10_add_arrays_vec`
- **Sortie attendue** :
```
Addition de 10000000 floats (10 iterations)
Temps moyen : ~12 ms
Verification : c[42] = 126.0 (attendu: 126.0)
```

### 11_add_sse.c
- **Section** : 27.7 - Vectorisation (SSE intrinsics)
- **Description** : Addition SSE (4 floats à la fois) vs scalaire avec aligned_alloc
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 -O2 -msse 11_add_sse.c -o 11_add_sse`
- **Note** : Sans `-pedantic` (intrinsics SIMD)
- **Sortie attendue** :
```
Scalaire : ~12 ms
SSE      : ~11 ms
```

### 12_add_avx.c
- **Section** : 27.7 - Vectorisation (AVX intrinsics)
- **Description** : Addition AVX (8 floats à la fois) avec aligned_alloc 32 bytes
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 -O2 -mavx 12_add_avx.c -o 12_add_avx`
- **Note** : Sans `-pedantic` (intrinsics SIMD)
- **Sortie attendue** :
```
AVX : ~11 ms (moyenne sur 10 iterations)
Verification : c[42] = 126.0 (attendu: 126.0)
```

---

## Section 27.8 : Link-Time Optimization (13)

### 13_lto_demo/ (multi-fichiers)
- **Section** : 27.8 - LTO
- **Fichiers** : `math_utils.h`, `math_utils.c`, `main.c`
- **Description** : Démonstration LTO - power_of_two non utilisée est éliminée, square/cube sont inlinées
- **Compilation sans LTO** :
```bash
cd 13_lto_demo
gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 -c math_utils.c -o math_utils.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 -c main.c -o main.o
gcc -O2 math_utils.o main.o -o programme_sans_lto
```
- **Compilation avec LTO** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 -flto -c math_utils.c -o math_utils.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 -flto -c main.c -o main.o
gcc -O2 -flto math_utils.o main.o -o programme_avec_lto
```
- **Sortie attendue** :
```
Square(10) = 100
Cube(10)   = 1000
```
- **Comparaison** : `ls -la programme_sans_lto programme_avec_lto` (LTO produit un binaire plus petit)

---

## Section 27.9 : Profile-Guided Optimization (14-15)

### 14_branch_pgo.c
- **Section** : 27.9 - PGO
- **Description** : Branchement 99% hot / 1% cold - compiler avec -fprofile-generate puis -fprofile-use
- **Compilation standard** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 14_branch_pgo.c -o 14_branch_pgo`
- **Compilation PGO** :
```bash
gcc -O2 -fprofile-generate 14_branch_pgo.c -o 14_branch_instr
./14_branch_instr
gcc -O2 -fprofile-use 14_branch_pgo.c -o 14_branch_pgo_opt
```
- **Sortie attendue** :
```
Resultat : 48009995100000
```

### 15_sort_pgo.c
- **Section** : 27.9 - PGO (exemple avancé)
- **Description** : Benchmark qsort avec distribution non uniforme (70% petites, 30% grandes valeurs)
- **Compilation standard** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 15_sort_pgo.c -o 15_sort_pgo`
- **Compilation PGO** :
```bash
gcc -O3 -fprofile-generate 15_sort_pgo.c -o 15_sort_instr
./15_sort_instr
gcc -O3 -fprofile-use 15_sort_pgo.c -o 15_sort_pgo_opt
```
- **Sortie attendue** :
```
Temps de tri : ~107 ms
```

---

## Section 27.10 : Benchmarking (16-18)

### 16_benchmark_simple.c
- **Section** : 27.10 - Benchmarking (template robuste)
- **Description** : Template de benchmark avec warm-up, statistiques (moyenne, écart-type, min, max) et barrière anti-optimisation
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 -O2 16_benchmark_simple.c -lm -o 16_benchmark_simple`
- **Note** : Sans `-pedantic` (`__asm__ __volatile__` extension GCC). Nécessite `-lm` (sqrt).
- **Sortie attendue** :
```
=== Resultats du benchmark ===
Iterations : 1000000
Runs       : 10
Temps moyen : ~0.4 ns
Ecart-type  : ~0.2 ns (~42%)
Min         : ~0.3 ns
Max         : ~0.8 ns
```

### 17_compare_versions.c
- **Section** : 27.10 - Benchmarking (comparaison)
- **Description** : Boucle O(n) vs formule O(1) pour somme 0..n avec médiane sur 10 runs
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 17_compare_versions.c -o 17_compare_versions`
- **Sortie attendue** :
```
=== Comparaison de performances ===
Taille : 10000, Iterations : 100000, Runs : 10
Baseline  (boucle)  : ~447 ms
Optimisee (formule) : ~0.167 ms
Speedup      : ~2678x
Amelioration : 100.0%
```

### 18_hash_benchmark.c
- **Section** : 27.10 - Benchmarking (exemple complet)
- **Description** : Benchmark djb2 vs FNV-1a sur 1M chaînes avec temps par hash en ns
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 18_hash_benchmark.c -o 18_hash_benchmark`
- **Sortie attendue** :
```
=== Benchmark : Hash Functions ===
Donnees : 1000000 chaines
Runs    : 20
djb2   : ~10 ms (~10 ns/hash)
FNV-1a : ~10 ms (~10 ns/hash)
FNV-1a est ~1.06x plus rapide
```

---

## Résumé des exceptions de compilation

| Fichier | Exception | Raison |
|---------|-----------|--------|
| 11_add_sse.c | Sans `-pedantic`, `-msse` | Intrinsics SIMD SSE |
| 12_add_avx.c | Sans `-pedantic`, `-mavx` | Intrinsics SIMD AVX |
| 16_benchmark_simple.c | Sans `-pedantic`, `-lm` | `__asm__ __volatile__`, sqrt() |

## Sections sans exemples compilables

| Section | Raison |
|---------|--------|
| 27.1 - Flags d'optimisation | Uniquement commandes shell et fragments illustratifs |
| 27.2 - Comprendre l'optimiseur | Uniquement fragments illustrant les transformations du compilateur |
