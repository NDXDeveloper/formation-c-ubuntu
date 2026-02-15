🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 22.6 va_list et macros variadiques

## Introduction

Dans le tutoriel précédent, nous avons découvert les fonctions variadiques avec `va_list`, `va_start()`, `va_arg()` et `va_end()`. Dans ce chapitre, nous allons approfondir ces concepts et découvrir une fonctionnalité complémentaire : **les macros variadiques**, introduites en C99.

### Deux mécanismes différents

Il est important de distinguer deux concepts :

1. **Fonctions variadiques** : Fonctions qui acceptent un nombre variable d'arguments à l'**exécution**
   - Utilisent `va_list` et les macros associées
   - Traitement dynamique au runtime

2. **Macros variadiques** : Macros du préprocesseur qui acceptent un nombre variable d'arguments à la **compilation**
   - Utilisent `__VA_ARGS__`
   - Expansion textuelle avant compilation

**Analogie** :
- **Fonction variadique** : Un restaurant avec un buffet où vous choisissez vos plats au moment du repas
- **Macro variadique** : Une recette de cuisine qui s'adapte selon le nombre d'ingrédients que vous notez avant de commencer

---

## Partie 1 : Approfondissement de va_list

### Anatomie de va_list

`va_list` est un type **opaque** défini dans `<stdarg.h>`. Son implémentation exacte dépend du système, mais conceptuellement, c'est une structure qui garde trace de :
- La position actuelle dans la liste d'arguments
- Les informations nécessaires pour récupérer le prochain argument

```c
#include <stdarg.h>

// va_list est un type opaque
// Implémentation typique (simplifiée, ne pas utiliser directement) :
// typedef struct {
//     unsigned int gp_offset;
//     unsigned int fp_offset;
//     void *overflow_arg_area;
//     void *reg_save_area;
// } va_list[1];
```

**Important** : Ne jamais accéder directement aux champs internes de `va_list`. Utilisez toujours les macros fournies.

### va_copy() en détail

La macro `va_copy()` permet de dupliquer une `va_list` pour la parcourir plusieurs fois.

```c
#include <stdio.h>
#include <stdarg.h>

void traiter_deux_fois(int count, ...) {
    va_list args_original;
    va_list args_copie;

    va_start(args_original, count);
    va_copy(args_copie, args_original);  // Copie indépendante

    // Premier parcours
    printf("Premier parcours : ");
    for (int i = 0; i < count; i++) {
        printf("%d ", va_arg(args_original, int));
    }
    printf("\n");

    // Deuxième parcours (avec la copie)
    printf("Deuxième parcours : ");
    for (int i = 0; i < count; i++) {
        printf("%d ", va_arg(args_copie, int));
    }
    printf("\n");

    va_end(args_original);
    va_end(args_copie);  // Important : nettoyer les deux !
}

int main(void) {
    traiter_deux_fois(4, 10, 20, 30, 40);
    return 0;
}
```

**Sortie** :
```
Premier parcours : 10 20 30 40  
Deuxième parcours : 10 20 30 40  
```

**Points clés** :
- `va_copy()` crée une copie **indépendante**
- Chaque `va_list` (original et copie) doit avoir son propre `va_end()`
- Introduit en C99, portable depuis

### Passer va_list à d'autres fonctions

Une technique courante est de créer des fonctions auxiliaires qui acceptent directement une `va_list` :

```c
#include <stdio.h>
#include <stdarg.h>

// Fonction auxiliaire qui prend une va_list
int somme_va(int count, va_list args) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(args, int);
    }
    return total;
}

// Fonction principale variadique
int somme(int count, ...) {
    va_list args;
    va_start(args, count);

    int resultat = somme_va(count, args);

    va_end(args);
    return resultat;
}

// Autre fonction qui réutilise la logique
int somme_double(int count, ...) {
    va_list args;
    va_start(args, count);

    int resultat = somme_va(count, args) * 2;

    va_end(args);
    return resultat;
}

int main(void) {
    printf("Somme : %d\n", somme(3, 10, 20, 30));
    printf("Somme doublée : %d\n", somme_double(3, 10, 20, 30));
    return 0;
}
```

**Sortie** :
```
Somme : 60  
Somme doublée : 120  
```

**Avantage** : Réutilisation du code de traitement.

### Wrapper de printf avec préfixe

Un exemple classique d'utilisation de `va_list` avec les fonctions `v*printf` :

```c
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void log_avec_timestamp(const char *format, ...) {
    // Obtenir l'heure actuelle
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Afficher le timestamp
    printf("[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);

    // Déléguer le formatage à vprintf
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

int main(void) {
    log_avec_timestamp("Application démarrée");
    log_avec_timestamp("Utilisateur %s connecté", "admin");
    log_avec_timestamp("Traitement de %d fichiers", 42);

    return 0;
}
```

**Sortie** (exemple) :
```
[14:23:45] Application démarrée
[14:23:45] Utilisateur admin connecté
[14:23:45] Traitement de 42 fichiers
```

### Fonctions v* de la bibliothèque standard

Voici un tableau complet des fonctions qui acceptent `va_list` :

| Fonction normale | Variante va_list | Header | Description |
|-----------------|------------------|--------|-------------|
| `printf()` | `vprintf()` | `<stdio.h>` | Sortie vers stdout |
| `fprintf()` | `vfprintf()` | `<stdio.h>` | Sortie vers fichier |
| `sprintf()` | `vsprintf()` | `<stdio.h>` | Sortie vers buffer (non sûr) |
| `snprintf()` | `vsnprintf()` | `<stdio.h>` | Sortie vers buffer (sûr) |
| `scanf()` | `vscanf()` | `<stdio.h>` | Lecture depuis stdin |
| `fscanf()` | `vfscanf()` | `<stdio.h>` | Lecture depuis fichier |
| `sscanf()` | `vsscanf()` | `<stdio.h>` | Lecture depuis string |
| `syslog()` | `vsyslog()` | `<syslog.h>` | Logging système |

### Exemple : Formatage dans une chaîne

```c
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char* formater_message(char *buffer, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, size, format, args);

    va_end(args);
    return buffer;
}

int main(void) {
    char msg[100];

    formater_message(msg, sizeof(msg), "Erreur %d : %s", 404, "Not Found");
    printf("%s\n", msg);

    formater_message(msg, sizeof(msg), "Utilisateur: %s, ID: %d", "Alice", 1001);
    printf("%s\n", msg);

    return 0;
}
```

**Sortie** :
```
Erreur 404 : Not Found  
Utilisateur: Alice, ID: 1001  
```

---

## Partie 2 : Macros variadiques

### Introduction aux macros variadiques

Les **macros variadiques** sont une fonctionnalité du préprocesseur C introduite en **C99**. Elles permettent de créer des macros qui acceptent un nombre variable d'arguments.

**Syntaxe de base** :
```c
#define NOM_MACRO(arg_fixe, ...) /* corps de la macro */
```

Le mot-clé spécial `__VA_ARGS__` représente tous les arguments variables.

### Premier exemple simple

```c
#include <stdio.h>

// Macro variadique simple
#define AFFICHER(...) printf(__VA_ARGS__)

int main(void) {
    AFFICHER("Bonjour\n");
    AFFICHER("x = %d\n", 42);
    AFFICHER("x = %d, y = %d\n", 10, 20);

    return 0;
}
```

**Sortie** :
```
Bonjour  
x = 42  
x = 10, y = 20  
```

**Explication** :
- `AFFICHER("Bonjour\n")` → `printf("Bonjour\n")`
- `AFFICHER("x = %d\n", 42)` → `printf("x = %d\n", 42)`

### Macro de debug classique

Un cas d'usage très courant :

```c
#include <stdio.h>

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) /* rien en mode release */
#endif

int main(void) {
    int x = 42;

    DEBUG_PRINT("Application démarrée");
    DEBUG_PRINT("Valeur de x : %d", x);
    DEBUG_PRINT("Calcul : %d + %d = %d", 10, 20, 10+20);

    return 0;
}
```

**Compilation en mode debug** :
```bash
gcc -DDEBUG programme.c -o programme
```

**Sortie** :
```
[DEBUG] programme.c:10: Application démarrée
[DEBUG] programme.c:11: Valeur de x : 42
[DEBUG] programme.c:12: Calcul : 10 + 20 = 30
```

**Sans `-DDEBUG`**, les macros ne produisent aucun code !

### L'opérateur ## (token pasting)

L'opérateur `##` avant `__VA_ARGS__` résout un problème : que se passe-t-il si aucun argument variable n'est passé ?

```c
#include <stdio.h>

// Sans ## : problème si pas d'arguments variables
#define LOG1(fmt, ...) printf(fmt, __VA_ARGS__)

// Avec ## : fonctionne même sans arguments variables
#define LOG2(fmt, ...) printf(fmt, ##__VA_ARGS__)

int main(void) {
    // LOG1("Bonjour\n");     // ❌ ERREUR : printf("Bonjour\n", )
    LOG2("Bonjour\n");        // ✅ OK : printf("Bonjour\n")
    LOG2("x = %d\n", 42);     // ✅ OK : printf("x = %d\n", 42)

    return 0;
}
```

**Explication** :
- `##__VA_ARGS__` supprime la virgule précédente si `__VA_ARGS__` est vide
- Extension GNU, mais largement supportée
- Standard C++20 a introduit `__VA_OPT__` comme alternative

### Macro de logging par niveau

```c
#include <stdio.h>
#include <time.h>

#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

#define LOG(level, fmt, ...) do { \
    time_t now = time(NULL); \
    struct tm *t = localtime(&now); \
    const char *level_str[] = {"", "INFO", "WARN", "ERROR"}; \
    printf("[%02d:%02d:%02d] [%5s] " fmt "\n", \
           t->tm_hour, t->tm_min, t->tm_sec, \
           level_str[level], ##__VA_ARGS__); \
} while(0)

#define LOG_INFO(...)  LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...)  LOG(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)

int main(void) {
    LOG_INFO("Application démarrée");
    LOG_WARN("Mémoire faible : %d%% utilisé", 85);
    LOG_ERROR("Échec de connexion à %s:%d", "192.168.1.1", 8080);

    return 0;
}
```

**Sortie** :
```
[14:30:15] [ INFO] Application démarrée
[14:30:15] [ WARN] Mémoire faible : 85% utilisé
[14:30:15] [ERROR] Échec de connexion à 192.168.1.1:8080
```

### Le pattern do { ... } while(0)

Pourquoi utiliser `do { ... } while(0)` dans les macros ?

```c
#include <stdio.h>

// ❌ Sans do-while : problème avec if-else
#define BAD_LOG(msg) printf(msg); printf("\n")

// ✅ Avec do-while : se comporte comme une instruction
#define GOOD_LOG(msg) do { printf(msg); printf("\n"); } while(0)

int main(void) {
    int condition = 1;

    // Problème avec BAD_LOG
    if (condition)
        BAD_LOG("test");  // Expande en deux instructions !
    else
        printf("else\n"); // ❌ Erreur de syntaxe !

    // OK avec GOOD_LOG
    if (condition)
        GOOD_LOG("test"); // ✅ Une seule instruction
    else
        printf("else\n");

    return 0;
}
```

**Règle** : Toujours entourer les macros multi-instructions avec `do { ... } while(0)`.

---

## Combinaison : Fonctions et macros variadiques

### Macro qui appelle une fonction variadique

```c
#include <stdio.h>
#include <stdarg.h>

// Fonction variadique
void log_impl(const char *file, int line, const char *format, ...) {
    printf("[%s:%d] ", file, line);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

// Macro variadique qui ajoute automatiquement __FILE__ et __LINE__
#define LOG(fmt, ...) log_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

int main(void) {
    LOG("Application démarrée");
    LOG("Valeur : %d", 42);

    return 0;
}
```

**Sortie** :
```
[programme.c:20] Application démarrée
[programme.c:21] Valeur : 42
```

**Avantage** : La macro ajoute automatiquement les informations de contexte.

### Système d'assertions personnalisé

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void assert_impl(int condition, const char *file, int line,
                 const char *expr, const char *format, ...) {
    if (!condition) {
        fprintf(stderr, "Assertion échouée : %s\n", expr);
        fprintf(stderr, "  Fichier : %s:%d\n", file, line);

        if (format != NULL && format[0] != '\0') {
            fprintf(stderr, "  Message : ");
            va_list args;
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
            fprintf(stderr, "\n");
        }

        abort();
    }
}

#define ASSERT(expr, ...) \
    assert_impl(!!(expr), __FILE__, __LINE__, #expr, "" __VA_ARGS__)

int main(void) {
    int x = 10;

    ASSERT(x > 0);  // OK
    ASSERT(x < 20, "x devrait être inférieur à 20, mais vaut %d", x);  // OK
    ASSERT(x > 100, "Erreur : x = %d est trop petit", x);  // ❌ Échec

    return 0;
}
```

**Sortie** (avant abort) :
```
Assertion échouée : x > 100
  Fichier : programme.c:27
  Message : Erreur : x = 10 est trop petit
Aborted
```

---

## Exemples pratiques avancés

### Macro de mesure de performance

```c
#include <stdio.h>
#include <time.h>

#define MEASURE_TIME(name, ...) do { \
    clock_t start = clock(); \
    __VA_ARGS__; \
    clock_t end = clock(); \
    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC; \
    printf("[PERF] %s : %.6f secondes\n", name, cpu_time); \
} while(0)

void calcul_lourd(void) {
    volatile long sum = 0;
    for (long i = 0; i < 10000000; i++) {
        sum += i;
    }
}

int main(void) {
    MEASURE_TIME("Calcul lourd", calcul_lourd());

    MEASURE_TIME("Boucle simple", {
        volatile int x = 0;
        for (int i = 0; i < 1000000; i++) {
            x += i;
        }
    });

    return 0;
}
```

**Sortie** (exemple) :
```
[PERF] Calcul lourd : 0.025467 secondes
[PERF] Boucle simple : 0.002134 secondes
```

### Macro pour tester plusieurs conditions

```c
#include <stdio.h>

#define CHECK_ALL(var, ...) ({ \
    int conditions[] = {__VA_ARGS__}; \
    int all_ok = 1; \
    for (size_t i = 0; i < sizeof(conditions)/sizeof(conditions[0]); i++) { \
        if (!conditions[i]) { \
            all_ok = 0; \
            break; \
        } \
    } \
    all_ok; \
})

int main(void) {
    int x = 50;

    if (CHECK_ALL(x, x > 0, x < 100, x % 2 == 0)) {
        printf("Toutes les conditions sont vraies\n");
    }

    return 0;
}
```

**Note** : Utilise des extensions GNU (`({...})` statement expressions pour créer une expression).

### Macro de formatage conditionnel

```c
#include <stdio.h>

#define PRINT_IF(condition, fmt, ...) do { \
    if (condition) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
} while(0)

int main(void) {
    int debug_mode = 1;
    int verbose_mode = 0;

    PRINT_IF(debug_mode, "Mode debug activé\n");
    PRINT_IF(verbose_mode, "Mode verbose activé\n");
    PRINT_IF(debug_mode, "Valeur : %d\n", 42);

    return 0;
}
```

**Sortie** :
```
Mode debug activé  
Valeur : 42  
```

---

## Compter le nombre d'arguments (techniques avancées)

### Macro pour compter les arguments (jusqu'à 10)

```c
#include <stdio.h>

// Astuce du préprocesseur pour compter les arguments
#define COUNT_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1)

#define PRINT_COUNT(...) \
    printf("Nombre d'arguments : %d\n", COUNT_ARGS(__VA_ARGS__))

int main(void) {
    PRINT_COUNT(1);
    PRINT_COUNT(1, 2);
    PRINT_COUNT(1, 2, 3, 4, 5);

    return 0;
}
```

**Sortie** :
```
Nombre d'arguments : 1  
Nombre d'arguments : 2  
Nombre d'arguments : 5  
```

**Comment ça marche ?** :
- `COUNT_ARGS(a, b, c)` → `COUNT_ARGS_IMPL(a, b, c, 10,9,8,7,6,5,4,3,2,1)`
- Les arguments décalent les nombres, et `N` prend la valeur correspondante

### Appliquer une opération à chaque argument

```c
#include <stdio.h>

// Macro pour appliquer printf à chaque argument
#define PRINT_EACH_1(x) printf("%d ", x);
#define PRINT_EACH_2(x, ...) printf("%d ", x); PRINT_EACH_1(__VA_ARGS__)
#define PRINT_EACH_3(x, ...) printf("%d ", x); PRINT_EACH_2(__VA_ARGS__)
#define PRINT_EACH_4(x, ...) printf("%d ", x); PRINT_EACH_3(__VA_ARGS__)
#define PRINT_EACH_5(x, ...) printf("%d ", x); PRINT_EACH_4(__VA_ARGS__)

// Sélecteur basé sur le nombre d'arguments
#define GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define PRINT_ALL(...) \
    GET_MACRO(__VA_ARGS__, PRINT_EACH_5, PRINT_EACH_4, PRINT_EACH_3, \
              PRINT_EACH_2, PRINT_EACH_1)(__VA_ARGS__)

int main(void) {
    PRINT_ALL(10);
    printf("\n");

    PRINT_ALL(10, 20, 30);
    printf("\n");

    PRINT_ALL(1, 2, 3, 4, 5);
    printf("\n");

    return 0;
}
```

**Sortie** :
```
10
10 20 30
1 2 3 4 5
```

---

## Pièges et limitations

### 1. Limite du nombre d'arguments

```c
// Beaucoup de préprocesseurs ont des limites
// (typiquement 127 ou 255 arguments)

// ⚠️ Peut échouer avec trop d'arguments
MACRO(arg1, arg2, ..., arg200);
```

### 2. Macros et effets de bord

```c
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    int x = 15;

    // ❌ PROBLÈME : x++ évalué deux fois !
    int m = MAX(x++, 10);
    printf("x = %d, m = %d\n", x, m);  // x = 17 (incrémenté 2 fois !), m = 16

    return 0;
}
```

**Solution** : Utiliser des fonctions inline au lieu de macros pour du code complexe.

### 3. Types et macros variadiques

```c
// ❌ Les macros ne connaissent pas les types
#define SUM(...) /* comment additionner des types différents ? */

// Les fonctions variadiques ont le même problème,
// mais peuvent au moins vérifier à l'exécution
```

### 4. Debugging difficile

```c
// Le code expansé peut être difficile à débugger
#define COMPLEX_MACRO(...) /* beaucoup de code */

// Utilisez gcc -E pour voir l'expansion
```

**Commande utile** :
```bash
gcc -E programme.c  # Affiche le code après préprocesseur
```

### 5. Compatibilité C89

```c
// ❌ Les macros variadiques nécessitent C99+
#define LOG(...)  // Erreur en mode C89

// Pour C89, utiliser des alternatives comme :
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 199901L
        #define LOG(...) printf(__VA_ARGS__)
    #else
        // Fallback pour C89
        #define LOG printf
    #endif
#endif
```

---

## Bonnes pratiques

### ✅ À FAIRE

1. **Utiliser do-while pour les macros multi-lignes**
```c
#define LOG(msg) do { \
    printf("[LOG] %s\n", msg); \
    fflush(stdout); \
} while(0)
```

2. **Utiliser ## pour les arguments optionnels**
```c
#define DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
```

3. **Documenter les macros variadiques**
```c
/**
 * LOG - Affiche un message de log
 * @fmt: Format string (comme printf)
 * @...: Arguments variables
 */
#define LOG(fmt, ...) /* ... */
```

4. **Préfixer les macros pour éviter les conflits**
```c
#define MYLIB_LOG(...)  // Au lieu de LOG(...)
#define MYLIB_ASSERT(...) // Au lieu de ASSERT(...)
```

5. **Combiner avec des fonctions pour la logique complexe**
```c
void log_impl(const char *file, int line, const char *fmt, ...);

#define LOG(fmt, ...) log_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
```

### ❌ À ÉVITER

1. **Logique trop complexe dans les macros**
```c
// ❌ Trop complexe
#define PROCESS(...) /* 50 lignes de code */

// ✅ Déléguer à une fonction
void process_impl(int count, ...);
#define PROCESS(...) process_impl(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
```

2. **Noms de macros en minuscules**
```c
// ❌ Confusion avec les fonctions
#define debug(...) printf(__VA_ARGS__)

// ✅ Convention : majuscules pour les macros
#define DEBUG(...) printf(__VA_ARGS__)
```

3. **Oublier les parenthèses**
```c
// ❌ Problème de précédence
#define SQUARE(x) x * x

// ✅ Toujours parenthéser
#define SQUARE(x) ((x) * (x))
```

---

## Cas d'usage recommandés

### ✅ Macros variadiques pour :

1. **Logging et debugging**
```c
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
```

2. **Assertions personnalisées**
```c
#define ASSERT(condition, ...)
```

3. **Instrumentation de code**
```c
#define TRACE_ENTER(func, ...)
#define TRACE_EXIT(func, ...)
```

4. **Génération de code répétitif**
```c
#define DEFINE_GETTER(type, name) /* ... */
```

### ✅ Fonctions variadiques pour :

1. **Logique complexe à l'exécution**
2. **Traitement de différents types**
3. **API de formatage (printf-like)**

### ❌ Éviter les deux pour :

1. **Code critique en performance**
   - Utiliser des fonctions spécialisées

2. **APIs publiques complexes**
   - Préférer des structures de configuration

---

## Comparaison récapitulative

| Aspect | Fonctions variadiques | Macros variadiques |
|--------|----------------------|-------------------|
| **Quand** | Runtime | Compile-time |
| **Syntaxe** | `type func(int n, ...)` | `#define M(...) code` |
| **Arguments** | `va_list`, `va_arg()` | `__VA_ARGS__` |
| **Type safety** | Non | Non |
| **Debugging** | Difficile | Très difficile |
| **Overhead** | Appel de fonction | Aucun (expansion) |
| **Flexibilité** | Logique dynamique | Génération de code |
| **Usage typique** | printf, logging | DEBUG, assertions |

---

## Exemple complet : Système de logging avancé

```c
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// Niveaux de log
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3
} LogLevel;

// Niveau global
static LogLevel g_log_level = LOG_LEVEL_INFO;

// Fonction variadique d'implémentation
void log_impl(LogLevel level, const char *file, int line,
              const char *func, const char *fmt, ...) {
    if (level < g_log_level) {
        return;  // Filtrage par niveau
    }

    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("[%02d:%02d:%02d] [%5s] %s:%d (%s): ",
           t->tm_hour, t->tm_min, t->tm_sec,
           level_str[level], file, line, func);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

// Macros variadiques pour chaque niveau
#define LOG_DEBUG(fmt, ...) \
    log_impl(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    log_impl(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    log_impl(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    log_impl(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

void traiter_fichier(const char *nom) {
    LOG_DEBUG("Début du traitement de %s", nom);

    if (nom == NULL) {
        LOG_ERROR("Nom de fichier NULL !");
        return;
    }

    LOG_INFO("Traitement de %s en cours", nom);
    LOG_WARN("Fichier volumineux : %s", nom);
}

int main(void) {
    LOG_INFO("Application démarrée");

    traiter_fichier("document.txt");
    traiter_fichier(NULL);

    LOG_INFO("Application terminée");

    return 0;
}
```

**Sortie** :
```
[15:30:45] [ INFO] exemple.c:58 (main): Application démarrée
[15:30:45] [ INFO] exemple.c:51 (traiter_fichier): Traitement de document.txt en cours
[15:30:45] [ WARN] exemple.c:52 (traiter_fichier): Fichier volumineux : document.txt
[15:30:45] [ERROR] exemple.c:47 (traiter_fichier): Nom de fichier NULL !
[15:30:45] [ INFO] exemple.c:62 (main): Application terminée
```

---

## Résumé

### va_list (fonctions variadiques)

**Composants** :
- `va_list` : Type opaque pour parcourir les arguments
- `va_start()` : Initialisation
- `va_arg()` : Récupération d'un argument
- `va_end()` : Nettoyage
- `va_copy()` : Copie pour parcours multiple

**Usage** :
```c
void fonction(int n, ...) {
    va_list args;
    va_start(args, n);
    int x = va_arg(args, int);
    va_end(args);
}
```

### Macros variadiques

**Composants** :
- `...` : Arguments variables dans la définition
- `__VA_ARGS__` : Expansion des arguments
- `##__VA_ARGS__` : Suppression virgule si vide

**Usage** :
```c
#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
```

### Quand utiliser quoi ?

| Besoin | Solution |
|--------|----------|
| Logique runtime complexe | Fonction variadique |
| Debug/logging simple | Macro variadique |
| Informations de compilation | Macro (`__FILE__`, `__LINE__`) |
| Formatage de chaînes | Fonction + vprintf |
| Code répétitif | Macro |
| Performance critique | Ni l'un ni l'autre |

**Combinaison optimale** : Macro qui appelle une fonction variadique !
```c
void log_impl(const char *file, int line, const char *fmt, ...);
#define LOG(fmt, ...) log_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
```

Les macros et fonctions variadiques sont des outils puissants pour créer des APIs flexibles et des systèmes de logging robustes. Utilisez-les avec discernement en comprenant leurs limites !

---


⏭️ [Macros et Préprocesseur avancé](/23-macros-preprocesseur/README.md)
