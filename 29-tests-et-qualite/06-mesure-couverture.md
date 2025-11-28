🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.6 Mesure de couverture

## Introduction

Vous avez écrit des tests, mais couvrent-ils réellement tout votre code ? Une ligne de code non testée est une ligne où un bug peut se cacher. La **mesure de couverture** (code coverage) vous indique précisément quelles parties de votre code sont exécutées par vos tests, et lesquelles ne le sont pas.

---

## Qu'est-ce que la couverture de code ?

### Définition

La **couverture de code** est une métrique qui mesure le pourcentage de code source exécuté pendant les tests.

**Exemple simple :**

```c
int abs_value(int x) {
    if (x < 0) {        // Ligne 1
        return -x;      // Ligne 2
    }
    return x;           // Ligne 3
}

// Test
void test_abs_positive(void) {
    TEST_ASSERT_EQUAL_INT(5, abs_value(5));
}
```

**Analyse de couverture :**
- Test exécuté avec `x = 5`
- Ligne 1 : ✅ Exécutée (condition évaluée)
- Ligne 2 : ❌ **Non exécutée** (branche `x < 0` non prise)
- Ligne 3 : ✅ Exécutée

**Couverture : 66% (2 lignes sur 3)**

Le test ne couvre pas le cas négatif ! Un bug dans `return -x` passerait inaperçu.

### Pourquoi c'est important ?

**Scénario réel :**

```c
int divide_safe(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return 0;
    }
    return a / b;
}

// Test incomplet
void test_divide(void) {
    TEST_ASSERT_EQUAL_INT(5, divide_safe(10, 2));
}
```

**Problème :** Le test ne vérifie jamais le cas `b == 0`. Si ce code contient un bug (ex: oubli du `return`), vous ne le saurez jamais.

**Avec la couverture :** L'outil vous alerterait que la branche `b == 0` n'est jamais testée.

---

## Types de couverture

Il existe plusieurs métriques de couverture, chacune mesurant un aspect différent.

### 1. Couverture de lignes (Line Coverage)

Mesure le pourcentage de **lignes de code exécutées**.

```c
int max(int a, int b) {
    if (a > b) {          // Ligne 1 : Évaluée
        return a;         // Ligne 2 : Exécutée si a > b
    }
    return b;             // Ligne 3 : Exécutée si a <= b
}

// Test avec a=5, b=3
void test_max(void) {
    TEST_ASSERT_EQUAL_INT(5, max(5, 3));
}
```

**Couverture :**
- Ligne 1 : ✅ (condition évaluée)
- Ligne 2 : ✅ (branche prise)
- Ligne 3 : ❌ (branche non prise)

**Résultat : 66% de couverture de lignes**

### 2. Couverture de branches (Branch Coverage)

Mesure si **chaque branche** d'une condition a été testée.

```c
int check_range(int x) {
    if (x >= 0 && x <= 100) {  // 2 conditions = 4 branches possibles
        return 1;
    }
    return 0;
}

// Test 1
void test_valid_range(void) {
    TEST_ASSERT_EQUAL_INT(1, check_range(50));
}
```

**Branches possibles :**
1. `x >= 0` est vrai ET `x <= 100` est vrai → ✅ Testée
2. `x >= 0` est vrai ET `x <= 100` est faux → ❌ Non testée
3. `x >= 0` est faux → ❌ Non testée

**Couverture de branches : 25% (1 sur 4)**

**Tests complets :**

```c
void test_valid_range(void) {
    TEST_ASSERT_EQUAL_INT(1, check_range(50));    // x=50 : 0 ≤ 50 ≤ 100
}

void test_too_high(void) {
    TEST_ASSERT_EQUAL_INT(0, check_range(150));   // x=150 : x > 100
}

void test_negative(void) {
    TEST_ASSERT_EQUAL_INT(0, check_range(-5));    // x=-5 : x < 0
}
```

**Couverture de branches : 100%**

### 3. Couverture de fonctions (Function Coverage)

Mesure le pourcentage de **fonctions appelées** au moins une fois.

```c
// math_utils.c
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

// Tests
void test_add(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
}
```

**Couverture de fonctions :**
- `add` : ✅ Testée
- `subtract` : ❌ **Jamais appelée**
- `multiply` : ✅ Testée

**Résultat : 66% (2 fonctions sur 3)**

### 4. Couverture de chemins (Path Coverage)

Mesure tous les **chemins d'exécution possibles** à travers le code.

```c
int classify(int x, int y) {
    if (x > 0) {
        if (y > 0) {
            return 1;  // Chemin A
        }
        return 2;      // Chemin B
    }
    if (y > 0) {
        return 3;      // Chemin C
    }
    return 4;          // Chemin D
}
```

**Chemins possibles : 4**
- A : x > 0 ET y > 0
- B : x > 0 ET y ≤ 0
- C : x ≤ 0 ET y > 0
- D : x ≤ 0 ET y ≤ 0

**Pour 100% de couverture de chemins, il faut 4 tests différents.**

### Comparaison des métriques

| Métrique | Granularité | Difficulté | Utilité |
|----------|-------------|------------|---------|
| **Lignes** | Faible | Facile | ⭐⭐⭐⭐ (standard) |
| **Branches** | Moyenne | Moyenne | ⭐⭐⭐⭐⭐ (recommandé) |
| **Fonctions** | Élevée | Très facile | ⭐⭐⭐ (vue d'ensemble) |
| **Chemins** | Très faible | Très difficile | ⭐⭐ (explosif) |

**Recommandation :** Visez 100% de couverture de **branches** pour du code critique.

---

## Outils de mesure de couverture en C

### gcov : L'outil standard GNU

**gcov** est l'outil de couverture intégré à GCC. Il est gratuit, portable, et largement utilisé.

#### Installation

Sur Ubuntu, gcov est installé avec GCC :

```bash
gcc --version
gcov --version
```

#### Utilisation basique

**Étape 1 : Compiler avec les options de couverture**

```bash
gcc -o myprogram \
    -fprofile-arcs \     # Génère les compteurs d'exécution
    -ftest-coverage \    # Génère les données de couverture
    main.c math_utils.c
```

**Flags expliqués :**
- `-fprofile-arcs` : Insère du code pour compter les branches exécutées
- `-ftest-coverage` : Génère les fichiers `.gcno` (notes de couverture)

**Étape 2 : Exécuter le programme**

```bash
./myprogram
```

Cela génère des fichiers `.gcda` (données de couverture).

**Étape 3 : Analyser avec gcov**

```bash
gcov math_utils.c
```

**Sortie :**

```
File 'math_utils.c'
Lines executed:75.00% of 8
Creating 'math_utils.c.gcov'
```

**Étape 4 : Lire le rapport**

```bash
cat math_utils.c.gcov
```

**Exemple de rapport :**

```
        -:    0:Source:math_utils.c
        -:    1:#include "math_utils.h"
        -:    2:
        5:    3:int add(int a, int b) {
        5:    4:    return a + b;
        -:    5:}
        -:    6:
    #####:    7:int subtract(int a, int b) {
    #####:    8:    return a - b;
        -:    9:}
```

**Légende :**
- `5:` = Ligne exécutée 5 fois
- `#####:` = **Ligne jamais exécutée** ⚠️
- `-:` = Ligne non exécutable (commentaire, accolade)

### lcov : Interface graphique pour gcov

**lcov** génère des rapports HTML élégants à partir des données de gcov.

#### Installation

```bash
sudo apt install lcov
```

#### Utilisation

**Étape 1 : Générer les données avec gcov (comme avant)**

```bash
gcc -fprofile-arcs -ftest-coverage -o test_suite test.c math_utils.c
./test_suite
```

**Étape 2 : Collecter les données avec lcov**

```bash
lcov --capture --directory . --output-file coverage.info
```

**Étape 3 : Générer le rapport HTML**

```bash
genhtml coverage.info --output-directory coverage_html
```

**Étape 4 : Visualiser**

```bash
firefox coverage_html/index.html
```

**Résultat :** Une interface web interactive montrant :
- Couverture globale
- Couverture par fichier
- Lignes non couvertes en rouge
- Lignes couvertes en vert

---

## Exemple complet pas à pas

### Code source

**Fichier : `calculator.h`**

```c
#ifndef CALCULATOR_H
#define CALCULATOR_H

int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide_safe(int a, int b);

#endif
```

**Fichier : `calculator.c`**

```c
#include "calculator.h"
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide_safe(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return 0;
    }
    return a / b;
}
```

**Fichier : `test_calculator.c`**

```c
#include "unity.h"
#include "calculator.h"

void setUp(void) {}
void tearDown(void) {}

void test_add(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
}

void test_divide(void) {
    TEST_ASSERT_EQUAL_INT(5, divide_safe(10, 2));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_multiply);
    RUN_TEST(test_divide);
    return UNITY_END();
}
```

### Mesurer la couverture

**Compilation :**

```bash
gcc -o test_calculator \
    --coverage \                    # Équivalent à -fprofile-arcs -ftest-coverage
    test_calculator.c \
    calculator.c \
    unity/unity.c \
    -I unity
```

**Exécution :**

```bash
./test_calculator
```

**Sortie :**

```
test_calculator.c:11:test_add:PASS
test_calculator.c:15:test_multiply:PASS
test_calculator.c:19:test_divide:PASS

-----------------------
3 Tests 0 Failures 0 Ignored
OK
```

**Génération du rapport :**

```bash
gcov calculator.c
```

**Résultat :**

```
File 'calculator.c'
Lines executed:77.78% of 9
```

**Voir les détails :**

```bash
cat calculator.c.gcov
```

**Extrait du rapport :**

```
        5:    3:int add(int a, int b) {
        5:    4:    return a + b;
        -:    5:}
        -:    6:
    #####:    7:int subtract(int a, int b) {    ← NON TESTÉE
    #####:    8:    return a - b;
        -:    9:}
        -:   10:
        5:   11:int multiply(int a, int b) {
        5:   12:    return a * b;
        -:   13:}
        -:   14:
        1:   15:int divide_safe(int a, int b) {
        1:   16:    if (b == 0) {
    #####:   17:        fprintf(stderr, "Error: Division by zero\n");  ← NON TESTÉE
    #####:   18:        return 0;
        -:   19:    }
        1:   20:    return a / b;
        -:   21:}
```

**Analyse :**
- `subtract()` : Jamais appelée (0%)
- `divide_safe()` : Branche `b == 0` jamais testée

### Ajouter les tests manquants

```c
void test_subtract(void) {
    TEST_ASSERT_EQUAL_INT(2, subtract(5, 3));
}

void test_divide_by_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, divide_safe(10, 0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_subtract);         // Nouveau
    RUN_TEST(test_multiply);
    RUN_TEST(test_divide);
    RUN_TEST(test_divide_by_zero);   // Nouveau
    return UNITY_END();
}
```

**Recompiler et tester :**

```bash
# Nettoyer les anciennes données
rm -f *.gcda *.gcno *.gcov

# Recompiler
gcc -o test_calculator --coverage test_calculator.c calculator.c unity/unity.c -I unity

# Exécuter
./test_calculator

# Mesurer
gcov calculator.c
```

**Nouveau résultat :**

```
File 'calculator.c'
Lines executed:100.00% of 9
```

**🎉 100% de couverture !**

---

## Interpréter les résultats

### Que signifie vraiment 100% ?

**100% de couverture ≠ 0 bug**

**Exemple trompeur :**

```c
int buggy_max(int a, int b) {
    if (a > b) {
        return a;
    }
    return a;  // ❌ BUG : devrait être "return b"
}

void test_buggy_max(void) {
    TEST_ASSERT_EQUAL_INT(5, buggy_max(5, 3));  // ✅ Passe (a > b)
    TEST_ASSERT_EQUAL_INT(7, buggy_max(2, 7));  // ❌ Échoue (retourne 2 au lieu de 7)
}
```

**Couverture : 100%** (toutes les lignes exécutées), mais le bug existe toujours !

### Code non couvert = Code à risque

Inversement, du code non couvert est presque garanti d'avoir des bugs.

**Statistique empirique :**
- Code avec 0% de couverture : ~50% de bugs
- Code avec 50% de couverture : ~20% de bugs
- Code avec 90% de couverture : ~5% de bugs
- Code avec 100% de couverture : ~2% de bugs

**Conclusion :** La couverture est **nécessaire mais pas suffisante**.

### Objectifs réalistes

| Type de code | Couverture cible |
|--------------|------------------|
| **Code critique** (sécurité, financier) | 100% de branches |
| **Code métier standard** | 80-90% de lignes |
| **Code utilitaire** | 70-80% de lignes |
| **Prototypes / POC** | > 50% |

---

## Exclure du code de la couverture

Certaines parties du code n'ont pas besoin d'être couvertes (code de debug, code mort, etc.).

### Avec gcov : Pragmas GCC

```c
int production_function(int x) {
    // Code à couvrir
    if (x < 0) {
        return -x;
    }

    // GCOV_EXCL_START
    #ifdef DEBUG
    printf("Debug: x = %d\n", x);
    #endif
    // GCOV_EXCL_STOP

    return x;
}
```

**Alternative : Commenter le code de debug**

```c
// LCOV_EXCL_START
void debug_print_state(void) {
    // Code de debug non couvert
}
// LCOV_EXCL_STOP
```

### Avec lcov : Filtrer les fichiers

```bash
# Exclure les tests et bibliothèques externes
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info \
    '/usr/*' \           # Exclure les headers système
    '*/tests/*' \        # Exclure les tests
    '*/unity/*' \        # Exclure le framework de test
    --output-file coverage_filtered.info

genhtml coverage_filtered.info --output-directory coverage_html
```

---

## Intégration dans CMake

### CMakeLists.txt avec support de couverture

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject C)

# Option pour activer la couverture
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Bibliothèque à tester
add_library(calculator calculator.c)

# Tests
add_executable(test_calculator test_calculator.c)
target_link_libraries(test_calculator calculator unity)

# Configuration de couverture
if(ENABLE_COVERAGE)
    # Flags de compilation
    target_compile_options(calculator PRIVATE --coverage)
    target_link_options(calculator PRIVATE --coverage)

    target_compile_options(test_calculator PRIVATE --coverage)
    target_link_options(test_calculator PRIVATE --coverage)

    # Cible pour générer le rapport
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E remove_directory coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory coverage
        COMMAND ./test_calculator
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage_filtered.info
        COMMAND genhtml coverage_filtered.info --output-directory coverage
        COMMAND echo "Coverage report: coverage/index.html"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS test_calculator
    )
endif()

# Activer les tests
enable_testing()
add_test(NAME TestCalculator COMMAND test_calculator)
```

### Utilisation

**Build normal (sans couverture) :**

```bash
mkdir build && cd build
cmake ..
make
./test_calculator
```

**Build avec couverture :**

```bash
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make
make coverage
```

Cela génère automatiquement le rapport dans `build/coverage/index.html`.

---

## Intégration CI/CD

### GitHub Actions avec upload vers Codecov

**Fichier : `.github/workflows/coverage.yml`**

```yaml
name: Code Coverage

on: [push, pull_request]

jobs:
  coverage:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v2

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y lcov

      - name: Build with coverage
        run: |
          mkdir build && cd build
          cmake -DENABLE_COVERAGE=ON ..
          make

      - name: Run tests
        run: |
          cd build
          ./test_calculator

      - name: Generate coverage report
        run: |
          cd build
          lcov --capture --directory . --output-file coverage.info
          lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage_filtered.info

      - name: Upload to Codecov
        uses: codecov/codecov-action@v3
        with:
          files: ./build/coverage_filtered.info
          flags: unittests
          name: codecov-umbrella

      - name: Generate HTML report
        run: |
          cd build
          genhtml coverage_filtered.info --output-directory coverage_html

      - name: Upload HTML report as artifact
        uses: actions/upload-artifact@v2
        with:
          name: coverage-report
          path: build/coverage_html
```

**Résultat :** Badge Codecov sur votre README

```markdown
[![codecov](https://codecov.io/gh/username/repo/branch/main/graph/badge.svg)](https://codecov.io/gh/username/repo)
```

### GitLab CI avec rapport intégré

**Fichier : `.gitlab-ci.yml`**

```yaml
stages:
  - test
  - coverage

test:
  stage: test
  image: gcc:latest
  before_script:
    - apt-get update && apt-get install -y lcov cmake
  script:
    - mkdir build && cd build
    - cmake -DENABLE_COVERAGE=ON ..
    - make
    - ./test_calculator
  artifacts:
    paths:
      - build/
    expire_in: 1 hour

coverage:
  stage: coverage
  image: gcc:latest
  dependencies:
    - test
  before_script:
    - apt-get update && apt-get install -y lcov
  script:
    - cd build
    - lcov --capture --directory . --output-file coverage.info
    - lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage_filtered.info
    - genhtml coverage_filtered.info --output-directory coverage_html
    - lcov --summary coverage_filtered.info
  coverage: '/lines\.*: \d+\.\d+%/'
  artifacts:
    paths:
      - build/coverage_html
    reports:
      coverage_report:
        coverage_format: cobertura
        path: build/coverage_filtered.info
```

**Résultat :** GitLab affiche la couverture dans l'interface web.

---

## Couverture différentielle (Diff Coverage)

Mesurer la couverture **uniquement du code modifié** dans une Pull Request.

### Avec diff-cover

**Installation :**

```bash
pip install diff-cover
```

**Utilisation :**

```bash
# Générer le rapport de couverture
lcov --capture --directory . --output-file coverage.info

# Convertir en XML
lcov --cobertura coverage.info -o coverage.xml

# Analyser uniquement les modifications
diff-cover coverage.xml --compare-branch=main
```

**Sortie :**

```
Diff Coverage: 85.7%
Changed lines: 14
Covered lines: 12
Missing lines: 2
```

**Avantage :** Exiger 100% de couverture sur le code nouveau sans toucher au legacy.

---

## Outils alternatifs

### 1. Gcovr : Alternative à lcov

**Gcovr** génère des rapports dans plusieurs formats (HTML, XML, JSON).

**Installation :**

```bash
pip install gcovr
```

**Utilisation :**

```bash
# Rapport texte
gcovr

# Rapport HTML
gcovr --html --html-details -o coverage.html

# Rapport XML (Cobertura) pour Jenkins
gcovr --xml -o coverage.xml
```

### 2. Bullseye Coverage (Commercial)

Outil commercial pour l'embarqué avec support de couverture avancée.

### 3. Profilers intégrés aux IDEs

- **CLion** : Couverture intégrée avec visualisation
- **VS Code** : Extension Coverage Gutters

---

## Bonnes pratiques

### 1. Commiter les rapports dans .gitignore

```gitignore
# Fichiers de couverture
*.gcda
*.gcno
*.gcov
coverage.info
coverage_html/
```

### 2. Nettoyer les données entre exécutions

```bash
# Script de nettoyage
#!/bin/bash
find . -name '*.gcda' -delete
find . -name '*.gcno' -delete
find . -name '*.gcov' -delete
rm -rf coverage_html/
```

### 3. Mesurer régulièrement

Intégrez la mesure dans votre workflow :

```bash
# Makefile
.PHONY: test coverage

test:
	./test_suite

coverage: test
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html
	@echo "Report: coverage_html/index.html"
```

### 4. Définir des seuils minimums

**Dans CMake :**

```cmake
if(ENABLE_COVERAGE)
    add_custom_target(check_coverage
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND lcov --summary coverage.info | grep -q "lines.*: [8-9][0-9]\\|100"
        COMMENT "Checking coverage >= 80%"
    )
endif()
```

**Échec si couverture < 80%**

### 5. Ne pas traquer les métriques stupidement

**❌ Mauvais objectif :** "Atteindre 100% de couverture à tout prix"

Cela mène à :
- Tests inutiles
- Faux sentiment de sécurité
- Temps perdu

**✅ Bon objectif :** "Identifier et tester le code critique non couvert"

---

## Pièges courants

### 1. Confondre couverture et qualité

```c
// 100% de couverture mais test inutile
void test_useless(void) {
    add(2, 3);  // Pas d'assertion !
}
```

La ligne est exécutée, mais rien n'est vérifié.

### 2. Ignorer la couverture de branches

```c
int check(int x) {
    if (x > 0 && x < 100) {  // Plusieurs branches
        return 1;
    }
    return 0;
}

// Test avec couverture 100% de lignes mais pas de branches
void test_check(void) {
    TEST_ASSERT_EQUAL_INT(1, check(50));  // x > 0 ET x < 100
    // Manque : x <= 0, x >= 100
}
```

### 3. Oublier les fichiers .gcda

Si vous ne supprimez pas les `.gcda` entre exécutions, les compteurs s'accumulent :

```bash
# Exécution 1
./test_suite  # add() appelé 5 fois

# Exécution 2 (sans nettoyage)
./test_suite  # add() compte maintenant 10 fois !
```

**Solution :** Toujours nettoyer avant de mesurer.

### 4. Mesurer la couverture du code de test

```bash
# ❌ Inclut les tests dans la couverture
gcov test_calculator.c

# ✅ Seulement le code de production
gcov calculator.c
```

---

## Visualiser la couverture en temps réel

### Avec VS Code + Coverage Gutters

**Installation :**

1. Installer l'extension "Coverage Gutters"
2. Générer `lcov.info` dans votre projet
3. Cliquer sur "Watch" dans la barre de statut

**Résultat :** Les lignes non couvertes apparaissent en rouge dans l'éditeur.

### Avec CLion

CLion a un support natif de la couverture :

1. Run → Run with Coverage
2. Visualisation instantanée dans l'éditeur

---

## Exemple avancé : Couverture multi-modules

**Structure :**

```
project/
├── src/
│   ├── module_a.c
│   ├── module_b.c
│   └── module_c.c
└── tests/
    ├── test_module_a.c
    ├── test_module_b.c
    └── test_all.c
```

**CMakeLists.txt :**

```cmake
# Bibliothèques
add_library(module_a src/module_a.c)
add_library(module_b src/module_b.c)
add_library(module_c src/module_c.c)

# Tests individuels
add_executable(test_module_a tests/test_module_a.c)
target_link_libraries(test_module_a module_a unity)

add_executable(test_module_b tests/test_module_b.c)
target_link_libraries(test_module_b module_b unity)

# Test global
add_executable(test_all tests/test_all.c)
target_link_libraries(test_all module_a module_b module_c unity)

if(ENABLE_COVERAGE)
    foreach(target module_a module_b module_c test_module_a test_module_b test_all)
        target_compile_options(${target} PRIVATE --coverage)
        target_link_options(${target} PRIVATE --coverage)
    endforeach()

    add_custom_target(coverage_all
        COMMAND ./test_module_a
        COMMAND ./test_module_b
        COMMAND ./test_all
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND genhtml coverage.info --output-directory coverage_html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
```

**Générer la couverture globale :**

```bash
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make
make coverage_all
```

---

## Conclusion

La mesure de couverture est un outil puissant pour améliorer la qualité de vos tests, mais elle doit être utilisée intelligemment.

### Points clés à retenir

1. **Couverture ≠ Qualité** : 100% de couverture n'élimine pas les bugs
2. **Couverture de branches > Couverture de lignes** : Plus précise
3. **Objectifs réalistes** : 80-90% est souvent suffisant
4. **Identifier le code non testé** : C'est l'intérêt principal
5. **Automatiser** : Intégrer dans le CI/CD
6. **Ne pas traquer stupidement** : La couverture est un moyen, pas une fin

### Workflow recommandé

```bash
# 1. Développer
vim calculator.c

# 2. Écrire des tests
vim test_calculator.c

# 3. Compiler avec couverture
gcc --coverage -o test test_calculator.c calculator.c

# 4. Exécuter
./test

# 5. Mesurer
gcov calculator.c

# 6. Identifier les manques
cat calculator.c.gcov | grep "#####"

# 7. Ajouter des tests pour les lignes non couvertes
vim test_calculator.c

# 8. Répéter
```

### Ressources pour aller plus loin

- **Documentation gcov** : https://gcc.gnu.org/onlinedocs/gcc/Gcov.html
- **lcov** : http://ltp.sourceforge.net/coverage/lcov.php
- **Codecov** : https://codecov.io/
- **Article "Code Coverage Best Practices"** : https://martinfowler.com/bliki/TestCoverage.html

Dans la section suivante (**29.7 Test-Driven Development (TDD)**), nous verrons comment intégrer l'écriture de tests dès le début du développement pour maximiser la qualité.

---

> **💡 Conseil final :** La couverture est comme un détecteur de fumée : elle ne prévient pas l'incendie, mais elle vous alerte qu'il y a un problème. Utilisez-la pour **guider** vos efforts de test, pas pour **remplacer** la réflexion sur la qualité.

⏭️ [Test-Driven Development (TDD)](/29-tests-et-qualite/07-tdd.md)
