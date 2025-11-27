🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.7 Static analysis et fuzzing

## Introduction

Même avec les meilleures intentions et pratiques de codage, des bugs peuvent se glisser dans votre code. Deux techniques complémentaires permettent de les détecter **avant** qu'ils ne causent des problèmes en production :

1. **Analyse statique** : Examiner le code **sans l'exécuter**
2. **Fuzzing** : Tester le programme avec des **entrées générées automatiquement**

### Analogie simple

**Analyse statique** = Inspection visuelle d'une voiture
- Le mécanicien regarde le moteur, les freins, les pneus
- Pas besoin de conduire pour trouver certains problèmes
- Rapide, mais peut manquer des problèmes subtils

**Fuzzing** = Test routier intensif d'une voiture
- Conduire sur tous types de routes, dans toutes conditions
- Accélérer, freiner brusquement, prendre des virages serrés
- Découvre des problèmes qui n'apparaissent qu'à l'usage
- Plus long, mais trouve des bugs réels

### Complémentarité

```
Analyse statique → Détecte les erreurs évidentes rapidement
         +
Fuzzing → Découvre les bugs subtils par l'exécution
         =
Couverture maximale des vulnérabilités
```

---

## Partie 1 : Analyse statique

### Qu'est-ce que l'analyse statique ?

L'analyse statique examine le **code source** pour détecter :
- Erreurs de programmation
- Vulnérabilités de sécurité
- Non-respect des conventions
- Code mort ou inutile
- Problèmes de performance potentiels

**Sans exécuter le programme !**

### Avantages

- ✅ **Rapide** : Quelques secondes à quelques minutes
- ✅ **Complet** : Analyse tout le code, même les branches rarement exécutées
- ✅ **Précoce** : Détecte les problèmes dès l'écriture du code
- ✅ **Pas de setup** : Pas besoin de données de test ou d'environnement d'exécution
- ✅ **Reproductible** : Toujours les mêmes résultats

### Limites

- ❌ **Faux positifs** : Peut signaler des "problèmes" qui n'en sont pas
- ❌ **Faux négatifs** : Peut manquer des bugs réels
- ❌ **Pas d'exécution** : Ne détecte pas les problèmes de logique métier
- ❌ **Dépendant du contexte** : Certains bugs n'apparaissent que dans certaines conditions

---

## Outils d'analyse statique

### 1. GCC/Clang warnings (Niveau 0 - Gratuit)

Le plus simple : activer tous les warnings du compilateur !

```bash
gcc -Wall -Wextra -Wpedantic -Werror mon_programme.c -o mon_programme
```

**Options importantes** :

| Flag | Description |
|------|-------------|
| `-Wall` | Active beaucoup de warnings courants |
| `-Wextra` | Active des warnings supplémentaires |
| `-Wpedantic` | Conformité stricte au standard C |
| `-Werror` | Traite les warnings comme des erreurs |
| `-Wformat=2` | Vérifications strictes des format strings |
| `-Wformat-security` | Détecte les format string vulnerabilities |
| `-Wconversion` | Détecte les conversions de types dangereuses |
| `-Wsign-conversion` | Détecte les conversions signed/unsigned |
| `-Wstrict-overflow=5` | Détecte les risques d'overflow |

**Exemple de détection** :

```c
// bug.c
#include <stdio.h>

int main(void) {
    int x;  // Variable non initialisée
    printf("%d\n", x);  // Utilisation de variable non initialisée

    unsigned int a = 10;
    int b = a;  // Conversion unsigned -> signed

    return 0;
}
```

**Compilation** :
```bash
$ gcc -Wall -Wextra -Wconversion bug.c -o bug
bug.c: In function 'main':
bug.c:6:20: warning: 'x' is used uninitialized in this function [-Wuninitialized]
     printf("%d\n", x);
                    ^
bug.c:9:13: warning: conversion from 'unsigned int' to 'int' may change value [-Wconversion]
     int b = a;
             ^
```

### 2. cppcheck (Niveau 1 - Simple)

**cppcheck** est un analyseur statique gratuit, facile à utiliser et efficace.

#### Installation

```bash
# Ubuntu/Debian
sudo apt install cppcheck

# Fedora
sudo dnf install cppcheck

# macOS
brew install cppcheck
```

#### Utilisation basique

```bash
# Analyse simple
cppcheck mon_programme.c

# Analyse complète
cppcheck --enable=all --suppress=missingIncludeSystem mon_programme.c

# Avec plus de détails
cppcheck --enable=all --verbose --inline-suppr mon_programme.c
```

#### Exemple de détection

```c
// buffer_overflow.c
#include <stdio.h>
#include <string.h>

void copier_donnees(char *source) {
    char buffer[10];
    strcpy(buffer, source);  // Potentiel buffer overflow
    printf("%s\n", buffer);
}

int main(void) {
    char *data = malloc(100);
    copier_donnees(data);
    // Oubli de free(data) → fuite mémoire
    return 0;
}
```

**Analyse** :
```bash
$ cppcheck --enable=all buffer_overflow.c
[buffer_overflow.c:6]: (warning) Dangerous usage of strcpy
[buffer_overflow.c:13]: (error) Memory leak: data
```

#### Options utiles

```bash
# Vérifier tout, y compris les performances
cppcheck --enable=all --inconclusive mon_code.c

# Format XML pour intégration CI/CD
cppcheck --xml --xml-version=2 mon_code.c 2> rapport.xml

# Vérifier un projet entier
cppcheck --enable=all src/
```

### 3. Clang Static Analyzer (Niveau 2 - Puissant)

**Clang Static Analyzer** est un analyseur très sophistiqué, plus précis que cppcheck.

#### Installation

```bash
# Ubuntu/Debian
sudo apt install clang clang-tools

# Fedora
sudo dnf install clang clang-analyzer

# macOS (livré avec Xcode)
xcode-select --install
```

#### Utilisation

```bash
# Méthode 1 : Directement
clang --analyze mon_programme.c

# Méthode 2 : scan-build (recommandé)
scan-build gcc -c mon_programme.c

# Pour un projet complet avec make
scan-build make

# Avec CMake
scan-build cmake --build build/
```

#### Exemple de détection

```c
// use_after_free.c
#include <stdlib.h>
#include <stdio.h>

void fonction_bugguee(void) {
    int *ptr = malloc(sizeof(int) * 10);

    if (ptr == NULL) {
        return;
    }

    *ptr = 42;
    printf("Valeur : %d\n", *ptr);

    free(ptr);

    // Use-after-free !
    printf("Valeur après free : %d\n", *ptr);
}

int main(void) {
    fonction_bugguee();
    return 0;
}
```

**Analyse** :
```bash
$ clang --analyze use_after_free.c
use_after_free.c:18:41: warning: Use of memory after it is freed
    printf("Valeur après free : %d\n", *ptr);
                                        ^~~~
```

**Rapport détaillé** :
```bash
$ scan-build clang use_after_free.c
scan-build: Using '/usr/bin/clang' for static analysis
scan-build: Analysis run complete.
scan-build: 1 bug found.
scan-build: Run 'scan-view /tmp/scan-build-xxx' to examine bug reports.
```

### 4. clang-tidy (Niveau 3 - Très complet)

**clang-tidy** est un outil de linting et de modernisation de code, avec des centaines de vérifications.

#### Installation

```bash
# Ubuntu/Debian
sudo apt install clang-tidy

# Fedora
sudo dnf install clang-tools-extra
```

#### Utilisation

```bash
# Toutes les vérifications
clang-tidy mon_programme.c --

# Vérifications spécifiques
clang-tidy mon_programme.c -checks='cert-*,clang-analyzer-*' --

# Avec corrections automatiques
clang-tidy mon_programme.c -checks='readability-*' -fix --
```

#### Catégories de vérifications

| Catégorie | Description | Exemple |
|-----------|-------------|---------|
| `cert-*` | Standards CERT C | `cert-err33-c` (vérifier retours) |
| `clang-analyzer-*` | Analyseur Clang | Fuites mémoire, use-after-free |
| `bugprone-*` | Patterns d'erreurs | Comparaisons incorrectes |
| `readability-*` | Lisibilité | Nommage, formatage |
| `performance-*` | Performance | Copies inutiles |
| `modernize-*` | Code moderne | Utiliser C11/C23 |
| `misc-*` | Divers | Erreurs courantes |

#### Exemple complet

```c
// problemes.c
#include <stdio.h>
#include <string.h>

void fonction_problematique(char *input) {
    char buffer[100];

    // Problème 1 : strcpy dangereux
    strcpy(buffer, input);

    // Problème 2 : comparaison de float
    float x = 0.1;
    if (x == 0.1) {  // Dangereux !
        printf("Equal\n");
    }

    // Problème 3 : return value non vérifiée
    scanf("%s", buffer);

    // Problème 4 : size_t en signed
    int taille = strlen(buffer);  // Devrait être size_t
}

int main(void) {
    return 0;
}
```

**Analyse** :
```bash
$ clang-tidy problemes.c -checks='*' --
problemes.c:8:5: warning: use of strcpy is insecure [cert-msc24-c]
    strcpy(buffer, input);
    ^
problemes.c:12:11: warning: comparing floating point with == [clang-diagnostic-float-equal]
    if (x == 0.1) {
          ^
problemes.c:17:5: warning: ignoring return value of 'scanf' [cert-err33-c]
    scanf("%s", buffer);
    ^
problemes.c:20:9: warning: narrowing conversion from 'size_t' to 'int' [bugprone-narrowing-conversions]
    int taille = strlen(buffer);
        ^
```

### 5. Infer (Niveau 4 - Avancé)

**Infer** est un analyseur développé par Facebook, utilisant des techniques d'analyse formelle.

#### Installation

```bash
# Via le binaire précompilé
wget https://github.com/facebook/infer/releases/download/v1.1.0/infer-linux64-v1.1.0.tar.xz
tar xf infer-linux64-v1.1.0.tar.xz
export PATH=$PATH:$(pwd)/infer-linux64-v1.1.0/bin

# Ou via package manager
# Ubuntu (nécessite ajout du PPA)
```

#### Utilisation

```bash
# Analyse simple
infer run -- gcc mon_programme.c

# Avec make
infer run -- make

# Avec CMake
infer run -- cmake --build build/
```

#### Exemple

```c
// null_deref.c
#include <stdlib.h>

int* retourner_null(int condition) {
    if (condition > 10) {
        return malloc(sizeof(int));
    }
    return NULL;
}

void fonction(void) {
    int *ptr = retourner_null(5);
    *ptr = 42;  // NULL dereference !
}
```

**Analyse** :
```bash
$ infer run -- gcc -c null_deref.c
null_deref.c:12: error: NULL_DEREFERENCE
  pointer 'ptr' last assigned on line 11 could be null and is dereferenced at line 12
```

---

## Configuration d'analyse statique dans un projet

### Fichier .clang-tidy

Créer un fichier `.clang-tidy` à la racine du projet :

```yaml
---
Checks: >
  -*,
  bugprone-*,
  cert-*,
  clang-analyzer-*,
  performance-*,
  readability-*,
  -readability-magic-numbers,
  -readability-braces-around-statements

WarningsAsErrors: '*'

CheckOptions:
  - key: readability-identifier-naming.VariableCase
    value: lower_case
  - key: readability-identifier-naming.FunctionCase
    value: lower_case
  - key: readability-identifier-naming.ConstantCase
    value: UPPER_CASE
```

### Makefile avec analyse statique

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wformat-security -O2
SOURCES = main.c utils.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = monprogramme

# Build normal
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Analyse statique
.PHONY: analyze
analyze: cppcheck clang-analyze clang-tidy

cppcheck:
	@echo "=== cppcheck ==="
	cppcheck --enable=all --suppress=missingIncludeSystem $(SOURCES)

clang-analyze:
	@echo "=== Clang Static Analyzer ==="
	scan-build --status-bugs $(MAKE) clean all

clang-tidy:
	@echo "=== clang-tidy ==="
	clang-tidy $(SOURCES) -- $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGET)
```

**Utilisation** :
```bash
make analyze
```

---

## Partie 2 : Fuzzing

### Qu'est-ce que le fuzzing ?

Le **fuzzing** (ou fuzz testing) consiste à :
1. Générer automatiquement des **entrées aléatoires ou semi-aléatoires**
2. Les fournir au programme
3. Observer si le programme **crashe** ou a un **comportement anormal**

### Pourquoi le fuzzing est efficace ?

- Teste des cas que vous n'auriez **jamais imaginés**
- Trouve des bugs dans des **combinaisons improbables** d'entrées
- Fonctionne **24/7 sans supervision**
- Découvre des bugs de sécurité **exploitables**

### Types de fuzzing

#### 1. Mutation-based fuzzing

Prend des entrées valides et les **mute** (modifie légèrement).

```
Input valide : "Hello World"
    ↓ Mutation
"Hellö World"    (caractère modifié)
"HelHeWorld"     (octets dupliqués)
"Hel"            (tronqué)
"Hello World\x00\xFF" (octets ajoutés)
```

#### 2. Generation-based fuzzing

Génère des entrées basées sur une **grammaire** ou un **format** connu.

```
Pour un parser JSON :
{"key": "value"}          ← Valide
{"key": [1, 2, 3]}       ← Valide
{"key": null}            ← Valide
{"key": {"nested": 1}}   ← Valide
{]                       ← Invalide (test du parser)
```

#### 3. Coverage-guided fuzzing

Le fuzzer observe quelle **partie du code** est exécutée et génère des entrées pour couvrir plus de chemins.

```
Programme avec if/else :
    ↓
Fuzzer génère des entrées qui activent toutes les branches
    ↓
Couverture maximale du code
```

---

## Outils de fuzzing

### 1. AFL (American Fuzzy Lop) - Le classique

**AFL** est le fuzzer le plus célèbre, très efficace et relativement simple à utiliser.

#### Installation

```bash
# Ubuntu/Debian
sudo apt install afl++

# Ou depuis les sources
git clone https://github.com/AFLplusplus/AFLplusplus
cd AFLplusplus
make
sudo make install
```

#### Utilisation basique

**Étape 1 : Compiler avec AFL**

```bash
# Au lieu de gcc
afl-gcc mon_programme.c -o mon_programme

# Ou avec clang (recommandé)
afl-clang-fast mon_programme.c -o mon_programme
```

**Étape 2 : Créer des cas de test initiaux**

```bash
mkdir testcases
echo "Hello" > testcases/input1.txt
echo "World" > testcases/input2.txt
echo "Test123" > testcases/input3.txt
```

**Étape 3 : Lancer AFL**

```bash
mkdir findings
afl-fuzz -i testcases -o findings ./mon_programme @@
# @@ sera remplacé par le chemin du fichier de test
```

**Étape 4 : Observer les résultats**

AFL affiche un tableau de bord en temps réel :
```
+----------------------------------------------------+
|        american fuzzy lop 4.0 (mon_programme)      |
+----------------------------------------------------+
| process timing                                     |
|   run time     : 0 days, 0 hrs, 5 min, 23 sec      |
|   last new path: 0 days, 0 hrs, 2 min, 8 sec       |
+----------------------------------------------------+
| overall results                                    |
|   cycles done  : 23                                |
|   total paths  : 145                               |
|   uniq crashes : 3        ← Crashes trouvés !      |
|   uniq hangs   : 1                                 |
+----------------------------------------------------+
```

#### Exemple complet

```c
// vulnerable.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void traiter_input(char *data) {
    char buffer[100];

    // Bug intentionnel : pas de vérification de longueur
    strcpy(buffer, data);

    // Bug subtil : débordement si data contient "AAAA"
    if (strncmp(data, "AAAA", 4) == 0) {
        int index = data[4] - '0';  // Pas de validation !
        buffer[index * 100] = 'X';  // Buffer overflow
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char buffer[1024];
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[n] = '\0';
    fclose(f);

    traiter_input(buffer);

    return 0;
}
```

**Fuzzing** :
```bash
# Compiler avec AFL
afl-clang-fast vulnerable.c -o vulnerable

# Créer testcases
mkdir in
echo "Hello" > in/test1.txt

# Lancer AFL
mkdir out
afl-fuzz -i in -o out ./vulnerable @@

# Après quelques minutes, AFL trouve des crashes
# Les cas qui causent des crashes sont dans out/crashes/
```

**Analyser un crash** :
```bash
# Reproduire un crash trouvé
./vulnerable out/crashes/id:000000,sig:11,src:000000,op:havoc,rep:16

# Avec GDB
gdb ./vulnerable
(gdb) run out/crashes/id:000000,sig:11,src:000000,op:havoc,rep:16
```

### 2. libFuzzer (Fuzzing en continu)

**libFuzzer** est intégré à LLVM et permet du fuzzing in-process (plus rapide).

#### Création d'une cible de fuzzing

```c
// fuzz_target.c
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Fonction à fuzzer
void fonction_vulnerable(const uint8_t *data, size_t size) {
    if (size > 0 && data[0] == 'B') {
        if (size > 1 && data[1] == 'U') {
            if (size > 2 && data[2] == 'G') {
                // Bug intentionnel
                char buffer[10];
                memcpy(buffer, data, size);  // Buffer overflow !
            }
        }
    }
}

// Point d'entrée pour libFuzzer
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    fonction_vulnerable(data, size);
    return 0;
}
```

**Compilation et exécution** :
```bash
# Compiler avec libFuzzer
clang -g -O1 -fsanitize=fuzzer,address fuzz_target.c -o fuzz_target

# Lancer le fuzzing
./fuzz_target

# libFuzzer trouve rapidement le bug :
# ==12345==ERROR: AddressSanitizer: stack-buffer-overflow
# Input that triggered: "BUG" + données supplémentaires
```

**Avantages de libFuzzer** :
- ✅ Très rapide (in-process, pas de fork)
- ✅ Coverage-guided
- ✅ Intégration avec AddressSanitizer
- ✅ Corpus minimisé automatiquement

### 3. Honggfuzz

**Honggfuzz** est un fuzzer moderne avec de bonnes performances.

```bash
# Installation
git clone https://github.com/google/honggfuzz
cd honggfuzz
make
```

**Utilisation** :
```bash
honggfuzz -i testcases -o findings -- ./mon_programme ___FILE___
```

### 4. OSS-Fuzz (Pour projets open source)

Si votre projet est open source, vous pouvez l'intégrer à **OSS-Fuzz** de Google pour du fuzzing continu gratuit.

---

## Fuzzing avec sanitizers

Pour maximiser l'efficacité du fuzzing, combinez-le avec les **sanitizers** :

### AddressSanitizer (ASan)

Détecte les erreurs mémoire.

```bash
# Compilation
afl-clang-fast -fsanitize=address -g mon_programme.c -o mon_programme

# Fuzzing
AFL_USE_ASAN=1 afl-fuzz -i in -o out -m none ./mon_programme @@
```

### UndefinedBehaviorSanitizer (UBSan)

Détecte les comportements indéfinis.

```bash
afl-clang-fast -fsanitize=undefined -g mon_programme.c -o mon_programme
```

### Combinaison de plusieurs sanitizers

```bash
afl-clang-fast -fsanitize=address,undefined -g mon_programme.c -o mon_programme
```

---

## Intégration dans le workflow de développement

### Pipeline CI/CD avec analyse statique

**GitHub Actions** :

```yaml
# .github/workflows/static-analysis.yml
name: Static Analysis

on: [push, pull_request]

jobs:
  analyze:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2

    - name: Install tools
      run: |
        sudo apt-get update
        sudo apt-get install -y cppcheck clang-tidy

    - name: cppcheck
      run: |
        cppcheck --enable=all --error-exitcode=1 \
          --suppress=missingIncludeSystem src/

    - name: clang-tidy
      run: |
        clang-tidy src/*.c -checks='cert-*,clang-analyzer-*' \
          -- -Isrc/include

    - name: Compiler warnings
      run: |
        gcc -Wall -Wextra -Werror -Wformat-security \
          -Wconversion -c src/*.c
```

### Pipeline CI/CD avec fuzzing

```yaml
# .github/workflows/fuzzing.yml
name: Fuzzing

on:
  schedule:
    - cron: '0 2 * * *'  # Tous les jours à 2h du matin

jobs:
  fuzz:
    runs-on: ubuntu-latest
    timeout-minutes: 60

    steps:
    - uses: actions/checkout@v2

    - name: Install AFL++
      run: |
        sudo apt-get update
        sudo apt-get install -y afl++

    - name: Build with AFL
      run: |
        afl-clang-fast -fsanitize=address src/parser.c -o parser

    - name: Prepare corpus
      run: |
        mkdir -p in
        cp testdata/*.txt in/

    - name: Run fuzzer (1 hour)
      run: |
        timeout 3600 afl-fuzz -i in -o out ./parser @@ || true

    - name: Check for crashes
      run: |
        if [ -d out/crashes ] && [ "$(ls -A out/crashes)" ]; then
          echo "Crashes found!"
          ls -la out/crashes/
          exit 1
        fi

    - name: Upload crashes
      if: failure()
      uses: actions/upload-artifact@v2
      with:
        name: crashes
        path: out/crashes/
```

### Workflow recommandé

```
1. Développement
   ├─ Compiler avec -Wall -Wextra -Werror
   └─ Lancer cppcheck régulièrement
         ↓
2. Pre-commit
   ├─ clang-tidy sur fichiers modifiés
   └─ Tests unitaires
         ↓
3. CI/CD (sur chaque commit)
   ├─ Analyse statique complète
   ├─ Compilation avec sanitizers
   └─ Tests automatiques
         ↓
4. Fuzzing nocturne
   ├─ AFL++ pendant quelques heures
   └─ Rapport des crashes
         ↓
5. Avant release
   └─ Fuzzing intensif (24-48h)
```

---

## Comparaison des approches

| Critère | Analyse statique | Fuzzing |
|---------|------------------|---------|
| **Vitesse** | ⚡ Très rapide (secondes) | 🐌 Lent (heures/jours) |
| **Configuration** | ✅ Simple | ⚠️ Nécessite corpus et setup |
| **Faux positifs** | ⚠️ Nombreux | ✅ Quasi-aucun (vrai crash) |
| **Couverture** | ✅ 100% du code | ⚠️ Dépend du temps |
| **Types de bugs** | Patterns connus | Bugs inattendus |
| **Bugs subtils** | ❌ Limité | ✅ Excellent |
| **CI/CD** | ✅ Facile à intégrer | ⚠️ Temps d'exécution long |

### Quand utiliser quoi ?

**Analyse statique** :
- ✅ À chaque compilation (intégré dans le Makefile)
- ✅ Pre-commit hooks
- ✅ Pull requests
- ✅ Détection rapide d'erreurs courantes

**Fuzzing** :
- ✅ Tests nocturnes automatisés
- ✅ Avant une release majeure
- ✅ Sur du code qui parse des entrées externes
- ✅ Pour trouver des bugs de sécurité critiques

**Les deux ensemble** : 🏆 Couverture optimale !

---

## Exemples de bugs trouvés

### Exemple 1 : Bug trouvé par cppcheck

```c
// Code original
void traiter_tableau(int *tableau, int taille) {
    for (int i = 0; i <= taille; i++) {  // Bug : <= au lieu de <
        tableau[i] = i * 2;
    }
}
```

**cppcheck détecte** :
```
[file.c:3]: (error) Array 'tableau[10]' accessed at index 10, which is out of bounds.
```

### Exemple 2 : Bug trouvé par AFL

```c
// Parser JSON simplifié
void parser_json(char *input) {
    if (strncmp(input, "{\"key\":", 7) == 0) {
        char *value = input + 7;

        // Bug : pas de vérification de guillemets fermants
        while (*value != '"') {  // Boucle infinie si pas de "
            value++;
        }
    }
}
```

**AFL trouve** :
```
Input: {"key":
(pas de guillemet fermant)
→ Hang (timeout) détecté
```

### Exemple 3 : Bug trouvé par Clang Static Analyzer

```c
int* obtenir_donnees(int condition) {
    int *data = malloc(sizeof(int) * 100);

    if (condition > 0) {
        // Traitement
        return data;
    }

    // Bug : fuite mémoire si condition <= 0
    return NULL;
}
```

**Clang détecte** :
```
[file.c:8]: warning: Potential memory leak
```

---

## Bonnes pratiques

### 1. Analyse statique

✅ **Activer tous les warnings dès le début du projet**
```bash
CFLAGS = -Wall -Wextra -Werror -Wformat-security -Wconversion
```

✅ **Intégrer dans le processus de build**
```makefile
check: cppcheck clang-tidy
```

✅ **Ne pas ignorer les warnings**
- Corriger ou justifier chaque warning
- Utiliser des suppressions explicites si nécessaire

✅ **Configurer .clang-tidy à la racine du projet**

✅ **Vérifier avant chaque commit**

### 2. Fuzzing

✅ **Créer un bon corpus initial**
- Inclure des exemples valides
- Inclure des cas limites
- Couvrir différents formats

✅ **Combiner avec sanitizers**
```bash
-fsanitize=address,undefined
```

✅ **Fuzzer régulièrement**
- Fuzzing nocturne automatisé
- Au moins 24h avant une release

✅ **Minimiser les crashs trouvés**
```bash
afl-tmin -i crash_file -o minimized ./programme @@
```

✅ **Corriger tous les crashs**
- Ne jamais ignorer un crash trouvé par fuzzing
- Même si l'input semble "irréaliste"

### 3. Général

✅ **Commencer tôt**
- Plus facile de corriger au fur et à mesure
- Éviter l'accumulation de dette technique

✅ **Automatiser**
- CI/CD pour analyse statique
- Fuzzing nocturne

✅ **Mesurer la couverture**
```bash
# Avec gcov
gcc -fprofile-arcs -ftest-coverage mon_programme.c
./mon_programme
gcov mon_programme.c
```

✅ **Documenter les exclusions**
- Pourquoi un warning est supprimé
- Pourquoi un code n'est pas fuzzé

---

## Outils complémentaires

### 1. Valgrind (mémoire)

```bash
valgrind --leak-check=full --show-leak-kinds=all ./programme
```

### 2. Callgrind (performance)

```bash
valgrind --tool=callgrind ./programme
kcachegrind callgrind.out.*
```

### 3. gcov/lcov (couverture)

```bash
# Compiler avec couverture
gcc -fprofile-arcs -ftest-coverage programme.c -o programme

# Exécuter
./programme

# Générer rapport
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory out
```

### 4. SonarQube (plateforme complète)

Pour les projets d'entreprise, SonarQube offre :
- Analyse statique centralisée
- Suivi de la qualité du code
- Détection de code dupliqué
- Métriques de complexité

---

## Checklist complète

### ✅ Analyse statique

- [ ] Flags de compilation activés (`-Wall -Wextra -Werror`)
- [ ] cppcheck dans le Makefile
- [ ] clang-tidy configuré (`.clang-tidy`)
- [ ] Analyse dans CI/CD
- [ ] Pas de warnings non traités
- [ ] Scan-build sur le projet complet

### ✅ Fuzzing

- [ ] Cibles de fuzzing identifiées (parsers, entrées réseau)
- [ ] Corpus de test créé
- [ ] Compilation avec AFL ou libFuzzer
- [ ] Sanitizers activés
- [ ] Fuzzing nocturne automatisé
- [ ] Procédure de traitement des crashs

### ✅ Intégration

- [ ] Pre-commit hooks pour analyse rapide
- [ ] CI/CD avec analyse complète
- [ ] Fuzzing régulier (au moins hebdomadaire)
- [ ] Couverture de code mesurée
- [ ] Tous les crashs corrigés avant release

---

## Ressources et liens

### Documentation officielle

- **AFL++** : https://aflplus.plus/
- **libFuzzer** : https://llvm.org/docs/LibFuzzer.html
- **cppcheck** : http://cppcheck.net/
- **Clang Static Analyzer** : https://clang-analyzer.llvm.org/
- **clang-tidy** : https://clang.llvm.org/extra/clang-tidy/

### Tutoriels et guides

- Google's OSS-Fuzz : https://github.com/google/oss-fuzz
- Fuzzing Project : https://fuzzing-project.org/
- CERT C Coding Standard : https://wiki.sei.cmu.edu/confluence/display/c

### Livres

- "Fuzzing for Software Security Testing and Quality Assurance" (Takanen et al.)
- "The Art of Software Security Assessment" (Dowd et al.)

---

## Conclusion

### Résumé des concepts clés

🔍 **Analyse statique** :
- Examine le code sans l'exécuter
- Rapide et facile à intégrer
- Trouve les erreurs évidentes et les patterns dangereux
- Indispensable dans tout projet professionnel

🎲 **Fuzzing** :
- Teste avec des entrées générées automatiquement
- Trouve des bugs subtils et inattendus
- Prend du temps mais très efficace
- Essentiel pour du code exposé (parsers, réseau)

### Hiérarchie de sécurité

```
Niveau 1 (Minimum vital) :
└─ Compiler avec -Wall -Wextra -Werror

Niveau 2 (Recommandé) :
└─ cppcheck dans le build
└─ Tests unitaires

Niveau 3 (Professionnel) :
└─ clang-tidy configuré
└─ Analyse statique en CI/CD
└─ Fuzzing occasionnel

Niveau 4 (Paranoïaque) :
└─ Scan-build complet
└─ Fuzzing continu 24/7
└─ Coverage-guided fuzzing
└─ Plusieurs analyseurs statiques
```

### Message final

> **Aucun outil n'est parfait, mais ensemble ils offrent une protection exceptionnelle.**

L'analyse statique et le fuzzing ne remplacent pas :
- ❌ Une bonne conception
- ❌ La validation des entrées
- ❌ Les revues de code
- ❌ Les tests unitaires

**Mais** ils les complètent parfaitement et détectent des bugs que vous n'auriez **jamais** trouvés autrement.

### Investissement vs Bénéfice

| Action | Temps d'installation | Bénéfice |
|--------|---------------------|----------|
| Flags compilateur | 5 minutes | ⭐⭐⭐⭐⭐ |
| cppcheck | 10 minutes | ⭐⭐⭐⭐ |
| clang-tidy | 30 minutes | ⭐⭐⭐⭐⭐ |
| Fuzzing basique | 1 heure | ⭐⭐⭐⭐ |
| Fuzzing avancé | 1 jour | ⭐⭐⭐⭐⭐ |

**ROI exceptionnel** : Quelques heures d'investissement peuvent éviter des semaines de débogage et des failles de sécurité critiques !

---

**🚀 Conseil final** : Commencez petit (flags de compilation + cppcheck), puis ajoutez progressivement les autres outils. L'important est de **les utiliser régulièrement** plutôt que d'avoir tous les outils mais de ne jamais les lancer !

**Prochaine étape** : La section suivante sur la compilation avec hardening flags vous montrera comment renforcer vos binaires contre l'exploitation.

⏭️ [Compilation avec hardening flags](/26-securite-code-defensif/08-hardening-flags.md)
