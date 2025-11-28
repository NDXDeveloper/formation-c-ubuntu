🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.3 Profiling

## Introduction

Le **profiling** (ou **profilage** en français) est l'analyse des performances d'un programme en cours d'exécution. C'est l'outil indispensable pour comprendre **où votre programme passe son temps** et **quelles parties consomment le plus de ressources**.

### Pourquoi le profiling est essentiel ?

Imaginez que votre programme prend 10 secondes à s'exécuter. Où passe-t-il son temps ?
- 🤔 Dans quelle fonction ?
- 🤔 À quelle ligne de code ?
- 🤔 Dans les calculs ou dans les accès mémoire ?

Sans profiling, vous optimisez **à l'aveugle**. Avec le profiling, vous optimisez **là où ça compte vraiment**.

### La règle des 90/10

> "90% du temps d'exécution est passé dans 10% du code"

Cette règle empirique signifie que :
- ✅ Optimiser les **10% critiques** peut améliorer drastiquement les performances
- ❌ Optimiser les 90% restants est souvent une perte de temps

**Le profiling vous montre ces 10% critiques !**

### Citation célèbre

> "Premature optimization is the root of all evil" — Donald Knuth

Traduction : "L'optimisation prématurée est la racine de tous les maux"

**Signification :** N'optimisez jamais sans avoir d'abord **mesuré** où se trouvent les vrais problèmes de performance.

---

## Types de profiling

### 1. Profiling CPU (temps d'exécution)

**Question :** Quelles fonctions consomment le plus de temps processeur ?

**Outils :** `gprof`, `perf`, Valgrind Callgrind

**Métriques :**
- Temps total passé dans chaque fonction
- Nombre d'appels de chaque fonction
- Temps par appel

### 2. Profiling mémoire

**Question :** Comment est utilisée la mémoire ? Y a-t-il des allocations inefficaces ?

**Outils :** Valgrind Massif, `heaptrack`

**Métriques :**
- Mémoire allouée par fonction
- Durée de vie des allocations
- Pics de consommation mémoire

### 3. Profiling cache

**Question :** Mon programme utilise-t-il efficacement le cache CPU ?

**Outils :** Valgrind Cachegrind, `perf`

**Métriques :**
- Cache hits vs cache misses
- Prédictions de branchement

### 4. Profiling I/O

**Question :** Combien de temps est passé à lire/écrire des fichiers ou réseau ?

**Outils :** `strace`, `perf`, `iotop`

---

## Workflow de profiling recommandé

```
1. Écrire du code fonctionnel
   ↓
2. Identifier un problème de performance
   ↓
3. PROFILER pour trouver le goulot d'étranglement
   ↓
4. Optimiser UNIQUEMENT la partie identifiée
   ↓
5. Mesurer à nouveau (a-t-on gagné en performance ?)
   ↓
6. Répéter si nécessaire
```

**Important :** Ne sautez jamais l'étape 3. Profiler d'abord, optimiser ensuite !

---

## Profiling avec gprof (Profiler classique)

### Introduction à gprof

`gprof` est le profiler historique de GNU. Il est simple, léger et disponible partout.

**Principe :** `gprof` échantillonne périodiquement l'exécution du programme pour déterminer dans quelle fonction il se trouve.

### Étapes d'utilisation

#### Étape 1 : Compiler avec le flag `-pg`

```bash
gcc -pg -O2 mon_programme.c -o mon_programme
```

Le flag `-pg` (profiling generation) ajoute du code d'instrumentation au binaire.

**⚠️ Note :** Ne combinez **jamais** `-pg` avec des flags d'optimisation trop agressifs comme `-fomit-frame-pointer`, car cela empêche le profiling de fonctionner correctement.

#### Étape 2 : Exécuter le programme normalement

```bash
./mon_programme
```

Le programme s'exécute normalement (avec un léger surcoût dû à l'instrumentation). À la fin de l'exécution, un fichier `gmon.out` est généré.

#### Étape 3 : Générer le rapport avec gprof

```bash
gprof mon_programme gmon.out > rapport_profiling.txt
```

Cette commande analyse `gmon.out` et produit un rapport texte détaillé.

#### Étape 4 : Analyser le rapport

```bash
less rapport_profiling.txt
```

### Exemple complet

**Code source : `test_perf.c`**

```c
#include <stdio.h>
#include <unistd.h>

void fonction_lente() {
    // Simule un calcul lourd
    for (int i = 0; i < 100000000; i++) {
        // Calcul inutile
    }
}

void fonction_rapide() {
    for (int i = 0; i < 1000; i++) {
        // Peu d'itérations
    }
}

void fonction_intermediaire() {
    fonction_lente();
    fonction_rapide();
}

int main() {
    printf("Début du programme\n");

    // Appelle plusieurs fois les fonctions
    for (int i = 0; i < 5; i++) {
        fonction_intermediaire();
    }

    fonction_rapide();

    printf("Fin du programme\n");
    return 0;
}
```

**Compilation et profiling :**

```bash
# 1. Compiler avec -pg
gcc -pg -O2 test_perf.c -o test_perf

# 2. Exécuter
./test_perf

# 3. Générer le rapport
gprof test_perf gmon.out > rapport.txt

# 4. Afficher les résultats
cat rapport.txt
```

### Interpréter le rapport gprof

Le rapport contient deux sections principales :

#### Section 1 : Flat profile (Profil plat)

```
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total
 time   seconds   seconds    calls  ms/call  ms/call  name
 85.71      0.60     0.60        5   120.00   120.00  fonction_lente
 12.86      0.69     0.09        6     1.50     1.50  fonction_rapide
  1.43      0.70     0.01        5     2.00   122.00  fonction_intermediaire
  0.00      0.70     0.00        1     0.00   700.00  main
```

**Colonnes importantes :**

- **% time** : Pourcentage du temps total passé dans cette fonction
  - `fonction_lente` : **85.71%** du temps → **C'est le goulot d'étranglement !**

- **cumulative seconds** : Temps cumulé jusqu'à cette ligne

- **self seconds** : Temps passé **dans** cette fonction (sans compter les fonctions qu'elle appelle)

- **calls** : Nombre d'appels

- **self ms/call** : Temps moyen par appel (millisecondes)

**Interprétation :** `fonction_lente` prend 85% du temps total. C'est là qu'il faut optimiser !

#### Section 2 : Call graph (Graphe d'appels)

```
index % time    self  children    called     name
                                                 <spontaneous>
[1]    100.0    0.00    0.70                 main [1]
                0.01    0.61       5/5           fonction_intermediaire [2]
                0.09    0.00       6/6           fonction_rapide [4]
-----------------------------------------------
                0.01    0.61       5/5           main [1]
[2]     88.6    0.01    0.61       5         fonction_intermediaire [2]
                0.60    0.00       5/5           fonction_lente [3]
                0.00    0.00       5/6           fonction_rapide [4]
-----------------------------------------------
                0.60    0.00       5/5           fonction_intermediaire [2]
[3]     85.7    0.60    0.00       5         fonction_lente [3]
-----------------------------------------------
                0.09    0.00       6/6           main [1]
                0.00    0.00       5/6           fonction_intermediaire [2]
[4]     12.9    0.09    0.00       6         fonction_rapide [4]
-----------------------------------------------
```

Cette section montre **qui appelle qui** :
- `main` appelle `fonction_intermediaire` 5 fois
- `fonction_intermediaire` appelle `fonction_lente` 5 fois
- etc.

**Utilité :** Comprendre le contexte d'appel d'une fonction coûteuse.

### Limites de gprof

- ❌ **Précision limitée** : Échantillonnage toutes les ~10ms (peut manquer des fonctions très rapides)
- ❌ **Pas de profiling multi-thread** : Ne fonctionne pas correctement avec `pthread`
- ❌ **Overhead** : L'instrumentation ralentit le programme (~10-30%)
- ❌ **Pas d'info sur le cache ou la mémoire**

**Alternative moderne :** `perf` (voir section suivante)

---

## Profiling avec perf (Outil moderne recommandé)

### Introduction à perf

`perf` est le profiler moderne de Linux, intégré au kernel. Il est **beaucoup plus puissant** que `gprof` :

- ✅ Précision élevée (échantillonnage basé sur les interruptions matérielles)
- ✅ Support multi-thread et multi-processus
- ✅ Profiling CPU, cache, branch prediction, I/O, etc.
- ✅ Overhead minimal (~1-5%)
- ✅ Pas besoin de recompiler avec `-pg`

### Installation

```bash
# Sur Ubuntu/Debian
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-$(uname -r)

# Vérifier l'installation
perf --version
```

### Workflow de base

#### Étape 1 : Compiler avec symboles de debug

```bash
gcc -O2 -g mon_programme.c -o mon_programme
```

Le flag `-g` ajoute les symboles de débogage pour que `perf` puisse afficher les noms de fonctions.

**Note :** Contrairement à `gprof`, pas besoin de `-pg` !

#### Étape 2 : Profiler l'exécution

```bash
perf record ./mon_programme
```

Cela génère un fichier `perf.data` contenant les données de profiling.

**Options utiles :**

```bash
# Échantillonner plus fréquemment (par défaut: 1000 Hz)
perf record -F 4000 ./mon_programme

# Profiler un processus existant (par PID)
perf record -p 12345

# Profiler tous les CPUs
perf record -a ./mon_programme

# Profiler pendant 10 secondes puis arrêter
perf record -a sleep 10
```

#### Étape 3 : Analyser les résultats

```bash
perf report
```

Cela ouvre une interface interactive (type `less`) montrant les fonctions les plus coûteuses.

**Navigation dans `perf report` :**
- Flèches haut/bas : Naviguer dans la liste
- Entrée : Voir les détails d'une fonction (désassemblage)
- `q` : Quitter

### Exemple avec perf

**Code source : `test_perf2.c`**

```c
#include <stdio.h>
#include <stdlib.h>

// Fonction avec beaucoup de calculs
double calcul_intensif(int n) {
    double resultat = 0.0;
    for (int i = 0; i < n; i++) {
        resultat += (double)i * i / (i + 1.0);
    }
    return resultat;
}

// Fonction avec allocations mémoire
void allocation_intensive(int n) {
    for (int i = 0; i < n; i++) {
        int *tableau = malloc(1000 * sizeof(int));
        for (int j = 0; j < 1000; j++) {
            tableau[j] = j;
        }
        free(tableau);
    }
}

int main() {
    printf("Début du profiling\n");

    // 80% du temps ici
    for (int i = 0; i < 1000; i++) {
        calcul_intensif(100000);
    }

    // 20% du temps ici
    allocation_intensive(10000);

    printf("Fin du profiling\n");
    return 0;
}
```

**Profiling :**

```bash
# 1. Compiler avec optimisations et symboles
gcc -O2 -g test_perf2.c -o test_perf2

# 2. Profiler
perf record ./test_perf2

# 3. Voir le rapport
perf report
```

**Exemple de sortie `perf report` :**

```
Samples: 5K of event 'cycles', Event count (approx.): 4521349087
Overhead  Command     Shared Object       Symbol
  78.45%  test_perf2  test_perf2          [.] calcul_intensif
  19.23%  test_perf2  test_perf2          [.] allocation_intensive
   1.89%  test_perf2  libc.so.6           [.] malloc
   0.43%  test_perf2  libc.so.6           [.] free
```

**Interprétation :**
- `calcul_intensif` : **78.45%** du temps → Optimiser ici en priorité !
- `allocation_intensive` : 19.23%
- Les appels à `malloc`/`free` prennent aussi du temps (2.32% cumulé)

### Profiling avancé avec perf

#### a) Profiler les cache misses

```bash
# Profiler les défauts de cache L1
perf record -e cache-misses ./mon_programme
perf report
```

**Interprétation :** Si une fonction a beaucoup de cache misses, cela signifie qu'elle accède à la mémoire de manière inefficace (accès non séquentiels, stride trop grand, etc.).

#### b) Profiler les branch misses (prédictions de branchement)

```bash
perf record -e branch-misses ./mon_programme
perf report
```

**Interprétation :** Beaucoup de branch misses indiquent que le CPU a du mal à prédire les conditions (`if`, boucles). Réorganiser le code peut aider.

#### c) Profiler les instructions

```bash
perf stat ./mon_programme
```

**Exemple de sortie :**

```
 Performance counter stats for './test_perf2':

          2,147.56 msec task-clock                #    0.998 CPUs utilized
                 3      context-switches          #    1.397 /sec
                 0      cpu-migrations            #    0.000 /sec
               156      page-faults               #   72.628 /sec
     8,521,349,087      cycles                    #    3.968 GHz
    12,456,789,123      instructions              #    1.46  insn per cycle
     2,345,678,901      branches                  # 1092.136 M/sec
        12,456,789      branch-misses             #    0.53% of all branches

       2.151234567 seconds time elapsed

       2.147890000 seconds user
       0.000000000 seconds sys
```

**Métriques importantes :**

- **cycles** : Nombre de cycles CPU
- **instructions** : Nombre d'instructions exécutées
- **IPC (insn per cycle)** : Nombre d'instructions par cycle
  - Valeur idéale : ~2-4 (dépend du CPU)
  - Valeur faible (<1) : CPU bloqué (attente mémoire, dépendances)
- **branch-misses** : Prédictions de branchement ratées
  - Bon : <5%
  - Mauvais : >10%

#### d) Flame Graphs (Graphiques en flammes)

Les **Flame Graphs** sont une visualisation très populaire du profiling.

**Installation :**

```bash
git clone https://github.com/brendangregg/FlameGraph
cd FlameGraph
```

**Génération :**

```bash
# Profiler
perf record -F 99 -g ./mon_programme

# Générer le stack trace
perf script > out.perf

# Créer le Flame Graph
./stackcollapse-perf.pl out.perf | ./flamegraph.pl > flamegraph.svg

# Ouvrir dans un navigateur
firefox flamegraph.svg
```

**Interprétation :**
- Largeur : Temps passé (plus large = plus lent)
- Hauteur : Profondeur de la pile d'appels
- Couleurs : Aléatoires (pour différencier)

Les Flame Graphs permettent de voir d'un coup d'œil toute la hiérarchie des appels et où le temps est dépensé.

---

## Profiling avec Valgrind Callgrind

### Introduction à Callgrind

**Callgrind** est un outil de Valgrind pour le profiling détaillé des appels de fonctions.

**Avantages :**
- ✅ Très précis (compte exact des instructions)
- ✅ Pas besoin de recompiler avec `-pg`
- ✅ Profiling déterministe (pas d'échantillonnage)

**Inconvénients :**
- ❌ **Très lent** (10-50x plus lent que l'exécution normale)
- ❌ Ne capture pas les performances réelles (simule le CPU)

### Utilisation

```bash
# Profiler avec Callgrind
valgrind --tool=callgrind ./mon_programme

# Cela génère un fichier : callgrind.out.<pid>
```

### Analyser avec KCachegrind (interface graphique)

```bash
# Installer KCachegrind
sudo apt-get install kcachegrind

# Ouvrir le fichier de profiling
kcachegrind callgrind.out.12345
```

**KCachegrind** offre une interface graphique superbe avec :
- Graphe d'appels interactif
- Liste des fonctions triées par coût
- Vue du code source annoté
- Carte de chaleur (heatmap)

### Analyser en ligne de commande

```bash
# Afficher les 10 fonctions les plus coûteuses
callgrind_annotate callgrind.out.12345 | head -30
```

**Exemple de sortie :**

```
--------------------------------------------------------------------------------
Profile data file 'callgrind.out.12345' (creator: callgrind-3.15.0)
--------------------------------------------------------------------------------
I1 cache:         32768 B, 64 B, 8-way associative
D1 cache:         32768 B, 64 B, 8-way associative
LL cache:         8388608 B, 64 B, 16-way associative
Command:          ./test_perf2
Data file:        callgrind.out.12345
Events recorded:  Ir
Events shown:     Ir
Event sort order: Ir
Thresholds:       99
Include dirs:
User annotated:
Auto-annotation:  off

--------------------------------------------------------------------------------
      Ir
--------------------------------------------------------------------------------
8,456,789,123  PROGRAM TOTALS

--------------------------------------------------------------------------------
      Ir  file:function
--------------------------------------------------------------------------------
6,745,432,123  test_perf2.c:calcul_intensif [/path/to/test_perf2]
1,234,567,890  test_perf2.c:allocation_intensive [/path/to/test_perf2]
  456,789,100  ???:malloc [/usr/lib/libc.so.6]
```

**Ir** = Instructions Retired (instructions exécutées)

**Interprétation :** `calcul_intensif` exécute 6.7 milliards d'instructions, soit ~80% du total.

---

## Profiling mémoire avec Valgrind Massif

### Introduction à Massif

**Massif** est un profiler de tas (heap) qui montre :
- Combien de mémoire est allouée
- Où est-elle allouée (quelle fonction)
- L'évolution dans le temps

### Utilisation

```bash
# Profiler la mémoire
valgrind --tool=massif ./mon_programme

# Génère : massif.out.<pid>
```

### Analyser avec ms_print

```bash
ms_print massif.out.12345 > rapport_memoire.txt
less rapport_memoire.txt
```

**Exemple de graphique dans le rapport :**

```
    MB
19.71^                                                                       #
     |                                                                       #
     |                                                                       #
     |                                                                       #
     |                                                                      :#
     |                                                                      :#
     |                                                                      :#
     |                                                                      :#
     |                                                                     ::#
     |                                                                     ::#
     |                                                                    :::#
     |                                                                    :::#
     |                                                                   ::::#
     |                                                                   ::::#
     |                                                                  :::::#
     |                                                                 ::::::#
     |                                                               @:::::::#
     |                                                           @@@@:::::::#
     |                                                      @@@@@@@@@:::::::#
     |                                            @@@@@@@@@@@@@@@@@@@@:::::::#
   0 +----------------------------------------------------------------------->Gi
     0                                                                   109.8

Number of snapshots: 52
 Detailed snapshots: [9, 19, 29, 39, 49 (peak)]
```

**Interprétation :**
- Pic de mémoire à ~19.71 MB
- Le `@` et `#` montrent l'évolution de l'allocation
- Les snapshots détaillés montrent où la mémoire est allouée

**Détail d'un snapshot (pic) :**

```
--------------------------------------------------------------------------------
  n        time(i)         total(B)   useful-heap(B) extra-heap(B)    stacks(B)
--------------------------------------------------------------------------------
 49      8,924,567       19,710,000       19,450,000       260,000            0
99.99% (19,450,000B) (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
->85.45% (16,834,000B) 0x4005E8: allocation_intensive (test_perf2.c:14)
| ->85.45% (16,834,000B) 0x400623: main (test_perf2.c:26)
|
->14.54% (2,616,000B) 0x4005A3: calcul_intensif (test_perf2.c:7)
  ->14.54% (2,616,000B) 0x400610: main (test_perf2.c:23)
```

**Interprétation :**
- `allocation_intensive` alloue 85.45% de la mémoire totale
- `calcul_intensif` alloue 14.54%

### Visualiser avec Massif-Visualizer (GUI)

```bash
# Installer
sudo apt-get install massif-visualizer

# Ouvrir
massif-visualizer massif.out.12345
```

Interface graphique élégante avec graphiques interactifs.

---

## Comparaison des outils de profiling

| Outil | Type | Précision | Overhead | Multithreading | GUI |
|-------|------|-----------|----------|----------------|-----|
| **gprof** | CPU | Moyenne | 10-30% | ❌ | ❌ |
| **perf** | CPU, cache, I/O | Haute | 1-5% | ✅ | Via FlameGraph |
| **Callgrind** | CPU | Très haute | 1000-5000% | ✅ | ✅ (KCachegrind) |
| **Massif** | Mémoire | Haute | 100-200% | ✅ | ✅ (massif-visualizer) |

**Recommandation générale :**
- 🥇 **Profiling quotidien** : `perf` (rapide, précis)
- 🥈 **Analyse détaillée** : Callgrind + KCachegrind
- 🥉 **Profiling mémoire** : Massif
- 📜 **Profiling simple** : gprof (si `perf` n'est pas disponible)

---

## Bonnes pratiques de profiling

### 1. Toujours profiler avec les optimisations activées

```bash
# ✅ BON : Profiler le code optimisé
gcc -O2 -g test.c -o test
perf record ./test

# ❌ MAUVAIS : Profiler sans optimisation
gcc -O0 -g test.c -o test
perf record ./test
```

**Pourquoi ?** Le code non optimisé ne représente pas les performances réelles.

### 2. Profiler avec des données réalistes

```bash
# ❌ MAUVAIS : Petit dataset
./mon_programme petit_fichier.txt

# ✅ BON : Dataset représentatif de la production
./mon_programme gros_fichier_production.txt
```

### 3. Profiler plusieurs fois et faire la moyenne

Les performances peuvent varier à cause :
- Du cache CPU
- De l'état du système
- D'autres processus

```bash
# Profiler 5 fois et comparer
for i in {1..5}; do
    perf stat ./mon_programme
done
```

### 4. Isoler les parties coûteuses

Si votre programme fait beaucoup de choses, profilez une partie à la fois :

```c
int main() {
    // Phase 1 : Chargement des données
    charger_donnees();

    // Phase 2 : Traitement (À PROFILER)
    traiter_donnees();

    // Phase 3 : Sauvegarde
    sauvegarder_resultats();
}
```

Ajoutez des flags ou commentez les parties non pertinentes.

### 5. Vérifier que l'optimisation a fonctionné

```bash
# Avant optimisation
perf stat ./mon_programme_v1

# Après optimisation
perf stat ./mon_programme_v2

# Comparer les métriques (cycles, time elapsed, etc.)
```

### 6. Documenter les résultats

Gardez une trace de vos mesures :

```bash
# Créer un fichier de benchmark
echo "Version 1.0 - $(date)" > benchmark.txt
perf stat ./mon_programme 2>> benchmark.txt

# Après modification
echo "Version 1.1 - $(date)" >> benchmark.txt
perf stat ./mon_programme 2>> benchmark.txt
```

---

## Exemple de workflow complet

### Problème : Programme trop lent

**Programme : Tri de grand tableau**

```c
// sort_benchmark.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 100000

void bubble_sort(int *tab, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (tab[j] > tab[j + 1]) {
                int temp = tab[j];
                tab[j] = tab[j + 1];
                tab[j + 1] = temp;
            }
        }
    }
}

int main() {
    int *tableau = malloc(TAILLE * sizeof(int));

    // Remplir avec valeurs aléatoires
    srand(time(NULL));
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand();
    }

    printf("Début du tri...\n");
    bubble_sort(tableau, TAILLE);
    printf("Tri terminé\n");

    free(tableau);
    return 0;
}
```

### Étape 1 : Mesurer les performances initiales

```bash
gcc -O2 -g sort_benchmark.c -o sort_benchmark

time ./sort_benchmark
# Résultat : 8.5 secondes ← Trop lent !
```

### Étape 2 : Profiler avec perf

```bash
perf record ./sort_benchmark
perf report
```

**Résultat :**
```
Overhead  Symbol
  99.8%   bubble_sort
   0.2%   main
```

**Conclusion :** 99.8% du temps est dans `bubble_sort`. C'est le goulot !

### Étape 3 : Analyser l'algorithme

Le bubble sort est O(n²), c'est un algorithme inefficace pour les grands tableaux.

**Solution :** Utiliser quicksort (O(n log n))

### Étape 4 : Optimiser

```c
// Utiliser qsort de la stdlib
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    int *tableau = malloc(TAILLE * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand();
    }

    printf("Début du tri...\n");
    qsort(tableau, TAILLE, sizeof(int), compare);  // ← Optimisé !
    printf("Tri terminé\n");

    free(tableau);
    return 0;
}
```

### Étape 5 : Re-mesurer

```bash
gcc -O2 -g sort_benchmark_v2.c -o sort_benchmark_v2

time ./sort_benchmark_v2
# Résultat : 0.015 secondes ← 500x plus rapide !
```

### Étape 6 : Profiler à nouveau

```bash
perf record ./sort_benchmark_v2
perf report
```

**Résultat :**
```
Overhead  Symbol
  78.2%   qsort
  21.8%   main
```

Le programme est maintenant si rapide que `perf` passe plus de temps dans l'initialisation (`main`) que dans le tri lui-même.

**Mission accomplie ! 🎉**

---

## Profiling de programmes multi-threadés

### Particularités

Les programmes multi-threadés posent des défis :
- Les threads peuvent s'exécuter en parallèle
- Le profiling doit capturer tous les threads
- Attention aux conditions de course (race conditions)

### Profiling avec perf

```bash
# Profiler tous les threads
perf record -g ./mon_programme_multithread

# Voir les résultats par thread
perf report --sort comm,dso
```

### Identifier les contentions (lock contention)

Si les threads passent beaucoup de temps à attendre des mutex :

```bash
perf record -e syscalls:sys_enter_futex ./mon_programme_multithread
perf report
```

**Interprétation :** Beaucoup d'appels à `futex` (Fast Userspace Mutex) indiquent que les threads se bloquent mutuellement.

**Solution potentielle :**
- Réduire la taille des sections critiques
- Utiliser des locks plus fins (fine-grained locking)
- Utiliser des structures lock-free (atomics)

---

## Outils complémentaires

### 1. time (mesure simple)

```bash
# Temps réel vs temps CPU
time ./mon_programme

# Résultat :
# real    0m2.547s  ← Temps écoulé (horloge murale)
# user    0m2.123s  ← Temps CPU en mode utilisateur
# sys     0m0.421s  ← Temps CPU en mode kernel
```

**Interprétation :**
- `real > user + sys` : Programme bloqué (I/O, sleep, attente réseau)
- `user + sys > real` : Programme multi-threadé (utilise plusieurs CPUs)

### 2. strace (profiling des appels système)

```bash
# Tracer tous les appels système
strace -c ./mon_programme

# Résultat :
# % time     seconds  usecs/call     calls    errors syscall
# ------ ----------- ----------- --------- --------- ----------------
#  45.23    0.012345        1234        10           read
#  32.45    0.008876         443        20           write
#  12.34    0.003377         169        20           open
```

**Utilité :** Identifier les I/O lentes ou les appels système excessifs.

### 3. htop / top (monitoring en temps réel)

```bash
htop
```

Montre l'utilisation CPU/mémoire en temps réel. Utile pour identifier les processus qui consomment trop de ressources.

---

## Checklist : Profiling efficace

Avant de profiler :
- ✅ Mon programme a-t-il un vrai problème de performance ?
- ✅ Ai-je compilé avec `-O2` et `-g` ?
- ✅ Ai-je des données de test représentatives ?

Pendant le profiling :
- ✅ Ai-je utilisé le bon outil (`perf` pour CPU, Massif pour mémoire) ?
- ✅ Ai-je exécuté plusieurs fois pour obtenir des résultats stables ?
- ✅ Ai-je identifié les fonctions qui prennent >10% du temps ?

Après le profiling :
- ✅ Ai-je optimisé SEULEMENT les parties identifiées comme lentes ?
- ✅ Ai-je re-mesuré pour vérifier que l'optimisation a fonctionné ?
- ✅ Ai-je documenté mes résultats ?

---

## Erreurs courantes à éviter

### ❌ Optimiser sans profiler

**Problème :** Vous passez du temps à optimiser une fonction qui ne représente que 1% du temps total.

**Solution :** Profilez d'abord, optimisez ensuite.

### ❌ Profiler du code non optimisé

**Problème :** Le code avec `-O0` a des patterns de performance complètement différents de `-O2`.

**Solution :** Toujours profiler avec les flags d'optimisation que vous utiliserez en production.

### ❌ Profiler avec de petites données

**Problème :** Un algorithme O(n²) peut sembler rapide avec n=10 mais exploser avec n=10000.

**Solution :** Utilisez des datasets réalistes.

### ❌ Ne pas re-mesurer après optimisation

**Problème :** Vous optimisez mais ne vérifiez pas si ça a vraiment amélioré les performances.

**Solution :** Toujours mesurer avant et après.

### ❌ Croire les microbenchmarks

**Problème :** Les microbenchmarks peuvent être trompeurs (overhead du framework de test, cache chaud, etc.).

**Solution :** Profilez le programme complet dans des conditions réelles.

---

## Résumé

| Objectif | Outil recommandé | Commande |
|----------|------------------|----------|
| Profiling CPU rapide | `perf` | `perf record ./prog && perf report` |
| Profiling CPU détaillé | Callgrind | `valgrind --tool=callgrind ./prog` + KCachegrind |
| Profiling mémoire | Massif | `valgrind --tool=massif ./prog` + ms_print |
| Mesure simple | `time` | `time ./prog` |
| Appels système | `strace` | `strace -c ./prog` |
| Flame Graph | `perf` | `perf record -g` + FlameGraph scripts |

---

## Pour aller plus loin

### Documentation

```bash
# Manuel de perf
man perf
man perf-record
man perf-report
man perf-stat

# Valgrind
man valgrind
man callgrind
man massif
```

### Ressources en ligne

- **Brendan Gregg's Blog** : http://www.brendangregg.com/perf.html (expert en profiling)
- **perf Tutorial** : https://perf.wiki.kernel.org/index.php/Tutorial
- **FlameGraph** : https://github.com/brendangregg/FlameGraph
- **Linux Performance** : http://www.brendangregg.com/linuxperf.html

### Livres recommandés

- **"Systems Performance"** par Brendan Gregg (bible du profiling)
- **"The Art of Computer Programming"** par Donald Knuth (algorithmes et performance)

---

## Conclusion

Le profiling est un outil **indispensable** pour optimiser efficacement :

1. ✅ **Mesurez avant d'optimiser** : Ne présumez jamais où sont les goulots
2. ✅ **Utilisez le bon outil** : `perf` pour le CPU, Massif pour la mémoire
3. ✅ **Profilez du code optimisé** : `-O2` minimum
4. ✅ **Optimisez les 10% critiques** : Règle des 90/10
5. ✅ **Re-mesurez toujours** : Vérifiez que l'optimisation a fonctionné

**Citation finale :**

> "In God we trust, all others bring data" — W. Edwards Deming

Traduction : "En Dieu nous croyons, tous les autres doivent apporter des données"

**Ne devinez pas, mesurez !**

---

*Prochaine section : 27.4 Cache awareness*

⏭️ [Cache awareness](/27-optimisation-performance/04-cache-awareness.md)
