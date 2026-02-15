🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.5 Branch prediction

## Introduction

La **prédiction de branchement** (branch prediction) est une technique utilisée par les processeurs modernes pour deviner quel chemin d'exécution sera pris lors d'une condition (`if`, boucles, etc.) **avant même** d'avoir évalué la condition.

### Pourquoi la prédiction de branchement est-elle nécessaire ?

Les processeurs modernes sont des **machines parallèles** qui exécutent plusieurs instructions simultanément grâce au **pipeline d'instructions**. Mais quand le CPU rencontre un branchement (une condition), il a un problème :

```c
if (x > 10) {
    // Chemin A
    y = x * 2;
} else {
    // Chemin B
    y = x / 2;
}
```

Le CPU doit attendre de connaître la valeur de `x` pour savoir quel chemin prendre. Pendant ce temps, le pipeline est **bloqué** → perte de performance !

**Solution :** Le CPU **devine** (prédit) quel chemin sera pris et commence à exécuter les instructions de ce chemin **avant** de savoir si la prédiction est correcte.

### Analogie : Le carrefour

Imaginez que vous conduisez et approchez d'un carrefour avec un feu tricolore au loin. Vous ne voyez pas encore la couleur du feu, mais :

- **Sans prédiction :** Vous ralentissez et attendez de voir la couleur → perte de temps
- **Avec prédiction :** Vous continuez à la même vitesse en **pariant** que le feu sera vert
  - Si le feu est effectivement vert → vous avez gagné du temps ✅
  - Si le feu est rouge → vous devez freiner brusquement et perdre du temps ❌

Le CPU fait la même chose : il parie sur le résultat d'un `if` et continue à avancer. Si la prédiction est bonne, tout va bien. Si elle est mauvaise (**branch misprediction**), il doit **annuler** toutes les instructions exécutées et repartir sur le bon chemin → **pénalité de 10-20 cycles** !

---

## Le pipeline d'instructions

### Architecture simplifiée

Un processeur moderne exécute les instructions en plusieurs étapes (pipeline) :

```
┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐
│  Fetch   │→  │  Decode  │→  │ Execute  │→  │  Memory  │→  │ Write-   │
│(Chargmt) │   │(Décodage)│   │(Exécut.) │   │ Access   │   │  Back    │
└──────────┘   └──────────┘   └──────────┘   └──────────┘   └──────────┘
```

**Sans branchement :** Plusieurs instructions avancent simultanément dans le pipeline.

**Exemple (sans branchement) :**

```
Cycle 1:  [Instr1 Fetch]  
Cycle 2:  [Instr1 Decode] [Instr2 Fetch]  
Cycle 3:  [Instr1 Exec]   [Instr2 Decode] [Instr3 Fetch]  
Cycle 4:  [Instr1 Mem]    [Instr2 Exec]   [Instr3 Decode] [Instr4 Fetch]  
Cycle 5:  [Instr1 WB]     [Instr2 Mem]    [Instr3 Exec]   [Instr4 Decode] [Instr5 Fetch]  
```

**Débit :** 1 instruction complétée par cycle (après le remplissage du pipeline).

### Avec un branchement mal prédit

```c
if (condition) {  // ← Branchement
    A;
} else {
    B;
}
```

**Si prédiction fausse :**

```
Cycle 1:  [IF Fetch]  
Cycle 2:  [IF Decode]  
Cycle 3:  [IF Exec] → Prédiction : condition vraie, on précharge A  
Cycle 4:  [A Fetch]  
Cycle 5:  [A Decode]  
Cycle 6:  [A Exec] → On découvre que la condition est FAUSSE !  
Cycle 7:  [FLUSH] ← Annuler A  
Cycle 8:  [FLUSH]  
Cycle 9:  [B Fetch] → Recommencer avec B  
Cycle 10: [B Decode]  
Cycle 11: [B Exec]  
```

**Pénalité :** ~10-20 cycles perdus à cause du misprediction !

---

## Types de prédicteurs de branchement

### 1. Prédicteur statique (simple)

**Règle simple :**
- Les branchements **vers l'arrière** (boucles) sont prédits **pris** (condition vraie)
- Les branchements **vers l'avant** (`if`) sont prédits **non pris** (condition fausse)

```c
// Branchement vers l'arrière (boucle) → prédit PRIS
for (int i = 0; i < 100; i++) {  // 99 fois vrai, 1 fois faux
    // ...
}
// Taux de réussite : 99%

// Branchement vers l'avant → prédit NON PRIS
if (condition_rare) {  // Rarement vrai
    // ...
}
```

**Limites :** Ne s'adapte pas au comportement dynamique du programme.

### 2. Prédicteur dynamique (moderne)

Les processeurs modernes utilisent des **prédicteurs adaptatifs** qui apprennent le comportement des branchements :

#### a) Prédicteur à 1 bit

Mémorise le **dernier résultat** de chaque branchement :

```
Branchement pris     → Prochaine fois : prédit PRIS  
Branchement non pris → Prochaine fois : prédit NON PRIS  
```

**Problème :** Mauvais pour les patterns alternés (vrai, faux, vrai, faux...).

#### b) Prédicteur à 2 bits (plus courant)

Utilise un **compteur à saturation** avec 4 états :

```
État 0: Fortement NON PRIS     (predict: false)
État 1: Faiblement NON PRIS    (predict: false)
État 2: Faiblement PRIS        (predict: true)
État 3: Fortement PRIS         (predict: true)
```

**Transitions :**
- Branchement pris : compteur++
- Branchement non pris : compteur--

**Avantage :** Plus tolérant aux exceptions occasionnelles.

#### c) Prédicteur à historique (branch history)

Mémorise les **N derniers résultats** pour détecter des patterns complexes.

**Exemple :** Pattern "vrai, vrai, faux" répété
```
T T F | T T F | T T F | T T F
```

Un prédicteur à historique peut apprendre ce pattern et prédire correctement.

### 3. Prédicteur basé sur la corrélation

Utilise l'historique des **autres branchements** pour améliorer la prédiction.

**Exemple :**
```c
if (a > 0) {
    // ...
}

if (b > 0) {  // Corrélé avec le premier if
    // ...
}
```

Si `a > 0` est vrai, il y a de fortes chances que `b > 0` soit aussi vrai. Le CPU peut apprendre ces corrélations.

---

## Impact sur les performances

### Coût d'un branch misprediction

- **Processeur moderne (Intel/AMD)** : 15-20 cycles perdus
- **ARM** : 10-15 cycles
- **À 3 GHz** : 15 cycles = ~5 nanosecondes

**Exemple concret :**

```c
// Boucle avec 1 million d'itérations
for (int i = 0; i < 1000000; i++) {
    if (condition_aleatoire) {  // 50% vrai, 50% faux
        // Traitement A
    } else {
        // Traitement B
    }
}
```

**Scénario 1 : Prédiction parfaite (100%)**
- 0 mispredictions
- Temps : ~5 ms

**Scénario 2 : Prédiction aléatoire (50% de réussite)**
- 500,000 mispredictions
- Pénalité : 500,000 × 15 cycles = 7,500,000 cycles
- À 3 GHz : 7,500,000 / 3,000,000,000 = **2.5 ms de perte** !
- Temps total : ~7.5 ms

**Conclusion :** Les branch mispredictions peuvent **doubler** le temps d'exécution !

---

## Mesurer les branch mispredictions

### Avec perf (Linux)

```bash
# Compiler le programme
gcc -O2 -g test_branch.c -o test_branch

# Mesurer les branches et mispredictions
perf stat -e branches,branch-misses ./test_branch
```

**Exemple de sortie :**

```
 Performance counter stats for './test_branch':

     123,456,789      branches
       6,789,012      branch-misses             #    5.50% of all branches

       0.523456789 seconds time elapsed
```

**Interprétation :**
- **Branch miss rate < 2%** : Excellent ✅
- **2-5%** : Bon
- **5-10%** : Acceptable
- **> 10%** : Problématique ❌ (optimisation nécessaire)

### Avec Valgrind Cachegrind

```bash
valgrind --tool=cachegrind --branch-sim=yes ./test_branch

# Voir les détails
cg_annotate cachegrind.out.<pid>
```

**Exemple de sortie :**

```
Branches:        123,456,789  (  100.0% of all Branches)  
Mispredicts:       6,789,012  (    5.5% of all Branches)  
```

---

## Exemples de code : Impact des branches

### Exemple 1 : Condition prévisible vs imprévisible

**Code :**

```c
// test_branch.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 10000000

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    int *tableau = malloc(TAILLE * sizeof(int));

    // Initialiser avec valeurs aléatoires
    srand(42);  // Seed fixe pour reproductibilité
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand() % 256;
    }

    clock_t debut, fin;
    long long somme;

    // Test 1 : Condition prévisible (valeurs triées)
    // On trie d'abord le tableau
    qsort(tableau, TAILLE, sizeof(int), compare);

    debut = clock();
    somme = 0;
    for (int i = 0; i < TAILLE; i++) {
        if (tableau[i] >= 128) {  // Prévisible car trié
            somme += tableau[i];
        }
    }
    fin = clock();

    printf("Condition prévisible:\n");
    printf("  Somme: %lld\n", somme);
    printf("  Temps: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    // Test 2 : Condition imprévisible (valeurs aléatoires)
    // Mélanger à nouveau
    srand(42);
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand() % 256;
    }

    debut = clock();
    somme = 0;
    for (int i = 0; i < TAILLE; i++) {
        if (tableau[i] >= 128) {  // Imprévisible !
            somme += tableau[i];
        }
    }
    fin = clock();

    printf("\nCondition imprévisible:\n");
    printf("  Somme: %lld\n", somme);
    printf("  Temps: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    free(tableau);
    return 0;
}
```

**Résultats typiques :**

```bash
gcc -O2 test_branch.c -o test_branch
./test_branch

# Sortie :
# Condition prévisible:
#   Somme: 915237845
#   Temps: 8.5 ms
#
# Condition imprévisible:
#   Somme: 915237845
#   Temps: 28.3 ms  ← 3.3x plus lent !
```

**Mesure avec perf :**

```bash
# Données triées (prévisibles)
perf stat -e branch-misses ./test_branch_sorted
# branch-misses: ~1.2% ✅

# Données aléatoires (imprévisibles)
perf stat -e branch-misses ./test_branch_random
# branch-misses: ~24.5% ❌
```

**Explication :**
- **Données triées** : Le prédicteur apprend rapidement que les premières valeurs sont < 128 (condition fausse) et les suivantes sont >= 128 (condition vraie).
- **Données aléatoires** : Le prédicteur ne peut pas deviner, taux de réussite ~50% → beaucoup de mispredictions → lent !

### Exemple 2 : Éliminer les branches avec du code branch-free

**Problème :**

```c
// Version avec branchement
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
```

Si `a > b` est imprévisible, il y aura beaucoup de mispredictions.

**Solution : Version sans branchement (branch-free)**

```c
// Version sans branchement (utilise une instruction conditionnelle CPU)
int max(int a, int b) {
    return a > b ? a : b;
}

// Ou avec arithmétique (tricky mais instructif)
int max_branchfree(int a, int b) {
    int diff = a - b;
    int sign = (diff >> 31);  // 0 si diff >= 0, -1 si diff < 0
    return a - (sign & diff);
}
```

**Note :** Les compilateurs modernes avec `-O2` transforment souvent le premier `if` en instruction conditionnelle (`cmov` sur x86) automatiquement !

**Vérification :**

```bash
gcc -O2 -S test_max.c -o test_max.s  
cat test_max.s  
```

Vous devriez voir une instruction `cmovg` ou `cmovl` (conditional move) au lieu d'un `jmp` (jump).

### Exemple 3 : Pattern de branchement régulier

**Code avec pattern régulier :**

```c
// Pattern régulier : vrai, faux, vrai, faux...
for (int i = 0; i < 1000000; i++) {
    if (i % 2 == 0) {  // Pattern parfaitement prévisible
        // Pair
    } else {
        // Impair
    }
}
```

Les prédicteurs modernes peuvent apprendre ce pattern → taux de réussite très élevé.

**Code avec pattern irrégulier :**

```c
// Pattern irrégulier
for (int i = 0; i < 1000000; i++) {
    if (hash(i) % 2 == 0) {  // Pattern imprévisible
        // ...
    }
}
```

Impossible à prédire → mauvaises performances.

---

## Techniques pour optimiser la prédiction de branchement

### 1. Éliminer les branches quand possible

#### a) Utiliser des opérations arithmétiques

**❌ MAUVAIS (avec branchement) :**

```c
int abs_value(int x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}
```

**✅ BON (sans branchement) :**

```c
int abs_value(int x) {
    int mask = x >> 31;        // 0 si positif, -1 si négatif
    return (x + mask) ^ mask;
}

// Ou encore plus simple (compilateur optimise)
int abs_value(int x) {
    return x < 0 ? -x : x;  // Devient une instruction CMOV
}
```

#### b) Utiliser des lookup tables

**❌ MAUVAIS :**

```c
int est_majuscule(char c) {
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}
```

**✅ BON (lookup table) :**

```c
// Table pré-calculée
static const int table_majuscule[256] = {
    0, 0, 0, ..., 0,  // 0-64
    1, 1, 1, ..., 1,  // 65-90 (A-Z)
    0, 0, 0, ..., 0   // 91-255
};

int est_majuscule(char c) {
    return table_majuscule[(unsigned char)c];  // Pas de branchement !
}
```

### 2. Réorganiser le code pour rendre les branches prévisibles

#### a) Trier les données

Comme vu dans l'exemple 1, trier les données avant de les traiter peut drastiquement améliorer la prédictibilité.

```c
// Avant traitement
qsort(tableau, taille, sizeof(int), compare);

// Maintenant les conditions sont prévisibles
for (int i = 0; i < taille; i++) {
    if (tableau[i] >= seuil) {
        traitement(tableau[i]);
    }
}
```

**Trade-off :** Le tri coûte O(n log n), mais si le traitement est fait plusieurs fois, ça peut valoir le coup.

#### b) Séparer les cas fréquents des cas rares

```c
// ❌ MAUVAIS : Cas rare testé en premier
if (condition_rare) {  // 1% de chance
    traitement_rare();
} else {
    traitement_normal();  // 99% de chance
}

// ✅ BON : Cas fréquent testé en premier
if (condition_frequente) {  // 99% de chance
    traitement_normal();
} else {
    traitement_rare();  // 1% de chance
}
```

Le prédicteur apprendra que le premier chemin est presque toujours pris.

### 3. Utiliser les fonctions prédictives (hints)

GCC offre des **built-ins** pour donner des hints au compilateur :

```c
// Indiquer qu'une condition est très probable
if (__builtin_expect(condition, 1)) {  // 1 = vrai probable
    // Chemin principal
} else {
    // Chemin rare
}

// Indiquer qu'une condition est peu probable
if (__builtin_expect(erreur != 0, 0)) {  // 0 = faux probable
    // Gestion d'erreur (rare)
}
```

**Macros courantes :**

```c
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

// Utilisation
if (unlikely(erreur)) {
    return -1;
}

if (likely(pointeur != NULL)) {
    *pointeur = valeur;
}
```

**Note :** Ces hints sont surtout utiles pour les chemins d'erreur rares. Pour du code normal, le prédicteur dynamique fait déjà un excellent travail.

### 4. Remplacer les branches par des instructions conditionnelles

Les CPUs modernes ont des instructions **conditional move** (CMOV sur x86) :

```c
// Le compilateur peut transformer ça en CMOV
int max = a > b ? a : b;

// En assembleur x86 (avec -O2) :
// cmp    eax, edx     ; Compare a et b
// cmovl  eax, edx     ; Si a < b, move b dans eax
```

Pas de branchement → pas de misprediction possible !

**Limitations :** Ne fonctionne que pour des opérations simples (affectation d'une valeur).

### 5. Dérouler les boucles avec des branches

Si une boucle contient une branche prévisible, le déroulage peut améliorer les performances :

```c
// ❌ Version compacte mais avec branches
for (int i = 0; i < n; i++) {
    if (condition[i]) {
        traitement(data[i]);
    }
}

// ✅ Version déroulée (si condition prévisible par blocs)
for (int i = 0; i < n; i += 4) {
    if (condition[i]) traitement(data[i]);
    if (condition[i+1]) traitement(data[i+1]);
    if (condition[i+2]) traitement(data[i+2]);
    if (condition[i+3]) traitement(data[i+3]);
}
```

**Avantage :** Moins de branches, meilleure utilisation du pipeline.

---

## Cas d'étude : Filtrage de données

### Problème : Filtrer des nombres > 128

```c
#define TAILLE 10000000

int main() {
    int *input = malloc(TAILLE * sizeof(int));
    int *output = malloc(TAILLE * sizeof(int));

    // Remplir avec valeurs aléatoires [0-255]
    for (int i = 0; i < TAILLE; i++) {
        input[i] = rand() % 256;
    }

    // Filtrer
    int count = 0;
    for (int i = 0; i < TAILLE; i++) {
        if (input[i] > 128) {
            output[count++] = input[i];
        }
    }

    printf("Éléments filtrés : %d\n", count);
}
```

**Problème :** La condition `input[i] > 128` est imprévisible (50% true, 50% false) → beaucoup de mispredictions.

### Solution 1 : Trier d'abord

```c
// Trier les données
qsort(input, TAILLE, sizeof(int), compare);

// Filtrer (maintenant prévisible)
int count = 0;  
for (int i = 0; i < TAILLE; i++) {  
    if (input[i] > 128) {
        output[count++] = input[i];
    }
}
```

**Résultat :** 2-3x plus rapide (si on peut se permettre de trier).

### Solution 2 : Branch-free avec masque

```c
int count = 0;  
for (int i = 0; i < TAILLE; i++) {  
    int condition = input[i] > 128;  // 0 ou 1
    output[count] = input[i];
    count += condition;  // Incrémente seulement si condition vraie
}
```

**Attention :** Cette version écrit parfois des valeurs inutiles dans `output`, mais évite les branches.

**Performance :** Peut être plus rapide si les mispredictions coûtent très cher.

### Solution 3 : Traitement par blocs

```c
// Compter d'abord le nombre d'éléments > 128 par bloc
#define BLOCK_SIZE 64

for (int block = 0; block < TAILLE; block += BLOCK_SIZE) {
    int local_count = 0;
    int local_output[BLOCK_SIZE];

    // Filtrer le bloc
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (input[block + i] > 128) {
            local_output[local_count++] = input[block + i];
        }
    }

    // Copier le résultat
    memcpy(&output[count], local_output, local_count * sizeof(int));
    count += local_count;
}
```

**Avantage :** Meilleure localité de cache, moins de branches dans la boucle externe.

---

## Profiler les branch mispredictions

### Script de benchmark complet

```c
// benchmark_branches.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 100000000

void test_predictable() {
    int somme = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if (i % 2 == 0) {  // Pattern régulier
            somme += i;
        }
    }
    printf("Somme (prévisible): %d\n", somme);
}

void test_unpredictable() {
    int somme = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if ((i * 1103515245 + 12345) & 1) {  // Pattern pseudo-aléatoire
            somme += i;
        }
    }
    printf("Somme (imprévisible): %d\n", somme);
}

int main() {
    clock_t debut, fin;

    printf("=== Test branches prévisibles ===\n");
    debut = clock();
    test_predictable();
    fin = clock();
    printf("Temps: %.3f ms\n\n", (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    printf("=== Test branches imprévisibles ===\n");
    debut = clock();
    test_unpredictable();
    fin = clock();
    printf("Temps: %.3f ms\n", (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    return 0;
}
```

**Compilation et profiling :**

```bash
# Compiler
gcc -O2 benchmark_branches.c -o benchmark_branches

# Exécuter avec perf
perf stat -e branches,branch-misses ./benchmark_branches

# Résultats typiques :
#
# === Test branches prévisibles ===
# Somme (prévisible): 1234567890
# Temps: 245.3 ms
#
# === Test branches imprévisibles ===
# Somme (imprévisible): 1234567890
# Temps: 892.7 ms  ← 3.6x plus lent !
#
# Performance counter stats:
#    200,000,000      branches
#     50,123,456      branch-misses     #   25.06% of all branches (version imprévisible)
```

---

## Optimisations du compilateur

### Vectorisation automatique

Avec `-O3` ou `-ftree-vectorize`, GCC peut transformer du code avec branches en code vectoriel sans branches :

```c
// Code original
for (int i = 0; i < N; i++) {
    if (a[i] > 0) {
        b[i] = a[i] * 2;
    } else {
        b[i] = a[i] / 2;
    }
}

// Après vectorisation (simplifié)
// Utilise des instructions SIMD pour traiter 4/8 éléments à la fois
// Calcule les deux branches et sélectionne avec un masque
```

**Vérifier la vectorisation :**

```bash
gcc -O3 -ftree-vectorize -fopt-info-vec-optimized test.c

# Sortie :
# test.c:10:5: optimized: loop vectorized using 128 bit vectors
```

### Transformation en code sans branches

GCC peut remplacer des `if` simples par des instructions conditionnelles :

```c
// Code source
int max = (a > b) ? a : b;

// Assembleur généré (x86 avec -O2)
cmp    eax, edx  
cmovl  eax, edx    // Conditional move, pas de branch !  
```

**Vérifier :**

```bash
gcc -O2 -S test.c -o test.s  
grep "cmov\|jmp" test.s  
```

Si vous voyez des `cmov` au lieu de `jmp`, c'est bon signe !

---

## Cas où la prédiction de branchement n'est PAS un problème

### 1. Branches très prévisibles (> 95%)

```c
// Gestion d'erreur (rare)
if (unlikely(pointeur == NULL)) {
    return -1;
}

// Le prédicteur apprend rapidement que cette branche est rarement prise
```

### 2. Code dominé par d'autres goulots

Si votre programme passe 90% de son temps à accéder à la mémoire (cache misses), optimiser les branches aura peu d'impact.

### 3. Boucles très courtes

Si le corps de la boucle est très court (< 10 cycles), la pénalité de misprediction est proportionnellement plus faible.

---

## Résumé des bonnes pratiques

### ✅ Faire

1. **Mesurer d'abord** avec `perf stat -e branch-misses`
2. **Trier les données** si possible pour rendre les branches prévisibles
3. **Séparer cas fréquents et rares** (code hot vs cold)
4. **Utiliser des opérations arithmétiques** au lieu de branches simples
5. **Utiliser `likely()` / `unlikely()`** pour les chemins d'erreur
6. **Laisser le compilateur optimiser** avec `-O2` / `-O3`
7. **Dérouler les boucles** avec des branches prévisibles par blocs

### ❌ Éviter

1. **Branches dans les boucles avec données aléatoires**
2. **Conditions complexes imprévisibles**
3. **Tester les cas rares en premier**
4. **Ignorer les branch mispredictions dans du code critique**

---

## Checklist : Code branch-friendly

Avant d'optimiser :
- ✅ Ai-je mesuré le taux de branch misses avec `perf` ?
- ✅ Le taux de misses est-il > 5% ?
- ✅ Les branches sont-elles dans une boucle critique ?

Pendant l'optimisation :
- ✅ Puis-je trier les données pour rendre les branches prévisibles ?
- ✅ Puis-je éliminer les branches avec du code arithmétique ?
- ✅ Puis-je utiliser une lookup table ?
- ✅ Puis-je réorganiser le code pour mettre les cas fréquents en premier ?

Après l'optimisation :
- ✅ Ai-je re-mesuré le taux de branch misses ?
- ✅ Les performances ont-elles vraiment été améliorées ?
- ✅ Le code reste-t-il lisible et maintenable ?

---

## Outils et commandes

### Mesure des branch misses

```bash
# Mesure basique
perf stat -e branches,branch-misses ./programme

# Mesure détaillée
perf stat -e branches,branch-misses,branch-instructions,branch-misses ./programme

# Profiler par fonction
perf record -e branch-misses ./programme  
perf report  

# Avec Valgrind
valgrind --tool=cachegrind --branch-sim=yes ./programme
```

### Analyse du code assembleur

```bash
# Voir les instructions de branchement
gcc -O2 -S test.c -o test.s  
grep -E "jmp|je|jne|jg|jl|cmov" test.s  

# Avec objdump
gcc -O2 test.c -o test  
objdump -d test | grep -E "jmp|je|jne|jg|jl|cmov"  
```

---

## Pour aller plus loin

### Lectures recommandées

- **"Computer Architecture: A Quantitative Approach"** par Hennessy & Patterson (chapitre sur la prédiction de branchement)
- **"Modern Processor Design"** par Shen & Lipasti
- **Intel Optimization Manual** : Section sur le branch prediction
- **Blog de Daniel Lemire** : Articles sur les optimisations sans branches

### Ressources en ligne

- **Why is processing a sorted array faster?** (Stack Overflow) : https://stackoverflow.com/questions/11227809/
- **Branch Prediction** (Wikipedia) : https://en.wikipedia.org/wiki/Branch_predictor
- **Agner Fog's Optimization Manuals** : https://www.agner.org/optimize/

### Techniques avancées

- **Branch elimination** : Remplacer les branches par des calculs
- **Predicated execution** : Instructions conditionnelles (ARM, IA-64)
- **Loop fusion** : Fusionner des boucles pour réduire les branches
- **Speculative execution** : Exécution spéculative (sujet des vulnérabilités Spectre/Meltdown)

---

## Conclusion

La prédiction de branchement est cruciale pour les performances modernes :

1. ✅ **Impact énorme** : 10-20 cycles perdus par misprediction
2. ✅ **Mesurable** : Utiliser `perf` pour identifier les problèmes
3. ✅ **Optimisable** : Trier, éliminer les branches, réorganiser le code
4. ✅ **Automatique** : Les CPUs modernes ont d'excellents prédicteurs
5. ✅ **Trade-off** : Lisibilité vs performance

**Citation :**

> "The best branch prediction is no branch at all" — Programmeur anonyme

**Règle d'or :** Écrivez du code simple et prévisible. Si vous identifiez un problème de branch misses avec `perf`, alors optimisez. Mais ne sacrifiez pas la lisibilité sans mesure préalable !

**Priorités d'optimisation :**
1. Algorithme efficace (O(n) vs O(n²))
2. Cache-friendliness (localité spatiale)
3. Branch prediction (branches prévisibles)
4. Micro-optimisations (SIMD, déroulage)

Les branch mispredictions sont importantes, mais pas au prix d'un algorithme fondamentalement mauvais ou d'un code qui fait des cache misses constants !

---

*Prochaine section : 27.6 Optimisations algorithmiques*

⏭️ [Optimisations algorithmiques](/27-optimisation-performance/06-optimisations-algorithmiques.md)
