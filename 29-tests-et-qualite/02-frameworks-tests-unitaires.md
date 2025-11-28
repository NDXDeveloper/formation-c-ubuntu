🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.2 Tests unitaires : frameworks

## Introduction

Contrairement aux langages modernes qui intègrent des systèmes de tests natifs, le C nécessite l'utilisation d'un framework externe pour faciliter l'écriture et l'exécution de tests unitaires. Cette section présente les principaux frameworks disponibles, leurs caractéristiques, et comment choisir celui qui convient à votre projet.

---

## Pourquoi utiliser un framework de tests ?

### Sans framework : Les tests manuels

Il est techniquement possible d'écrire des tests sans framework :

```c
// test_math.c
#include <stdio.h>
#include <assert.h>
#include "math_utils.h"

int main(void) {
    // Test 1
    assert(add(2, 3) == 5);
    printf("✓ Test add(2, 3)\n");

    // Test 2
    assert(multiply(4, 5) == 20);
    printf("✓ Test multiply(4, 5)\n");

    printf("Tous les tests ont réussi!\n");
    return 0;
}
```

**Problèmes de cette approche :**

1. **Pas de structure** : Difficile d'organiser des centaines de tests
2. **Arrêt au premier échec** : `assert()` fait crasher le programme
3. **Pas de rapport détaillé** : Impossible de savoir combien de tests ont réussi/échoué
4. **Pas de setup/teardown** : Difficile de réinitialiser l'état entre tests
5. **Pas de gestion des ressources** : Les fuites mémoire ne sont pas détectées automatiquement

### Avec un framework : Organisation et robustesse

Un bon framework de tests fournit :

- **Assertions riches** : `assert_equal`, `assert_not_null`, `assert_string_equal`, etc.
- **Rapports détaillés** : Nombre de tests passés/échoués, temps d'exécution
- **Isolation** : Chaque test s'exécute indépendamment
- **Setup/Teardown** : Initialisation et nettoyage automatique
- **Fixtures** : Données de test réutilisables
- **Découverte automatique** : Le framework trouve vos tests automatiquement

---

## Panorama des frameworks populaires

### Tableau comparatif

| Framework | Complexité | Dépendances | Popularité | Cas d'usage |
|-----------|------------|-------------|------------|-------------|
| **Unity** | Faible | Aucune | ⭐⭐⭐⭐⭐ | Embarqué, débutants |
| **Check** | Moyenne | libcheck | ⭐⭐⭐⭐ | Linux/Unix, projets moyens |
| **cmocka** | Moyenne | Aucune | ⭐⭐⭐⭐ | Mocking avancé |
| **CUnit** | Moyenne | libcunit | ⭐⭐⭐ | Projets académiques |
| **Criterion** | Élevée | C++ compiler | ⭐⭐⭐ | Projets modernes |

---

## Unity : Le framework minimaliste et portable

### Présentation

**Unity** est développé par ThrowTheSwitch et est le framework le plus populaire pour l'embarqué. Sa philosophie : être **simple, portable, et sans dépendance**.

**Avantages :**
- ✅ **Extrêmement portable** : Fonctionne partout (Linux, Windows, bare metal)
- ✅ **Zéro dépendance** : Juste 2 fichiers `.c` et `.h` à ajouter à votre projet
- ✅ **Léger** : ~1000 lignes de code
- ✅ **Syntaxe simple** : Parfait pour débuter
- ✅ **Documentation excellente**

**Inconvénients :**
- ❌ Pas de découverte automatique des tests (nécessite un runner manuel)
- ❌ Fonctionnalités limitées (pas de mocking natif, pas de fixtures complexes)

### Installation

Unity n'est pas un paquet système. Vous l'intégrez directement dans votre projet :

```bash
# Cloner Unity
git clone https://github.com/ThrowTheSwitch/Unity.git

# Structure recommandée
project/
├── src/           # Code source
├── tests/         # Vos tests
└── unity/         # Framework Unity
    ├── unity.c
    └── unity.h
```

### Premier test avec Unity

**Fichier : `src/math_utils.h`**

```c
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int add(int a, int b);
int multiply(int a, int b);

#endif
```

**Fichier : `src/math_utils.c`**

```c
#include "math_utils.h"

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}
```

**Fichier : `tests/test_math_utils.c`**

```c
#include "unity.h"
#include "../src/math_utils.h"

// Fonction appelée avant chaque test
void setUp(void) {
    // Initialisation si nécessaire
}

// Fonction appelée après chaque test
void tearDown(void) {
    // Nettoyage si nécessaire
}

// Test 1 : Addition simple
void test_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

// Test 2 : Addition avec zéro
void test_add_with_zero(void) {
    TEST_ASSERT_EQUAL_INT(7, add(7, 0));
    TEST_ASSERT_EQUAL_INT(7, add(0, 7));
}

// Test 3 : Multiplication
void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(20, multiply(4, 5));
    TEST_ASSERT_EQUAL_INT(0, multiply(0, 100));
}

// Point d'entrée du test
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_with_zero);
    RUN_TEST(test_multiply);

    return UNITY_END();
}
```

### Compilation et exécution

```bash
# Compiler le test
gcc -o test_runner \
    tests/test_math_utils.c \
    src/math_utils.c \
    unity/unity.c \
    -I unity \
    -I src

# Exécuter
./test_runner
```

**Sortie :**

```
test_math_utils.c:12:test_add_positive_numbers:PASS
test_math_utils.c:17:test_add_with_zero:PASS
test_math_utils.c:23:test_multiply:PASS

-----------------------
3 Tests 0 Failures 0 Ignored
OK
```

### Assertions principales de Unity

```c
// Égalité de valeurs
TEST_ASSERT_EQUAL_INT(expected, actual);
TEST_ASSERT_EQUAL_UINT(expected, actual);
TEST_ASSERT_EQUAL_FLOAT(expected, actual);

// Comparaisons
TEST_ASSERT_GREATER_THAN(threshold, actual);
TEST_ASSERT_LESS_THAN(threshold, actual);

// Pointeurs
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);
TEST_ASSERT_EQUAL_PTR(expected, actual);

// Chaînes de caractères
TEST_ASSERT_EQUAL_STRING("expected", actual);
TEST_ASSERT_EQUAL_MEMORY(expected, actual, num_bytes);

// Booléens
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);

// Tableaux
TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements);
```

### Exemple avancé : Tester une structure

```c
typedef struct {
    int x;
    int y;
} Point;

Point create_point(int x, int y) {
    Point p = {x, y};
    return p;
}

void test_create_point(void) {
    Point p = create_point(10, 20);

    TEST_ASSERT_EQUAL_INT(10, p.x);
    TEST_ASSERT_EQUAL_INT(20, p.y);
}
```

---

## Check : Le framework pour Linux/Unix

### Présentation

**Check** est un framework mature spécialement conçu pour les systèmes POSIX (Linux, macOS, BSD). Il offre des fonctionnalités avancées comme l'isolation de processus.

**Avantages :**
- ✅ **Isolation par fork** : Chaque test s'exécute dans un processus séparé
- ✅ **Détection de segfaults** : Un test qui crash n'arrête pas les autres
- ✅ **Rapports XML** : Compatible avec Jenkins/CI
- ✅ **Timeouts** : Détection des tests qui bloquent

**Inconvénients :**
- ❌ Nécessite libcheck installée
- ❌ Non portable (POSIX uniquement)
- ❌ Configuration plus complexe

### Installation sur Ubuntu

```bash
sudo apt install check
```

### Premier test avec Check

**Fichier : `tests/check_math_utils.c`**

```c
#include <check.h>
#include "../src/math_utils.h"

// Test 1
START_TEST(test_add_positive) {
    ck_assert_int_eq(add(2, 3), 5);
}
END_TEST

// Test 2
START_TEST(test_multiply_by_zero) {
    ck_assert_int_eq(multiply(100, 0), 0);
}
END_TEST

// Suite de tests
Suite *math_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Math");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_add_positive);
    tcase_add_test(tc_core, test_multiply_by_zero);

    suite_add_tcase(s, tc_core);
    return s;
}

// Runner
int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = math_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
```

### Compilation avec Check

```bash
gcc -o test_runner \
    tests/check_math_utils.c \
    src/math_utils.c \
    -lcheck -lm -lsubunit -lpthread

./test_runner
```

**Sortie :**

```
Running suite Math
100%: Checks: 2, Failures: 0, Errors: 0
```

### Fonctionnalités avancées de Check

#### 1. Setup et Teardown par suite

```c
// Variables partagées
static int *test_array;

// Setup : appelé avant chaque test
void setup(void) {
    test_array = malloc(10 * sizeof(int));
}

// Teardown : appelé après chaque test
void teardown(void) {
    free(test_array);
}

Suite *my_suite(void) {
    Suite *s = suite_create("My Suite");
    TCase *tc = tcase_create("Core");

    // Enregistrer setup/teardown
    tcase_add_checked_fixture(tc, setup, teardown);

    tcase_add_test(tc, test_something);
    suite_add_tcase(s, tc);
    return s;
}
```

#### 2. Tests qui doivent crasher

```c
// Test qu'une fonction crash bien avec NULL
START_TEST(test_crash_on_null) {
    // Ce test DOIT provoquer un segfault
    int *ptr = NULL;
    *ptr = 42;
}
END_TEST

// Déclarer que ce test doit échouer avec un signal
tcase_add_test_raise_signal(tc_core, test_crash_on_null, SIGSEGV);
```

#### 3. Timeout sur les tests

```c
// Test qui ne doit pas dépasser 2 secondes
tcase_set_timeout(tc_core, 2);
```

### Assertions principales de Check

```c
// Entiers
ck_assert_int_eq(expected, actual);
ck_assert_int_ne(expected, actual);
ck_assert_int_lt(actual, threshold);
ck_assert_int_gt(actual, threshold);

// Chaînes
ck_assert_str_eq("expected", actual);
ck_assert_str_ne("not_expected", actual);

// Pointeurs
ck_assert_ptr_null(pointer);
ck_assert_ptr_nonnull(pointer);
ck_assert_ptr_eq(expected, actual);

// Mémoire
ck_assert_mem_eq(expected, actual, num_bytes);

// Messages personnalisés
ck_assert_msg(condition, "Message d'erreur custom");
```

---

## cmocka : Le framework orienté mocking

### Présentation

**cmocka** est développé par le projet Samba. Son point fort : le support natif du **mocking** (simulation de fonctions).

**Avantages :**
- ✅ **Mocking intégré** : Remplacer des fonctions pour les tests
- ✅ **Portable** : Fonctionne sous Linux, Windows, macOS
- ✅ **Léger** : Pas de dépendances externes
- ✅ **Support des exceptions** : Vérifie que le code gère les erreurs

**Inconvénients :**
- ❌ Syntaxe plus verbeuse
- ❌ Courbe d'apprentissage plus élevée

### Installation sur Ubuntu

```bash
sudo apt install libcmocka-dev
```

### Premier test avec cmocka

**Fichier : `tests/cmocka_math_utils.c`**

```c
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/math_utils.h"

// Test simple
static void test_add(void **state) {
    (void) state; // Unused
    assert_int_equal(add(2, 3), 5);
}

static void test_multiply(void **state) {
    (void) state;
    assert_int_equal(multiply(4, 5), 20);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add),
        cmocka_unit_test(test_multiply),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

### Compilation avec cmocka

```bash
gcc -o test_runner \
    tests/cmocka_math_utils.c \
    src/math_utils.c \
    -lcmocka

./test_runner
```

**Sortie :**

```
[==========] Running 2 test(s).
[ RUN      ] test_add
[       OK ] test_add
[ RUN      ] test_multiply
[       OK ] test_multiply
[==========] 2 test(s) run.
[  PASSED  ] 2 test(s).
```

### Mocking avec cmocka

Le mocking permet de remplacer une fonction par une version de test.

**Exemple : Tester une fonction qui lit un fichier**

```c
// Code de production
int read_config_value(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;

    int value;
    fscanf(f, "%d", &value);
    fclose(f);
    return value;
}
```

**Problème pour tester :** Cette fonction dépend du système de fichiers.

**Solution : Wrapper la fonction fopen**

```c
// Wrapper mockable
FILE *__wrap_fopen(const char *path, const char *mode) {
    // En mode test, retourner une valeur de test
    check_expected(path);
    return (FILE *)mock();
}

// Test avec mock
static void test_read_config_with_mock(void **state) {
    FILE *fake_file = (FILE *)0x12345; // Adresse factice

    // Configurer le mock
    expect_string(__wrap_fopen, path, "config.txt");
    will_return(__wrap_fopen, fake_file);

    // Appeler la fonction
    // (nécessite plus de configuration pour un test complet)
}
```

**Compilation avec wrapping :**

```bash
gcc -o test_runner test.c \
    -Wl,--wrap=fopen \
    -lcmocka
```

### Assertions principales de cmocka

```c
// Égalité
assert_int_equal(expected, actual);
assert_int_not_equal(expected, actual);
assert_string_equal("expected", actual);
assert_memory_equal(expected, actual, size);

// Comparaisons
assert_in_range(value, min, max);
assert_not_in_range(value, min, max);

// Booléens
assert_true(condition);
assert_false(condition);

// Pointeurs
assert_null(pointer);
assert_non_null(pointer);
assert_ptr_equal(expected, actual);
```

---

## CUnit : Le framework académique

### Présentation

**CUnit** est un framework traditionnel, souvent utilisé dans les universités.

**Avantages :**
- ✅ Interface utilisateur interactive (mode console)
- ✅ Sorties multiples (console, XML, HTML)
- ✅ Documentation complète

**Inconvénients :**
- ❌ API verbeuse
- ❌ Moins maintenu que les autres
- ❌ Performances moyennes

### Installation

```bash
sudo apt install libcunit1-dev
```

### Exemple rapide

```c
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/math_utils.h"

void test_add(void) {
    CU_ASSERT_EQUAL(add(2, 3), 5);
}

void test_multiply(void) {
    CU_ASSERT_EQUAL(multiply(4, 5), 20);
}

int main(void) {
    CU_pSuite suite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    suite = CU_add_suite("Math Suite", 0, 0);
    if (NULL == suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "test_add", test_add);
    CU_add_test(suite, "test_multiply", test_multiply);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
```

---

## Criterion : Le framework moderne

### Présentation

**Criterion** est un framework moderne avec une syntaxe élégante inspirée de frameworks comme RSpec (Ruby) ou Jest (JavaScript).

**Avantages :**
- ✅ Découverte automatique des tests (pas besoin de main)
- ✅ Rapports colorés et détaillés
- ✅ Parametric tests (exécuter un test avec plusieurs jeux de données)
- ✅ Syntaxe moderne

**Inconvénients :**
- ❌ Nécessite un compilateur C++ (pour la compilation)
- ❌ Moins portable
- ❌ Installation plus complexe

### Installation

```bash
sudo apt install libcriterion-dev
```

### Exemple

```c
#include <criterion/criterion.h>
#include "math_utils.h"

Test(math, add) {
    cr_assert_eq(add(2, 3), 5);
}

Test(math, multiply) {
    cr_assert_eq(multiply(4, 5), 20);
}

// Test paramétrique
ParameterizedTestParameters(math, parametric_add) {
    static struct {
        int a;
        int b;
        int expected;
    } params[] = {
        {1, 2, 3},
        {5, 5, 10},
        {-1, 1, 0},
    };

    return cr_make_param_array(params, sizeof(params));
}

ParameterizedTest(struct {int a; int b; int expected;} *param, math, parametric_add) {
    cr_assert_eq(add(param->a, param->b), param->expected);
}
```

**Compilation :**

```bash
gcc -o test_runner test.c math_utils.c -lcriterion
./test_runner
```

**Sortie colorée :**

```
[====] Synthesis: Tested: 3 | Passing: 3 | Failing: 0 | Crashing: 0
```

---

## Comment choisir son framework ?

### Critères de choix

| Critère | Unity | Check | cmocka | CUnit | Criterion |
|---------|-------|-------|--------|-------|-----------|
| **Simplicité** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| **Portabilité** | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ |
| **Mocking** | ⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐ | ⭐⭐⭐ |
| **Performances** | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| **CI/CD** | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |

### Recommandations par cas d'usage

**1. Projet embarqué ou avec contraintes de portabilité**
→ **Unity** (léger, portable, zéro dépendance)

**2. Application Linux/Unix standard**
→ **Check** (robuste, isolation par fork, rapports XML)

**3. Code avec beaucoup de dépendances externes**
→ **cmocka** (mocking puissant)

**4. Projet académique ou apprentissage**
→ **Unity** (simple) ou **CUnit** (classique)

**5. Nouveau projet moderne avec flexibilité**
→ **Criterion** (syntaxe élégante, découverte automatique)

---

## Intégration avec CMake

Quel que soit le framework choisi, CMake facilite grandement l'intégration des tests.

### Exemple avec Unity

**Fichier : `CMakeLists.txt`**

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject C)

# Options de compilation
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Werror)

# Bibliothèque à tester
add_library(math_utils src/math_utils.c)

# Activer les tests
enable_testing()

# Ajouter Unity
add_subdirectory(unity)

# Test exécutable
add_executable(test_math tests/test_math_utils.c)
target_link_libraries(test_math math_utils unity)

# Déclarer le test
add_test(NAME MathTests COMMAND test_math)
```

**Exécution :**

```bash
mkdir build && cd build
cmake ..
make
ctest --verbose
```

### Exemple avec Check

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)

add_executable(test_math tests/check_math_utils.c)
target_link_libraries(test_math math_utils ${CHECK_LIBRARIES})
target_include_directories(test_math PUBLIC ${CHECK_INCLUDE_DIRS})

add_test(NAME MathTests COMMAND test_math)
```

---

## Bonnes pratiques communes à tous les frameworks

### 1. Un fichier de test par module

```
tests/
├── test_math_utils.c
├── test_string_utils.c
└── test_list.c
```

### 2. Nommage cohérent

```c
// Convention : test_<fonction>_<cas>
void test_add_positive_numbers(void);
void test_add_with_zero(void);
void test_add_negative_numbers(void);
```

### 3. Tests indépendants

Chaque test doit pouvoir s'exécuter seul, dans n'importe quel ordre.

### 4. Setup/Teardown pour la répétition

Si plusieurs tests nécessitent la même initialisation, utilisez les fonctions setup/teardown :

```c
static char *buffer;

void setUp(void) {
    buffer = malloc(1024);
}

void tearDown(void) {
    free(buffer);
}
```

### 5. Assertions explicites

Préférez :
```c
TEST_ASSERT_EQUAL_INT(5, result);
```

À :
```c
TEST_ASSERT(result == 5);
```

Le premier donne un message d'erreur plus clair : `Expected 5 but was 7`.

---

## Intégration Continue (CI)

Tous les frameworks modernes s'intègrent facilement dans les pipelines CI/CD.

### Exemple GitHub Actions avec Unity

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc cmake

      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          make

      - name: Run tests
        run: |
          cd build
          ctest --output-on-failure
```

### Exemple avec Check (sortie XML)

```c
// Modifier le runner pour sortir en XML
srunner_run_all(sr, CK_NORMAL);
srunner_print(sr, CK_XML_LOG_FILE);
```

Cela génère un fichier `check_results.xml` compatible avec Jenkins/GitLab CI.

---

## Migration entre frameworks

Si vous devez changer de framework, voici les correspondances d'assertions :

| Concept | Unity | Check | cmocka |
|---------|-------|-------|--------|
| **Int égal** | `TEST_ASSERT_EQUAL_INT(exp, act)` | `ck_assert_int_eq(exp, act)` | `assert_int_equal(exp, act)` |
| **String égal** | `TEST_ASSERT_EQUAL_STRING(exp, act)` | `ck_assert_str_eq(exp, act)` | `assert_string_equal(exp, act)` |
| **Pointeur NULL** | `TEST_ASSERT_NULL(ptr)` | `ck_assert_ptr_null(ptr)` | `assert_null(ptr)` |
| **Vrai/Faux** | `TEST_ASSERT_TRUE(cond)` | `ck_assert(cond)` | `assert_true(cond)` |

---

## Conclusion : Démarrer simplement

### Pour un débutant

Si c'est votre première expérience avec les tests en C :

1. **Commencez par Unity** : Simple, portable, excellent tutoriel
2. Écrivez vos premiers tests pour des fonctions simples (math, strings)
3. Ajoutez progressivement des tests pour code plus complexe
4. Intégrez Valgrind pour détecter les fuites

### Pour un projet professionnel

1. **Check** (Linux) ou **cmocka** (multi-plateforme) sont de bons choix
2. Configurez CMake dès le début pour faciliter l'ajout de tests
3. Intégrez les tests dans votre CI/CD
4. Visez 70-80% de couverture de code

### Ressources pour aller plus loin

- **Unity** : https://github.com/ThrowTheSwitch/Unity
- **Check** : https://libcheck.github.io/check/
- **cmocka** : https://cmocka.org/
- **CUnit** : http://cunit.sourceforge.net/
- **Criterion** : https://criterion.readthedocs.io/

---

> **💡 Conseil pratique :** Ne perdez pas des semaines à choisir le "meilleur" framework. Unity fonctionne pour 80% des cas. Démarrez, écrivez des tests, et vous pourrez toujours migrer plus tard si nécessaire. **Le plus important est de tester, pas le framework choisi.**

⏭️ [Écriture de tests et assertions](/29-tests-et-qualite/03-ecriture-tests.md)
