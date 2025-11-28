🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.8 Link-Time Optimization (LTO)

## Introduction

Le **Link-Time Optimization** (LTO) ou **Optimisation lors de l'édition de liens** est une technique qui permet au compilateur d'optimiser le programme **dans son ensemble**, en analysant et optimisant le code **entre plusieurs fichiers source** lors de l'étape finale de linkage.

### Le problème traditionnel

Normalement, le compilateur optimise chaque fichier `.c` **séparément**, sans connaître les autres fichiers du projet :

```
fichier1.c → [Compilation] → fichier1.o
fichier2.c → [Compilation] → fichier2.o
fichier3.c → [Compilation] → fichier3.o
                ↓
    [Linkage] → programme final
```

**Problème :** Le compilateur ne peut pas optimiser **entre les fichiers**. Il ne peut pas :
- Rendre inline une fonction définie dans `fichier2.c` et appelée depuis `fichier1.c`
- Supprimer une fonction non utilisée définie dans `fichier3.c`
- Propager des constantes entre fichiers
- Optimiser globalement l'ensemble du programme

### La solution : LTO

Avec LTO, le compilateur conserve une **représentation intermédiaire** du code dans les fichiers `.o`, puis effectue des optimisations **sur l'ensemble du programme** lors du linkage :

```
fichier1.c → [Compilation + IR] → fichier1.o (contient IR)
fichier2.c → [Compilation + IR] → fichier2.o (contient IR)
fichier3.c → [Compilation + IR] → fichier3.o (contient IR)
                ↓
    [Linkage + Optimisation globale (LTO)] → programme final optimisé
```

**IR** = Intermediate Representation (représentation intermédiaire, comme GIMPLE pour GCC)

### Analogie : La bibliothèque

**Sans LTO (compilation séparée) :**

Imaginez que vous écrivez un livre en équipe :
- Alice écrit le chapitre 1 seule
- Bob écrit le chapitre 2 seul
- Charlie écrit le chapitre 3 seul

Chacun optimise son chapitre sans savoir ce que les autres écrivent. Résultat : des répétitions, des incohérences, des références inutiles.

**Avec LTO (optimisation globale) :**

Maintenant, un éditeur lit **tous les chapitres** avant publication et :
- Supprime les répétitions
- Corrige les incohérences
- Fusionne les sections similaires
- Optimise l'ensemble pour la cohérence globale

Même principe avec le code : LTO voit tout le programme et peut faire des optimisations impossibles lors de la compilation séparée.

---

## Comment fonctionne LTO

### Les étapes avec LTO

1. **Compilation** : Chaque fichier `.c` est compilé en représentation intermédiaire (IR) + code objet
   ```bash
   gcc -flto -c fichier1.c -o fichier1.o
   ```

2. **Stockage de l'IR** : Les fichiers `.o` contiennent à la fois :
   - Le code machine (pour compatibilité)
   - La représentation intermédiaire (pour LTO)

3. **Linkage + Optimisation** : Le linker appelle le compilateur pour :
   - Analyser toute l'IR du programme
   - Effectuer des optimisations inter-procédurales et inter-fichiers
   - Générer le code final optimisé
   ```bash
   gcc -flto fichier1.o fichier2.o fichier3.o -o programme
   ```

### Représentation des fichiers

**Fichier .o normal (sans LTO) :**
```
┌─────────────────────┐
│   Code machine      │  ← Uniquement du code assembleur compilé
│   (assembleur)      │
└─────────────────────┘
Taille : ~50 KB
```

**Fichier .o avec LTO :**
```
┌─────────────────────┐
│   Code machine      │  ← Pour compatibilité (si pas de LTO au link)
├─────────────────────┤
│ Représentation      │  ← IR (GIMPLE/LLVM) pour optimisation
│  intermédiaire (IR) │
└─────────────────────┘
Taille : ~150 KB (plus gros !)
```

---

## Activer LTO

### Compilation simple (un seul fichier)

```bash
# Sans LTO
gcc -O2 programme.c -o programme

# Avec LTO (peu d'effet sur un seul fichier)
gcc -O2 -flto programme.c -o programme
```

### Projet multi-fichiers

```bash
# Étape 1 : Compiler chaque fichier avec -flto
gcc -O2 -flto -c fichier1.c -o fichier1.o
gcc -O2 -flto -c fichier2.c -o fichier2.o
gcc -O2 -flto -c fichier3.c -o fichier3.o

# Étape 2 : Linker avec -flto
gcc -O2 -flto fichier1.o fichier2.o fichier3.o -o programme
```

### Avec Make

```makefile
CC = gcc
CFLAGS = -O2 -flto
LDFLAGS = -flto

OBJECTS = fichier1.o fichier2.o fichier3.o

programme: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o programme

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o programme
```

### Avec CMake

```cmake
cmake_minimum_required(VERSION 3.9)
project(MonProjet C)

# Activer LTO pour tout le projet
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Ou pour une cible spécifique
add_executable(programme fichier1.c fichier2.c fichier3.c)
set_property(TARGET programme PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

# Niveau d'optimisation
target_compile_options(programme PRIVATE -O2)
```

---

## Optimisations activées par LTO

### 1. Inlining inter-fichiers

**Sans LTO :**

```c
// utils.c
int add(int a, int b) {
    return a + b;
}

// main.c
extern int add(int a, int b);

int main() {
    int result = add(5, 10);  // Appel de fonction (overhead)
    return 0;
}
```

Le compilateur ne peut pas rendre `add()` inline car elle est dans un autre fichier.

**Avec LTO :**

Le linker voit que `add()` est petite et peut la rendre inline :

```c
// Code optimisé équivalent après LTO
int main() {
    int result = 15;  // Fonction inline + propagation de constantes !
    return 0;
}
```

### 2. Élimination de code mort (Dead Code Elimination)

**Sans LTO :**

```c
// utils.c
void fonction_utilisee() {
    // ...
}

void fonction_jamais_utilisee() {
    // Ce code reste dans le binaire final !
}
```

Le compilateur conserve `fonction_jamais_utilisee()` car il ne sait pas si elle est utilisée ailleurs.

**Avec LTO :**

Le linker analyse tout le programme et voit que `fonction_jamais_utilisee()` n'est jamais appelée → **elle est supprimée** du binaire final.

**Impact :** Binaire plus petit (réduction typique : 5-15%)

### 3. Propagation de constantes inter-fichiers

**Sans LTO :**

```c
// config.c
int get_max_size() {
    return 1024;
}

// main.c
extern int get_max_size();

int main() {
    int size = get_max_size();  // Appel de fonction
    int buffer[size];  // Allocation variable (VLA)
    // ...
}
```

**Avec LTO :**

```c
// Code optimisé équivalent
int main() {
    int buffer[1024];  // Taille constante ! Plus rapide, pas de VLA
    // ...
}
```

### 4. Optimisation des appels de fonction

**Devirtualization :** Transformer des appels indirects en appels directs
**Cloning :** Créer des versions spécialisées d'une fonction selon les contextes d'appel

### 5. Meilleure analyse des alias

LTO peut mieux déterminer si deux pointeurs peuvent pointer vers la même mémoire, permettant plus d'optimisations.

---

## Exemple concret : Projet multi-fichiers

### Code source

**math_utils.h :**
```c
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int square(int x);
int cube(int x);
int power_of_two(int x);

#endif
```

**math_utils.c :**
```c
#include "math_utils.h"

int square(int x) {
    return x * x;
}

int cube(int x) {
    return x * x * x;
}

// Cette fonction n'est jamais utilisée
int power_of_two(int x) {
    int result = 1;
    for (int i = 0; i < x; i++) {
        result *= 2;
    }
    return result;
}
```

**main.c :**
```c
#include <stdio.h>
#include "math_utils.h"

int main() {
    int n = 10;

    // Utilise square et cube
    int s = square(n);
    int c = cube(n);

    printf("Square: %d, Cube: %d\n", s, c);

    return 0;
}
```

### Compilation sans LTO

```bash
gcc -O2 -c math_utils.c -o math_utils.o
gcc -O2 -c main.c -o main.o
gcc -O2 math_utils.o main.o -o programme_sans_lto

# Voir la taille du binaire
size programme_sans_lto

# Vérifier les symboles
nm programme_sans_lto | grep -E "square|cube|power_of_two"
```

**Résultats typiques :**
```
   text    data     bss     dec     hex filename
   1892     608       8    2508     9cc programme_sans_lto

Symboles :
00000000000011a9 T cube
0000000000001195 T power_of_two  ← Fonction inutilisée présente !
0000000000001189 T square
```

### Compilation avec LTO

```bash
gcc -O2 -flto -c math_utils.c -o math_utils.o
gcc -O2 -flto -c main.c -o main.o
gcc -O2 -flto math_utils.o main.o -o programme_avec_lto

size programme_avec_lto
nm programme_avec_lto | grep -E "square|cube|power_of_two"
```

**Résultats typiques :**
```
   text    data     bss     dec     hex filename
   1756     608       8    2372     944 programme_avec_lto  ← Plus petit !

Symboles :
(aucun symbole square, cube, power_of_two visible)
```

**Analyse :**
1. **Binaire plus petit** : 1892 → 1756 bytes (-7%)
2. **`power_of_two()` supprimée** : Code mort éliminé
3. **`square()` et `cube()` inline** : Plus de symboles visibles = fonctions intégrées dans `main()`

**Vérification dans le code assembleur :**

```bash
objdump -d programme_avec_lto | grep -A 20 "<main>"
```

Vous devriez voir le calcul de `square(10)` et `cube(10)` directement dans `main`, sans appels de fonction.

---

## Benchmark : Impact de LTO

### Projet de test

**Fichiers : 10 fichiers source, 500 lignes chacun, beaucoup de petites fonctions**

```c
// functions.c (répété 10 fois avec différents noms)
int func1(int x) { return x + 1; }
int func2(int x) { return x * 2; }
int func3(int x) { return x - 1; }
// ... 50 fonctions par fichier

// main.c
// Appelle 200 de ces fonctions
int main() {
    int result = 0;
    for (int i = 0; i < 1000000; i++) {
        result += func1(i);
        result += func5(i);
        result += func12(i);
        // ... etc
    }
    return result;
}
```

### Résultats

| Métrique | Sans LTO | Avec LTO | Amélioration |
|----------|----------|----------|--------------|
| **Taille du binaire** | 87 KB | 73 KB | -16% |
| **Temps de compilation** | 1.2 s | 3.5 s | -192% (plus lent) |
| **Temps d'exécution** | 42 ms | 31 ms | +35% (plus rapide) |
| **Appels de fonction** | 200M | 0 | -100% (tout inline) |

**Conclusion :** LTO rend la compilation plus lente, mais le programme final est plus rapide et plus compact.

---

## LTO et bibliothèques

### Bibliothèques statiques (.a)

**Problème :** Les bibliothèques statiques sont des archives de fichiers `.o`. LTO fonctionne, mais nécessite que la bibliothèque soit compilée avec `-flto`.

**Créer une bibliothèque avec LTO :**

```bash
# Compiler les fichiers avec -flto
gcc -O2 -flto -c lib_func1.c -o lib_func1.o
gcc -O2 -flto -c lib_func2.c -o lib_func2.o

# Créer l'archive (bibliothèque statique)
ar rcs libmonlib.a lib_func1.o lib_func2.o

# Utiliser la bibliothèque avec LTO
gcc -O2 -flto main.c -L. -lmonlib -o programme
```

**Note :** Le plugin LTO de GCC doit être configuré pour `ar` et `ranlib` :

```bash
# Utiliser les versions LTO-aware
AR=gcc-ar
RANLIB=gcc-ranlib

gcc-ar rcs libmonlib.a lib_func1.o lib_func2.o
gcc-ranlib libmonlib.a
```

### Bibliothèques dynamiques (.so)

LTO fonctionne aussi pour les bibliothèques partagées, mais avec des limitations :

```bash
# Créer une bibliothèque partagée avec LTO
gcc -O2 -flto -fPIC -shared lib_func1.c lib_func2.c -o libmonlib.so

# Utiliser
gcc -O2 -flto main.c -L. -lmonlib -o programme
```

**Limitation :** LTO ne peut pas optimiser **entre** le programme et la bibliothèque partagée si elles sont linkées séparément. L'optimisation se fait uniquement **à l'intérieur** de la bibliothèque.

---

## LTO et parallélisme

### LTO parallèle (plus rapide)

Par défaut, LTO utilise un seul thread, ce qui peut être lent sur de gros projets.

**Activer le LTO parallèle :**

```bash
# Utiliser N threads (par exemple, 4)
gcc -O2 -flto=4 -c *.c
gcc -O2 -flto=4 *.o -o programme

# Ou auto-detect du nombre de CPUs
gcc -O2 -flto=auto -c *.c
gcc -O2 -flto=auto *.o -o programme
```

**Impact :** Réduction du temps de linkage de 50-80% sur les gros projets.

### Partitionnement (pour très gros projets)

GCC divise le programme en partitions qui sont optimisées en parallèle :

```bash
# Contrôler le nombre de partitions
gcc -O2 -flto -flto-partition=balanced *.o -o programme
```

Options :
- `balanced` : Équilibre temps de compilation et qualité (défaut)
- `one` : Une seule partition (optimisation maximale, mais lent)
- `none` : Pas de partitionnement (chaque fichier séparément)

---

## Combiner LTO avec d'autres optimisations

### LTO + PGO (Profile-Guided Optimization)

La combinaison LTO + PGO donne les **meilleures performances** :

```bash
# Étape 1 : Compiler avec LTO + instrumentation
gcc -O2 -flto -fprofile-generate *.c -o programme_instr

# Étape 2 : Exécuter avec données représentatives
./programme_instr < test_data.txt

# Étape 3 : Recompiler avec LTO + profil
gcc -O2 -flto -fprofile-use *.c -o programme_optimise
```

**Amélioration typique :** 10-30% plus rapide que LTO seul.

### LTO + Optimisations agressives

```bash
# Combine tout
gcc -O3 -flto -march=native -fprofile-use *.c -o programme_max
```

**Flags combinés :**
- `-O3` : Optimisations agressives
- `-flto` : Optimisation inter-fichiers
- `-march=native` : Instructions CPU spécifiques
- `-fprofile-use` : Profil d'exécution

**Amélioration totale :** 2-5x plus rapide qu'un code `-O0` de base (selon le projet).

---

## Problèmes et limitations de LTO

### 1. Temps de compilation augmenté

**Problème :** LTO rend le linkage beaucoup plus lent (2-10x).

**Solution :**
- Utiliser `-flto=auto` pour paralléliser
- Désactiver LTO pour les builds de développement
- Activer LTO uniquement pour les builds de release

**Exemple de Makefile :**

```makefile
# Build de développement (rapide)
dev:
	gcc -O0 -g *.c -o programme_dev

# Build de release (optimisé)
release:
	gcc -O3 -flto=auto *.c -o programme_release
```

### 2. Consommation mémoire élevée

LTO peut consommer beaucoup de RAM lors du linkage (plusieurs GB pour de gros projets).

**Solution :**
- Augmenter la RAM disponible
- Utiliser `-flto-partition=balanced` ou `none`
- Compiler sur une machine puissante

### 3. Problèmes de débogage

Avec LTO, le code est fortement optimisé et réorganisé, ce qui complique le débogage :
- Variables optimisées (non visibles dans GDB)
- Fonctions inline (pas de stack frame)
- Code réordonné

**Solution :**

```bash
# Build debug sans LTO
gcc -O0 -g *.c -o programme_debug

# Build release avec LTO mais symboles debug
gcc -O2 -flto -g *.c -o programme_release_debug
```

### 4. Incompatibilité avec certaines bibliothèques

Certaines bibliothèques externes ne sont pas compilées avec `-flto` et peuvent causer des problèmes.

**Solution :**
- Compiler la bibliothèque depuis les sources avec `-flto`
- Ou désactiver LTO pour ce projet

### 5. Bugs du compilateur

LTO est une fonctionnalité complexe et peut parfois exposer des bugs du compilateur ou du code.

**Solution :**
- Tester rigoureusement le code avec et sans LTO
- Utiliser une version récente de GCC (bugs corrigés)
- Signaler les bugs sur https://gcc.gnu.org/bugzilla/

---

## Cas où LTO n'aide PAS

### 1. Programme à un seul fichier

LTO optimise **entre fichiers**. Un seul fichier → pas de gain.

### 2. Bibliothèques déjà très optimisées

Les bibliothèques comme OpenBLAS, FFTW sont déjà optimisées au maximum. LTO n'apportera rien.

### 3. Code dominé par I/O

Si votre programme passe 90% du temps à lire/écrire des fichiers, LTO n'améliorera pas les performances.

### 4. Petits programmes

Pour un programme de 100 lignes, LTO est overkill. Le gain est négligeable et le temps de compilation augmenté.

---

## Vérifier l'impact de LTO

### Comparer la taille des binaires

```bash
# Compiler sans LTO
gcc -O2 *.c -o programme_sans_lto
strip programme_sans_lto  # Enlever les symboles
ls -lh programme_sans_lto

# Compiler avec LTO
gcc -O2 -flto *.c -o programme_avec_lto
strip programme_avec_lto
ls -lh programme_avec_lto

# Comparer
du -h programme_*
```

**Réduction typique :** 5-20% (selon le projet)

### Comparer les performances

```bash
# Sans LTO
time ./programme_sans_lto

# Avec LTO
time ./programme_avec_lto
```

**Amélioration typique :** 5-15% (très variable selon le projet)

### Analyser les symboles

```bash
# Compter les fonctions dans le binaire
nm programme_sans_lto | grep " T " | wc -l
nm programme_avec_lto | grep " T " | wc -l
```

Moins de symboles avec LTO = plus de fonctions inline/éliminées.

---

## Bonnes pratiques

### ✅ À faire

1. **Activer LTO pour les builds de release**
   ```bash
   gcc -O2 -flto -march=native *.c -o programme_release
   ```

2. **Utiliser LTO parallèle pour les gros projets**
   ```bash
   gcc -O2 -flto=auto *.c -o programme
   ```

3. **Combiner avec d'autres optimisations**
   ```bash
   gcc -O3 -flto -march=native *.c -o programme
   ```

4. **Désactiver pour les builds de développement**
   ```bash
   gcc -O0 -g *.c -o programme_dev  # Rapide à compiler
   ```

5. **Tester rigoureusement**
   - Comparer les résultats avec/sans LTO
   - Benchmark pour vérifier le gain
   - Tests unitaires pour détecter les bugs

6. **Utiliser les outils gcc-ar et gcc-ranlib pour les bibliothèques statiques**
   ```bash
   gcc-ar rcs libmonlib.a *.o
   gcc-ranlib libmonlib.a
   ```

### ❌ À éviter

1. **Activer LTO pour chaque build de dev** (compilation trop lente)
2. **Oublier de tester avec LTO** (peut exposer des bugs)
3. **Utiliser LTO sur des machines avec peu de RAM** (< 4 GB)
4. **Combiner LTO avec des bibliothèques non-LTO** sans précaution
5. **Ignorer les warnings du linker** (peuvent indiquer des problèmes)

---

## Checklist : Ai-je besoin de LTO ?

- ✅ Mon projet a **plusieurs fichiers source** ?
- ✅ Mon projet a **beaucoup de petites fonctions** ?
- ✅ Je veux **réduire la taille** du binaire ?
- ✅ Je peux **attendre** un linkage plus long ?
- ✅ J'ai assez de **RAM** (>4 GB) ?
- ✅ Je compile pour une **release** (pas du développement) ?

Si vous répondez **OUI** à la plupart de ces questions, LTO est recommandé.

Si vous êtes en phase de développement actif avec compilation fréquente → **non**, restez avec `-O0` ou `-O2` sans LTO.

---

## Comparaison : LTO vs autres optimisations

| Optimisation | Gain typique | Temps de compilation | Complexité |
|--------------|--------------|----------------------|------------|
| **-O0 → -O2** | 2-3x | +50% | Facile |
| **-O2 → -O3** | 1.2-1.5x | +30% | Facile |
| **+ -flto** | 1.1-1.3x | +200-500% | Facile |
| **+ -march=native** | 1.1-1.2x | +10% | Facile |
| **+ PGO** | 1.2-1.5x | +300% | Moyen |
| **Algorithme optimal** | 10-1000x | 0% | Difficile |

**Conclusion :** LTO est utile, mais pas aussi impactant qu'un bon algorithme ou même `-O2`.

---

## Exemples de projets utilisant LTO

### Projets open-source qui activent LTO

- **Firefox** : Compile avec LTO pour réduire la taille et améliorer les performances
- **Linux Kernel** : Support LTO depuis la version 5.12
- **LLVM/Clang** : S'auto-compile avec LTO
- **Python (CPython)** : Option `--with-lto` lors du build
- **Git** : Makefile avec support LTO optionnel

### Configuration pour Python

```bash
# Compiler Python avec LTO (10-15% plus rapide)
./configure --with-lto
make
sudo make install
```

### Configuration pour le kernel Linux

```bash
# Dans .config
CONFIG_LTO_CLANG=y
CONFIG_LTO_CLANG_THIN=y

make -j$(nproc)
```

---

## Alternatives à LTO

### Whole Program Optimization (WPO)

Certains compilateurs (comme MSVC sur Windows) utilisent WPO (Whole Program Optimization) qui est similaire à LTO.

### Unity Builds

Une autre approche : **tout compiler en un seul fichier** en incluant tous les `.c` :

```c
// unity.c
#include "fichier1.c"
#include "fichier2.c"
#include "fichier3.c"
// ...
```

```bash
gcc -O3 unity.c -o programme
```

**Avantages :**
- ✅ Le compilateur voit tout le code d'un coup
- ✅ Optimisations inter-fichiers sans LTO
- ✅ Compilation potentiellement plus rapide que LTO

**Inconvénients :**
- ❌ Gestion complexe (conflits de noms, macros)
- ❌ Recompilation complète à chaque modification
- ❌ Peu utilisé en pratique

---

## Résumé

LTO est une optimisation puissante pour les projets multi-fichiers :

1. ✅ **Gain typique** : 5-15% de performance, 5-20% de réduction de taille
2. ✅ **Activation facile** : `-flto` en compilation et linkage
3. ✅ **Optimisations** : Inlining inter-fichiers, élimination de code mort, propagation de constantes
4. ✅ **Cas d'usage** : Builds de release, projets avec beaucoup de fichiers
5. ❌ **Inconvénient** : Temps de compilation multiplié par 2-5x
6. ❌ **Limitation** : Nécessite beaucoup de RAM pour les gros projets

**Hiérarchie des optimisations (rappel) :**

```
1. Bon algorithme (O(n) vs O(n²))          → 100-1000x
2. Structure de données adaptée            → 10-100x
3. SIMD vectorisation                      → 4-16x
4. Optimisations cache                     → 2-10x
5. Flags de compilation (-O2 → -O3)        → 1.5-3x
6. LTO                                     → 1.1-1.3x       ← Nous sommes ici
7. PGO                                     → 1.2-1.5x
```

**Citation :**

> "LTO is not a silver bullet, but it's a useful tool in the optimization toolbox" — GCC Documentation

**Règle d'or :** Activez LTO pour vos builds de **release/production**, mais désactivez-le pour le **développement** (compilation trop lente). Combinez-le avec `-O3` et `-march=native` pour un maximum de performance !

---

*Prochaine section : 27.9 Profile-Guided Optimization (PGO)*

⏭️ [Profile-Guided Optimization (PGO)](/27-optimisation-performance/09-pgo.md)
