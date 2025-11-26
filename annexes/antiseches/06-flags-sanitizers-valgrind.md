🔝 Retour au [Sommaire](/SOMMAIRE.md)

# B.6 Flags de Sanitizers et Valgrind - Antisèche

## 📌 Introduction

Les **sanitizers** et **Valgrind** sont des outils essentiels pour détecter les bugs difficiles à trouver : fuites mémoire, accès invalides, race conditions, comportements indéfinis, etc.

### Différences clés

| Outil | Type | Performance | Détection |
|-------|------|-------------|-----------|
| **Sanitizers** | Instrumentation compile-time | ~2-5x plus lent | Erreurs au runtime |
| **Valgrind** | Instrumentation runtime | ~10-50x plus lent | Erreurs + profiling |

**Quand utiliser quoi ?**
- 🚀 **Sanitizers** : Développement quotidien (rapides, intégrés)
- 🔍 **Valgrind** : Analyse approfondie, profiling, bugs complexes

---

## 🛡️ Les Sanitizers (GCC/Clang)

Les sanitizers sont des outils intégrés aux compilateurs modernes (GCC ≥ 4.8, Clang ≥ 3.1) qui instrumentent votre code pour détecter des erreurs au runtime.

### Règles générales

```bash
# Compilation avec sanitizer
gcc -fsanitize=address -g programme.c -o programme

# Exécution (détection automatique des erreurs)
./programme
```

**Options communes à tous les sanitizers :**
- `-g` : **Obligatoire** pour des rapports d'erreur lisibles (symboles de débogage)
- `-fno-omit-frame-pointer` : Améliore les stack traces (recommandé)
- `-O1` ou `-O2` : Optimisation modérée (éviter `-O0` qui peut masquer bugs)

---

## 🔴 AddressSanitizer (ASan)

Détecte les erreurs d'accès mémoire.

### Erreurs détectées
- ✅ Buffer overflow (heap et stack)
- ✅ Use-after-free (utilisation après libération)
- ✅ Double-free
- ✅ Use-after-return (utilisation après retour de fonction)
- ✅ Use-after-scope
- ✅ Memory leaks (fuites mémoire)

### Compilation

```bash
# Basique
gcc -fsanitize=address -g programme.c -o programme

# Recommandé (avec optimisation légère)
gcc -fsanitize=address -g -O1 -fno-omit-frame-pointer programme.c -o programme

# Avec Clang
clang -fsanitize=address -g -O1 programme.c -o programme
```

### Variables d'environnement

```bash
# Activer la détection de leaks (désactivée par défaut sur certaines plateformes)
export ASAN_OPTIONS=detect_leaks=1

# Désactiver les leaks (utile si on se concentre sur d'autres bugs)
export ASAN_OPTIONS=detect_leaks=0

# Stack traces plus détaillées
export ASAN_OPTIONS=symbolize=1:fast_unwind_on_malloc=0

# Vérifier l'initialisation des variables
export ASAN_OPTIONS=check_initialization_order=1

# Combiner plusieurs options
export ASAN_OPTIONS=detect_leaks=1:symbolize=1:halt_on_error=0
```

### Options importantes

| Option | Valeur | Description |
|--------|--------|-------------|
| `detect_leaks` | 0/1 | Détecter les fuites mémoire |
| `halt_on_error` | 0/1 | S'arrêter à la première erreur (défaut: 1) |
| `symbolize` | 0/1 | Symboliser les stack traces |
| `log_path` | chemin | Fichier de sortie pour les erreurs |
| `verbosity` | 0-2 | Niveau de détail |
| `detect_stack_use_after_return` | 0/1 | Détecter use-after-return |

### Exemple d'utilisation

```c
// bug.c
#include <stdlib.h>
#include <stdio.h>

int main() {
    int *arr = malloc(10 * sizeof(int));
    arr[10] = 42;  // Buffer overflow !
    free(arr);
    return 0;
}
```

```bash
# Compilation
gcc -fsanitize=address -g -O1 bug.c -o bug

# Exécution
./bug

# Sortie typique :
# =================================================================
# ==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address ...
# WRITE of size 4 at ...
#     #0 0x... in main bug.c:6
# ...
```

### Exemple de leak detection

```c
// leak.c
#include <stdlib.h>

int main() {
    int *ptr = malloc(100);
    // Oubli du free !
    return 0;
}
```

```bash
# Compilation
gcc -fsanitize=address -g leak.c -o leak

# Exécution avec détection de leaks
ASAN_OPTIONS=detect_leaks=1 ./leak

# Sortie :
# ==12345==ERROR: LeakSanitizer: detected memory leaks
# Direct leak of 100 byte(s) in 1 object(s) allocated from:
#     #0 0x... in malloc
#     #1 0x... in main leak.c:4
```

---

## ⚠️ UndefinedBehaviorSanitizer (UBSan)

Détecte les comportements indéfinis du langage C.

### Erreurs détectées
- ✅ Integer overflow (débordement d'entiers signés)
- ✅ Division par zéro
- ✅ Null pointer dereference
- ✅ Alignement mémoire invalide
- ✅ Conversions de types illégales
- ✅ Array bounds (indices hors limites)

### Compilation

```bash
# Basique
gcc -fsanitize=undefined -g programme.c -o programme

# Avec options spécifiques
gcc -fsanitize=undefined,integer,bounds -g programme.c -o programme

# Strictement tous les checks
gcc -fsanitize=undefined -fno-sanitize-recover=all -g programme.c -o programme
```

### Sous-sanitizers activables

```bash
# Vérifications d'alignement
-fsanitize=alignment

# Débordements arithmétiques
-fsanitize=signed-integer-overflow
-fsanitize=unsigned-integer-overflow

# Vérifications de pointeurs
-fsanitize=pointer-overflow
-fsanitize=null

# Bounds checking
-fsanitize=bounds
```

### Variables d'environnement

```bash
# Ne pas continuer après une erreur
export UBSAN_OPTIONS=halt_on_error=1

# Afficher le type d'erreur
export UBSAN_OPTIONS=print_stacktrace=1

# Log vers fichier
export UBSAN_OPTIONS=log_path=ubsan.log
```

### Exemple d'utilisation

```c
// undef.c
#include <stdio.h>

int main() {
    int a = 2147483647;  // INT_MAX
    int b = a + 1;       // Signed integer overflow !

    int arr[5];
    arr[10] = 42;        // Out of bounds !

    int *ptr = NULL;
    *ptr = 10;           // Null pointer dereference !

    return 0;
}
```

```bash
# Compilation
gcc -fsanitize=undefined -g undef.c -o undef

# Exécution
./undef

# Sorties typiques :
# undef.c:5:17: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
# undef.c:8:5: runtime error: index 10 out of bounds for type 'int [5]'
# undef.c:11:10: runtime error: store to null pointer of type 'int'
```

---

## 🔵 ThreadSanitizer (TSan)

Détecte les race conditions et autres bugs de concurrence.

### Erreurs détectées
- ✅ Data races (accès concurrent non synchronisé)
- ✅ Deadlocks
- ✅ Utilisation incorrecte des mutex
- ✅ Utilisation de variables non initialisées partagées

### Compilation

```bash
# Basique
gcc -fsanitize=thread -g -O1 programme.c -o programme -pthread

# Avec optimisation
gcc -fsanitize=thread -g -O2 programme.c -o programme -pthread
```

⚠️ **Important :** Toujours compiler avec `-pthread` pour les programmes multi-threadés.

### Variables d'environnement

```bash
# Configuration de base
export TSAN_OPTIONS=verbosity=1

# Ignorer certains avertissements
export TSAN_OPTIONS=suppressions=tsan.supp

# Historique plus long (détecte plus de races)
export TSAN_OPTIONS=history_size=7

# Second deadlock detector
export TSAN_OPTIONS=second_deadlock_stack=1
```

### Exemple d'utilisation

```c
// race.c
#include <pthread.h>
#include <stdio.h>

int counter = 0;  // Variable partagée non protégée

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // Race condition !
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter: %d\n", counter);
    return 0;
}
```

```bash
# Compilation
gcc -fsanitize=thread -g -O1 race.c -o race -pthread

# Exécution
./race

# Sortie typique :
# ==================
# WARNING: ThreadSanitizer: data race (pid=12345)
#   Write of size 4 at 0x... by thread T1:
#     #0 increment race.c:8
#   Previous write of size 4 at 0x... by thread T2:
#     #0 increment race.c:8
```

---

## 💧 LeakSanitizer (LSan)

Détecte les fuites mémoire (inclus dans ASan par défaut).

### Compilation

```bash
# Standalone (sans ASan)
gcc -fsanitize=leak -g programme.c -o programme

# Ou via ASan
gcc -fsanitize=address -g programme.c -o programme
```

### Variables d'environnement

```bash
# Activer les rapports de leaks
export LSAN_OPTIONS=report_objects=1

# Supprimer certains leaks connus
export LSAN_OPTIONS=suppressions=lsan.supp

# Verbosité
export LSAN_OPTIONS=verbosity=1
```

### Fichier de suppression (lsan.supp)

```
# Ignorer les leaks dans certaines fonctions
leak:fonction_externe_connue
leak:libtierce.so
```

---

## 🧪 MemorySanitizer (MSan) - Clang uniquement

Détecte l'utilisation de mémoire non initialisée.

### Compilation

```bash
# Clang uniquement
clang -fsanitize=memory -g -O1 programme.c -o programme
```

⚠️ **Note :** MSan n'est pas disponible dans GCC. Nécessite de recompiler toutes les bibliothèques avec MSan.

### Variables d'environnement

```bash
export MSAN_OPTIONS=poison_in_dtor=1
```

---

## 🔄 Combiner les Sanitizers

Certains sanitizers sont compatibles entre eux.

### Combinaisons possibles

```bash
# ASan + UBSan (recommandé pour le développement)
gcc -fsanitize=address,undefined -g -O1 programme.c -o programme

# ASan + LSan (redondant, LSan est déjà dans ASan)
gcc -fsanitize=address,leak -g programme.c -o programme

# UBSan + bounds
gcc -fsanitize=undefined,bounds -g programme.c -o programme
```

### Combinaisons incompatibles

❌ **Ne pas combiner :**
- ASan + TSan (mutuellement exclusifs)
- ASan + MSan
- TSan + MSan

```bash
# ❌ ERREUR : ASan et TSan incompatibles
gcc -fsanitize=address,thread programme.c  # Ne compile pas
```

---

## 🐛 Valgrind

Valgrind est une suite d'outils d'analyse dynamique qui exécute le programme dans une machine virtuelle.

### Installation

```bash
# Ubuntu/Debian
sudo apt install valgrind

# Vérifier l'installation
valgrind --version
```

### Compilation pour Valgrind

```bash
# Compilation avec symboles de débogage (pas de flag sanitizer !)
gcc -g -O0 programme.c -o programme
```

⚠️ **Important :**
- Utiliser `-g` pour avoir des stack traces lisibles
- Ne **PAS** utiliser de sanitizers avec Valgrind (conflits)
- `-O0` recommandé pour débogage (ou `-O1` max)

---

## 🔍 Memcheck (Outil par défaut)

Détecte les erreurs mémoire.

### Erreurs détectées
- ✅ Memory leaks (fuites mémoire)
- ✅ Invalid read/write (accès mémoire invalides)
- ✅ Use of uninitialized values
- ✅ Invalid free / double free
- ✅ Mismatched malloc/free, new/delete

### Utilisation basique

```bash
# Lancer Memcheck (outil par défaut)
valgrind ./programme

# Avec options détaillées
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./programme

# Sauvegarder dans un fichier
valgrind --leak-check=full --log-file=valgrind.log ./programme
```

### Options importantes

| Option | Description |
|--------|-------------|
| `--leak-check=full` | Rapport détaillé des leaks |
| `--show-leak-kinds=all` | Montrer tous types de leaks |
| `--track-origins=yes` | Tracer l'origine des valeurs non initialisées |
| `--verbose` | Mode verbeux |
| `--log-file=FILE` | Écrire dans un fichier |
| `--suppressions=FILE` | Fichier de suppressions |
| `--gen-suppressions=all` | Générer les suppressions |

### Exemple d'utilisation

```c
// leak.c
#include <stdlib.h>
#include <string.h>

int main() {
    char *str = malloc(100);
    strcpy(str, "Hello");
    // Oubli du free !

    int *arr = malloc(50 * sizeof(int));
    arr[60] = 42;  // Out of bounds
    free(arr);

    return 0;
}
```

```bash
# Compilation
gcc -g -O0 leak.c -o leak

# Exécution avec Valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./leak

# Sortie typique :
# ==12345== Invalid write of size 4
# ==12345==    at 0x...: main (leak.c:10)
#
# ==12345== LEAK SUMMARY:
# ==12345==    definitely lost: 100 bytes in 1 blocks
# ==12345==    indirectly lost: 0 bytes in 0 blocks
```

### Types de leaks

| Type | Description |
|------|-------------|
| **definitely lost** | Bloc alloué sans référence (vraie fuite) |
| **indirectly lost** | Bloc pointé par un bloc perdu |
| **possibly lost** | Bloc avec référence interne uniquement |
| **still reachable** | Bloc encore référencé à la fin (pas critique) |

---

## 🔀 Helgrind (Détection de race conditions)

Détecte les erreurs de synchronisation dans les programmes multi-threadés.

### Erreurs détectées
- ✅ Data races
- ✅ Deadlocks potentiels
- ✅ Ordres de verrouillage incohérents
- ✅ Destruction de mutex verrouillés

### Utilisation

```bash
# Lancer Helgrind
valgrind --tool=helgrind ./programme

# Avec options
valgrind --tool=helgrind --history-level=full ./programme
```

### Options importantes

| Option | Description |
|--------|-------------|
| `--history-level=full` | Historique complet des accès |
| `--conflict-cache-size=N` | Taille du cache (défaut: 1000000) |

### Exemple

```c
// race.c
#include <pthread.h>
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 1000; i++)
        counter++;  // Race !
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
```

```bash
gcc -g -pthread race.c -o race
valgrind --tool=helgrind ./race

# Sortie :
# ==12345== Possible data race during write of size 4 at 0x...
# ==12345==    at 0x...: increment (race.c:6)
```

---

## 📊 Cachegrind (Profilage de cache)

Analyse l'utilisation du cache CPU.

### Utilisation

```bash
# Lancer Cachegrind
valgrind --tool=cachegrind ./programme

# Analyse du fichier de sortie
cg_annotate cachegrind.out.12345

# Analyse avec fichier source
cg_annotate cachegrind.out.12345 programme.c
```

### Informations collectées
- Cache L1 instruction/données
- Cache L2 unifié
- Ratios de miss

---

## 📞 Callgrind (Profilage de fonctions)

Mesure le coût des appels de fonctions.

### Utilisation

```bash
# Lancer Callgrind
valgrind --tool=callgrind ./programme

# Visualiser avec KCachegrind
kcachegrind callgrind.out.12345
```

### Options

```bash
# Ne pas lancer la collection au démarrage
valgrind --tool=callgrind --instr-atstart=no ./programme

# Contrôle programmatique avec callgrind.h
#include <valgrind/callgrind.h>
CALLGRIND_START_INSTRUMENTATION;
// Code à profiler
CALLGRIND_STOP_INSTRUMENTATION;
```

---

## 📈 Massif (Profilage de heap)

Analyse l'utilisation du tas (heap) au fil du temps.

### Utilisation

```bash
# Lancer Massif
valgrind --tool=massif ./programme

# Visualiser
ms_print massif.out.12345

# Visualiser graphiquement
massif-visualizer massif.out.12345
```

### Options

```bash
# Échantillonnage détaillé
valgrind --tool=massif --detailed-freq=1 ./programme

# Inclure la stack
valgrind --tool=massif --stacks=yes ./programme
```

---

## 📋 Tableau Comparatif

| Outil | Détection | Overhead | Compile | Usage |
|-------|-----------|----------|---------|-------|
| **ASan** | Mémoire, leaks | ~2x | `-fsanitize=address` | Dev quotidien |
| **UBSan** | Undefined behavior | ~1.5x | `-fsanitize=undefined` | Validation |
| **TSan** | Race conditions | ~5-10x | `-fsanitize=thread` | Concurrence |
| **Valgrind Memcheck** | Mémoire | ~10-50x | `-g` | Analyse poussée |
| **Helgrind** | Threads | ~20-50x | `-g` | Debugging threads |
| **Cachegrind** | Cache | ~20-100x | `-g` | Optimisation |
| **Callgrind** | Profiling | ~20-50x | `-g` | Profiling |
| **Massif** | Heap | ~20x | `-g` | Analyse mémoire |

---

## 🎯 Workflow Recommandé

### Développement quotidien
```bash
# Compiler avec ASan + UBSan
gcc -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer programme.c -o programme

# Exécuter
ASAN_OPTIONS=detect_leaks=1 ./programme
```

### Tests multi-threadés
```bash
# Compiler avec TSan
gcc -fsanitize=thread -g -O1 programme.c -o programme -pthread

# Exécuter
./programme
```

### Analyse approfondie (avant release)
```bash
# Compiler sans sanitizers
gcc -g -O1 programme.c -o programme

# Valgrind complet
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./programme

# Si multi-threadé
valgrind --tool=helgrind ./programme
```

### Optimisation de performance
```bash
# Profiling
valgrind --tool=callgrind ./programme
kcachegrind callgrind.out.xxxxx

# Analyse cache
valgrind --tool=cachegrind ./programme
cg_annotate cachegrind.out.xxxxx
```

---

## 🚨 Interpréter les Erreurs Courantes

### ASan : Heap buffer overflow
```
ERROR: AddressSanitizer: heap-buffer-overflow
WRITE of size 4 at 0x602000000034 thread T0
    #0 0x... in main test.c:10
```
➡️ **Solution :** Vérifier les indices de tableaux, taille des allocations.

---

### ASan : Use after free
```
ERROR: AddressSanitizer: heap-use-after-free
READ of size 4 at 0x602000000010 thread T0
    #0 0x... in main test.c:15
freed by thread T0 here:
    #0 0x... in free
    #1 0x... in main test.c:12
```
➡️ **Solution :** Ne pas utiliser de pointeur après `free()`.

---

### TSan : Data race
```
WARNING: ThreadSanitizer: data race
  Write of size 4 at 0x... by thread T2:
    #0 worker test.c:8
  Previous write of size 4 at 0x... by thread T1:
    #0 worker test.c:8
```
➡️ **Solution :** Protéger l'accès avec un mutex.

---

### Valgrind : Definitely lost
```
LEAK SUMMARY:
   definitely lost: 100 bytes in 1 blocks
```
➡️ **Solution :** Ajouter `free()` pour libérer la mémoire.

---

### Valgrind : Invalid read
```
Invalid read of size 4
   at 0x...: main (test.c:12)
 Address 0x... is 0 bytes after a block of size 40 alloc'd
```
➡️ **Solution :** Accès hors limite, vérifier les indices.

---

## 💡 Bonnes Pratiques

### 1. Toujours compiler avec `-g`
```bash
# ✅ BON
gcc -fsanitize=address -g programme.c

# ❌ Sans -g, stack traces illisibles
gcc -fsanitize=address programme.c
```

### 2. Utiliser les sanitizers en développement
Activez ASan + UBSan par défaut dans votre Makefile/CMakeLists.txt en mode Debug.

```cmake
# CMakeLists.txt
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(programme PRIVATE -fsanitize=address,undefined)
    target_link_options(programme PRIVATE -fsanitize=address,undefined)
endif()
```

### 3. Ne pas mélanger sanitizers et Valgrind
```bash
# ❌ MAUVAIS
gcc -fsanitize=address programme.c -o programme
valgrind ./programme  # Conflit !

# ✅ BON : Build séparé
gcc -g programme.c -o programme_valgrind
valgrind ./programme_valgrind
```

### 4. Utiliser CI/CD pour automatiser
```yaml
# .github/workflows/test.yml
- name: Test with ASan
  run: |
    gcc -fsanitize=address,undefined -g test.c -o test
    ASAN_OPTIONS=detect_leaks=1 ./test
```

### 5. Garder des fichiers de suppression
Créez `asan.supp`, `lsan.supp`, `valgrind.supp` pour ignorer les faux positifs connus.

---

## 📚 Ressources Complémentaires

### Documentation officielle
- ASan : https://github.com/google/sanitizers/wiki/AddressSanitizer
- TSan : https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual
- Valgrind : https://valgrind.org/docs/manual/

### Commandes utiles
```bash
# Lister tous les sanitizers disponibles
gcc --help=sanitize

# Options d'un sanitizer
ASAN_OPTIONS=help=1 ./programme

# Version de Valgrind
valgrind --version
```

---

**💡 Conseil final :** Intégrez les sanitizers dans votre workflow quotidien. Un bug détecté immédiatement est 100x plus facile à corriger qu'un bug découvert en production. Valgrind est votre allié pour l'analyse finale avant release !

⏭️ [Configuration de l'environnement](/annexes/configuration-environnement/README.md)
