🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.3 Écriture de tests et assertions

## Introduction

Savoir **quoi** tester et **comment** l'exprimer clairement est tout aussi important que choisir un framework. Un test mal écrit peut donner une fausse confiance ou être impossible à déboguer. Cette section vous apprend à écrire des tests clairs, maintenables et efficaces.

---

## Anatomie d'un test unitaire

### Structure d'un test : AAA Pattern

La plupart des tests suivent le pattern **AAA** (Arrange, Act, Assert) :

```c
void test_addition(void) {
    // ARRANGE : Préparer les données
    int a = 5;
    int b = 3;
    int expected = 8;

    // ACT : Exécuter l'action à tester
    int result = add(a, b);

    // ASSERT : Vérifier le résultat
    TEST_ASSERT_EQUAL_INT(expected, result);
}
```

**Pourquoi cette structure ?**

- **Lisibilité** : Chacun comprend immédiatement ce que fait le test
- **Débogage** : Si le test échoue, on identifie rapidement la source du problème
- **Maintenance** : Facile de modifier ou d'ajouter des assertions

### Test minimal vs Test complet

**Test minimal (déconseillé) :**

```c
void test_add(void) {
    TEST_ASSERT_EQUAL_INT(8, add(5, 3));
}
```

**Test complet (recommandé) :**

```c
void test_add_positive_numbers(void) {
    // Arrange
    int a = 5;
    int b = 3;
    int expected = 8;

    // Act
    int actual = add(a, b);

    // Assert
    TEST_ASSERT_EQUAL_INT(expected, actual);
}
```

Le second est plus verbeux mais **beaucoup plus clair**, surtout lorsque le test échoue.

---

## Les types d'assertions

### Assertions d'égalité

Ce sont les assertions les plus courantes. Elles vérifient qu'une valeur correspond exactement à ce qui est attendu.

#### Entiers

```c
void test_integer_assertions(void) {
    int age = 25;

    // Égalité stricte
    TEST_ASSERT_EQUAL_INT(25, age);

    // Avec message personnalisé (Unity)
    TEST_ASSERT_EQUAL_INT_MESSAGE(25, age, "L'âge devrait être 25");

    // Entiers signés vs non signés
    unsigned int count = 10;
    TEST_ASSERT_EQUAL_UINT(10, count);

    // Entiers 8, 16, 32, 64 bits
    int8_t small = 5;
    TEST_ASSERT_EQUAL_INT8(5, small);

    int64_t large = 1000000000LL;
    TEST_ASSERT_EQUAL_INT64(1000000000LL, large);
}
```

**Conseil :** Utilisez toujours le type d'assertion correspondant au type de votre variable. Cela évite les conversions implicites qui peuvent masquer des bugs.

#### Nombres à virgule flottante

Les nombres flottants nécessitent une attention particulière car ils sont imprécis.

```c
void test_float_assertions(void) {
    float pi = 3.14159f;

    // ❌ MAUVAIS : Comparaison directe (peut échouer)
    // TEST_ASSERT_EQUAL_FLOAT(3.14159f, pi);

    // ✅ BON : Avec tolérance
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.14159f, pi);

    // Pour les doubles
    double e = 2.71828;
    TEST_ASSERT_DOUBLE_WITHIN(0.00001, 2.71828, e);
}
```

**Pourquoi une tolérance ?**

Les opérations en virgule flottante introduisent des erreurs d'arrondi :

```c
float result = 0.1f + 0.2f; // Résultat: 0.30000001 (pas exactement 0.3)  
TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.3f, result); // ✅ Passe  
TEST_ASSERT_EQUAL_FLOAT(0.3f, result);            // ❌ Échoue  
```

### Assertions de chaînes de caractères

Les chaînes en C sont des tableaux de `char` terminés par `\0`. Leur comparaison nécessite des assertions spéciales.

```c
void test_string_assertions(void) {
    const char *name = "Alice";

    // Égalité de chaînes
    TEST_ASSERT_EQUAL_STRING("Alice", name);

    // ❌ ERREUR FRÉQUENTE : Comparer les pointeurs
    // TEST_ASSERT_EQUAL_PTR("Alice", name); // Compare les adresses, pas le contenu

    // Chaînes vides
    const char *empty = "";
    TEST_ASSERT_EQUAL_STRING("", empty);

    // Sensibilité à la casse
    TEST_ASSERT_EQUAL_STRING("alice", "alice");  // ✅ Passe
    // TEST_ASSERT_EQUAL_STRING("alice", "Alice"); // ❌ Échoue
}
```

**Cas particulier : Chaînes NULL**

```c
void test_null_strings(void) {
    const char *str = NULL;

    // Vérifier explicitement NULL
    TEST_ASSERT_NULL(str);

    // ❌ DANGER : Ne jamais comparer une string NULL
    // TEST_ASSERT_EQUAL_STRING("", str); // SEGFAULT !
}
```

### Assertions de tableaux

Comparer des tableaux élément par élément.

```c
void test_array_assertions(void) {
    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    // Comparer les 5 premiers éléments
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 5);

    // Tableaux de différents types
    float expected_f[] = {1.0f, 2.0f, 3.0f};
    float actual_f[] = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_f, actual_f, 3);

    // Tableau de chaînes
    const char *expected_str[] = {"foo", "bar", "baz"};
    const char *actual_str[] = {"foo", "bar", "baz"};
    TEST_ASSERT_EQUAL_STRING_ARRAY(expected_str, actual_str, 3);
}
```

**Attention aux tailles :**

```c
void test_array_size_mismatch(void) {
    int arr1[] = {1, 2, 3};
    int arr2[] = {1, 2, 3, 4, 5};

    // Compare seulement les 3 premiers éléments
    TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr2, 3); // ✅ Passe

    // Les éléments 4 et 5 ne sont pas vérifiés !
}
```

### Assertions de mémoire brute

Pour comparer des zones mémoire brutes (structures, buffers binaires).

```c
void test_memory_assertions(void) {
    typedef struct {
        int x;
        int y;
    } Point;

    Point p1 = {10, 20};
    Point p2 = {10, 20};

    // Comparer byte par byte
    TEST_ASSERT_EQUAL_MEMORY(&p1, &p2, sizeof(Point));

    // Buffers binaires
    uint8_t buffer1[] = {0x01, 0x02, 0x03, 0xFF};
    uint8_t buffer2[] = {0x01, 0x02, 0x03, 0xFF};
    TEST_ASSERT_EQUAL_MEMORY(buffer1, buffer2, 4);
}
```

**Piège du padding :**

```c
typedef struct {
    char c;    // 1 byte
    // 3 bytes de padding (sur architecture 32-bit)
    int x;     // 4 bytes
} StructWithPadding;

void test_padding_issue(void) {
    StructWithPadding s1 = {.c = 'A', .x = 42};
    StructWithPadding s2 = {.c = 'A', .x = 42};

    // Peut échouer si le padding contient des valeurs aléatoires
    // TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(StructWithPadding));

    // ✅ Solution : Comparer champ par champ
    TEST_ASSERT_EQUAL_INT8(s1.c, s2.c);
    TEST_ASSERT_EQUAL_INT(s1.x, s2.x);
}
```

### Assertions de pointeurs

```c
void test_pointer_assertions(void) {
    int value = 42;
    int *ptr = &value;
    int *null_ptr = NULL;

    // Vérifier qu'un pointeur n'est pas NULL
    TEST_ASSERT_NOT_NULL(ptr);

    // Vérifier qu'un pointeur est NULL
    TEST_ASSERT_NULL(null_ptr);

    // Comparer deux pointeurs (même adresse ?)
    int *ptr2 = &value;
    TEST_ASSERT_EQUAL_PTR(ptr, ptr2);

    // Vérifier la valeur pointée
    TEST_ASSERT_EQUAL_INT(42, *ptr);
}
```

**Cas d'usage typique :** Vérifier qu'une allocation a réussi

```c
void test_malloc_success(void) {
    int *array = malloc(10 * sizeof(int));

    // Toujours vérifier que malloc a réussi
    TEST_ASSERT_NOT_NULL(array);

    // Utiliser le tableau...
    array[0] = 42;
    TEST_ASSERT_EQUAL_INT(42, array[0]);

    // Libérer
    free(array);
}
```

---

## Assertions booléennes et conditionnelles

### Assertions simples

```c
void test_boolean_assertions(void) {
    int age = 18;

    // Vérifier une condition vraie
    TEST_ASSERT_TRUE(age >= 18);

    // Vérifier une condition fausse
    TEST_ASSERT_FALSE(age < 0);

    // ❌ ANTI-PATTERN : Ne pas comparer avec true/false
    // TEST_ASSERT_TRUE(is_valid == true); // Redondant
    // ✅ Préférer
    TEST_ASSERT_TRUE(is_valid);
}
```

### Assertions avec message personnalisé

Tous les frameworks permettent d'ajouter un message d'erreur personnalisé :

```c
void test_with_custom_message(void) {
    int score = calculate_score();

    // Message affiché en cas d'échec
    TEST_ASSERT_TRUE_MESSAGE(score >= 0, "Le score ne peut pas être négatif");
    TEST_ASSERT_TRUE_MESSAGE(score <= 100, "Le score ne peut pas dépasser 100");
}
```

**Quand utiliser des messages personnalisés ?**

- Lorsque la condition testée n'est pas évidente
- Pour donner du contexte sur les valeurs attendues
- Dans des boucles où plusieurs assertions similaires sont faites

```c
void test_array_processing(void) {
    int values[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        char msg[100];
        sprintf(msg, "La valeur à l'index %d devrait être positive", i);
        TEST_ASSERT_TRUE_MESSAGE(values[i] > 0, msg);
    }
}
```

---

## Assertions de comparaison

### Comparaisons numériques

```c
void test_comparison_assertions(void) {
    int temperature = 25;

    // Plus grand que
    TEST_ASSERT_GREATER_THAN(20, temperature);

    // Plus petit que
    TEST_ASSERT_LESS_THAN(30, temperature);

    // Plus grand ou égal
    TEST_ASSERT_GREATER_OR_EQUAL(25, temperature);

    // Plus petit ou égal
    TEST_ASSERT_LESS_OR_EQUAL(25, temperature);

    // Dans un intervalle (Unity uniquement)
    TEST_ASSERT_INT_WITHIN(5, 25, temperature); // 25 ± 5
}
```

**Exemple concret : Valider une température**

```c
typedef enum {
    TEMP_OK,
    TEMP_TOO_LOW,
    TEMP_TOO_HIGH
} TempStatus;

TempStatus check_temperature(int temp) {
    if (temp < 18) return TEMP_TOO_LOW;
    if (temp > 25) return TEMP_TOO_HIGH;
    return TEMP_OK;
}

void test_temperature_ranges(void) {
    // Cas limites bas
    TEST_ASSERT_EQUAL_INT(TEMP_TOO_LOW, check_temperature(17));
    TEST_ASSERT_EQUAL_INT(TEMP_OK, check_temperature(18));

    // Cas limites haut
    TEST_ASSERT_EQUAL_INT(TEMP_OK, check_temperature(25));
    TEST_ASSERT_EQUAL_INT(TEMP_TOO_HIGH, check_temperature(26));

    // Cas nominal
    TEST_ASSERT_EQUAL_INT(TEMP_OK, check_temperature(22));
}
```

---

## Tester les cas limites (Edge Cases)

### Les cas limites en C

En C, les cas limites sont souvent la source principale de bugs. Voici les plus importants à tester :

#### 1. Valeurs NULL

```c
char *safe_string_copy(const char *src) {
    if (src == NULL) return NULL;

    char *dest = malloc(strlen(src) + 1);
    if (dest == NULL) return NULL;

    strcpy(dest, src);
    return dest;
}

void test_string_copy_edge_cases(void) {
    // Cas normal
    char *result = safe_string_copy("Hello");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Hello", result);
    free(result);

    // ✅ Cas limite : NULL en entrée
    result = safe_string_copy(NULL);
    TEST_ASSERT_NULL(result);

    // ✅ Cas limite : Chaîne vide
    result = safe_string_copy("");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("", result);
    free(result);
}
```

#### 2. Valeurs à zéro

```c
int divide(int a, int b) {
    if (b == 0) return 0; // Gestion de l'erreur
    return a / b;
}

void test_divide_edge_cases(void) {
    // Cas normal
    TEST_ASSERT_EQUAL_INT(5, divide(10, 2));

    // ✅ Division par zéro
    TEST_ASSERT_EQUAL_INT(0, divide(10, 0));

    // ✅ Zéro divisé par quelque chose
    TEST_ASSERT_EQUAL_INT(0, divide(0, 5));
}
```

#### 3. Tableaux vides

```c
int sum_array(const int *arr, size_t len) {
    if (arr == NULL || len == 0) return 0;

    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

void test_sum_array_edge_cases(void) {
    // Cas normal
    int arr[] = {1, 2, 3};
    TEST_ASSERT_EQUAL_INT(6, sum_array(arr, 3));

    // ✅ Tableau vide (len = 0)
    TEST_ASSERT_EQUAL_INT(0, sum_array(arr, 0));

    // ✅ Pointeur NULL
    TEST_ASSERT_EQUAL_INT(0, sum_array(NULL, 5));

    // ✅ Un seul élément
    int single[] = {42};
    TEST_ASSERT_EQUAL_INT(42, sum_array(single, 1));
}
```

#### 4. Débordements d'entiers

```c
int safe_add(int a, int b) {
    // Vérifier le débordement
    if (a > 0 && b > INT_MAX - a) return INT_MAX;
    if (a < 0 && b < INT_MIN - a) return INT_MIN;
    return a + b;
}

void test_integer_overflow(void) {
    // Cas normal
    TEST_ASSERT_EQUAL_INT(5, safe_add(2, 3));

    // ✅ Débordement positif
    TEST_ASSERT_EQUAL_INT(INT_MAX, safe_add(INT_MAX, 1));

    // ✅ Débordement négatif
    TEST_ASSERT_EQUAL_INT(INT_MIN, safe_add(INT_MIN, -1));
}
```

#### 5. Limites des types

```c
void test_type_limits(void) {
    // Limites d'un int8_t
    int8_t max8 = INT8_MAX;  // 127
    int8_t min8 = INT8_MIN;  // -128

    TEST_ASSERT_EQUAL_INT8(127, max8);
    TEST_ASSERT_EQUAL_INT8(-128, min8);

    // Limites d'un uint32_t
    uint32_t max32 = UINT32_MAX; // 4294967295
    TEST_ASSERT_EQUAL_UINT32(4294967295U, max32);
}
```

---

## Tester les structures de données

### Structures simples

```c
typedef struct {
    char name[50];
    int age;
    float salary;
} Employee;

Employee create_employee(const char *name, int age, float salary) {
    Employee e;
    strncpy(e.name, name, sizeof(e.name) - 1);
    e.name[sizeof(e.name) - 1] = '\0';
    e.age = age;
    e.salary = salary;
    return e;
}

void test_create_employee(void) {
    // Arrange
    const char *expected_name = "Alice";
    int expected_age = 30;
    float expected_salary = 50000.0f;

    // Act
    Employee emp = create_employee(expected_name, expected_age, expected_salary);

    // Assert - Vérifier chaque champ
    TEST_ASSERT_EQUAL_STRING(expected_name, emp.name);
    TEST_ASSERT_EQUAL_INT(expected_age, emp.age);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_salary, emp.salary);
}
```

**Conseil :** Testez chaque champ individuellement plutôt que d'utiliser `TEST_ASSERT_EQUAL_MEMORY` pour les structures.

### Listes chaînées

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *create_node(int data) {
    Node *node = malloc(sizeof(Node));
    if (node) {
        node->data = data;
        node->next = NULL;
    }
    return node;
}

void test_create_node(void) {
    // Arrange & Act
    Node *node = create_node(42);

    // Assert
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_INT(42, node->data);
    TEST_ASSERT_NULL(node->next);

    // Cleanup
    free(node);
}

void test_linked_list_insertion(void) {
    // Créer une liste : 1 -> 2 -> 3
    Node *head = create_node(1);
    head->next = create_node(2);
    head->next->next = create_node(3);

    // Vérifier la structure
    TEST_ASSERT_NOT_NULL(head);
    TEST_ASSERT_EQUAL_INT(1, head->data);
    TEST_ASSERT_NOT_NULL(head->next);
    TEST_ASSERT_EQUAL_INT(2, head->next->data);
    TEST_ASSERT_NOT_NULL(head->next->next);
    TEST_ASSERT_EQUAL_INT(3, head->next->next->data);
    TEST_ASSERT_NULL(head->next->next->next);

    // Cleanup
    free(head->next->next);
    free(head->next);
    free(head);
}
```

---

## Tester les fonctions avec gestion d'erreurs

### Codes de retour

Beaucoup de fonctions C retournent un code d'erreur (0 = succès, négatif = erreur).

```c
typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER = -1,
    ERROR_OUT_OF_RANGE = -2,
    ERROR_ALLOCATION_FAILED = -3
} ErrorCode;

ErrorCode set_array_value(int *arr, size_t len, size_t index, int value) {
    if (arr == NULL) return ERROR_NULL_POINTER;
    if (index >= len) return ERROR_OUT_OF_RANGE;

    arr[index] = value;
    return SUCCESS;
}

void test_set_array_value_success(void) {
    int arr[5] = {0};
    ErrorCode result = set_array_value(arr, 5, 2, 42);

    TEST_ASSERT_EQUAL_INT(SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(42, arr[2]);
}

void test_set_array_value_errors(void) {
    int arr[5] = {0};

    // Erreur : NULL pointer
    TEST_ASSERT_EQUAL_INT(ERROR_NULL_POINTER, set_array_value(NULL, 5, 0, 42));

    // Erreur : Index hors limites
    TEST_ASSERT_EQUAL_INT(ERROR_OUT_OF_RANGE, set_array_value(arr, 5, 10, 42));

    // Vérifier que le tableau n'a pas été modifié
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(0, arr[i]);
    }
}
```

### errno

Les fonctions de la bibliothèque standard utilisent souvent `errno`.

```c
#include <errno.h>

void test_file_operations_with_errno(void) {
    // Tenter d'ouvrir un fichier inexistant
    FILE *f = fopen("/path/does/not/exist.txt", "r");

    // Vérifier l'échec
    TEST_ASSERT_NULL(f);
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);

    // Réinitialiser errno pour le prochain test
    errno = 0;
}
```

**Important :** Toujours réinitialiser `errno` entre les tests si vous le testez.

---

## Organisation et nommage des tests

### Convention de nommage

Un bon nom de test doit indiquer :
1. **Quoi** est testé (fonction ou comportement)
2. **Dans quel contexte** (conditions d'entrée)
3. **Quel résultat** est attendu

**Format recommandé :** `test_<fonction>_<contexte>_<résultat>`

```c
// ✅ Bons noms
void test_add_positive_numbers_returns_sum(void);  
void test_divide_by_zero_returns_error(void);  
void test_malloc_failure_returns_null(void);  
void test_empty_list_has_zero_size(void);  

// ❌ Mauvais noms
void test1(void);              // Pas de contexte  
void test_add(void);           // Trop vague  
void test_stuff(void);         // Pas descriptif  
void my_test(void);            // Pas de convention  
```

### Grouper les tests par fonctionnalité

```c
// Tests pour la fonction add()
void test_add_positive_numbers(void);  
void test_add_negative_numbers(void);  
void test_add_with_zero(void);  
void test_add_overflow(void);  

// Tests pour la fonction multiply()
void test_multiply_positive_numbers(void);  
void test_multiply_by_zero(void);  
void test_multiply_by_one(void);  
void test_multiply_negative_numbers(void);  
```

---

## Erreurs courantes à éviter

### 1. Tests qui testent plusieurs choses

```c
// ❌ MAUVAIS : Un test qui fait trop de choses
void test_math_operations(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
    TEST_ASSERT_EQUAL_INT(2, divide(6, 3));
    TEST_ASSERT_EQUAL_INT(1, subtract(3, 2));
}
```

**Problème :** Si le test échoue, on ne sait pas quelle opération est en cause.

```c
// ✅ BON : Un test par concept
void test_add(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
}

void test_divide(void) {
    TEST_ASSERT_EQUAL_INT(2, divide(6, 3));
}

void test_subtract(void) {
    TEST_ASSERT_EQUAL_INT(1, subtract(3, 2));
}
```

### 2. Tests dépendants de l'ordre

```c
static int counter = 0;

// ❌ MAUVAIS : Dépend de l'ordre d'exécution
void test_increment_first(void) {
    counter++;
    TEST_ASSERT_EQUAL_INT(1, counter);
}

void test_increment_second(void) {
    counter++;
    TEST_ASSERT_EQUAL_INT(2, counter); // Échoue si exécuté seul
}
```

```c
// ✅ BON : Chaque test est indépendant
void test_increment_from_zero(void) {
    int counter = 0;
    counter++;
    TEST_ASSERT_EQUAL_INT(1, counter);
}

void test_increment_from_five(void) {
    int counter = 5;
    counter++;
    TEST_ASSERT_EQUAL_INT(6, counter);
}
```

### 3. Oublier de libérer la mémoire

```c
// ❌ MAUVAIS : Fuite mémoire
void test_create_list(void) {
    List *list = list_create();
    TEST_ASSERT_NOT_NULL(list);
    // Oubli du free → fuite détectée par Valgrind
}

// ✅ BON : Nettoyage systématique
void test_create_list(void) {
    List *list = list_create();
    TEST_ASSERT_NOT_NULL(list);
    list_destroy(list); // Libération
}
```

### 4. Tests qui ne peuvent jamais échouer

```c
// ❌ MAUVAIS : Ce test passe toujours
void test_useless(void) {
    int result = add(2, 3);
    // Pas d'assertion !
}

// ❌ MAUVAIS : Tautologie
void test_tautology(void) {
    int x = 5;
    TEST_ASSERT_EQUAL_INT(x, x); // Toujours vrai
}
```

### 5. Comparer des pointeurs au lieu du contenu

```c
// ❌ MAUVAIS : Compare les adresses
void test_string_wrong(void) {
    const char *s1 = "hello";
    const char *s2 = get_greeting();
    TEST_ASSERT_EQUAL_PTR(s1, s2); // Échoue même si le contenu est identique
}

// ✅ BON : Compare le contenu
void test_string_correct(void) {
    const char *s1 = "hello";
    const char *s2 = get_greeting();
    TEST_ASSERT_EQUAL_STRING(s1, s2);
}
```

---

## Assertions personnalisées

Pour rendre vos tests plus expressifs, créez vos propres macros d'assertion.

### Exemple : Vérifier qu'une liste est vide

```c
#define ASSERT_LIST_EMPTY(list) \
    do { \
        TEST_ASSERT_NOT_NULL(list); \
        TEST_ASSERT_EQUAL_INT(0, list->size); \
        TEST_ASSERT_NULL(list->head); \
    } while(0)

void test_list_initialization(void) {
    List *list = list_create();
    ASSERT_LIST_EMPTY(list);
    list_destroy(list);
}
```

### Exemple : Vérifier une plage de valeurs

```c
#define ASSERT_IN_RANGE(value, min, max) \
    do { \
        TEST_ASSERT_GREATER_OR_EQUAL(min, value); \
        TEST_ASSERT_LESS_OR_EQUAL(max, value); \
    } while(0)

void test_random_number(void) {
    int num = get_random_number(1, 10);
    ASSERT_IN_RANGE(num, 1, 10);
}
```

**Avantages :**
- Code plus lisible
- Réutilisabilité
- Facilite la maintenance

---

## Tests paramétriques (Data-Driven Tests)

Évitez la duplication en testant plusieurs jeux de données avec la même logique.

### Approche manuelle

```c
typedef struct {
    int a;
    int b;
    int expected;
} AddTestCase;

void test_add_multiple_cases(void) {
    AddTestCase cases[] = {
        {2, 3, 5},
        {-1, 1, 0},
        {0, 0, 0},
        {100, -50, 50},
        {INT_MAX, 0, INT_MAX}
    };

    size_t num_cases = sizeof(cases) / sizeof(cases[0]);

    for (size_t i = 0; i < num_cases; i++) {
        int result = add(cases[i].a, cases[i].b);

        char msg[100];
        sprintf(msg, "Échec pour add(%d, %d)", cases[i].a, cases[i].b);
        TEST_ASSERT_EQUAL_INT_MESSAGE(cases[i].expected, result, msg);
    }
}
```

**Avantage :** Un seul test vérifie de nombreux cas.

**Inconvénient :** Si un cas échoue, le test s'arrête (les cas suivants ne sont pas testés).

### Avec Criterion (tests paramétriques natifs)

Si vous utilisez Criterion, les tests paramétriques sont intégrés :

```c
struct add_params {
    int a;
    int b;
    int expected;
};

ParameterizedTestParameters(math, add) {
    static struct add_params params[] = {
        {2, 3, 5},
        {-1, 1, 0},
        {0, 0, 0},
    };
    return cr_make_param_array(struct add_params, params,
                               sizeof(params) / sizeof(params[0]));
}

ParameterizedTest(struct add_params *param, math, add) {
    cr_assert_eq(add(param->a, param->b), param->expected);
}
```

---

## Documentation des tests

### Commentaires dans les tests

Les tests doivent être auto-documentés, mais des commentaires peuvent aider :

```c
void test_division_edge_cases(void) {
    // Cas normal : division simple
    TEST_ASSERT_EQUAL_INT(5, divide(10, 2));

    // Cas limite : division par 1
    TEST_ASSERT_EQUAL_INT(42, divide(42, 1));

    // Cas d'erreur : division par zéro
    // La fonction doit retourner 0 et logger une erreur
    TEST_ASSERT_EQUAL_INT(0, divide(10, 0));

    // Cas limite : résultat négatif
    TEST_ASSERT_EQUAL_INT(-5, divide(10, -2));
}
```

### Messages d'assertions explicites

```c
void test_with_helpful_messages(void) {
    int score = calculate_final_score(80, 90, 85);

    TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(
        0, score,
        "Le score final ne peut jamais être négatif"
    );

    TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(
        100, score,
        "Le score final ne peut jamais dépasser 100"
    );
}
```

---

## Récapitulatif : Checklist pour un bon test

Avant de valider un test, vérifiez qu'il respecte ces critères :

- [ ] **Nom descriptif** : On comprend ce qui est testé sans lire le code
- [ ] **Structure AAA** : Arrange, Act, Assert clairement séparés
- [ ] **Une seule responsabilité** : Le test vérifie un seul comportement
- [ ] **Indépendant** : Peut s'exécuter seul, dans n'importe quel ordre
- [ ] **Déterministe** : Donne toujours le même résultat
- [ ] **Rapide** : S'exécute en < 100ms
- [ ] **Assertions explicites** : Utilise les bonnes assertions (pas de `assert(true)`)
- [ ] **Gestion mémoire** : Libère toute mémoire allouée (validé par Valgrind)
- [ ] **Cas limites couverts** : NULL, zéro, limites de types
- [ ] **Messages d'erreur clairs** : En cas d'échec, on comprend pourquoi

---

## Conclusion

L'art d'écrire de bons tests réside dans la **clarté** et la **précision**. Un test doit être aussi simple que possible, tout en vérifiant rigoureusement le comportement attendu.

### Points clés à retenir

1. **Une assertion = une vérification** : Ne surchargez pas vos tests
2. **Testez les cas limites** : C'est là que se cachent les bugs
3. **Nommez clairement** : Le nom du test doit être auto-explicatif
4. **Isolez vos tests** : Aucune dépendance entre tests
5. **Utilisez les bonnes assertions** : `_STRING` pour strings, `_FLOAT_WITHIN` pour flottants, etc.

Dans la section suivante (**29.4 Mocking et Stubbing**), nous verrons comment tester du code qui dépend de ressources externes (fichiers, réseau, bases de données) en les simulant.

---

> **💡 Conseil final :** La qualité d'un test se mesure à sa capacité à détecter un bug. Un test qui passe toujours, même quand le code est cassé, est inutile. Cassez volontairement votre code pour vérifier que vos tests le détectent !

⏭️ [Mocking et stubbing](/29-tests-et-qualite/04-mocking-stubbing.md)
