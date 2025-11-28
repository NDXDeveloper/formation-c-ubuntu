🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.10 Benchmarking rigoureux

## Introduction

Le **benchmarking** est l'art de mesurer les performances d'un programme de manière **fiable, reproductible et significative**. C'est une compétence essentielle car :

> "You can't optimize what you don't measure" — Proverbe de l'optimisation

Sans benchmarking rigoureux, vous risquez de :
- ❌ Optimiser la mauvaise partie du code
- ❌ Croire qu'une optimisation fonctionne alors qu'elle ralentit le programme
- ❌ Obtenir des résultats non reproductibles
- ❌ Tirer des conclusions erronées de mesures bruitées

### Analogie : Le chronomètre de course

**Mauvais benchmark (chronomètre approximatif) :**

Vous chronométrez un coureur avec une montre ordinaire :
- Vous démarrez "à peu près" au départ
- Vous arrêtez "à peu près" à l'arrivée
- Résultat : "environ 10 secondes" ± 2 secondes

**Bon benchmark (chronomètre de précision) :**

Vous utilisez un chronomètre électronique professionnel :
- Départ au centième de seconde près
- Arrivée au centième de seconde près
- Plusieurs essais pour éliminer les variations
- Conditions identiques (même piste, même météo, même coureur)
- Résultat : "10.23 secondes" ± 0.02 secondes

Même principe pour benchmarker du code !

---

## Pourquoi le benchmarking est difficile

### Les sources de bruit

Les mesures de performance peuvent être affectées par :

1. **L'état du cache CPU** (chaud vs froid)
2. **Les autres processus** en cours d'exécution
3. **Le CPU throttling** (réduction de fréquence)
4. **Les interruptions système**
5. **Le garbage collector** (pour les langages managés)
6. **La latence réseau** (si applicable)
7. **L'état du disque** (I/O en cours)
8. **La température du CPU** (thermal throttling)
9. **Les optimisations du compilateur** (peuvent éliminer du code)
10. **Les variations de temps système**

**Exemple concret :**

```bash
# Même commande, résultats différents !
$ time ./mon_programme
real    0m0.123s

$ time ./mon_programme
real    0m0.145s  ← +18% de différence !

$ time ./mon_programme
real    0m0.119s
```

**Pourquoi ?** Cache froid/chaud, autres processus, interruptions...

---

## Principes du benchmarking rigoureux

### 1. Isoler le système

Minimiser les interférences externes :

```bash
# ❌ Mauvais : Beaucoup de programmes en arrière-plan
# Navigateur ouvert, musique, compilation en cours...
./benchmark

# ✅ Bon : Environnement contrôlé
# Fermer tous les programmes inutiles
# Désactiver les services non essentiels
sudo systemctl stop bluetooth
sudo systemctl stop cups
# Etc.
```

**Encore mieux : Isoler un CPU**

```bash
# Réserver le CPU 3 uniquement pour le benchmark
sudo isolcpus=3

# Lancer le programme sur ce CPU
taskset -c 3 ./benchmark
```

### 2. Mesurer plusieurs fois

**Ne jamais faire confiance à une seule mesure !**

```bash
# ❌ Mauvais : Une seule mesure
time ./programme

# ✅ Bon : Plusieurs mesures
for i in {1..10}; do
    time ./programme
done
```

**Analyse statistique :**
- Calculer la **moyenne**
- Calculer l'**écart-type** (pour voir la variabilité)
- Éliminer les **outliers** (valeurs aberrantes)
- Rapporter le **minimum** (meilleur cas) et le **médian** (typique)

### 3. Réchauffer le cache (warm-up)

La première exécution est souvent plus lente (cache froid).

```c
// ❌ Mauvais : Mesurer la première exécution
clock_t start = clock();
fonction_a_benchmarker();
clock_t end = clock();
printf("Temps: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

// ✅ Bon : Réchauffer d'abord
fonction_a_benchmarker();  // Warm-up (non mesuré)
fonction_a_benchmarker();  // Warm-up

clock_t start = clock();
fonction_a_benchmarker();  // Mesure avec cache chaud
clock_t end = clock();
```

### 4. Exécuter suffisamment longtemps

Si votre fonction s'exécute en 1 microseconde, la précision de `clock()` n'est pas suffisante.

```c
// ❌ Mauvais : Fonction trop rapide
clock_t start = clock();
fonction_rapide();  // 1 microseconde
clock_t end = clock();
// Précision insuffisante, mesure = 0 !

// ✅ Bon : Boucler pour obtenir un temps mesurable
clock_t start = clock();
for (int i = 0; i < 1000000; i++) {
    fonction_rapide();
}
clock_t end = clock();
double temps_moyen = (double)(end - start) / CLOCKS_PER_SEC / 1000000;
```

### 5. Empêcher les optimisations indésirables du compilateur

Le compilateur peut **éliminer** du code qu'il juge inutile !

```c
// ❌ Mauvais : Le compilateur peut tout éliminer
int resultat = calcul_complexe();
// resultat n'est jamais utilisé → tout le calcul peut être éliminé !

// ✅ Bon : Utiliser le résultat
int resultat = calcul_complexe();
printf("%d\n", resultat);  // Force le calcul

// ✅ Encore mieux : volatile ou __asm__
volatile int resultat = calcul_complexe();

// Ou forcer le compilateur à ne pas optimiser
__asm__ __volatile__("" : : "r,m"(resultat) : "memory");
```

---

## Outils de mesure

### 1. clock() (Temps CPU)

**Avantages :**
- ✅ Simple
- ✅ Standard C
- ✅ Mesure uniquement le temps CPU du programme

**Inconvénients :**
- ❌ Précision limitée (~1 ms)
- ❌ Ne compte pas le temps d'attente (I/O, sleep)

**Utilisation :**

```c
#include <time.h>

clock_t start = clock();
// Code à mesurer
clock_t end = clock();

double temps_cpu = (double)(end - start) / CLOCKS_PER_SEC;
printf("Temps CPU: %.6f secondes\n", temps_cpu);
```

### 2. gettimeofday() (Temps réel)

**Avantages :**
- ✅ Précision microseconde
- ✅ Mesure le temps "horloge murale" (wall-clock)

**Inconvénients :**
- ❌ Affecté par les changements d'heure système
- ❌ Moins précis que clock_gettime()

**Utilisation :**

```c
#include <sys/time.h>

struct timeval start, end;

gettimeofday(&start, NULL);
// Code à mesurer
gettimeofday(&end, NULL);

double elapsed = (end.tv_sec - start.tv_sec) +
                 (end.tv_usec - start.tv_usec) / 1000000.0;
printf("Temps écoulé: %.6f secondes\n", elapsed);
```

### 3. clock_gettime() (Recommandé, haute précision)

**Avantages :**
- ✅ Haute précision (nanoseconde)
- ✅ Plusieurs horloges disponibles
- ✅ Non affecté par les changements d'heure

**Horloges disponibles :**
- `CLOCK_MONOTONIC` : Temps monotone (ne recule jamais)
- `CLOCK_PROCESS_CPUTIME_ID` : Temps CPU du processus
- `CLOCK_THREAD_CPUTIME_ID` : Temps CPU du thread
- `CLOCK_REALTIME` : Temps réel (peut être ajusté)

**Utilisation :**

```c
#include <time.h>

struct timespec start, end;

clock_gettime(CLOCK_MONOTONIC, &start);
// Code à mesurer
clock_gettime(CLOCK_MONOTONIC, &end);

double elapsed = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1000000000.0;
printf("Temps écoulé: %.9f secondes\n", elapsed);
```

### 4. Compteurs de cycles CPU (RDTSC)

**Avantages :**
- ✅ Précision maximale (cycle CPU)
- ✅ Très rapide (quelques cycles)

**Inconvénients :**
- ❌ Architecture spécifique (x86)
- ❌ Affecté par la fréquence variable du CPU (Turbo Boost)
- ❌ Complexe à utiliser correctement

**Utilisation :**

```c
#include <x86intrin.h>

unsigned long long start = __rdtsc();
// Code à mesurer (très court)
unsigned long long end = __rdtsc();

unsigned long long cycles = end - start;
printf("Cycles CPU: %llu\n", cycles);
```

**Note :** Utilisez `__rdtscp()` pour un ordonnancement correct des instructions.

### 5. perf (Compteurs matériels)

**Le plus complet et précis pour Linux !**

```bash
# Mesurer le temps d'exécution et les métriques
perf stat ./programme

# Exemple de sortie :
#  Performance counter stats for './programme':
#
#           1234.56 msec task-clock                #    0.998 CPUs utilized
#                12      context-switches          #    9.718 /sec
#                 0      cpu-migrations            #    0.000 /sec
#               156      page-faults               #  126.348 /sec
#      4,567,890,123      cycles                    #    3.698 GHz
#      6,789,012,345      instructions              #    1.49  insn per cycle
#      1,234,567,890      branches                  # 1000.123 M/sec
#         12,345,678      branch-misses             #    1.00% of all branches
#
#        1.236789000 seconds time elapsed
```

---

## Template de benchmark robuste

### Benchmark simple mais correct

```c
// benchmark_simple.c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 1000000
#define WARMUP_RUNS 3
#define BENCHMARK_RUNS 10

// Fonction à benchmarker
int fonction_a_tester(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i;
    }
    return sum;
}

// Empêcher l'optimisation
void escape(void *p) {
    __asm__ __volatile__("" : : "g"(p) : "memory");
}

void clobber() {
    __asm__ __volatile__("" : : : "memory");
}

int main() {
    struct timespec start, end;
    double times[BENCHMARK_RUNS];

    // Warm-up
    for (int i = 0; i < WARMUP_RUNS; i++) {
        volatile int result = fonction_a_tester(1000);
        escape(&result);
    }

    // Benchmarks
    for (int run = 0; run < BENCHMARK_RUNS; run++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < ITERATIONS; i++) {
            volatile int result = fonction_a_tester(1000);
            escape(&result);
            clobber();
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed = (end.tv_sec - start.tv_sec) +
                         (end.tv_nsec - start.tv_nsec) / 1e9;
        times[run] = elapsed / ITERATIONS;  // Temps par itération
    }

    // Analyse statistique
    double sum = 0, min = times[0], max = times[0];
    for (int i = 0; i < BENCHMARK_RUNS; i++) {
        sum += times[i];
        if (times[i] < min) min = times[i];
        if (times[i] > max) max = times[i];
    }
    double mean = sum / BENCHMARK_RUNS;

    // Écart-type
    double variance = 0;
    for (int i = 0; i < BENCHMARK_RUNS; i++) {
        variance += (times[i] - mean) * (times[i] - mean);
    }
    double stddev = sqrt(variance / BENCHMARK_RUNS);

    printf("=== Résultats du benchmark ===\n");
    printf("Itérations: %d\n", ITERATIONS);
    printf("Runs: %d\n", BENCHMARK_RUNS);
    printf("\n");
    printf("Temps moyen:  %.3f ns\n", mean * 1e9);
    printf("Écart-type:   %.3f ns (%.1f%%)\n",
           stddev * 1e9, (stddev/mean)*100);
    printf("Min:          %.3f ns\n", min * 1e9);
    printf("Max:          %.3f ns\n", max * 1e9);

    return 0;
}
```

**Compilation :**

```bash
gcc -O2 -o benchmark benchmark_simple.c -lm
./benchmark
```

**Résultat typique :**

```
=== Résultats du benchmark ===
Itérations: 1000000
Runs: 10

Temps moyen:  523.456 ns
Écart-type:   12.345 ns (2.4%)
Min:          510.123 ns
Max:          545.678 ns
```

**Analyse :** Écart-type de 2.4% → Résultats stables ✅

---

## Comparer deux versions

### Méthodologie

Pour comparer deux implémentations (baseline vs optimisée) :

1. **Mesurer la baseline** (version de référence)
2. **Mesurer la version optimisée**
3. **Calculer le speedup** (amélioration)
4. **Vérifier la significativité statistique**

### Script de comparaison

```c
// compare_versions.c
#include <stdio.h>
#include <time.h>
#include <math.h>

#define ITERATIONS 1000000
#define RUNS 20

// Version 1 : Baseline
int version_baseline(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i;
    }
    return sum;
}

// Version 2 : Optimisée (formule mathématique)
int version_optimisee(int n) {
    return n * (n - 1) / 2;
}

double benchmark_function(int (*func)(int), int n) {
    struct timespec start, end;
    double times[RUNS];

    // Warm-up
    for (int i = 0; i < 3; i++) {
        volatile int r = func(n);
    }

    // Mesures
    for (int run = 0; run < RUNS; run++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < ITERATIONS; i++) {
            volatile int result = func(n);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        times[run] = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    }

    // Calculer la médiane (plus robuste que la moyenne)
    qsort(times, RUNS, sizeof(double), compare_double);
    return times[RUNS / 2];
}

int compare_double(const void *a, const void *b) {
    double diff = *(double*)a - *(double*)b;
    return (diff > 0) - (diff < 0);
}

int main() {
    int n = 10000;

    printf("=== Comparaison de performances ===\n");
    printf("Taille: %d, Itérations: %d, Runs: %d\n\n", n, ITERATIONS, RUNS);

    double time_baseline = benchmark_function(version_baseline, n);
    double time_optimisee = benchmark_function(version_optimisee, n);

    printf("Baseline:   %.3f ms\n", time_baseline * 1000);
    printf("Optimisée:  %.3f ms\n", time_optimisee * 1000);
    printf("\n");

    double speedup = time_baseline / time_optimisee;
    double improvement = ((time_baseline - time_optimisee) / time_baseline) * 100;

    printf("Speedup:       %.2fx\n", speedup);
    printf("Amélioration:  %.1f%%\n", improvement);

    if (speedup > 1.05) {
        printf("✅ Optimisation significative !\n");
    } else if (speedup < 0.95) {
        printf("❌ Régression de performance !\n");
    } else {
        printf("⚠️  Pas de différence significative\n");
    }

    return 0;
}
```

**Résultat typique :**

```
=== Comparaison de performances ===
Taille: 10000, Itérations: 1000000, Runs: 20

Baseline:   245.678 ms
Optimisée:  12.345 ms

Speedup:       19.89x
Amélioration:  95.0%
✅ Optimisation significative !
```

---

## Pièges courants du benchmarking

### Piège 1 : Dead Code Elimination

**Problème :** Le compilateur élimine le code non utilisé.

```c
// ❌ Mauvais
clock_t start = clock();
int result = calcul_lourd();
// result n'est jamais utilisé → tout éliminé par -O2 !
clock_t end = clock();

// ✅ Bon
clock_t start = clock();
int result = calcul_lourd();
printf("%d\n", result);  // Ou volatile, ou escape()
clock_t end = clock();
```

### Piège 2 : Constant Folding

**Problème :** Le compilateur calcule les constantes à la compilation.

```c
// ❌ Mauvais
clock_t start = clock();
int result = fibonacci(10);  // Calculé à la compilation !
clock_t end = clock();
// Temps mesuré ≈ 0

// ✅ Bon
int n;
scanf("%d", &n);  // Valeur inconnue à la compilation
clock_t start = clock();
int result = fibonacci(n);
clock_t end = clock();
```

### Piège 3 : Loop Unrolling et Vectorization

**Problème :** Le compilateur optimise différemment selon le contexte.

```c
// Code de benchmark
for (int i = 0; i < 1000000; i++) {
    fonction();  // Peut être vectorisée/déroulée
}

// Code réel d'usage
for (int i = 0; i < n; i++) {  // n inconnu
    fonction();  // Optimisation différente
}
```

**Solution :** Benchmarker avec des conditions similaires à l'usage réel.

### Piège 4 : Cache Effects

**Problème :** Le cache peut fausser les résultats.

```c
// ❌ Mauvais : Données en cache
int data[1000];
for (int i = 0; i < 1000; i++) data[i] = i;

// Benchmark (cache chaud)
clock_t start = clock();
for (int i = 0; i < 1000; i++) {
    process(data[i]);
}
clock_t end = clock();

// ✅ Bon : Invalider le cache entre les mesures
for (int run = 0; run < 10; run++) {
    flush_cache();  // Ou réallouer les données

    clock_t start = clock();
    for (int i = 0; i < 1000; i++) {
        process(data[i]);
    }
    clock_t end = clock();
}
```

### Piège 5 : CPU Frequency Scaling

**Problème :** Le CPU change de fréquence dynamiquement (Turbo Boost, PowerSave).

```bash
# Vérifier la fréquence actuelle
cat /proc/cpuinfo | grep MHz

# Fixer la fréquence (nécessite root)
sudo cpupower frequency-set -g performance

# Ou désactiver le Turbo Boost
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
```

### Piège 6 : Mesurer du code trop court

**Problème :** Précision insuffisante pour du code qui s'exécute en < 1 µs.

```c
// ❌ Mauvais
start = clock();
x = a + b;  // Quelques nanosecondes
end = clock();
// Résultat = 0 ou très imprécis

// ✅ Bon
start = clock();
for (int i = 0; i < 1000000; i++) {
    x = a + b;
}
end = clock();
double temps_par_operation = elapsed / 1000000.0;
```

---

## Bibliothèques de benchmarking

### Google Benchmark (C++, mais utilisable avec C)

**Avantages :**
- ✅ Très sophistiqué
- ✅ Gère automatiquement le warm-up, les itérations, les statistiques
- ✅ Détecte et élimine les outliers
- ✅ Format de sortie standardisé

**Exemple :**

```cpp
#include <benchmark/benchmark.h>

static void BM_MonAlgorithme(benchmark::State& state) {
    // Setup
    int n = state.range(0);
    int* data = new int[n];

    // Benchmark
    for (auto _ : state) {
        process_data(data, n);
    }

    // Cleanup
    delete[] data;
}

BENCHMARK(BM_MonAlgorithme)->Range(8, 8<<10);
BENCHMARK_MAIN();
```

### Criterion (Pour C uniquement)

**Avantages :**
- ✅ Spécialement conçu pour C
- ✅ Analyse statistique automatique
- ✅ Détection de régression

**Exemple :**

```c
#include <criterion/criterion.h>

Test(benchmark, somme_tableau) {
    int data[1000];
    for (int i = 0; i < 1000; i++) data[i] = i;

    cr_benchmark {
        int sum = 0;
        for (int i = 0; i < 1000; i++) {
            sum += data[i];
        }
    }
}
```

### hyperfine (Outil en ligne de commande)

**Idéal pour comparer des programmes complets !**

```bash
# Installer hyperfine
sudo apt install hyperfine

# Comparer deux versions
hyperfine './programme_v1' './programme_v2'

# Sortie :
# Benchmark 1: ./programme_v1
#   Time (mean ± σ):      23.4 ms ±   0.8 ms    [User: 22.1 ms, System: 1.2 ms]
#   Range (min … max):    22.1 ms …  25.3 ms    100 runs
#
# Benchmark 2: ./programme_v2
#   Time (mean ± σ):      15.2 ms ±   0.5 ms    [User: 14.8 ms, System: 0.4 ms]
#   Range (min … max):    14.5 ms …  16.8 ms    150 runs
#
# Summary
#   './programme_v2' ran
#     1.54 ± 0.06 times faster than './programme_v1'
```

**Avantages :**
- ✅ Très simple à utiliser
- ✅ Analyse statistique automatique
- ✅ Comparaison claire avec speedup
- ✅ Warm-up automatique

---

## Bonnes pratiques de reporting

### Informations à inclure

Un bon rapport de benchmark contient :

1. **Environnement**
   - OS et version (Ubuntu 22.04)
   - CPU (Intel i7-12700K @ 3.6 GHz)
   - RAM (32 GB DDR4-3200)
   - Compilateur (GCC 11.3.0)
   - Flags de compilation (`-O3 -march=native`)

2. **Méthodologie**
   - Nombre d'itérations
   - Nombre de runs
   - Warm-up effectué ou non
   - Taille des données d'entrée

3. **Résultats**
   - Temps moyen ± écart-type
   - Minimum et maximum
   - Médiane (si applicable)
   - Speedup relatif (si comparaison)

4. **Reproductibilité**
   - Code source du benchmark
   - Commande de compilation
   - Script de lancement

### Exemple de rapport

```
=== Benchmark : Algorithme de tri ===

Environnement :
- OS: Ubuntu 22.04 LTS
- CPU: Intel Core i7-12700K @ 3.6 GHz (12 cores)
- RAM: 32 GB DDR4-3200
- Compilateur: GCC 11.3.0
- Flags: -O3 -march=native -flto

Méthodologie :
- Données: 1,000,000 entiers aléatoires
- Itérations: 100 (avec warm-up de 10)
- Isolation: CPU 3 dédié (isolcpus=3)

Résultats :

QuickSort (baseline):
  Temps moyen: 125.3 ± 3.2 ms
  Min: 121.8 ms
  Max: 132.1 ms

MergeSort (version optimisée):
  Temps moyen: 98.7 ± 2.1 ms
  Min: 96.2 ms
  Max: 102.4 ms

Comparaison:
  Speedup: 1.27x
  Amélioration: 21.2%
  ✅ Amélioration statistiquement significative (p < 0.01)

Code et reproduction:
  https://github.com/user/projet/benchmarks/sort_bench.c
  $ gcc -O3 -march=native -flto sort_bench.c -o bench
  $ taskset -c 3 ./bench
```

---

## Checklist : Benchmark rigoureux

Avant de publier des résultats :

- ✅ **Environnement contrôlé** (fermer les autres programmes)
- ✅ **CPU fréquence fixe** (mode performance, pas de Turbo Boost)
- ✅ **Warm-up** (plusieurs exécutions avant mesure)
- ✅ **Mesures multiples** (minimum 10 runs)
- ✅ **Analyse statistique** (moyenne, écart-type, outliers)
- ✅ **Vérification du code** (pas d'élimination par le compilateur)
- ✅ **Données représentatives** (taille et distribution réalistes)
- ✅ **Reproductibilité** (documenter l'environnement et la méthode)
- ✅ **Vérification des résultats** (corrects, pas seulement rapides !)
- ✅ **Contexte clair** (ce qui est mesuré exactement)

---

## Erreurs à éviter

### ❌ Erreur 1 : Mesurer une seule fois

```bash
# Mauvais
time ./programme
# 0.123s
# "Mon programme prend 0.123 secondes"
```

**Pourquoi c'est mauvais :** Cette mesure unique peut être un outlier.

**Solution :** Toujours mesurer plusieurs fois et analyser statistiquement.

### ❌ Erreur 2 : Ignorer la variabilité

```
Run 1: 100 ms
Run 2: 150 ms
Run 3: 98 ms
Moyenne: 116 ms
```

**Problème :** Écart-type de 30 ms (26% !) → Résultats instables !

**Solution :** Investiguer pourquoi il y a autant de variabilité avant de conclure.

### ❌ Erreur 3 : Comparer des pommes et des oranges

```bash
# Mauvais : Compilations différentes
gcc -O0 version1.c -o v1
gcc -O3 version2.c -o v2
# Comparer v1 et v2 → Non significatif !
```

**Solution :** Toujours compiler avec les mêmes flags.

### ❌ Erreur 4 : Oublier de vérifier la correction

```c
// Version "optimisée" ultra-rapide
int somme(int *tableau, int n) {
    return 0;  // "Optimisation" : toujours retourner 0 !
}
```

**Solution :** Toujours vérifier que les résultats sont **corrects** !

### ❌ Erreur 5 : Benchmarker du code debug

```bash
# Mauvais
gcc -O0 -g programme.c -o programme_debug
time ./programme_debug
```

**Solution :** Benchmarker du code **optimisé pour la production** (`-O2` ou `-O3`).

---

## Microbenchmarks vs Macrobenchmarks

### Microbenchmarks

**Définition :** Mesurer une petite partie isolée du code (une fonction, une boucle).

**Avantages :**
- ✅ Rapide à exécuter
- ✅ Isole précisément l'impact d'une optimisation
- ✅ Reproductible

**Inconvénients :**
- ❌ Peut ne pas refléter la performance réelle (cache chaud, contexte artificiel)
- ❌ Le compilateur peut sur-optimiser du code isolé

**Quand utiliser :** Pour comparer deux implémentations d'un algorithme spécifique.

### Macrobenchmarks

**Définition :** Mesurer le programme complet avec des données réelles.

**Avantages :**
- ✅ Reflète la performance réelle
- ✅ Prend en compte tous les facteurs (I/O, cache, etc.)

**Inconvénients :**
- ❌ Lent à exécuter
- ❌ Difficile d'isoler l'effet d'une optimisation précise

**Quand utiliser :** Pour valider qu'une optimisation améliore vraiment le programme en conditions réelles.

**Recommandation :** Faire **les deux** ! Microbenchmark pour développer, macrobenchmark pour valider.

---

## Exemple complet : Benchmark de fonction de hachage

```c
// hash_benchmark.c
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define DATA_SIZE 1000000
#define RUNS 20

// Hash function 1: djb2
uint32_t hash_djb2(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Hash function 2: FNV-1a
uint32_t hash_fnv1a(const char *str) {
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (uint8_t)(*str++);
        hash *= 16777619;
    }
    return hash;
}

double benchmark_hash(uint32_t (*hash_func)(const char*),
                      char **data, int count) {
    struct timespec start, end;

    // Warm-up
    for (int i = 0; i < count / 10; i++) {
        volatile uint32_t h = hash_func(data[i % count]);
    }

    // Mesure
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < count; i++) {
        volatile uint32_t h = hash_func(data[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    // Générer des données de test
    char **data = malloc(DATA_SIZE * sizeof(char*));
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = malloc(20);
        snprintf(data[i], 20, "string_%d", i);
    }

    printf("=== Benchmark : Hash Functions ===\n");
    printf("Données: %d chaînes\n", DATA_SIZE);
    printf("Runs: %d\n\n", RUNS);

    double times_djb2[RUNS], times_fnv1a[RUNS];

    // Benchmarks
    for (int run = 0; run < RUNS; run++) {
        times_djb2[run] = benchmark_hash(hash_djb2, data, DATA_SIZE);
        times_fnv1a[run] = benchmark_hash(hash_fnv1a, data, DATA_SIZE);
    }

    // Analyse
    double avg_djb2 = 0, avg_fnv1a = 0;
    for (int i = 0; i < RUNS; i++) {
        avg_djb2 += times_djb2[i];
        avg_fnv1a += times_fnv1a[i];
    }
    avg_djb2 /= RUNS;
    avg_fnv1a /= RUNS;

    printf("djb2:   %.3f ms (%.2f ns/hash)\n",
           avg_djb2 * 1000, avg_djb2 / DATA_SIZE * 1e9);
    printf("FNV-1a: %.3f ms (%.2f ns/hash)\n",
           avg_fnv1a * 1000, avg_fnv1a / DATA_SIZE * 1e9);
    printf("\n");

    if (avg_djb2 < avg_fnv1a) {
        printf("djb2 est %.2fx plus rapide\n", avg_fnv1a / avg_djb2);
    } else {
        printf("FNV-1a est %.2fx plus rapide\n", avg_djb2 / avg_fnv1a);
    }

    // Cleanup
    for (int i = 0; i < DATA_SIZE; i++) {
        free(data[i]);
    }
    free(data);

    return 0;
}
```

---

## Ressources et outils

### Outils de benchmarking

- **hyperfine** : https://github.com/sharkdp/hyperfine (CLI, simple, excellent)
- **Google Benchmark** : https://github.com/google/benchmark (C++, sophistiqué)
- **Criterion** : https://github.com/Snaipe/Criterion (C, complet)
- **perf** : Intégré à Linux (le plus précis)

### Lectures recommandées

- **"Systems Performance"** — Brendan Gregg (bible du benchmarking)
- **"Benchmarking Crimes"** — Gernot Heiser (article sur les erreurs courantes)
- **Google's "How to Benchmark Code"** : https://github.com/google/benchmark/blob/main/docs/user_guide.md

### Commandes utiles

```bash
# Fixer la fréquence CPU
sudo cpupower frequency-set -g performance

# Désactiver Turbo Boost (Intel)
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

# Voir les CPUs isolés
cat /sys/devices/system/cpu/isolated

# Lancer sur un CPU spécifique
taskset -c 3 ./programme

# Profiler avec perf
perf stat -r 10 ./programme
```

---

## Résumé

Le benchmarking rigoureux est essentiel pour l'optimisation :

1. ✅ **Mesurer plusieurs fois** : Jamais une seule mesure
2. ✅ **Analyser statistiquement** : Moyenne, écart-type, outliers
3. ✅ **Contrôler l'environnement** : Isolation, fréquence CPU fixe
4. ✅ **Warm-up** : Cache chaud avant mesure
5. ✅ **Éviter les pièges** : Dead code elimination, constant folding
6. ✅ **Utiliser les bons outils** : `clock_gettime()`, `perf`, `hyperfine`
7. ✅ **Documenter** : Environnement, méthodologie, reproductibilité
8. ✅ **Vérifier la correction** : Résultats corrects, pas seulement rapides

**Citation finale :**

> "In benchmarking, precision matters more than accuracy. You need reproducible results to detect small improvements" — Anonyme

**Règle d'or :**

```
Bon benchmark = Mesures multiples + Analyse statistique + Environnement contrôlé + Vérification
```

Sans benchmark rigoureux, vous optimisez à l'aveugle. Avec un bon benchmark, vous pouvez prendre des décisions éclairées et mesurer objectivement l'impact de vos optimisations !

---

*Fin du Module 27 : Optimisation et Performance*

⏭️ [Interopérabilité](/28-interoperabilite/README.md)
