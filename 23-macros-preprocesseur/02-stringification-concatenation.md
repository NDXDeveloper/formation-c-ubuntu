🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.2 Stringification et concaténation

## Introduction

Le préprocesseur C offre deux opérateurs spéciaux qui permettent de manipuler le texte des macros de manière avancée :

- **L'opérateur `#`** (stringification) : transforme un argument de macro en chaîne de caractères
- **L'opérateur `##`** (concaténation) : colle deux tokens ensemble pour en former un nouveau

Ces opérateurs sont exclusifs aux macros et permettent de générer du code de manière dynamique au moment de la préprocession.

---

## L'opérateur # : Stringification

### Qu'est-ce que la stringification ?

La **stringification** consiste à transformer un argument de macro en une chaîne de caractères littérale (entre guillemets).

### Syntaxe de base

```c
#define STRINGIFIER(x) #x
```

L'opérateur `#` placé devant le paramètre `x` transforme l'argument en chaîne.

### Premier exemple simple

```c
#include <stdio.h>

#define STRINGIFIER(x) #x

int main(void) {
    printf("%s\n", STRINGIFIER(Hello));
    // Affiche : Hello

    printf("%s\n", STRINGIFIER(123));
    // Affiche : 123

    printf("%s\n", STRINGIFIER(3 + 5));
    // Affiche : 3 + 5

    return 0;
}
```

**Comment ça fonctionne ?**

Le préprocesseur remplace :
- `STRINGIFIER(Hello)` → `"Hello"`
- `STRINGIFIER(123)` → `"123"`
- `STRINGIFIER(3 + 5)` → `"3 + 5"` (note : pas d'évaluation, juste conversion en texte)

### Exemple pratique : Debug printing

Un cas d'usage très courant de la stringification est l'affichage de variables avec leur nom :

```c
#include <stdio.h>

#define PRINT_VAR(var) printf(#var " = %d\n", var)

int main(void) {
    int age = 25;
    int score = 1337;

    PRINT_VAR(age);
    // Affiche : age = 25

    PRINT_VAR(score);
    // Affiche : score = 1337

    return 0;
}
```

**Explication** :
- `PRINT_VAR(age)` devient → `printf("age" " = %d\n", age)`
- Les chaînes adjacentes en C sont automatiquement concaténées : `"age" " = %d\n"` → `"age = %d\n"`

### Macro de débogage avancée

```c
#include <stdio.h>

#define DEBUG(var) \
    printf("[DEBUG] %s:%d - " #var " = %d\n", __FILE__, __LINE__, var)

int main(void) {
    int compteur = 42;
    int resultat = 100;

    DEBUG(compteur);
    // Affiche : [DEBUG] main.c:10 - compteur = 42

    DEBUG(resultat);
    // Affiche : [DEBUG] main.c:13 - resultat = 100

    return 0;
}
```

### Stringification avec différents types

```c
#include <stdio.h>

#define PRINT_INT(var) printf(#var " = %d\n", var)
#define PRINT_FLOAT(var) printf(#var " = %.2f\n", var)
#define PRINT_STRING(var) printf(#var " = %s\n", var)

int main(void) {
    int entier = 42;
    double pi = 3.14159;
    const char* nom = "Alice";

    PRINT_INT(entier);
    // Affiche : entier = 42

    PRINT_FLOAT(pi);
    // Affiche : pi = 3.14

    PRINT_STRING(nom);
    // Affiche : nom = Alice

    return 0;
}
```

### Piège : Stringification et expansion de macro

**Important** : La stringification se fait **avant** l'expansion des macros imbriquées.

```c
#include <stdio.h>

#define VALUE 100
#define STRINGIFY(x) #x

int main(void) {
    printf("%s\n", STRINGIFY(VALUE));
    // Affiche : VALUE (pas 100 !)

    return 0;
}
```

Le préprocesseur stringifie d'abord, donc `VALUE` devient `"VALUE"` sans être expansé.

**Solution : Double expansion**

```c
#include <stdio.h>

#define VALUE 100

// Macro d'indirection pour forcer l'expansion
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

int main(void) {
    printf("%s\n", STRINGIFY(VALUE));
    // Affiche : 100 ✓

    return 0;
}
```

**Explication** :
1. `STRINGIFY(VALUE)` → appelle `STRINGIFY_HELPER(VALUE)`
2. `VALUE` est expansé en `100` **avant** d'être passé à `STRINGIFY_HELPER`
3. `STRINGIFY_HELPER(100)` → `"100"`

---

## L'opérateur ## : Concaténation (Token Pasting)

### Qu'est-ce que la concaténation ?

La **concaténation** (ou *token pasting*) permet de coller deux tokens ensemble pour créer un nouveau token (identificateur, nombre, etc.).

### Syntaxe de base

```c
#define CONCAT(a, b) a##b
```

L'opérateur `##` colle les arguments `a` et `b` ensemble.

### Premier exemple simple

```c
#include <stdio.h>

#define CONCAT(a, b) a##b

int main(void) {
    int xy = 42;

    printf("%d\n", CONCAT(x, y));
    // CONCAT(x, y) devient xy
    // Affiche : 42

    return 0;
}
```

**Comment ça fonctionne ?**

Le préprocesseur remplace :
- `CONCAT(x, y)` → `xy` (les tokens `x` et `y` sont collés)

### Génération de noms de variables

Un usage courant est de générer des noms de variables dynamiquement :

```c
#include <stdio.h>

#define DECLARE_VAR(type, prefix, suffix) type prefix##suffix

int main(void) {
    DECLARE_VAR(int, compteur, _principal) = 100;
    DECLARE_VAR(int, compteur, _secondaire) = 50;

    // Équivalent à :
    // int compteur_principal = 100;
    // int compteur_secondaire = 50;

    printf("Principal : %d\n", compteur_principal);
    printf("Secondaire : %d\n", compteur_secondaire);

    return 0;
}
```

### Génération de fonctions similaires

```c
#include <stdio.h>

// Macro qui génère des fonctions getter
#define DEFINE_GETTER(type, name) \
    type get_##name(void) { \
        return name; \
    }

// Variables globales (pour l'exemple)
static int age = 30;  
static double salaire = 45000.50;  

// Génération automatique des getters
DEFINE_GETTER(int, age)  
DEFINE_GETTER(double, salaire)  

// Équivalent à écrire :
// int get_age(void) { return age; }
// double get_salaire(void) { return salaire; }

int main(void) {
    printf("Âge : %d\n", get_age());
    printf("Salaire : %.2f\n", get_salaire());

    return 0;
}
```

### Génération de getters et setters

```c
#include <stdio.h>

#define DEFINE_PROPERTY(type, name) \
    static type _##name; \
    type get_##name(void) { return _##name; } \
    void set_##name(type value) { _##name = value; }

// Génère automatiquement :
// - Une variable privée _id
// - Un getter get_id()
// - Un setter set_id()
DEFINE_PROPERTY(int, id)  
DEFINE_PROPERTY(double, prix)  

int main(void) {
    set_id(12345);
    set_prix(99.99);

    printf("ID : %d\n", get_id());
    printf("Prix : %.2f\n", get_prix());

    return 0;
}
```

### Concaténation avec des chaînes et nombres

```c
#include <stdio.h>

#define MAKE_VERSION(major, minor) v##major##_##minor
#define MAKE_CONSTANT(prefix, num) prefix##num

int main(void) {
    // Génération d'identificateurs
    int MAKE_VERSION(2, 5) = 250;
    int MAKE_CONSTANT(MAX_, 100) = 100;

    // Équivalent à :
    // int v2_5 = 250;
    // int MAX_100 = 100;

    printf("Version : %d\n", v2_5);
    printf("Constante : %d\n", MAX_100);

    return 0;
}
```

### Piège : Concaténation et expansion de macro

Comme pour la stringification, la concaténation se fait **avant** l'expansion des macros.

```c
#include <stdio.h>

#define PREFIX value
#define CONCAT(a, b) a##b

int main(void) {
    int value_10 = 100;

    printf("%d\n", CONCAT(PREFIX, _10));
    // N'affiche PAS 100, mais génère une erreur !
    // Car CONCAT(PREFIX, _10) devient PREFIX_10 (sans expansion)

    return 0;
}
```

**Solution : Double expansion**

```c
#include <stdio.h>

#define PREFIX value

// Macro d'indirection pour forcer l'expansion
#define CONCAT_HELPER(a, b) a##b
#define CONCAT(a, b) CONCAT_HELPER(a, b)

int main(void) {
    int value_10 = 100;

    printf("%d\n", CONCAT(PREFIX, _10));
    // PREFIX est d'abord expansé en value
    // Puis value##_10 devient value_10
    // Affiche : 100 ✓

    return 0;
}
```

---

## Combinaison de # et ##

Les deux opérateurs peuvent être utilisés ensemble dans des macros complexes.

### Exemple : Macro de test unitaire

```c
#include <stdio.h>

#define TEST(name) \
    void test_##name(void); \
    void test_##name(void)

#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("ÉCHEC: " #actual " (%d) != " #expected " (%d)\n", \
                   (actual), (expected)); \
        } else { \
            printf("OK: " #actual " == " #expected "\n"); \
        } \
    } while(0)

// Définition d'un test
TEST(addition) {
    int resultat = 2 + 3;
    ASSERT_EQUAL(resultat, 5);
}

TEST(multiplication) {
    int resultat = 4 * 5;
    ASSERT_EQUAL(resultat, 20);
}

int main(void) {
    test_addition();
    test_multiplication();

    return 0;
}
```

**Sortie** :
```
OK: resultat == 5  
OK: resultat == 20  
```

### Exemple : Génération de tableaux de structures

```c
#include <stdio.h>

// Macro qui génère une structure et ses accesseurs
#define DEFINE_STRUCT(name) \
    typedef struct { \
        int id; \
        char nom[50]; \
    } name##_t; \
    \
    void print_##name(name##_t* obj) { \
        printf(#name " { id=%d, nom=%s }\n", obj->id, obj->nom); \
    }

// Génération automatique
DEFINE_STRUCT(Personne)  
DEFINE_STRUCT(Produit)  

// Équivalent à :
// typedef struct { int id; char nom[50]; } Personne_t;
// void print_Personne(Personne_t* obj) { ... }
//
// typedef struct { int id; char nom[50]; } Produit_t;
// void print_Produit(Produit_t* obj) { ... }

int main(void) {
    Personne_t p1 = {1, "Alice"};
    Produit_t prod1 = {100, "Laptop"};

    print_Personne(&p1);
    print_Produit(&prod1);

    return 0;
}
```

**Sortie** :
```
Personne { id=1, nom=Alice }  
Produit { id=100, nom=Laptop }  
```

---

## Cas d'usage pratiques

### 1. Logging avec niveaux

```c
#include <stdio.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

#define LOG(level, msg) \
    printf("[%s] %s:%d - %s\n", #level, __FILE__, __LINE__, msg)

int main(void) {
    LOG(INFO, "Démarrage du programme");
    LOG(WARNING, "Mémoire faible");
    LOG(ERROR, "Fichier introuvable");

    return 0;
}
```

**Sortie** :
```
[INFO] main.c:15 - Démarrage du programme
[WARNING] main.c:16 - Mémoire faible
[ERROR] main.c:17 - Fichier introuvable
```

### 2. Génération de code répétitif

```c
#include <stdio.h>

// Macro pour créer des fonctions de conversion
#define DEFINE_CONVERTER(from, to) \
    double from##_to_##to(double value) { \
        printf("Conversion " #from " -> " #to ": %.2f\n", value); \
        return value; /* Conversion simplifiée */ \
    }

DEFINE_CONVERTER(celsius, fahrenheit)  
DEFINE_CONVERTER(metres, kilometres)  
DEFINE_CONVERTER(euros, dollars)  

int main(void) {
    celsius_to_fahrenheit(25.0);
    metres_to_kilometres(1500.0);
    euros_to_dollars(100.0);

    return 0;
}
```

### 3. Interface de configuration

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    int port;
    char host[100];
    int timeout;
} Config;

// Macro pour générer des setters
#define CONFIG_SETTER(field, type) \
    void config_set_##field(Config* cfg, type value) { \
        cfg->field = value; \
        printf("Configuration mise à jour: " #field " = %d\n", value); \
    }

CONFIG_SETTER(port, int)  
CONFIG_SETTER(timeout, int)  

int main(void) {
    Config cfg = {0};

    config_set_port(&cfg, 8080);
    config_set_timeout(&cfg, 5000);

    printf("\nConfiguration finale:\n");
    printf("Port: %d\n", cfg.port);
    printf("Timeout: %d\n", cfg.timeout);

    return 0;
}
```

### 4. Macro d'énumération avec chaînes

Une technique très utile pour associer automatiquement des noms aux valeurs d'enum :

```c
#include <stdio.h>

// Liste des couleurs
#define COLOR_LIST \
    X(ROUGE) \
    X(VERT) \
    X(BLEU) \
    X(JAUNE)

// Génération de l'enum
#define X(name) COLOR_##name,
typedef enum {
    COLOR_LIST
    COLOR_COUNT
} Color;
#undef X

// Génération du tableau de chaînes
#define X(name) #name,
const char* color_names[] = {
    COLOR_LIST
};
#undef X

int main(void) {
    Color ma_couleur = COLOR_VERT;

    printf("Couleur sélectionnée: %s\n", color_names[ma_couleur]);

    printf("\nToutes les couleurs:\n");
    for (int i = 0; i < COLOR_COUNT; i++) {
        printf("  %d: %s\n", i, color_names[i]);
    }

    return 0;
}
```

**Sortie** :
```
Couleur sélectionnée: VERT

Toutes les couleurs:
  0: ROUGE
  1: VERT
  2: BLEU
  3: JAUNE
```

---

## Règles et limitations

### Règles de la stringification (#)

1. **Ne peut être utilisé que dans une macro** :
   ```c
   // ❌ Erreur : en dehors d'une macro
   const char* str = #variable;
   ```

2. **Espaces multiples sont réduits à un seul** :
   ```c
   #define STRINGIFY(x) #x
   STRINGIFY(a    b    c)  // Devient "a b c"
   ```

3. **Les caractères spéciaux sont échappés** :
   ```c
   #define STRINGIFY(x) #x
   STRINGIFY("hello")  // Devient "\"hello\""
   ```

### Règles de la concaténation (##)

1. **Ne peut être utilisé que dans une macro** :
   ```c
   // ❌ Erreur : en dehors d'une macro
   int x##y = 10;
   ```

2. **Le résultat doit être un token valide** :
   ```c
   #define CONCAT(a, b) a##b
   CONCAT(1, e5)  // OK : devient 1e5 (notation scientifique)
   CONCAT(+, +)   // OK : devient ++
   CONCAT(/, *)   // ❌ Erreur : /* n'est pas un token valide seul
   ```

3. **Les espaces autour de ## sont ignorés** :
   ```c
   #define CONCAT(a, b) a ## b  // ✅ Équivalent
   #define CONCAT(a, b) a##b    // ✅ Équivalent (style plus courant)
   ```

---

## Pièges courants et solutions

### Piège 1 : Oublier la double expansion

**Problème** :
```c
#define VALUE 100
#define STRINGIFY(x) #x

// STRINGIFY(VALUE) → "VALUE" (pas "100")
```

**Solution** :
```c
#define VALUE 100
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

// STRINGIFY(VALUE) → STRINGIFY_HELPER(100) → "100" ✓
```

### Piège 2 : Concaténation produisant un token invalide

**Problème** :
```c
#define CONCAT(a, b) a##b
CONCAT(/, /)  // Erreur : // n'est pas un token, c'est un commentaire !
```

**Solution** : Vérifier que le résultat est un token C valide.

### Piège 3 : Utilisation avec des types

**Problème** :
```c
#define MAKE_POINTER(type) type##*
MAKE_POINTER(int) ptr;  // Erreur : int* n'est pas un preprocessing token valide
// ## exige que le résultat de la concaténation soit un token unique valide
```

**Solution** : Ne pas utiliser `##` quand ce n'est pas nécessaire :
```c
#define MAKE_POINTER(type) type *
MAKE_POINTER(int) ptr;  // OK : devient int * ptr (pas de concaténation)
```

---

## Debugging de macros

Pour voir le résultat du préprocesseur :

```bash
# Avec GCC
gcc -E fichier.c

# Sauvegarder dans un fichier
gcc -E fichier.c > fichier_preprocessed.c

# Avec options de formatage
gcc -E -P fichier.c  # -P supprime les directives de ligne
```

### Exemple de debug

**Fichier source** :
```c
#define STRINGIFY(x) #x
#define CONCAT(a, b) a##b

int main(void) {
    int xy = CONCAT(x, y);
    printf(STRINGIFY(Hello));
    return 0;
}
```

**Après préprocessing** :
```c
int main(void) {
    int xy = xy;
    printf("Hello");
    return 0;
}
```

---

## Bonnes pratiques

### 1. Documenter les macros complexes

```c
/**
 * @brief Génère une fonction getter pour un champ
 * @param type Type du champ
 * @param name Nom du champ
 *
 * Exemple: DEFINE_GETTER(int, age)
 * Génère: int get_age(void) { return age; }
 */
#define DEFINE_GETTER(type, name) \
    type get_##name(void) { \
        return name; \
    }
```

### 2. Utiliser des noms explicites

```c
// ✅ Bon : noms clairs
#define STRINGIFY(x) #x
#define CONCAT(a, b) a##b

// ❌ Mauvais : noms cryptiques
#define S(x) #x
#define C(a, b) a##b
```

### 3. Prévoir la double expansion si nécessaire

```c
// Toujours fournir une version avec expansion
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#define CONCAT_HELPER(a, b) a##b
#define CONCAT(a, b) CONCAT_HELPER(a, b)
```

### 4. Tester avec différents arguments

```c
#include <stdio.h>

#define TEST_STRINGIFY(x) printf("STRINGIFY(" #x ") = \"%s\"\n", #x)

int main(void) {
    TEST_STRINGIFY(123);
    TEST_STRINGIFY(abc);
    TEST_STRINGIFY(x + y);
    TEST_STRINGIFY("string");

    return 0;
}
```

---

## Exemple complet : Framework de tests

Voici un exemple complet qui combine # et ## pour créer un mini-framework de tests :

```c
#include <stdio.h>
#include <string.h>

// Compteurs de tests
static int tests_total = 0;  
static int tests_reussis = 0;  

// Macro de test qui génère automatiquement le nom de fonction
#define TEST(name) \
    void test_##name(void); \
    void run_test_##name(void) { \
        printf("\n▶ Test: " #name "\n"); \
        tests_total++; \
        test_##name(); \
    } \
    void test_##name(void)

// Macros d'assertion
#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            printf("  ✓ " #actual " == " #expected "\n"); \
            tests_reussis++; \
        } else { \
            printf("  ✗ " #actual " (%d) != " #expected " (%d)\n", \
                   (actual), (expected)); \
        } \
    } while(0)

#define ASSERT_STRING_EQUAL(actual, expected) \
    do { \
        if (strcmp((actual), (expected)) == 0) { \
            printf("  ✓ " #actual " == " #expected "\n"); \
            tests_reussis++; \
        } else { \
            printf("  ✗ " #actual " (\"%s\") != " #expected " (\"%s\")\n", \
                   (actual), (expected)); \
        } \
    } while(0)

// Définition des tests
TEST(addition) {
    int resultat = 2 + 3;
    ASSERT_EQUAL(resultat, 5);
}

TEST(multiplication) {
    int resultat = 4 * 5;
    ASSERT_EQUAL(resultat, 20);
}

TEST(concatenation) {
    char buffer[100];
    strcpy(buffer, "Hello");
    strcat(buffer, " World");
    ASSERT_STRING_EQUAL(buffer, "Hello World");
}

TEST(division) {
    int resultat = 20 / 4;
    ASSERT_EQUAL(resultat, 5);
}

// Fonction principale
int main(void) {
    printf("=== Début des tests ===\n");

    run_test_addition();
    run_test_multiplication();
    run_test_concatenation();
    run_test_division();

    printf("\n=== Résultats ===\n");
    printf("Tests réussis: %d/%d\n", tests_reussis, tests_total);

    return (tests_reussis == tests_total) ? 0 : 1;
}
```

**Sortie** :
```
=== Début des tests ===

▶ Test: addition
  ✓ resultat == 5

▶ Test: multiplication
  ✓ resultat == 20

▶ Test: concatenation
  ✓ buffer == "Hello World"

▶ Test: division
  ✓ resultat == 5

=== Résultats ===
Tests réussis: 4/4
```

---

## Résumé

### Opérateur # (Stringification)

✅ **Utilité** :
- Transforme un argument de macro en chaîne de caractères
- Idéal pour le logging et le debugging
- Permet d'afficher les noms de variables automatiquement

⚠️ **Points d'attention** :
- Ne fonctionne que dans les macros
- Nécessite une double expansion pour expanser d'autres macros
- Échappe automatiquement les caractères spéciaux

### Opérateur ## (Concaténation)

✅ **Utilité** :
- Colle deux tokens ensemble
- Génère des identificateurs dynamiquement
- Automatise la création de code répétitif

⚠️ **Points d'attention** :
- Ne fonctionne que dans les macros
- Le résultat doit être un token valide
- Nécessite une double expansion pour expanser d'autres macros

### Quand les utiliser ?

**Utilisations appropriées** :
- Frameworks de tests et debugging
- Génération automatique de getters/setters
- Logging avec métadonnées
- Réduction de code répétitif (DRY)

**À éviter** :
- Macros trop complexes difficiles à comprendre
- Cas où une fonction normale suffit
- Sur-ingénierie avec des macros imbriquées

**Règle d'or** : Si une macro devient difficile à lire ou à maintenir, envisagez une approche alternative (fonctions, structures de données, etc.).

---

**Prochaine section** : 23.3 Compilation conditionnelle (directives #if, #ifdef, #ifndef)

⏭️ [Compilation conditionnelle](/23-macros-preprocesseur/03-compilation-conditionnelle.md)
