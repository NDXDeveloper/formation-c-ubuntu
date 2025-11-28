🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.7 Test-Driven Development (TDD)

## Introduction

Imaginez construire une maison en commençant par les plans d'inspection plutôt que par les fondations. C'est contre-intuitif, mais c'est exactement le principe du **Test-Driven Development** (TDD) : écrire les tests **avant** le code de production. Cette approche, bien que déroutante au début, transforme radicalement la qualité et la conception de votre code.

---

## Qu'est-ce que le TDD ?

### Définition

Le **Test-Driven Development** (développement piloté par les tests) est une méthodologie où :

1. On écrit d'abord un test qui échoue
2. On écrit le minimum de code pour faire passer ce test
3. On améliore le code sans casser les tests

### Développement traditionnel vs TDD

**Approche traditionnelle :**

```
Écrire le code → Compiler → Tester manuellement → Déboguer → Écrire des tests (peut-être)
```

**Problèmes :**
- Tests écrits après coup (ou jamais)
- Code difficile à tester (couplage fort)
- Bugs découverts tard
- Sur-ingénierie (code inutilisé)

**Approche TDD :**

```
Écrire un test → Code minimal → Test passe → Refactorer → Répéter
```

**Avantages :**
- Code testé par conception
- Pas de code inutile
- Bugs détectés immédiatement
- Design émergent naturellement

### Exemple visuel

**Sans TDD :**

```c
// Écrire d'abord tout le code
int calculate_average(int *arr, size_t len) {
    if (!arr || len == 0) return 0;

    long long sum = 0;  // Sur-ingénierie : prévention débordement
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }

    // BUG : retourne un int, perd la précision
    return sum / len;
}

// Puis tester (et découvrir les bugs)
void test_average(void) {
    int arr[] = {1, 2, 3};
    TEST_ASSERT_EQUAL_INT(2, calculate_average(arr, 3));  // Passe par chance
}
```

**Avec TDD :**

```c
// 1. D'abord le test (qui ne compile même pas)
void test_average_simple_case(void) {
    int arr[] = {1, 2, 3};
    int result = calculate_average(arr, 3);
    TEST_ASSERT_EQUAL_INT(2, result);
}

// 2. Code minimal pour compiler et passer
int calculate_average(int *arr, size_t len) {
    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum / len;
}

// 3. Test suivant : cas limite
void test_average_empty_array(void) {
    TEST_ASSERT_EQUAL_INT(0, calculate_average(NULL, 0));
}

// 4. Ajouter la gestion d'erreur
int calculate_average(int *arr, size_t len) {
    if (!arr || len == 0) return 0;  // Ajouté pour le test

    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum / len;
}
```

---

## Le cycle Red-Green-Refactor

Le TDD suit un cycle en 3 étapes, répété pour chaque fonctionnalité.

### Étape 1 : Red (Rouge) 🔴

**Écrire un test qui échoue.**

```c
// Test d'une fonction qui n'existe pas encore
void test_string_reverse(void) {
    char str[] = "hello";
    string_reverse(str);  // ❌ Fonction non définie
    TEST_ASSERT_EQUAL_STRING("olleh", str);
}
```

**Compilation :**

```bash
gcc test.c -o test
# Erreur : undefined reference to 'string_reverse'
```

**Pourquoi c'est important :**
- Valide que le test détecte vraiment le problème
- Évite les "faux positifs" (tests qui passent toujours)
- Force à penser à l'API avant l'implémentation

### Étape 2 : Green (Vert) 🟢

**Écrire le code minimal pour faire passer le test.**

```c
void string_reverse(char *str) {
    if (!str) return;

    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}
```

**Test :**

```bash
gcc test.c string_utils.c -o test
./test
# ✅ Test passe
```

**Principe clé :** Ne pas sur-optimiser. Juste faire passer le test.

### Étape 3 : Refactor (Refactoriser) ♻️

**Améliorer le code sans changer le comportement.**

```c
// Version refactorisée : plus lisible
void string_reverse(char *str) {
    if (!str) return;

    char *start = str;
    char *end = str + strlen(str) - 1;

    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}
```

**Vérifier :**

```bash
./test
# ✅ Test passe toujours
```

**Ce qu'on peut refactoriser :**
- Noms de variables plus clairs
- Extraction de fonctions
- Simplification de la logique
- Optimisations de performance

### Répéter le cycle

```
Red 🔴 → Green 🟢 → Refactor ♻️ → Red 🔴 → Green 🟢 → Refactor ♻️ → ...
```

Chaque itération ajoute une fonctionnalité ou gère un cas limite.

---

## Exemple complet : Développer une fonction avec TDD

### Objectif

Créer une fonction `is_valid_email(const char *email)` qui valide un email.

**Cahier des charges :**
- Doit contenir un `@`
- Doit avoir du texte avant et après le `@`
- Doit avoir un `.` dans la partie après `@`

### Itération 1 : Test le plus simple

**Test :**

```c
void test_valid_email_simple(void) {
    TEST_ASSERT_TRUE(is_valid_email("user@example.com"));
}
```

**Compilation : ❌ Échec (fonction non définie)**

**Code minimal :**

```c
#include <stdbool.h>

bool is_valid_email(const char *email) {
    return true;  // Tricher pour faire passer le test
}
```

**Test : ✅ Passe**

**Problème :** Le test est trop faible. La fonction accepte tout.

### Itération 2 : Ajouter un test négatif

**Test :**

```c
void test_email_without_at(void) {
    TEST_ASSERT_FALSE(is_valid_email("userexample.com"));
}
```

**Test : ❌ Échec (retourne toujours true)**

**Code amélioré :**

```c
bool is_valid_email(const char *email) {
    if (!email) return false;

    // Chercher le @
    char *at = strchr(email, '@');
    return at != NULL;
}
```

**Tests : ✅ Les deux passent**

### Itération 3 : Texte avant le @

**Test :**

```c
void test_email_at_at_start(void) {
    TEST_ASSERT_FALSE(is_valid_email("@example.com"));
}
```

**Test : ❌ Échec**

**Code amélioré :**

```c
bool is_valid_email(const char *email) {
    if (!email || strlen(email) == 0) return false;

    char *at = strchr(email, '@');
    if (!at) return false;

    // Vérifier qu'il y a du texte avant le @
    if (at == email) return false;

    return true;
}
```

**Tests : ✅ Tous passent**

### Itération 4 : Texte après le @

**Test :**

```c
void test_email_at_at_end(void) {
    TEST_ASSERT_FALSE(is_valid_email("user@"));
}
```

**Test : ❌ Échec**

**Code amélioré :**

```c
bool is_valid_email(const char *email) {
    if (!email || strlen(email) == 0) return false;

    char *at = strchr(email, '@');
    if (!at) return false;
    if (at == email) return false;

    // Vérifier qu'il y a du texte après le @
    if (*(at + 1) == '\0') return false;

    return true;
}
```

**Tests : ✅ Tous passent**

### Itération 5 : Point dans la partie domaine

**Test :**

```c
void test_email_missing_dot_in_domain(void) {
    TEST_ASSERT_FALSE(is_valid_email("user@examplecom"));
}
```

**Test : ❌ Échec**

**Code amélioré :**

```c
bool is_valid_email(const char *email) {
    if (!email || strlen(email) == 0) return false;

    char *at = strchr(email, '@');
    if (!at) return false;
    if (at == email) return false;
    if (*(at + 1) == '\0') return false;

    // Vérifier qu'il y a un . après le @
    char *dot = strchr(at + 1, '.');
    if (!dot) return false;

    return true;
}
```

**Tests : ✅ Tous passent (5/5)**

### Refactorisation finale

**Améliorer la lisibilité :**

```c
bool is_valid_email(const char *email) {
    if (!email || strlen(email) == 0) {
        return false;
    }

    // Trouver le @
    const char *at_sign = strchr(email, '@');
    if (!at_sign || at_sign == email) {
        return false;  // Pas de @, ou @ au début
    }

    // Vérifier la partie après le @
    const char *domain = at_sign + 1;
    if (*domain == '\0') {
        return false;  // Pas de domaine
    }

    // Vérifier le . dans le domaine
    const char *dot = strchr(domain, '.');
    if (!dot || dot == domain || *(dot + 1) == '\0') {
        return false;  // Pas de ., ou . au début/fin
    }

    return true;
}
```

**Tests : ✅ Tous passent**

### Bilan de l'approche TDD

**Ce que nous avons obtenu :**
1. ✅ Code qui fonctionne
2. ✅ Tests complets (5 cas différents)
3. ✅ Design simple et clair
4. ✅ Confiance dans le code

**Ce que nous avons évité :**
- ❌ Sur-ingénierie (regex complexe inutile)
- ❌ Bugs cachés
- ❌ Code non testé

---

## Avantages du TDD

### 1. Meilleur design

Le TDD force à penser à l'**interface** avant l'**implémentation**.

**Sans TDD :**

```c
// Design compliqué : trop de paramètres
int parse_and_validate_and_store_user(
    char *input,
    Database *db,
    Logger *log,
    Config *cfg,
    ErrorHandler *err
);
```

**Avec TDD :**

```c
// Test simple force un design simple
void test_parse_user(void) {
    User *user = parse_user("John,30");
    TEST_ASSERT_EQUAL_STRING("John", user->name);
}

// Interface claire et simple
User *parse_user(const char *input);
```

### 2. Documentation vivante

Les tests documentent l'utilisation du code.

```c
// Ce test montre comment utiliser la fonction
void test_stack_push_pop(void) {
    Stack *stack = stack_create(10);

    stack_push(stack, 42);
    stack_push(stack, 17);

    TEST_ASSERT_EQUAL_INT(17, stack_pop(stack));
    TEST_ASSERT_EQUAL_INT(42, stack_pop(stack));

    stack_destroy(stack);
}
```

Un développeur qui lit ce test comprend immédiatement :
- Comment créer une pile
- Comment ajouter des éléments
- L'ordre LIFO
- Comment libérer la mémoire

### 3. Refactorisation sans peur

Avec une suite de tests complète, on peut refactoriser en toute confiance.

```c
// Version originale : Boucle for
int sum_array(int *arr, size_t len) {
    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

// Refactorisation : Pointeurs (plus idiomatique en C)
int sum_array(int *arr, size_t len) {
    int sum = 0;
    int *end = arr + len;
    for (int *p = arr; p < end; p++) {
        sum += *p;
    }
    return sum;
}

// Les tests passent toujours → Refactorisation réussie
```

### 4. Détection précoce des bugs

Les bugs sont détectés **en écrivant le code**, pas en production.

```c
// Test qui échoue immédiatement
void test_divide_by_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, safe_divide(10, 0));
}

// Force à gérer le cas
int safe_divide(int a, int b) {
    if (b == 0) return 0;  // Ajouté grâce au test
    return a / b;
}
```

### 5. Moins de debugging

**Statistique empirique :** Les équipes TDD passent **40% moins de temps** à déboguer.

**Raison :** Les bugs sont détectés ligne par ligne, pas après avoir écrit 500 lignes.

---

## Inconvénients et défis du TDD

### 1. Courbe d'apprentissage

Le TDD nécessite de **penser différemment**. Les premiers jours sont frustrants.

**Symptôme typique du débutant :**

```c
// Test trop compliqué pour démarrer
void test_complete_application(void) {
    init_database();
    init_network();
    start_server();
    send_request();
    // ... 50 lignes
    TEST_ASSERT_TRUE(everything_works);
}
```

**Solution :** Commencer par le test le plus simple possible.

### 2. Temps initial plus long

Écrire les tests prend du temps au début.

**Comparaison :**
- Sans TDD : 2h de code + 4h de debugging = **6h**
- Avec TDD : 3h de code+tests + 1h de debugging = **4h**

**Le TDD est plus lent au début, mais plus rapide globalement.**

### 3. Tests difficiles pour certains types de code

**Code difficile à tester avec TDD :**
- Interface graphique (GUI)
- Code bas niveau (drivers, inline assembly)
- Code legacy existant

**Solutions :**
- Isoler la logique métier (testable) de l'UI (non testable)
- Faire des tests d'intégration plutôt qu'unitaires
- Refactoriser progressivement le legacy

### 4. Tentation de tricher

```c
// ❌ Tricher pour faire passer le test
bool is_prime(int n) {
    if (n == 2) return true;  // Fonctionne seulement pour le test actuel
    return false;
}

void test_is_prime_2(void) {
    TEST_ASSERT_TRUE(is_prime(2));  // ✅ Passe
}

void test_is_prime_3(void) {
    TEST_ASSERT_TRUE(is_prime(3));  // ❌ Échoue
}
```

**Solution :** Écrire plusieurs tests rapidement pour éviter les solutions triviales.

---

## TDD en C : Spécificités

### 1. Gestion de la mémoire

En C, chaque allocation doit être testée et libérée.

**Cycle TDD avec malloc :**

```c
// Test 1 : Création
void test_list_create(void) {
    List *list = list_create();
    TEST_ASSERT_NOT_NULL(list);
    list_destroy(list);  // ⚠️ Ne pas oublier !
}

// Code minimal
List *list_create(void) {
    return malloc(sizeof(List));
}

void list_destroy(List *list) {
    free(list);
}

// Test 2 : Vérifier Valgrind
// make test && valgrind ./test
// 0 leaks ✅
```

### 2. Compilation incrémentale

En C, le code doit compiler à chaque étape.

**Technique : Stub minimal**

```c
// Test qui force la compilation
void test_database_connect(void) {
    Database *db = db_connect("test.db");
    TEST_ASSERT_NOT_NULL(db);
    db_close(db);
}

// Stub minimal (compile et passe)
typedef struct { int dummy; } Database;

Database *db_connect(const char *path) {
    return malloc(sizeof(Database));
}

void db_close(Database *db) {
    free(db);
}

// Ensuite, on ajoute la vraie logique progressivement
```

### 3. Fichiers headers

Créer le `.h` en même temps que le test.

**Workflow :**

```c
// 1. Écrire le test
#include "string_utils.h"
void test_string_length(void) {
    TEST_ASSERT_EQUAL_INT(5, string_length("hello"));
}

// 2. Créer le header pour compiler
// string_utils.h
#ifndef STRING_UTILS_H
#define STRING_UTILS_H
size_t string_length(const char *str);
#endif

// 3. Implémenter
// string_utils.c
size_t string_length(const char *str) {
    return strlen(str);
}
```

---

## Patterns TDD pour différents types de code

### 1. Algorithmes

**Approche :** Cas simples → Cas complexes → Cas limites

```c
// Test 1 : Cas trivial
void test_fibonacci_0(void) {
    TEST_ASSERT_EQUAL_INT(0, fibonacci(0));
}

// Code : Return 0
int fibonacci(int n) {
    return 0;
}

// Test 2 : Cas suivant
void test_fibonacci_1(void) {
    TEST_ASSERT_EQUAL_INT(1, fibonacci(1));
}

// Code : Gérer 0 et 1
int fibonacci(int n) {
    if (n <= 1) return n;
    return 0;
}

// Test 3 : Cas général
void test_fibonacci_5(void) {
    TEST_ASSERT_EQUAL_INT(5, fibonacci(5));
}

// Code : Implémentation complète
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}
```

### 2. Structures de données

**Approche :** Création → Opération de base → Opérations complexes

```c
// Test 1 : Création d'une pile vide
void test_stack_create(void) {
    Stack *s = stack_create();
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_TRUE(stack_is_empty(s));
    stack_destroy(s);
}

// Test 2 : Push d'un élément
void test_stack_push_one(void) {
    Stack *s = stack_create();
    stack_push(s, 42);
    TEST_ASSERT_FALSE(stack_is_empty(s));
    TEST_ASSERT_EQUAL_INT(1, stack_size(s));
    stack_destroy(s);
}

// Test 3 : Push + Pop
void test_stack_push_pop(void) {
    Stack *s = stack_create();
    stack_push(s, 42);
    int value = stack_pop(s);
    TEST_ASSERT_EQUAL_INT(42, value);
    TEST_ASSERT_TRUE(stack_is_empty(s));
    stack_destroy(s);
}

// Test 4 : Cas limite (pile pleine)
void test_stack_overflow(void) {
    Stack *s = stack_create_with_capacity(2);
    stack_push(s, 1);
    stack_push(s, 2);
    bool result = stack_push(s, 3);
    TEST_ASSERT_FALSE(result);  // Push échoue
    stack_destroy(s);
}
```

### 3. I/O et fichiers

**Approche :** Utiliser des fichiers temporaires

```c
void test_save_user_to_file(void) {
    // Setup : Fichier temporaire
    const char *temp_file = tmpnam(NULL);

    // Action
    User user = {.id = 1, .name = "Alice"};
    bool result = save_user_to_file(&user, temp_file);

    // Vérifications
    TEST_ASSERT_TRUE(result);

    // Vérifier le contenu
    FILE *f = fopen(temp_file, "r");
    char line[100];
    fgets(line, sizeof(line), f);
    TEST_ASSERT_EQUAL_STRING("1,Alice\n", line);
    fclose(f);

    // Cleanup
    remove(temp_file);
}
```

### 4. Parsers

**Approche :** Chaîne valide simple → Variations → Erreurs

```c
// Test 1 : Cas le plus simple
void test_parse_json_empty_object(void) {
    JsonValue *val = json_parse("{}");
    TEST_ASSERT_NOT_NULL(val);
    TEST_ASSERT_EQUAL_INT(JSON_OBJECT, val->type);
    json_free(val);
}

// Test 2 : Objet avec un champ
void test_parse_json_one_field(void) {
    JsonValue *val = json_parse("{\"name\":\"Alice\"}");
    TEST_ASSERT_EQUAL_STRING("Alice", json_get_string(val, "name"));
    json_free(val);
}

// Test 3 : Gestion d'erreur
void test_parse_json_invalid(void) {
    JsonValue *val = json_parse("{invalid}");
    TEST_ASSERT_NULL(val);
}
```

---

## Bonnes pratiques TDD

### 1. Un test à la fois

**❌ Mauvais : Écrire 10 tests d'un coup**

```c
void test_all_math_operations(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
    TEST_ASSERT_EQUAL_INT(1, subtract(3, 2));
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
    // ... 7 autres tests
}
```

**✅ Bon : Un test, puis le code, puis le suivant**

```
1. Test add → Code add → ✅
2. Test subtract → Code subtract → ✅
3. Test multiply → Code multiply → ✅
```

### 2. Tests simples d'abord

**Commencer par le "happy path", puis les cas limites.**

```c
// ✅ Ordre recommandé
void test_divide_normal(void);           // 1. Cas normal
void test_divide_by_one(void);           // 2. Cas trivial
void test_divide_by_zero(void);          // 3. Cas limite
void test_divide_negative(void);         // 4. Variation
void test_divide_large_numbers(void);    // 5. Cas extrême
```

### 3. Baby steps (petits pas)

**Chaque cycle Red-Green-Refactor doit prendre < 5 minutes.**

```c
// ❌ Trop gros : Implémenter toute une liste chaînée
void test_linked_list_complete(void);

// ✅ Petit pas : Un concept à la fois
void test_list_create(void);        // 2 minutes
void test_list_insert(void);        // 3 minutes
void test_list_remove(void);        // 4 minutes
void test_list_find(void);          // 3 minutes
```

### 4. Ne pas sauter le refactoring

**Après chaque Green, se demander : "Puis-je améliorer ce code ?"**

```c
// Version Green (fonctionne mais inélégant)
bool is_palindrome(const char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        if (str[i] != str[len - 1 - i]) {
            return false;
        }
    }
    return true;
}

// Version Refactor (plus lisible)
bool is_palindrome(const char *str) {
    const char *start = str;
    const char *end = str + strlen(str) - 1;

    while (start < end) {
        if (*start++ != *end--) {
            return false;
        }
    }
    return true;
}
```

### 5. Tests explicites et auto-documentés

**Nommer les tests clairement.**

```c
// ❌ Nom vague
void test1(void);
void test2(void);

// ✅ Nom descriptif
void test_email_validation_accepts_valid_email(void);
void test_email_validation_rejects_missing_at_sign(void);
void test_email_validation_rejects_empty_string(void);
```

---

## TDD et code legacy

### Problème

Le code legacy n'a souvent pas de tests. Comment appliquer le TDD ?

### Stratégie : Wrap and test

**Étape 1 : Identifier la fonction à modifier**

```c
// Code legacy (sans tests)
void process_order(Order *order) {
    // 200 lignes de code spaghetti
    validate_order(order);
    calculate_total(order);
    apply_discounts(order);
    send_confirmation_email(order);
    // ...
}
```

**Étape 2 : Écrire des tests de caractérisation**

Tests qui documentent le comportement actuel (même si bugué).

```c
void test_process_order_current_behavior(void) {
    Order order = {.id = 123, .total = 100};
    process_order(&order);
    // Documenter ce qui se passe actuellement
    TEST_ASSERT_EQUAL_INT(90, order.total);  // Réduction de 10%
}
```

**Étape 3 : Refactoriser avec TDD**

```c
// Extraire la logique dans une nouvelle fonction testable
double calculate_discount(double total, bool is_premium) {
    if (is_premium) {
        return total * 0.10;  // 10% de réduction
    }
    return 0;
}

// Nouveaux tests TDD pour la fonction extraite
void test_calculate_discount_premium(void) {
    TEST_ASSERT_EQUAL_DOUBLE(10.0, calculate_discount(100.0, true));
}

void test_calculate_discount_regular(void) {
    TEST_ASSERT_EQUAL_DOUBLE(0.0, calculate_discount(100.0, false));
}
```

---

## Quand utiliser (ou ne pas utiliser) le TDD

### Utiliser le TDD pour :

✅ **Logique métier complexe**
- Calculs financiers
- Algorithmes
- Validations

✅ **Code critique**
- Sécurité
- Transactions
- Traitement de données sensibles

✅ **Refactorisation**
- Modifier du code existant en toute sécurité

✅ **Apprentissage d'un nouveau domaine**
- Les tests aident à comprendre les exigences

### Ne pas utiliser le TDD pour :

❌ **Prototypes jetables**
- Exploration rapide d'une idée

❌ **Code d'interface graphique**
- Difficile à tester unitairement

❌ **Code bas niveau très spécifique**
- Drivers matériels
- Code assembleur

❌ **Quand les exigences sont floues**
- Il faut d'abord clarifier ce qu'on veut

---

## Outils pour faciliter le TDD en C

### 1. Frameworks de tests légers

**Unity** est parfait pour le TDD : rapide, simple, léger.

```c
#include "unity.h"

void test_add(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    return UNITY_END();
}
```

### 2. Compilation rapide

**Utiliser un Makefile intelligent :**

```makefile
# Recompiler seulement ce qui a changé
test: test_runner
	./test_runner

test_runner: test.c math_utils.c
	gcc -o test_runner $^

# Lancer automatiquement à chaque changement (avec entr)
watch:
	ls *.c *.h | entr make test
```

### 3. IDE avec exécution rapide

**VS Code + Tasks :**

```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "make test",
            "group": {
                "kind": "test",
                "isDefault": true
            }
        }
    ]
}
```

**Raccourci : `Ctrl+Shift+T` pour lancer les tests**

---

## Exemple de session TDD complète

### Objectif : Implémenter un buffer circulaire

**Test 1 : Créer le buffer**

```c
void test_circular_buffer_create(void) {
    CircularBuffer *buf = cb_create(5);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL_INT(0, cb_size(buf));
    cb_destroy(buf);
}
```

**Code :**

```c
typedef struct {
    int *data;
    size_t capacity;
    size_t size;
    size_t head;
    size_t tail;
} CircularBuffer;

CircularBuffer *cb_create(size_t capacity) {
    CircularBuffer *buf = malloc(sizeof(CircularBuffer));
    buf->data = malloc(capacity * sizeof(int));
    buf->capacity = capacity;
    buf->size = 0;
    buf->head = 0;
    buf->tail = 0;
    return buf;
}

size_t cb_size(CircularBuffer *buf) {
    return buf->size;
}

void cb_destroy(CircularBuffer *buf) {
    free(buf->data);
    free(buf);
}
```

**Test 2 : Ajouter un élément**

```c
void test_circular_buffer_push(void) {
    CircularBuffer *buf = cb_create(5);
    cb_push(buf, 42);
    TEST_ASSERT_EQUAL_INT(1, cb_size(buf));
    cb_destroy(buf);
}
```

**Code :**

```c
void cb_push(CircularBuffer *buf, int value) {
    buf->data[buf->tail] = value;
    buf->tail = (buf->tail + 1) % buf->capacity;
    buf->size++;
}
```

**Test 3 : Lire un élément**

```c
void test_circular_buffer_pop(void) {
    CircularBuffer *buf = cb_create(5);
    cb_push(buf, 42);
    int value = cb_pop(buf);
    TEST_ASSERT_EQUAL_INT(42, value);
    TEST_ASSERT_EQUAL_INT(0, cb_size(buf));
    cb_destroy(buf);
}
```

**Code :**

```c
int cb_pop(CircularBuffer *buf) {
    int value = buf->data[buf->head];
    buf->head = (buf->head + 1) % buf->capacity;
    buf->size--;
    return value;
}
```

**Test 4 : Buffer plein**

```c
void test_circular_buffer_full(void) {
    CircularBuffer *buf = cb_create(3);
    cb_push(buf, 1);
    cb_push(buf, 2);
    cb_push(buf, 3);
    TEST_ASSERT_TRUE(cb_is_full(buf));
    cb_destroy(buf);
}
```

**Code :**

```c
bool cb_is_full(CircularBuffer *buf) {
    return buf->size == buf->capacity;
}
```

**Test 5 : Débordement (overwrite)**

```c
void test_circular_buffer_overwrite(void) {
    CircularBuffer *buf = cb_create(3);
    cb_push(buf, 1);
    cb_push(buf, 2);
    cb_push(buf, 3);
    cb_push(buf, 4);  // Écrase le 1

    TEST_ASSERT_EQUAL_INT(2, cb_pop(buf));
    TEST_ASSERT_EQUAL_INT(3, cb_pop(buf));
    TEST_ASSERT_EQUAL_INT(4, cb_pop(buf));

    cb_destroy(buf);
}
```

**Code :**

```c
void cb_push(CircularBuffer *buf, int value) {
    buf->data[buf->tail] = value;
    buf->tail = (buf->tail + 1) % buf->capacity;

    if (buf->size < buf->capacity) {
        buf->size++;
    } else {
        // Buffer plein : avancer head pour écraser
        buf->head = (buf->head + 1) % buf->capacity;
    }
}
```

**Refactorisation :** Aucun besoin ici, le code est clair.

**Bilan :** 5 tests, 5 itérations, buffer circulaire fonctionnel.

---

## Métriques et feedback

### Mesurer la progression

```bash
# Exécuter les tests avec compteur
./test_runner | grep "Tests"
# Output: 5 Tests 0 Failures 0 Ignored
```

### Couverture de code

```bash
# Vérifier que chaque test augmente la couverture
gcc --coverage -o test test.c code.c
./test
gcov code.c
# Lines executed: 85.00% of 20
```

**Objectif TDD : Chaque itération augmente la couverture.**

---

## Conclusion

Le Test-Driven Development est plus qu'une technique : c'est une **philosophie de développement**. Au lieu de tester pour vérifier, on teste pour **concevoir**.

### Points clés à retenir

1. **Red-Green-Refactor** : Le cycle fondamental du TDD
2. **Tests d'abord** : Penser à l'API avant l'implémentation
3. **Baby steps** : Progresser par petits incréments
4. **Refactorisation sans peur** : Les tests donnent confiance
5. **Code plus simple** : Le TDD combat la sur-ingénierie

### Comment démarrer avec le TDD

**Semaine 1 :** Appliquer le TDD sur une fonction simple (ex: calculatrice)
**Semaine 2 :** Appliquer sur une structure de données (ex: liste, pile)
**Semaine 3 :** Appliquer sur un module complet (ex: parser)
**Semaine 4 :** Intégrer dans vos projets quotidiens

### Citation inspirante

> "Je ne suis pas un grand programmeur ; je suis juste un bon programmeur avec de bonnes habitudes."
> — Kent Beck, créateur du TDD

Le TDD est l'une de ces bonnes habitudes.

---

> **💡 Conseil final :** Le TDD peut sembler contre-intuitif au début, mais après quelques semaines de pratique, vous ne pourrez plus vous en passer. Donnez-vous le temps d'apprendre, et le TDD transformera votre manière de coder.

⏭️ [Intégration Continue moderne](/30-integration-continue/README.md)
