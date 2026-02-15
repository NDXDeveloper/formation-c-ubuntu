🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.9 Profile-Guided Optimization (PGO)

## Introduction

Le **Profile-Guided Optimization** (PGO) ou **Optimisation guidée par profil** est une technique d'optimisation qui utilise des **données d'exécution réelles** du programme pour guider les décisions du compilateur.

### Le concept

Au lieu de faire des suppositions générales sur le comportement du programme, le compilateur utilise des **statistiques d'exécution** pour savoir :
- Quelles parties du code sont exécutées le plus souvent (hot paths)
- Quelles branches sont prises le plus fréquemment
- Quelles fonctions sont appelées ensemble
- Quelles boucles itèrent le plus

Avec ces informations, le compilateur peut optimiser précisément **là où ça compte vraiment**.

### Analogie : L'architecte et la maison

**Sans PGO (optimisation théorique) :**

Un architecte conçoit une maison sans connaître les habitants :
- Il suppose que le salon sera utilisé 50% du temps
- Il suppose que la cuisine sera utilisée 20% du temps
- Il fait une chambre de taille standard

**Résultat :** Maison "moyenne" qui ne convient pas parfaitement.

**Avec PGO (optimisation basée sur l'usage réel) :**

L'architecte observe d'abord la famille pendant un mois :
- Ils passent 70% du temps dans la cuisine (famille de cuisiniers)
- Ils utilisent rarement le salon
- Ils ont besoin d'un grand bureau (télétravail)

**Résultat :** Il conçoit une maison avec une cuisine spacieuse, un grand bureau, et un petit salon. **Parfaitement adaptée** aux besoins réels !

Même principe avec PGO : le compilateur observe comment le programme s'exécute **réellement**, puis l'optimise en conséquence.

---

## Comment fonctionne PGO

PGO se déroule en **trois étapes** :

### Étape 1 : Instrumentation

Compiler le programme avec du code supplémentaire qui **enregistre** les statistiques d'exécution.

```bash
gcc -O2 -fprofile-generate programme.c -o programme_instr
```

**Effet :** Le binaire contient du code qui collecte des données lors de l'exécution :
- Compteurs d'appels de fonctions
- Compteurs de branches prises/non prises
- Informations sur les boucles
- Données de cache

**Fichier généré :** `programme_instr` (plus gros et plus lent que normal)

### Étape 2 : Collecte des données (profiling)

Exécuter le programme instrumenté avec des **données représentatives** de l'usage réel.

```bash
./programme_instr < donnees_typiques.txt
```

**Important :** Les données utilisées doivent être **représentatives** de l'utilisation en production !

**Fichiers générés :** `*.gcda` (GCC Coverage Data) contenant les statistiques d'exécution

### Étape 3 : Recompilation optimisée

Recompiler le programme en utilisant les données collectées pour guider les optimisations.

```bash
gcc -O2 -fprofile-use programme.c -o programme_optimise
```

**Résultat :** Binaire optimisé selon le profil d'exécution réel.

### Schéma du workflow

```
┌─────────────────────────────────────────────┐
│  Étape 1 : Instrumentation                  │
│  gcc -fprofile-generate programme.c         │
│  → programme_instr                          │
└────────────────┬────────────────────────────┘
                 ↓
┌─────────────────────────────────────────────┐
│  Étape 2 : Exécution et collecte            │
│  ./programme_instr < donnees.txt            │
│  → *.gcda (fichiers de profil)              │
└────────────────┬────────────────────────────┘
                 ↓
┌─────────────────────────────────────────────┐
│  Étape 3 : Recompilation optimisée          │
│  gcc -fprofile-use programme.c              │
│  → programme_optimise                       │
└─────────────────────────────────────────────┘
```

---

## Exemple simple : Branchement prévisible

### Code source

```c
// branch_test.c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int sum = 0;

    for (int i = 0; i < 10000000; i++) {
        // Dans 99% des cas, i < 9900000
        if (i < 9900000) {
            sum += i;  // Branche "chaude" (hot)
        } else {
            sum -= i;  // Branche "froide" (cold)
        }
    }

    printf("Résultat: %d\n", sum);
    return 0;
}
```

### Sans PGO

```bash
gcc -O2 branch_test.c -o branch_test_O2  
time ./branch_test_O2  
```

**Résultat typique :**
```
Résultat: 48510000000  
real    0m0.035s  
```

Le compilateur ne sait pas quelle branche est la plus fréquente, donc il fait une supposition générique.

### Avec PGO

```bash
# Étape 1 : Instrumentation
gcc -O2 -fprofile-generate branch_test.c -o branch_test_instr

# Étape 2 : Profiling
./branch_test_instr
# Génère branch_test.gcda

# Étape 3 : Recompilation optimisée
gcc -O2 -fprofile-use branch_test.c -o branch_test_pgo

# Test
time ./branch_test_pgo
```

**Résultat typique :**
```
Résultat: 48510000000  
real    0m0.028s  ← 20% plus rapide !  
```

**Explication :** Le compilateur sait maintenant que `i < 9900000` est vrai 99% du temps, donc il :
- Organise le code pour optimiser ce chemin
- Place ce code dans les zones de cache optimales
- Améliore la prédiction de branchement

### Vérification dans l'assembleur

```bash
# Comparer l'assembleur
gcc -O2 -S branch_test.c -o branch_test_O2.s  
gcc -O2 -fprofile-use -S branch_test.c -o branch_test_pgo.s  
diff branch_test_O2.s branch_test_pgo.s  
```

Vous verrez que l'organisation du code est différente, avec le chemin chaud optimisé.

---

## Optimisations activées par PGO

### 1. Optimisation de la disposition du code (Code Layout)

**Problème :** Le code des fonctions chaudes peut être éparpillé en mémoire.

**Solution PGO :** Réorganiser le code pour que les fonctions fréquemment appelées ensemble soient **contiguës** en mémoire.

**Bénéfice :** Meilleure utilisation du cache d'instructions.

### 2. Optimisation des branches (Branch Optimization)

**Problème :** Le compilateur ne sait pas quelle branche du `if` est la plus probable.

**Solution PGO :** Placer le code de la branche **la plus fréquente** en premier (moins de sauts).

**Exemple :**

```c
// Code original
if (condition_rare) {  // 1% de chance
    // Branche rare
} else {
    // Branche fréquente (99%)
}

// Après PGO, réorganisé comme :
if (!condition_rare) {  // Test inversé
    // Branche fréquente (placée en premier)
} else {
    // Branche rare
}
```

**Bénéfice :** Moins de mispredictions de branchement.

### 3. Inlining sélectif

**Problème :** Quelles fonctions méritent d'être inline ?

**Solution PGO :** Rendre inline uniquement les fonctions **fréquemment appelées**.

**Exemple :**

```c
// fonction_chaude() est appelée 1 million de fois
// fonction_froide() est appelée 10 fois

// Sans PGO : Le compilateur devine
// Avec PGO : Inline fonction_chaude(), pas fonction_froide()
```

**Bénéfice :** Taille du code optimale (inline seulement ce qui compte).

### 4. Optimisation des boucles

**Problème :** Combien d'itérations fait une boucle en moyenne ?

**Solution PGO :** Optimiser les boucles **avec beaucoup d'itérations** (déroulage, vectorisation).

**Exemple :**

```c
// Boucle qui itère 1000000 fois en moyenne
for (int i = 0; i < n; i++) {
    // PGO peut décider de dérouler cette boucle
}

// Boucle qui itère 3 fois en moyenne
for (int i = 0; i < m; i++) {
    // PGO ne déroule pas (overhead > bénéfice)
}
```

### 5. Élimination de code mort basée sur le profil

**Problème :** Certaines fonctions sont compilées mais jamais utilisées.

**Solution PGO :** Supprimer ou déprioriser le code **jamais exécuté** lors du profiling.

### 6. Optimisation des appels indirects (Indirect Call Promotion)

**Problème :** Appels de fonction via pointeurs (difficiles à optimiser).

```c
void (*fonction_ptr)(int);

// Appel indirect
fonction_ptr(42);
```

**Solution PGO :** Si le profil montre que `fonction_ptr` pointe toujours vers `fonction_A`, transformer en :

```c
if (fonction_ptr == fonction_A) {
    fonction_A(42);  // Appel direct (plus rapide)
} else {
    fonction_ptr(42);  // Fallback
}
```

**Bénéfice :** Appel direct dans 99% des cas → plus rapide.

---

## Cas d'étude : Serveur web

### Scénario

Un serveur web qui traite différents types de requêtes :
- 80% : Requêtes GET de pages statiques
- 15% : Requêtes POST de formulaires
- 4% : Requêtes API REST
- 1% : Autres (WebSockets, etc.)

### Code sans PGO

```c
// server.c
void traiter_requete(Request *req) {
    if (req->type == GET) {
        servir_page_statique(req);
    } else if (req->type == POST) {
        traiter_formulaire(req);
    } else if (req->type == API) {
        traiter_api(req);
    } else {
        traiter_autre(req);
    }
}
```

**Sans PGO :** Le compilateur ne sait pas que GET est 80% des cas.

### Avec PGO

```bash
# Étape 1 : Instrumentation
gcc -O2 -fprofile-generate server.c -o server_instr

# Étape 2 : Profiling avec un échantillon de trafic réel
./server_instr < trafic_reel_1jour.log

# Étape 3 : Recompilation
gcc -O2 -fprofile-use server.c -o server_pgo
```

**Résultats :**

| Métrique | Sans PGO | Avec PGO | Amélioration |
|----------|----------|----------|--------------|
| Requêtes/seconde | 15,000 | 19,500 | +30% |
| Latence P95 | 45 ms | 32 ms | -29% |
| Cache misses | 12.3% | 8.1% | -34% |

**Explication :** Le compilateur a optimisé le chemin GET (80% des requêtes), placé ce code dans les zones optimales du cache, et amélioré la prédiction de branchement.

---

## Exemple avancé : Algorithme de tri

### Code source

```c
// sort_benchmark.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main(void) {
    int *data = malloc(SIZE * sizeof(int));

    // Générer des données selon un pattern réel
    srand(42);
    for (int i = 0; i < SIZE; i++) {
        // 70% des valeurs entre 0-1000
        // 30% des valeurs entre 1000-100000
        if (rand() % 100 < 70) {
            data[i] = rand() % 1000;
        } else {
            data[i] = rand() % 100000;
        }
    }

    clock_t start = clock();
    qsort(data, SIZE, sizeof(int), compare);
    clock_t end = clock();

    printf("Temps de tri: %.3f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

    free(data);
    return 0;
}
```

### Sans PGO

```bash
gcc -O3 sort_benchmark.c -o sort_O3
./sort_O3
```

**Résultat :**
```
Temps de tri: 145.3 ms
```

### Avec PGO

```bash
# Instrumentation
gcc -O3 -fprofile-generate sort_benchmark.c -o sort_instr

# Profiling
./sort_instr
# Génère les fichiers .gcda

# Recompilation
gcc -O3 -fprofile-use sort_benchmark.c -o sort_pgo

# Test
./sort_pgo
```

**Résultat :**
```
Temps de tri: 123.7 ms  ← 15% plus rapide
```

**Explication :** PGO a optimisé la fonction `compare()` et le parcours de `qsort()` selon la distribution réelle des données (70% petites valeurs).

---

## PGO avec plusieurs scénarios

### Problème : Profil unique vs profil diversifié

Si vous profilez avec un seul scénario, l'optimisation peut être **trop spécialisée**.

**Mauvais exemple :**
```bash
# Profiler uniquement avec de petits fichiers
./programme_instr petit_fichier.txt
```

**Bon exemple : Fusionner plusieurs profils**

```bash
# Profiler avec plusieurs scénarios
./programme_instr petit_fichier.txt
./programme_instr fichier_moyen.txt
./programme_instr gros_fichier.txt

# Les profils sont automatiquement fusionnés dans les .gcda
```

GCC fusionne automatiquement les compteurs, donc le profil final représente **tous les scénarios**.

### Pondération des profils

```bash
# Exécuter plusieurs fois avec des poids différents
./programme_instr scenario_frequent.txt   # 10 exécutions
./programme_instr scenario_frequent.txt
# ...
./programme_instr scenario_rare.txt        # 1 exécution
```

Le profil reflétera naturellement la fréquence relative (10:1).

---

## Combiner PGO avec d'autres optimisations

### PGO + LTO (Link-Time Optimization)

**Combinaison recommandée !** PGO et LTO sont complémentaires :

```bash
# Étape 1 : Instrumentation avec LTO
gcc -O3 -flto -fprofile-generate *.c -o programme_instr

# Étape 2 : Profiling
./programme_instr < donnees.txt

# Étape 3 : Recompilation avec PGO + LTO
gcc -O3 -flto -fprofile-use *.c -o programme_optimise
```

**Amélioration typique :** 20-40% plus rapide qu'un code `-O2` de base.

### PGO + SIMD + LTO + march=native (Maximum)

```bash
# Instrumentation
gcc -O3 -flto -march=native -fprofile-generate *.c -o programme_instr

# Profiling
./programme_instr < donnees.txt

# Recompilation avec toutes les optimisations
gcc -O3 -flto -march=native -fprofile-use *.c -o programme_max
```

**Amélioration totale :** 2-5x plus rapide qu'un code `-O0` (selon le projet).

---

## Problèmes et limitations de PGO

### 1. Qualité des données de profiling

**Problème crucial :** Si les données de profiling ne sont pas représentatives, PGO peut **dégrader** les performances !

**Exemple :**

```bash
# Mauvais : Profiler avec des petits fichiers
./programme_instr petit.txt  # 10 KB
# Recompiler et utiliser en production avec de gros fichiers
./programme_pgo gros.txt     # 10 GB  ← Peut être plus lent !
```

**Solution :** Profiler avec des **données représentatives** de la production.

### 2. Coût du processus

PGO nécessite :
1. Compilation instrumentée
2. Exécution du programme (peut être longue)
3. Recompilation

**Impact :** Temps de build total multiplié par 2-3.

**Solution :** Réserver PGO pour les builds de release/production.

### 3. Maintenance des profils

Les profils deviennent **obsolètes** quand le code change significativement.

**Solution :** Recréer les profils régulièrement (tous les X releases majeures).

### 4. Taille des fichiers de profil

Les fichiers `.gcda` peuvent être **très gros** (plusieurs MB à GB pour de gros projets).

**Solution :**
```bash
# Nettoyer après usage
rm -f *.gcda *.gcno
```

### 5. Difficulté avec du code très dynamique

Si le comportement du programme varie beaucoup (IA, jeux, etc.), un profil unique peut ne pas être représentatif.

**Solution :** Fusionner plusieurs profils de scénarios différents.

---

## Vérifier l'impact de PGO

### Comparer les performances

```bash
# Sans PGO
time ./programme_O2 < test.txt

# Avec PGO
time ./programme_pgo < test.txt
```

### Analyser les branches

```bash
# Compter les branch mispredictions
perf stat -e branch-misses ./programme_O2  
perf stat -e branch-misses ./programme_pgo  
```

**Résultat attendu :** Moins de branch misses avec PGO.

### Analyser la disposition du code

```bash
# Voir l'ordre des fonctions dans le binaire
nm -n programme_O2 > ordre_O2.txt  
nm -n programme_pgo > ordre_pgo.txt  
diff ordre_O2.txt ordre_pgo.txt  
```

Avec PGO, les fonctions chaudes sont regroupées ensemble.

---

## Cas où PGO n'aide PAS

### 1. Code très uniforme

Si toutes les branches ont 50/50, toutes les fonctions sont également fréquentes, PGO n'apporte rien.

### 2. Code déjà très optimisé

Si vous avez manuellement optimisé le code (inlining, layout), PGO apporte peu.

### 3. Profil non représentatif

Si les données de profiling sont complètement différentes de l'usage réel, PGO peut **dégrader** les performances.

### 4. Programmes courts

Pour un programme qui s'exécute en < 1 ms, le gain de PGO est négligeable.

### 5. Code dominé par I/O

Si 90% du temps est passé à lire/écrire des fichiers, PGO n'aidera pas.

---

## Bonnes pratiques

### ✅ À faire

1. **Utiliser des données représentatives de la production**
   ```bash
   ./programme_instr < donnees_production_echantillon.txt
   ```

2. **Fusionner plusieurs scénarios**
   ```bash
   ./programme_instr scenario1.txt
   ./programme_instr scenario2.txt
   ./programme_instr scenario3.txt
   ```

3. **Combiner avec LTO**
   ```bash
   gcc -O3 -flto -fprofile-generate *.c
   # ...
   gcc -O3 -flto -fprofile-use *.c
   ```

4. **Vérifier le gain avec des benchmarks**
   ```bash
   time ./programme_O2 < test.txt
   time ./programme_pgo < test.txt
   ```

5. **Recréer les profils après des changements majeurs**

6. **Documenter le processus de profiling**
   - Quelles données ont été utilisées ?
   - Combien d'exécutions ?
   - Quels scénarios ?

### ❌ À éviter

1. **Profiler avec des données non représentatives**
2. **Utiliser un profil obsolète** (code changé depuis)
3. **Oublier de tester le binaire PGO** (peut avoir des bugs)
4. **Activer PGO pour chaque build de dev** (trop lent)
5. **Profiler une seule fois avec un seul scénario**
6. **Ne pas vérifier le gain réel** (assumer que PGO aide toujours)

---

## Workflow recommandé pour la production

### Développement quotidien

```bash
# Build rapide sans optimisation
make dev
```

```makefile
# Makefile
dev:
	gcc -O0 -g *.c -o programme_dev
```

### Build de release standard

```bash
# Build avec optimisations standard
make release
```

```makefile
release:
	gcc -O3 -flto -march=native *.c -o programme_release
```

### Build de production avec PGO (périodique)

```bash
# Build PGO complet (à faire tous les 3-6 mois)
make pgo
```

```makefile
pgo:
	# Instrumentation
	gcc -O3 -flto -march=native -fprofile-generate *.c -o programme_instr

	# Profiling avec données représentatives
	./programme_instr < donnees/scenario1.txt
	./programme_instr < donnees/scenario2.txt
	./programme_instr < donnees/scenario3.txt

	# Recompilation optimisée
	gcc -O3 -flto -march=native -fprofile-use *.c -o programme_pgo

	# Nettoyage
	rm -f programme_instr *.gcda *.gcno
```

---

## Exemple avec CMake

```cmake
cmake_minimum_required(VERSION 3.13)  
project(MonProjet C)  

# Options
option(PGO_GENERATE "Generate PGO profile" OFF)  
option(PGO_USE "Use PGO profile" OFF)

add_executable(programme main.c utils.c)

# Configuration selon le mode
if(PGO_GENERATE)
    target_compile_options(programme PRIVATE -fprofile-generate)
    target_link_options(programme PRIVATE -fprofile-generate)
    message(STATUS "PGO: Génération du profil activée")
elseif(PGO_USE)
    target_compile_options(programme PRIVATE -fprofile-use)
    target_link_options(programme PRIVATE -fprofile-use)
    message(STATUS "PGO: Utilisation du profil activée")
endif()

# Optimisations standard
target_compile_options(programme PRIVATE -O3)
```

**Utilisation :**

```bash
# Étape 1 : Build instrumenté
cmake -DPGO_GENERATE=ON ..  
make  
./programme < donnees.txt

# Étape 2 : Build optimisé
cmake -DPGO_USE=ON ..  
make  
```

---

## PGO dans les projets open-source

### Projets utilisant PGO

- **Firefox** : Gains de 5-15% en performance
- **Chromium** : Amélioration significative du temps de démarrage
- **Python (CPython)** : 10-20% plus rapide avec PGO
- **LLVM/Clang** : S'auto-compile avec PGO
- **MySQL** : Optimisations de requêtes

### Exemple : Compiler Python avec PGO

```bash
# Télécharger les sources
wget https://www.python.org/ftp/python/3.11.0/Python-3.11.0.tgz  
tar xzf Python-3.11.0.tgz  
cd Python-3.11.0  

# Configure avec PGO
./configure --enable-optimizations

# Cette option active automatiquement PGO :
# 1. Compile avec instrumentation
# 2. Exécute la suite de tests (profiling)
# 3. Recompile avec le profil

make -j$(nproc)  
sudo make install  

# Résultat : Python 10-20% plus rapide
```

---

## Checklist : Ai-je besoin de PGO ?

- ✅ Mon programme a des **branches avec fréquences inégales** ?
- ✅ Mon programme a des **chemins chauds/froids** bien définis ?
- ✅ J'ai des **données représentatives** de l'usage réel ?
- ✅ Je peux **exécuter le programme instrumenté** facilement ?
- ✅ Je compile pour une **release/production** ?
- ✅ Le temps de build supplémentaire est **acceptable** ?
- ✅ Je peux **maintenir** les profils dans le temps ?

Si vous répondez **OUI** à la plupart de ces questions, PGO est recommandé.

Si vous développez activement avec compilation fréquente → **non**, PGO est trop lent.

---

## Comparaison des techniques d'optimisation

| Optimisation | Gain typique | Temps compil. | Difficulté | Quand utiliser |
|--------------|--------------|---------------|------------|----------------|
| **-O0 → -O2** | 2-3x | +50% | Facile | Toujours |
| **-O2 → -O3** | 1.2-1.5x | +30% | Facile | Release |
| **+ -march=native** | 1.1-1.2x | +10% | Facile | Même CPU |
| **+ -flto** | 1.1-1.3x | +300% | Facile | Multi-fichiers |
| **+ PGO** | 1.2-1.5x | +200% | Moyen | Profil disponible |
| **LTO + PGO** | 1.3-1.8x | +500% | Moyen | Maximum perf |
| **Algorithme optimal** | 10-1000x | 0% | Difficile | Priorité #1 |

**Hiérarchie des optimisations (rappel) :**

```
1. Bon algorithme (O(n) vs O(n²))          → 100-1000x
2. Structure de données adaptée            → 10-100x
3. SIMD vectorisation                      → 4-16x
4. Optimisations cache                     → 2-10x
5. Flags de compilation (-O2 → -O3)        → 1.5-3x
6. LTO                                     → 1.1-1.3x
7. PGO                                     → 1.2-1.5x       ← Nous sommes ici
8. LTO + PGO                               → 1.3-1.8x       ← Combinaison optimale
```

---

## Résumé

PGO est une optimisation puissante basée sur le comportement réel :

1. ✅ **Gain typique** : 10-30% de performance
2. ✅ **Principe** : Utiliser des données d'exécution pour guider le compilateur
3. ✅ **Process** : Instrumentation → Profiling → Recompilation
4. ✅ **Optimisations** : Code layout, branches, inlining, boucles
5. ✅ **Combinaison** : Excellent avec LTO pour un maximum de performance
6. ❌ **Inconvénient** : Nécessite des données représentatives et un workflow en 3 étapes
7. ❌ **Risque** : Profil non représentatif peut dégrader les performances

**Citations :**

> "Profile-guided optimization is the closest thing we have to a crystal ball for compilers" — Anonyme

> "The best profile is one that matches production workload" — GCC Documentation

**Règle d'or :**

1. **Développement quotidien** : `-O0` ou `-O2` (rapide à compiler)
2. **Release standard** : `-O3 -flto -march=native` (bon compromis)
3. **Production critique** : `-O3 -flto -march=native -fprofile-use` (maximum de performance)

PGO est particulièrement efficace pour :
- Serveurs web/API (patterns de requêtes prévisibles)
- Bases de données (requêtes typiques connues)
- Compilateurs (auto-hébergement)
- Moteurs de jeux (profils de scènes représentatives)
- Applications avec des chemins chauds/froids bien définis

**Ne pas utiliser PGO si :**
- Comportement très variable/aléatoire
- Impossible d'obtenir des données représentatives
- Programme trop court (< 1 ms)
- Code dominé par I/O ou attente réseau

---

*Prochaine section : 27.10 Benchmarking rigoureux*

⏭️ [Benchmarking rigoureux](/27-optimisation-performance/10-benchmarking.md)
