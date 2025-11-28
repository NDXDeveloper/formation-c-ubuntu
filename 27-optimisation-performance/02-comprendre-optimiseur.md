🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.2 Comprendre l'optimiseur

## Introduction

L'**optimiseur** est le composant du compilateur GCC qui transforme votre code source en un code machine plus efficace. Contrairement à ce que l'on pourrait penser, le compilateur ne traduit pas littéralement votre code C en assembleur : il l'analyse, le comprend, puis génère un code équivalent mais plus performant.

Cette section vous explique comment fonctionne l'optimiseur, quelles transformations il effectue, et surtout comment vous pouvez **visualiser et comprendre ses décisions**.

### Pourquoi comprendre l'optimiseur ?

- 🔍 **Déboguer des comportements surprenants** : Certains bugs n'apparaissent qu'avec les optimisations
- 📈 **Écrire du code "optimiseur-friendly"** : Aider le compilateur à mieux optimiser
- 🧠 **Améliorer votre compréhension** : Savoir ce qui se passe "sous le capot"
- ⚡ **Identifier les goulots d'étranglement** : Comprendre pourquoi une partie du code est lente

---

## Le pipeline de compilation et d'optimisation

Lorsque vous compilez un fichier C, voici les grandes étapes traversées par votre code :

```
Code Source C
     ↓
 [Préprocesseur]
     ↓
Code C étendu (après macros, includes)
     ↓
 [Parseur / Analyseur]
     ↓
AST (Arbre de Syntaxe Abstraite)
     ↓
 [Front-end : optimisations indépendantes du langage]
     ↓
Représentation Intermédiaire (GIMPLE en GCC)
     ↓
 [Middle-end : OPTIMISEUR PRINCIPAL] ← C'EST ICI !
     ↓
RTL (Register Transfer Language)
     ↓
 [Back-end : optimisations spécifiques au CPU]
     ↓
Code Assembleur
     ↓
 [Assembleur]
     ↓
Code Machine (binaire)
```

L'**optimiseur principal** travaille sur une représentation intermédiaire (GIMPLE dans GCC), qui est plus simple à manipuler que le code assembleur final. Il applique des centaines de transformations différentes avant de passer le code au générateur de code machine.

---

## Principes fondamentaux de l'optimisation

### 1. Préserver la sémantique

**Règle d'or** : L'optimiseur **ne doit jamais changer le comportement observable** du programme.

```c
// Code original
int a = 5;
int b = 3;
int c = a + b;
printf("%d\n", c);

// L'optimiseur peut remplacer par
printf("%d\n", 8);

// Mais SEULEMENT si personne ne peut observer que 'a', 'b' et 'c' n'existent plus
```

Si votre programme produit le résultat `8`, l'optimiseur peut remplacer tous les calculs par la constante `8` directement. C'est une transformation **sémantiquement équivalente**.

### 2. Analyse du flux de données

L'optimiseur analyse comment les données circulent dans votre programme :

```c
int x = 10;      // x vaut 10
int y = x + 5;   // y vaut 15, x est lu
x = 20;          // x vaut maintenant 20
// Si x n'est plus utilisé après, cette ligne peut être supprimée !
```

L'optimiseur construit un graphe montrant :
- **Où chaque variable est définie** (affectée)
- **Où elle est utilisée** (lue)
- **Quelle valeur elle peut avoir** à chaque point du programme

### 3. Analyse du flux de contrôle

L'optimiseur comprend la structure de votre programme (if, boucles, fonctions) :

```c
if (1) {  // Condition toujours vraie
    printf("Hello\n");
} else {
    printf("World\n");  // Code mort, jamais exécuté
}

// L'optimiseur transforme en :
printf("Hello\n");
```

---

## Les grandes familles d'optimisations

### 1. Optimisations locales (au niveau d'un bloc de base)

Un **bloc de base** est une séquence d'instructions sans branchement (pas de `if`, `for`, etc.). Ce sont les optimisations les plus simples.

#### a) Élimination des sous-expressions communes (CSE)

**Avant :**
```c
int a = b + c;
int d = b + c;  // On recalcule la même chose !
```

**Après :**
```c
int a = b + c;
int d = a;      // Réutilisation du résultat
```

L'optimiseur détecte que `b + c` est calculé deux fois avec les mêmes valeurs.

#### b) Propagation de constantes

**Avant :**
```c
int x = 5;
int y = x + 3;
int z = y * 2;
```

**Après :**
```c
int z = 16;  // (5 + 3) * 2 = 16
```

Si toutes les valeurs sont connues à la compilation, l'optimiseur effectue les calculs directement.

#### c) Algèbre de simplification

**Avant :**
```c
int x = a * 1;        // Multiplication par 1
int y = b + 0;        // Addition de 0
int z = c - c;        // Soustraction identique
int w = d / 1;        // Division par 1
```

**Après :**
```c
int x = a;
int y = b;
int z = 0;
int w = d;
```

L'optimiseur connaît les propriétés mathématiques de base.

#### d) Repliement de constantes (Constant Folding)

**Avant :**
```c
int surface = 10 * 20;
```

**Après :**
```c
int surface = 200;
```

Les calculs entre constantes sont effectués à la compilation.

---

### 2. Optimisations globales (au niveau d'une fonction)

Ces optimisations analysent plusieurs blocs de base et les chemins d'exécution possibles.

#### a) Élimination de code mort (Dead Code Elimination)

**Avant :**
```c
void fonction() {
    int x = 10;  // Variable jamais utilisée
    int y = 20;
    printf("%d\n", y);
}
```

**Après :**
```c
void fonction() {
    int y = 20;
    printf("%d\n", y);
}
```

Si une variable n'est jamais lue, son calcul et son stockage sont inutiles.

#### b) Inlining de fonctions

**Avant :**
```c
int carre(int x) {
    return x * x;
}

int main() {
    int resultat = carre(5);
}
```

**Après :**
```c
int main() {
    int resultat = 5 * 5;  // La fonction est "intégrée"
}
```

**Avantages :**
- ✅ Élimine l'overhead de l'appel de fonction (push/pop de la pile)
- ✅ Permet d'autres optimisations (propagation de constantes)

**Inconvénients :**
- ❌ Augmente la taille du code
- ❌ Peut saturer le cache d'instructions

**Critères de décision :**
- Taille de la fonction (petites fonctions sont des candidates idéales)
- Fréquence d'appel (fonctions appelées souvent sont prioritaires)
- Niveau d'optimisation (`-O2` est conservateur, `-O3` est agressif)

#### c) Optimisation de boucles : Déplacement de code invariant

**Avant :**
```c
for (int i = 0; i < 1000; i++) {
    int limite = calcul_complexe();  // Même résultat à chaque itération
    tableau[i] = i * limite;
}
```

**Après :**
```c
int limite = calcul_complexe();  // Sorti de la boucle !
for (int i = 0; i < 1000; i++) {
    tableau[i] = i * limite;
}
```

Si un calcul dans une boucle produit toujours le même résultat, l'optimiseur le déplace à l'extérieur.

#### d) Déroulage de boucles (Loop Unrolling)

**Avant :**
```c
for (int i = 0; i < 4; i++) {
    tableau[i] = i;
}
```

**Après :**
```c
tableau[0] = 0;
tableau[1] = 1;
tableau[2] = 2;
tableau[3] = 3;
```

**Avantages :**
- ✅ Élimine les sauts conditionnels (instructions de branchement)
- ✅ Permet la parallélisation par le CPU (instruction-level parallelism)
- ✅ Réduit les itérations de boucle

**Inconvénients :**
- ❌ Augmente fortement la taille du code
- ❌ Peut dégrader les performances si le code dépasse le cache

**GCC déroule automatiquement :**
- Petites boucles avec un nombre d'itérations connu
- Avec `-O3` ou `-funroll-loops`

---

### 3. Optimisations inter-procédurales (IPO)

Ces optimisations analysent **plusieurs fonctions** ensemble, parfois même plusieurs fichiers avec `-flto`.

#### a) Propagation de constantes entre fonctions

**Avant :**
```c
int calcul(int x) {
    return x * 2 + 10;
}

int main() {
    int resultat = calcul(5);
}
```

**Après :**
```c
int main() {
    int resultat = 20;  // (5 * 2 + 10) calculé à la compilation
}
```

Si l'optimiseur voit que `calcul()` est toujours appelée avec `5`, il peut calculer le résultat directement.

#### b) Élimination de fonctions non utilisées

Avec `-flto`, GCC peut détecter qu'une fonction n'est appelée nulle part dans le programme entier et la supprimer complètement.

---

## Représentation intermédiaire : GIMPLE

### Qu'est-ce que GIMPLE ?

**GIMPLE** est une représentation simplifiée de votre code C, plus facile à analyser et transformer. Chaque instruction GIMPLE est très simple (3 adresses maximum).

**Exemple de transformation :**

**Code C :**
```c
int resultat = (a + b) * (c + d);
```

**Représentation GIMPLE (simplifiée) :**
```
t1 = a + b
t2 = c + d
resultat = t1 * t2
```

Chaque opération complexe est décomposée en opérations élémentaires sur des **variables temporaires** (t1, t2...).

### Visualiser le GIMPLE

```bash
# Générer la représentation GIMPLE
gcc -fdump-tree-gimple -O2 mon_programme.c

# Cela crée un fichier : mon_programme.c.004t.gimple
cat mon_programme.c.004t.gimple
```

**Exemple complet :**

```c
// fichier: exemple.c
int somme(int a, int b) {
    return a + b;
}

int main() {
    int x = 5;
    int y = 10;
    int z = somme(x, y);
    return 0;
}
```

**Commande :**
```bash
gcc -fdump-tree-all -O2 exemple.c
```

Cela génère de nombreux fichiers `.t` montrant chaque passe d'optimisation :
- `exemple.c.004t.gimple` : GIMPLE initial
- `exemple.c.010t.cfg` : Control Flow Graph
- `exemple.c.030t.inline` : Après inlining
- `exemple.c.235t.optimized` : GIMPLE final optimisé
- etc.

---

## Visualiser les optimisations en action

### Méthode 1 : Comparer l'assembleur

**Exemple de code :**
```c
// fichier: test.c
int somme_tableau(int *tab, int taille) {
    int resultat = 0;
    for (int i = 0; i < taille; i++) {
        resultat += tab[i];
    }
    return resultat;
}
```

**Sans optimisation :**
```bash
gcc -O0 -S test.c -o test_O0.s
```

**Avec optimisation :**
```bash
gcc -O2 -S test.c -o test_O2.s
```

**Comparaison :**
```bash
# Compter les lignes d'assembleur
wc -l test_O0.s test_O2.s

# Exemple de résultat :
# 45 test_O0.s
# 28 test_O2.s  ← Moins de lignes = plus optimisé
```

Vous pouvez ouvrir les deux fichiers et voir concrètement les différences.

### Méthode 2 : Utiliser Compiler Explorer (Godbolt)

**Outil en ligne** : https://godbolt.org/

Cet outil génial vous permet de :
1. Écrire du code C dans le navigateur
2. Voir l'assembleur généré **en temps réel**
3. Comparer différents niveaux d'optimisation côte à côte
4. Colorer les correspondances entre code C et assembleur

**Exemple d'utilisation :**
```c
int carre(int x) {
    return x * x;
}

int main() {
    return carre(5);
}
```

Sur Godbolt, vous verrez qu'avec `-O2`, tout le code se réduit à :
```asm
main:
    mov eax, 25    ; Résultat directement !
    ret
```

### Méthode 3 : Dumps d'optimisation de GCC

GCC peut générer des rapports détaillés de ses optimisations :

```bash
# Afficher toutes les optimisations appliquées
gcc -O2 -fopt-info-all test.c -o test

# Optimisations de boucles uniquement
gcc -O2 -fopt-info-loop test.c -o test

# Inlining
gcc -O2 -fopt-info-inline test.c -o test

# Vectorisation
gcc -O2 -fopt-info-vec test.c -o test
```

**Exemple de sortie :**
```
test.c:5:5: optimized: loop vectorized using 16 byte vectors
test.c:12:9: optimized: Inlining calcul into main
```

---

## Limites de l'optimiseur

### 1. L'optimiseur n'est pas omniscient

L'optimiseur ne peut optimiser que ce qu'il **peut prouver**.

**Exemple : Aliasing de pointeurs**

```c
void increment(int *a, int *b) {
    *a = *a + 1;
    *b = *b + 1;
}
```

L'optimiseur **ne sait pas** si `a` et `b` pointent vers la même adresse ou non. Il doit donc générer du code qui fonctionne dans les deux cas, ce qui limite les optimisations possibles.

**Solution : Le mot-clé `restrict`**
```c
void increment(int *restrict a, int *restrict b) {
    *a = *a + 1;
    *b = *b + 1;
}
```

`restrict` est une promesse au compilateur que `a` et `b` ne pointent **jamais** vers la même zone mémoire. Cela permet de meilleures optimisations.

### 2. Comportement indéfini (Undefined Behavior)

L'optimiseur **suppose que votre code ne contient pas de comportement indéfini**.

**Exemple de piège :**
```c
int main() {
    int x = 10;
    x = x + 1;  // OK

    int y;
    y = y + 1;  // UNDEFINED BEHAVIOR : y non initialisé

    return y;
}
```

Avec `-O0`, `y` pourrait contenir 0 (ou une valeur aléatoire). Avec `-O2`, l'optimiseur pourrait **supprimer complètement** la variable `y` car son comportement est indéfini.

**Règle d'or :** Toujours initialiser vos variables !

### 3. L'optimiseur peut être "trompé"

Certains patterns de code empêchent l'optimiseur de faire son travail.

**Exemple : Appels de fonction opaques**

```c
int calcul_complexe(int x);  // Définie ailleurs

void traitement(int *tab, int taille) {
    for (int i = 0; i < taille; i++) {
        tab[i] = calcul_complexe(i);  // Appel de fonction
    }
}
```

Si `calcul_complexe()` est dans un autre fichier (compilation séparée sans LTO), l'optimiseur ne peut pas l'analyser et donc ne peut pas :
- La rendre inline
- Optimiser la boucle en fonction de son contenu
- Propager des constantes à travers elle

**Solution :** Utiliser `-flto` (Link-Time Optimization) pour l'optimisation inter-fichiers.

---

## Aider l'optimiseur : Bonnes pratiques

### 1. Marquer les fonctions `inline`

Pour les petites fonctions appelées fréquemment :

```c
inline int min(int a, int b) {
    return (a < b) ? a : b;
}
```

Le mot-clé `inline` est un **hint** (suggestion) au compilateur, pas une obligation. Avec `-O2`, GCC ignore souvent votre `inline` et prend ses propres décisions.

### 2. Utiliser `const` pour les paramètres

```c
int somme_tableau(const int *tab, int taille) {
    // L'optimiseur sait que 'tab' ne sera pas modifié
}
```

`const` aide l'optimiseur à comprendre les intentions du code.

### 3. Utiliser `restrict` pour les pointeurs

```c
void copie(int *restrict dest, const int *restrict src, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
```

### 4. Déclarer les fonctions `static` quand possible

```c
static int fonction_interne(int x) {
    return x * 2;
}
```

Une fonction `static` n'est pas visible hors du fichier. L'optimiseur peut donc l'optimiser plus agressivement (inlining, suppression si non utilisée, etc.).

### 5. Éviter les variables globales

```c
// Mauvais : difficile à optimiser
int compteur_global = 0;

void increment() {
    compteur_global++;
}

// Meilleur : optimisation locale possible
void traitement() {
    int compteur = 0;
    compteur++;
}
```

Les variables globales peuvent être modifiées par n'importe quelle fonction, ce qui complique l'analyse de l'optimiseur.

### 6. Écrire du code simple et clair

**L'optimiseur préfère le code simple !**

```c
// Code "malin" mais difficile à optimiser
int valeur = (x & 1) ? ((x >> 1) + 1) : (x >> 1);

// Code clair et facile à optimiser
int valeur = x / 2;
if (x % 2 == 1) {
    valeur++;
}
```

Paradoxalement, le code le plus simple est souvent le plus rapide après optimisation.

---

## Cas d'étude : Optimisation d'une boucle

### Code initial (naïf)

```c
int somme_carres(int n) {
    int resultat = 0;
    for (int i = 1; i <= n; i++) {
        resultat = resultat + (i * i);
    }
    return resultat;
}
```

### Étapes d'optimisation par GCC (avec `-O2`)

**Étape 1 : Simplification des expressions**
```c
int somme_carres(int n) {
    int resultat = 0;
    for (int i = 1; i <= n; i++) {
        resultat += i * i;  // Opérateur composé
    }
    return resultat;
}
```

**Étape 2 : Déroulage partiel de boucle (si activé)**
```c
int somme_carres(int n) {
    int resultat = 0;
    int i = 1;

    // Traiter 4 éléments à la fois
    for (; i <= n - 3; i += 4) {
        resultat += i * i;
        resultat += (i+1) * (i+1);
        resultat += (i+2) * (i+2);
        resultat += (i+3) * (i+3);
    }

    // Reste de la boucle
    for (; i <= n; i++) {
        resultat += i * i;
    }
    return resultat;
}
```

**Étape 3 : Vectorisation (si CPU supporte SIMD)**

Le compilateur peut utiliser des instructions vectorielles (SSE, AVX) pour calculer plusieurs carrés en parallèle.

### Résultat final en assembleur

Avec `-O2 -march=native`, le code peut être réduit à quelques instructions vectorielles ultra-efficaces.

**Vérification :**
```bash
gcc -O2 -march=native -S somme_carres.c
cat somme_carres.s
```

---

## Optimisations spécifiques selon l'architecture

### Optimisations pour x86-64

- **Utilisation des registres** : x86-64 a 16 registres généraux (vs 8 en 32-bit)
- **Instructions SIMD** : SSE, AVX, AVX2, AVX-512 pour le calcul parallèle
- **Prédiction de branchement** : Réorganisation du code pour aider le CPU

### Optimisations pour ARM

- **Load/Store architecture** : Plus de load/store, moins d'instructions complexes
- **Conditional execution** : Instructions conditionnelles natives
- **NEON** : Instructions SIMD pour ARM

### Voir les optimisations appliquées

```bash
# Verbose : affiche les optimisations activées
gcc -O2 -fopt-info-vec-optimized -march=native test.c
```

---

## Outils pour analyser l'optimisation

### 1. `objdump` : Désassembler un binaire

```bash
# Compiler
gcc -O2 test.c -o test

# Désassembler
objdump -d test > test.asm
```

Utile pour voir le code machine final.

### 2. `perf` : Profiler les performances

```bash
# Compiler avec symboles de debug
gcc -O2 -g test.c -o test

# Profiler
perf record ./test
perf report
```

`perf` montre quelles parties du code consomment le plus de temps CPU.

### 3. `gprof` : Profiler classique

```bash
# Compiler avec profiling
gcc -O2 -pg test.c -o test

# Exécuter
./test

# Générer le rapport
gprof test gmon.out > rapport.txt
```

### 4. `valgrind --tool=cachegrind` : Analyser le cache

```bash
valgrind --tool=cachegrind ./test
cg_annotate cachegrind.out.<pid>
```

Montre les miss de cache (accès mémoire lents).

---

## Erreurs d'optimisation courantes

### 1. L'optimiseur "casse" mon code !

**Symptôme :** Le code fonctionne avec `-O0` mais plante avec `-O2`.

**Causes possibles :**
- **Comportement indéfini** : Variables non initialisées, dépassement de buffer, etc.
- **Violation de l'aliasing** : Cast de pointeurs incompatibles
- **Code dépendant de l'ordre d'évaluation**

**Solution :** Utilisez les sanitizers pour détecter les bugs :
```bash
gcc -O2 -fsanitize=address,undefined test.c -o test
./test
```

### 2. L'optimisation rend le débogage impossible

**Symptôme :** Avec GDB, les variables affichent des valeurs bizarres, le code "saute".

**Cause :** Les optimisations modifient l'ordre du code et suppriment des variables.

**Solution :** Compiler avec `-Og` (optimisations compatibles avec le débogage) :
```bash
gcc -Og -g test.c -o test
gdb ./test
```

### 3. Le code optimisé est plus lent !

**Symptôme :** `-O3` est plus lent que `-O2`.

**Cause :** Le code déroulé/inliné est trop gros et sature le cache d'instructions.

**Solution :** Mesurer les performances réelles et rester sur `-O2` si c'est plus rapide.

---

## Résumé des concepts clés

| Concept | Description | Impact |
|---------|-------------|--------|
| **CSE** | Élimination des calculs redondants | ⬆️ Vitesse |
| **Propagation de constantes** | Calculs à la compilation | ⬆️ Vitesse, ⬇️ Taille |
| **Dead code elimination** | Suppression du code inutile | ⬇️ Taille |
| **Inlining** | Intégration de fonctions | ⬆️ Vitesse, ⬆️ Taille |
| **Loop unrolling** | Déroulage de boucles | ⬆️ Vitesse, ⬆️⬆️ Taille |
| **Vectorisation** | Utilisation SIMD | ⬆️⬆️ Vitesse |
| **IPO/LTO** | Optimisation inter-fichiers | ⬆️ Vitesse, ⬇️ Taille |

---

## Checklist : Écrire du code optimisable

- ✅ Initialiser toutes les variables
- ✅ Utiliser `const` pour les paramètres en lecture seule
- ✅ Utiliser `restrict` pour les pointeurs non-aliasés
- ✅ Marquer les fonctions internes comme `static`
- ✅ Préférer les variables locales aux globales
- ✅ Écrire du code simple et lisible
- ✅ Éviter les casts de pointeurs dangereux
- ✅ Compiler avec `-Wall -Wextra` pour détecter les problèmes
- ✅ Tester avec les sanitizers (`-fsanitize=address,undefined`)
- ✅ Mesurer les performances avant d'optimiser manuellement

---

## Pour aller plus loin

### Visualiser l'optimiseur en action

1. **Compiler Explorer (Godbolt)** : https://godbolt.org/
   - Voir l'assembleur en temps réel
   - Comparer différents compilateurs et niveaux d'optimisation

2. **GCC Dump Files** :
   ```bash
   gcc -fdump-tree-all -O2 test.c
   ls -l test.c.*t.*  # Tous les dumps intermédiaires
   ```

3. **Rapport d'optimisation** :
   ```bash
   gcc -O2 -fopt-info-all test.c
   ```

### Documentation officielle

```bash
# Documentation GCC sur les optimisations
info gcc "Optimize Options"

# Liste des passes d'optimisation
gcc --help=optimizers
```

### Lectures recommandées

- **"Engineering a Compiler"** par Cooper & Torczon (livre de référence)
- **GCC Internals Manual** : https://gcc.gnu.org/onlinedocs/gccint/
- **Blog de Matt Godbolt** : créateur de Compiler Explorer

---

## Conclusion

Comprendre l'optimiseur vous permet de :

1. **Écrire du meilleur code** : Code "optimiseur-friendly"
2. **Déboguer efficacement** : Comprendre pourquoi le code optimisé se comporte différemment
3. **Faire des choix éclairés** : Savoir quand utiliser `-O2`, `-O3`, ou `-Os`
4. **Éviter les pièges** : Détecter le comportement indéfini avant qu'il ne cause des bugs

**Règle d'or :** L'optimiseur est votre allié, mais il ne peut optimiser que du code correct. Écrivez d'abord du code propre, sans bugs, puis laissez le compilateur faire le reste !

---

*Prochaine section : 27.3 Profiling*

⏭️ [Profiling](/27-optimisation-performance/03-profiling.md)
