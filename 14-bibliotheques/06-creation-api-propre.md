🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 14.6 Création d'une API propre

## Introduction

Lorsque vous créez une bibliothèque en C, vous créez en réalité deux choses distinctes :
1. **L'implémentation** : Le code qui fait fonctionner votre bibliothèque (fichiers `.c`)
2. **L'API (Application Programming Interface)** : L'interface publique que les utilisateurs vont utiliser (fichiers `.h`)

Une **API propre** est une interface bien conçue, facile à utiliser, difficile à mal utiliser, et stable dans le temps. C'est la différence entre une bibliothèque que tout le monde veut utiliser et une bibliothèque que personne ne comprend.

Dans cette section, nous allons apprendre à concevoir des APIs de qualité professionnelle en C.

---

## Qu'est-ce qu'une API propre ?

### Définition

Une **API propre** possède ces caractéristiques :

- ✅ **Claire** : Facile à comprendre sans lire la documentation
- ✅ **Cohérente** : Utilise les mêmes conventions partout
- ✅ **Complète** : Fournit toutes les fonctionnalités nécessaires
- ✅ **Minimaliste** : N'expose que ce qui est strictement nécessaire
- ✅ **Sûre** : Difficile à mal utiliser
- ✅ **Stable** : Les changements ne cassent pas le code existant
- ✅ **Documentée** : Chaque fonction est expliquée clairement

---

### Exemple de mauvaise vs bonne API

#### ❌ Mauvaise API

```c
// bad_api.h
int process(char *d, int t, void *p);  // Noms cryptiques
void cleanup();                         // Nom générique
extern int global_state;               // État global exposé
#define MAX 100                        // Nom trop générique
```

**Problèmes** :
- Noms de fonctions et paramètres non explicites
- Pas de préfixe de bibliothèque (risque de collision)
- État interne exposé
- Noms trop génériques

---

#### ✅ Bonne API

```c
// mylib.h
#ifndef MYLIB_H
#define MYLIB_H

#include <stddef.h>
#include <stdbool.h>

// Types opaques (implémentation cachée)
typedef struct mylib_context mylib_context_t;

// Constantes avec préfixe
#define MYLIB_MAX_BUFFER_SIZE 100
#define MYLIB_VERSION "1.0.0"

// Codes d'erreur
typedef enum {
    MYLIB_SUCCESS = 0,
    MYLIB_ERROR_NULL_POINTER = -1,
    MYLIB_ERROR_INVALID_ARGUMENT = -2,
    MYLIB_ERROR_OUT_OF_MEMORY = -3
} mylib_error_t;

// Fonctions avec noms explicites et préfixe
mylib_error_t mylib_init(mylib_context_t **ctx);
mylib_error_t mylib_process_data(mylib_context_t *ctx,
                                  const char *data,
                                  size_t data_len);
void mylib_cleanup(mylib_context_t **ctx);

#endif // MYLIB_H
```

**Avantages** :
- Noms clairs et explicites
- Préfixe `mylib_` partout (pas de collision)
- Type opaque (encapsulation)
- Codes d'erreur explicites
- Documentation intégrée possible

---

## Principe 1 : Séparation Interface / Implémentation

### Le concept

En C, la **séparation interface/implémentation** se fait via deux fichiers :

```
mylib.h   → Interface publique (ce que l'utilisateur voit)
mylib.c   → Implémentation (les détails internes)
```

---

### Exemple : Gestionnaire de file (Queue)

#### Interface publique (queue.h)

```c
// queue.h - Interface publique
#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>

// Type opaque - l'utilisateur ne voit pas la structure interne
typedef struct queue queue_t;

// Création et destruction
queue_t* queue_create(void);
void queue_destroy(queue_t *q);

// Opérations
bool queue_push(queue_t *q, int value);
bool queue_pop(queue_t *q, int *value);
bool queue_peek(const queue_t *q, int *value);
size_t queue_size(const queue_t *q);
bool queue_is_empty(const queue_t *q);

#endif // QUEUE_H
```

✅ **Avantages** :
- L'utilisateur n'a pas besoin de connaître l'implémentation
- Vous pouvez changer l'implémentation sans casser le code utilisateur
- Interface simple et claire

---

#### Implémentation privée (queue.c)

```c
// queue.c - Implémentation privée
#include "queue.h"
#include <stdlib.h>

// Structure interne (invisible pour l'utilisateur)
typedef struct node {
    int value;
    struct node *next;
} node_t;

struct queue {
    node_t *head;
    node_t *tail;
    size_t size;
};

// Implémentation
queue_t* queue_create(void) {
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

bool queue_push(queue_t *q, int value) {
    if (!q) return false;

    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return false;

    new_node->value = value;
    new_node->next = NULL;

    if (q->tail) {
        q->tail->next = new_node;
    } else {
        q->head = new_node;
    }
    q->tail = new_node;
    q->size++;

    return true;
}

bool queue_pop(queue_t *q, int *value) {
    if (!q || !q->head || !value) return false;

    node_t *old_head = q->head;
    *value = old_head->value;

    q->head = old_head->next;
    if (!q->head) {
        q->tail = NULL;
    }

    free(old_head);
    q->size--;

    return true;
}

bool queue_peek(const queue_t *q, int *value) {
    if (!q || !q->head || !value) return false;
    *value = q->head->value;
    return true;
}

size_t queue_size(const queue_t *q) {
    return q ? q->size : 0;
}

bool queue_is_empty(const queue_t *q) {
    return !q || q->head == NULL;
}

void queue_destroy(queue_t *q) {
    if (!q) return;

    while (q->head) {
        node_t *temp = q->head;
        q->head = q->head->next;
        free(temp);
    }

    free(q);
}
```

✅ **Avantages** :
- Structure interne cachée
- Possibilité de passer de liste chaînée à tableau sans changer l'API
- Encapsulation complète

---

## Principe 2 : Conventions de nommage cohérentes

### Préfixer tout avec le nom de la bibliothèque

```c
// ❌ MAUVAIS : Noms génériques
void init(void);
int process(char *data);
typedef struct context context_t;

// ✅ BON : Tout est préfixé
void mylib_init(void);
int mylib_process(char *data);
typedef struct mylib_context mylib_context_t;
```

💡 **Pourquoi ?** Éviter les collisions de noms avec d'autres bibliothèques.

---

### Convention de nommage standard

```c
// Nom de bibliothèque : "vector"

// Types : vector_xxx_t
typedef struct vector_context vector_context_t;
typedef enum vector_error vector_error_t;

// Fonctions : vector_action_objet()
vector_context_t* vector_create(void);
void vector_destroy(vector_context_t *v);
bool vector_push_back(vector_context_t *v, int value);
int vector_get_at(const vector_context_t *v, size_t index);

// Constantes/Macros : VECTOR_XXX
#define VECTOR_DEFAULT_CAPACITY 16
#define VECTOR_VERSION "2.0.0"

// Codes d'erreur : VECTOR_ERROR_XXX
#define VECTOR_ERROR_NULL_POINTER -1
#define VECTOR_ERROR_OUT_OF_BOUNDS -2
```

---

### Ordre logique dans les noms de fonctions

```c
// Format : prefix_verbe_complément

// Création/Destruction
mylib_create()
mylib_destroy()

// Opérations de lecture (get/is/has)
mylib_get_value()
mylib_is_empty()
mylib_has_data()

// Opérations d'écriture (set/add/remove)
mylib_set_value()
mylib_add_item()
mylib_remove_item()

// Opérations complexes
mylib_process_data()
mylib_compute_result()
```

---

## Principe 3 : Types opaques (Encapsulation)

### Qu'est-ce qu'un type opaque ?

Un **type opaque** est une structure dont la définition complète n'est **pas visible** dans le fichier d'en-tête.

---

### Exemple complet

#### Fichier d'en-tête (database.h)

```c
// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <stddef.h>
#include <stdbool.h>

// Déclaration incomplète (forward declaration)
typedef struct database database_t;

// L'utilisateur ne peut pas accéder directement aux champs
database_t* db_open(const char *filename);
void db_close(database_t *db);
bool db_insert(database_t *db, int key, const char *value);
bool db_query(database_t *db, int key, char *buffer, size_t buf_size);

#endif
```

---

#### Fichier d'implémentation (database.c)

```c
// database.c
#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Définition complète (invisible pour l'utilisateur)
struct database {
    FILE *file;
    char *filename;
    size_t entry_count;
    // ... autres détails d'implémentation
};

database_t* db_open(const char *filename) {
    database_t *db = malloc(sizeof(database_t));
    if (!db) return NULL;

    db->file = fopen(filename, "a+");
    if (!db->file) {
        free(db);
        return NULL;
    }

    db->filename = strdup(filename);
    db->entry_count = 0;

    return db;
}

void db_close(database_t *db) {
    if (!db) return;

    if (db->file) fclose(db->file);
    free(db->filename);
    free(db);
}

// ... autres implémentations
```

---

### Avantages des types opaques

- ✅ **Encapsulation** : L'utilisateur ne peut pas accéder directement aux champs
- ✅ **Flexibilité** : Vous pouvez changer la structure interne sans casser le code utilisateur
- ✅ **Sécurité** : Évite les modifications accidentelles
- ✅ **ABI stable** : Pas besoin de recompiler le code utilisateur si vous changez la structure

---

### Comparaison : Structure publique vs opaque

#### Structure publique (exposée)

```c
// vector.h
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} vector_t;

// L'utilisateur peut faire :
vector_t v;
v.data = NULL;  // ❌ Bypass de l'initialisation
v.size = 9999;  // ❌ État incohérent
```

❌ **Problème** : Aucun contrôle sur l'état interne

---

#### Structure opaque

```c
// vector.h
typedef struct vector vector_t;

vector_t* vector_create(void);
size_t vector_size(const vector_t *v);

// L'utilisateur NE PEUT PAS faire :
vector_t *v = vector_create();
v->size = 9999;  // ❌ Erreur de compilation : type incomplet
```

✅ **Avantage** : Contrôle total sur l'état interne

---

## Principe 4 : Gestion des erreurs cohérente

### Approche 1 : Codes de retour

```c
// mylib.h
typedef enum {
    MYLIB_SUCCESS = 0,
    MYLIB_ERROR_NULL_POINTER = -1,
    MYLIB_ERROR_INVALID_ARGUMENT = -2,
    MYLIB_ERROR_OUT_OF_MEMORY = -3,
    MYLIB_ERROR_IO_FAILURE = -4
} mylib_error_t;

// Toutes les fonctions retournent un code d'erreur
mylib_error_t mylib_init(mylib_context_t **ctx);
mylib_error_t mylib_process(mylib_context_t *ctx, const char *data);
mylib_error_t mylib_cleanup(mylib_context_t **ctx);
```

**Utilisation** :

```c
mylib_context_t *ctx;
mylib_error_t err;

err = mylib_init(&ctx);
if (err != MYLIB_SUCCESS) {
    fprintf(stderr, "Init failed: %d\n", err);
    return 1;
}

err = mylib_process(ctx, "hello");
if (err != MYLIB_SUCCESS) {
    fprintf(stderr, "Process failed: %d\n", err);
    mylib_cleanup(&ctx);
    return 1;
}

mylib_cleanup(&ctx);
```

---

### Approche 2 : Booléens + errno

Pour les fonctions simples :

```c
// queue.h
#include <stdbool.h>
#include <errno.h>

bool queue_push(queue_t *q, int value);  // true = succès, false = échec

// Implémentation
bool queue_push(queue_t *q, int value) {
    if (!q) {
        errno = EINVAL;  // Invalid argument
        return false;
    }

    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) {
        errno = ENOMEM;  // Out of memory
        return false;
    }

    // ... reste de l'implémentation
    return true;
}
```

---

### Approche 3 : Fonction de récupération d'erreur

```c
// mylib.h
typedef enum {
    MYLIB_SUCCESS = 0,
    MYLIB_ERROR_NULL_POINTER,
    MYLIB_ERROR_INVALID_ARGUMENT,
    MYLIB_ERROR_OUT_OF_MEMORY
} mylib_error_t;

// Opérations retournent un booléen ou un pointeur
bool mylib_process(mylib_context_t *ctx, const char *data);

// Fonction séparée pour récupérer l'erreur détaillée
mylib_error_t mylib_get_last_error(mylib_context_t *ctx);
const char* mylib_error_string(mylib_error_t error);
```

**Utilisation** :

```c
if (!mylib_process(ctx, data)) {
    mylib_error_t err = mylib_get_last_error(ctx);
    fprintf(stderr, "Error: %s\n", mylib_error_string(err));
}
```

---

## Principe 5 : Documentation dans les headers

### Format de documentation : Style Doxygen

```c
/**
 * @brief Create a new vector with default capacity
 *
 * Allocates and initializes a new dynamic vector. The vector starts
 * with VECTOR_DEFAULT_CAPACITY elements of space.
 *
 * @return Pointer to the newly created vector, or NULL on allocation failure
 *
 * @note The returned vector must be freed with vector_destroy()
 * @see vector_destroy(), vector_create_with_capacity()
 *
 * @example
 * vector_t *v = vector_create();
 * if (!v) {
 *     // Handle allocation error
 * }
 * // Use vector...
 * vector_destroy(v);
 */
vector_t* vector_create(void);

/**
 * @brief Destroy a vector and free its memory
 *
 * Frees all memory associated with the vector, including the vector
 * structure itself. After calling this function, the pointer becomes
 * invalid and must not be used.
 *
 * @param v Pointer to the vector to destroy (can be NULL)
 *
 * @note Passing NULL is safe and does nothing
 * @warning Do not use the vector pointer after calling this function
 */
void vector_destroy(vector_t *v);

/**
 * @brief Add an element to the end of the vector
 *
 * Appends a new element to the end of the vector. If the current
 * capacity is insufficient, the vector is automatically resized.
 *
 * @param v Pointer to the vector (must not be NULL)
 * @param value Value to add
 * @return true on success, false on allocation failure
 *
 * @pre v must be a valid vector created with vector_create()
 * @post If successful, vector size is increased by 1
 */
bool vector_push_back(vector_t *v, int value);
```

---

### Commentaires essentiels

```c
// mylib.h

/**
 * @brief Initialize the library context
 *
 * This function MUST be called before any other library function.
 *
 * @param[out] ctx Pointer to store the created context
 * @return MYLIB_SUCCESS or error code
 *
 * @note Thread-safe
 * @warning Calling library functions without initialization leads to undefined behavior
 */
mylib_error_t mylib_init(mylib_context_t **ctx);

/**
 * @brief Process data with the given context
 *
 * @param[in] ctx Context created with mylib_init()
 * @param[in] data Data to process (must not be NULL)
 * @param[in] data_len Length of data in bytes
 * @return MYLIB_SUCCESS or error code
 *
 * @pre ctx must be initialized
 * @pre data must point to at least data_len bytes
 * @post On success, data is processed according to context configuration
 *
 * @note This function is NOT thread-safe. External synchronization required.
 */
mylib_error_t mylib_process(mylib_context_t *ctx,
                            const char *data,
                            size_t data_len);
```

---

## Principe 6 : Gestion de la mémoire claire

### Règle 1 : Qui alloue, qui libère ?

**Convention standard** : Celui qui alloue doit fournir la fonction de libération.

```c
// Bonne pratique : paires create/destroy
vector_t* vector_create(void);        // Alloue
void vector_destroy(vector_t *v);      // Libère

database_t* db_open(const char *path); // Alloue
void db_close(database_t *db);         // Libère

mylib_context_t* mylib_init(void);     // Alloue
void mylib_cleanup(mylib_context_t *ctx); // Libère
```

---

### Règle 2 : Double pointeur pour les fonctions de destruction

```c
// ✅ BON : Double pointeur
void mylib_destroy(mylib_context_t **ctx) {
    if (!ctx || !*ctx) return;

    // Libérer les ressources
    free((*ctx)->data);

    // Libérer la structure
    free(*ctx);

    // Mettre à NULL pour éviter les dangling pointers
    *ctx = NULL;
}

// Utilisation
mylib_context_t *ctx = mylib_create();
mylib_destroy(&ctx);  // ctx est automatiquement mis à NULL
// ctx == NULL ici, pas de dangling pointer
```

---

### Règle 3 : Documenter la propriété de la mémoire

```c
/**
 * @brief Get a read-only view of the internal buffer
 *
 * @param ctx Context
 * @return Pointer to internal buffer (DO NOT FREE)
 *
 * @warning The returned pointer is owned by the context and becomes
 *          invalid when the context is destroyed.
 * @note The caller must NOT free this pointer
 */
const char* mylib_get_buffer(mylib_context_t *ctx);

/**
 * @brief Create a copy of the internal buffer
 *
 * @param ctx Context
 * @return Newly allocated copy of the buffer (MUST BE FREED by caller)
 *
 * @note The caller is responsible for freeing the returned string
 */
char* mylib_copy_buffer(mylib_context_t *ctx);
```

---

## Principe 7 : Constance et immutabilité

### Utiliser const pour les paramètres en lecture seule

```c
// ✅ BON : const pour les paramètres non modifiés
size_t vector_size(const vector_t *v);
bool vector_find(const vector_t *v, int value);
void vector_print(const vector_t *v);

// ✅ BON : const pour les chaînes de caractères
mylib_error_t mylib_set_name(mylib_context_t *ctx, const char *name);

// ❌ MAUVAIS : Pas de const alors que la fonction ne modifie pas
size_t vector_size(vector_t *v);  // L'appelant pourrait penser que v est modifié
```

---

### Utiliser const pour les valeurs de retour

```c
// Retourner un pointeur constant vers des données internes
const char* mylib_get_version(void);
const int* vector_data(const vector_t *v);  // Accès en lecture seule

// Retourner une copie (pas const car l'appelant en est propriétaire)
char* mylib_copy_string(const mylib_context_t *ctx);
```

---

## Principe 8 : Versioning de l'API

### Fournir les informations de version

```c
// mylib.h
#define MYLIB_VERSION_MAJOR 2
#define MYLIB_VERSION_MINOR 1
#define MYLIB_VERSION_PATCH 0
#define MYLIB_VERSION "2.1.0"

// Fonction pour récupérer la version au runtime
const char* mylib_get_version(void);

// Vérification de compatibilité
bool mylib_is_compatible(int major, int minor);
```

---

### Gestion de la rétrocompatibilité

#### Exemple : Ajouter un paramètre optionnel

```c
// Version 1.0
mylib_error_t mylib_process(mylib_context_t *ctx, const char *data);

// Version 2.0 - Nouvelle fonction avec options
mylib_error_t mylib_process_ex(mylib_context_t *ctx,
                               const char *data,
                               const mylib_options_t *options);

// Garder l'ancienne fonction pour compatibilité
mylib_error_t mylib_process(mylib_context_t *ctx, const char *data) {
    return mylib_process_ex(ctx, data, NULL);  // Options par défaut
}
```

---

### Marquage de dépréciation

```c
// mylib.h

// Marquer une fonction obsolète
#ifdef __GNUC__
#define MYLIB_DEPRECATED __attribute__((deprecated))
#else
#define MYLIB_DEPRECATED
#endif

// Ancienne API (obsolète)
MYLIB_DEPRECATED
mylib_error_t mylib_old_function(int param);

// Nouvelle API (recommandée)
mylib_error_t mylib_new_function(const mylib_params_t *params);
```

---

## Principe 9 : Symbol Visibility

### Contrôler quels symboles sont exportés

Par défaut, tous les symboles d'une bibliothèque partagée sont publics. C'est souvent **trop**.

---

### Approche 1 : Attributs GCC

```c
// mylib_internal.h (pour l'usage interne uniquement)
#ifdef __GNUC__
#define MYLIB_INTERNAL __attribute__((visibility("hidden")))
#define MYLIB_PUBLIC __attribute__((visibility("default")))
#else
#define MYLIB_INTERNAL
#define MYLIB_PUBLIC
#endif

// Fonction publique (visible dans l'API)
MYLIB_PUBLIC mylib_error_t mylib_init(mylib_context_t **ctx);

// Fonction interne (invisible pour l'utilisateur)
MYLIB_INTERNAL void mylib_internal_helper(void);
```

---

### Approche 2 : Compilation avec -fvisibility=hidden

```bash
# Compiler avec visibilité cachée par défaut
gcc -shared -fPIC -fvisibility=hidden mylib.c -o libmylib.so

# Seules les fonctions marquées __attribute__((visibility("default")))
# seront exportées
```

---

### Vérifier les symboles exportés

```bash
# Lister les symboles exportés
$ nm -D libmylib.so | grep " T "
00000000000011a9 T mylib_init
0000000000001205 T mylib_process
0000000000001310 T mylib_cleanup

# mylib_internal_helper n'apparaît pas (caché)
```

✅ **Avantage** : Réduction de la taille des symboles et meilleure encapsulation

---

## Exemple complet : API de logger

### Fichier d'en-tête (logger.h)

```c
// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
#include <stdbool.h>

// Version
#define LOGGER_VERSION_MAJOR 1
#define LOGGER_VERSION_MINOR 0
#define LOGGER_VERSION_PATCH 0
#define LOGGER_VERSION "1.0.0"

// Type opaque
typedef struct logger logger_t;

// Niveaux de log
typedef enum {
    LOGGER_LEVEL_DEBUG = 0,
    LOGGER_LEVEL_INFO = 1,
    LOGGER_LEVEL_WARNING = 2,
    LOGGER_LEVEL_ERROR = 3
} logger_level_t;

// Codes d'erreur
typedef enum {
    LOGGER_SUCCESS = 0,
    LOGGER_ERROR_NULL_POINTER = -1,
    LOGGER_ERROR_INVALID_LEVEL = -2,
    LOGGER_ERROR_IO_FAILURE = -3,
    LOGGER_ERROR_OUT_OF_MEMORY = -4
} logger_error_t;

/**
 * @brief Create a new logger
 *
 * @param filename Log file path (NULL for stdout)
 * @param min_level Minimum level to log
 * @return Pointer to logger or NULL on error
 *
 * @note Caller must free with logger_destroy()
 */
logger_t* logger_create(const char *filename, logger_level_t min_level);

/**
 * @brief Destroy a logger and close resources
 *
 * @param logger Pointer to pointer to logger (set to NULL after)
 */
void logger_destroy(logger_t **logger);

/**
 * @brief Log a message
 *
 * @param logger Logger instance
 * @param level Log level
 * @param format Printf-style format string
 * @param ... Format arguments
 * @return LOGGER_SUCCESS or error code
 */
logger_error_t logger_log(logger_t *logger,
                         logger_level_t level,
                         const char *format, ...);

/**
 * @brief Set minimum log level
 *
 * @param logger Logger instance
 * @param min_level New minimum level
 * @return LOGGER_SUCCESS or error code
 */
logger_error_t logger_set_min_level(logger_t *logger, logger_level_t min_level);

/**
 * @brief Get minimum log level
 *
 * @param logger Logger instance
 * @return Current minimum level
 */
logger_level_t logger_get_min_level(const logger_t *logger);

/**
 * @brief Get library version
 *
 * @return Version string
 */
const char* logger_get_version(void);

/**
 * @brief Convert error code to string
 *
 * @param error Error code
 * @return Error description
 */
const char* logger_error_string(logger_error_t error);

// Macros de commodité
#define logger_debug(logger, ...) \
    logger_log(logger, LOGGER_LEVEL_DEBUG, __VA_ARGS__)

#define logger_info(logger, ...) \
    logger_log(logger, LOGGER_LEVEL_INFO, __VA_ARGS__)

#define logger_warning(logger, ...) \
    logger_log(logger, LOGGER_LEVEL_WARNING, __VA_ARGS__)

#define logger_error(logger, ...) \
    logger_log(logger, LOGGER_LEVEL_ERROR, __VA_ARGS__)

#endif // LOGGER_H
```

---

### Utilisation de l'API

```c
#include "logger.h"
#include <stdio.h>

int main(void) {
    // Créer un logger
    logger_t *log = logger_create("app.log", LOGGER_LEVEL_INFO);
    if (!log) {
        fprintf(stderr, "Failed to create logger\n");
        return 1;
    }

    // Utiliser le logger
    logger_info(log, "Application started");
    logger_debug(log, "This won't be logged (below min level)");
    logger_warning(log, "Configuration file not found, using defaults");

    // Changer le niveau de log
    logger_set_min_level(log, LOGGER_LEVEL_DEBUG);
    logger_debug(log, "Now this will be logged");

    // Vérifier les erreurs
    logger_error_t err = logger_log(log, LOGGER_LEVEL_ERROR, "Critical error occurred");
    if (err != LOGGER_SUCCESS) {
        fprintf(stderr, "Logging failed: %s\n", logger_error_string(err));
    }

    // Nettoyer
    logger_destroy(&log);
    // log est maintenant NULL

    return 0;
}
```

---

## Checklist : API propre

### ✅ Structure

- [ ] Séparation claire interface (.h) / implémentation (.c)
- [ ] Types opaques pour les structures internes
- [ ] Tous les symboles ont un préfixe de bibliothèque
- [ ] Gardes d'inclusion dans tous les headers

### ✅ Nommage

- [ ] Conventions cohérentes partout
- [ ] Noms explicites et auto-documentés
- [ ] Préfixe unique pour éviter les collisions
- [ ] Format : `prefix_verbe_complement()`

### ✅ Gestion des erreurs

- [ ] Codes d'erreur cohérents et documentés
- [ ] Toutes les fonctions gèrent les erreurs
- [ ] Comportement clair en cas d'erreur
- [ ] Fonction pour convertir les erreurs en chaînes

### ✅ Mémoire

- [ ] Paires create/destroy claires
- [ ] Documentation de la propriété de la mémoire
- [ ] Pas de fuites mémoire
- [ ] Gestion correcte des échecs d'allocation

### ✅ Documentation

- [ ] Chaque fonction publique est documentée
- [ ] Paramètres et valeurs de retour expliqués
- [ ] Préconditions et postconditions mentionnées
- [ ] Exemples d'utilisation fournis

### ✅ Compatibilité

- [ ] Versioning de l'API
- [ ] Rétrocompatibilité maintenue
- [ ] Fonctions obsolètes marquées comme dépréciées
- [ ] ABI stable (pas de changements de structures publiques)

### ✅ Sécurité

- [ ] Validation des paramètres
- [ ] Utilisation de const appropriée
- [ ] Pas d'accès direct aux structures internes
- [ ] Symboles internes cachés (visibility)

---

## Anti-patterns à éviter

### ❌ Anti-pattern 1 : Exposition de l'implémentation

```c
// mylib.h - MAUVAIS
typedef struct {
    char *internal_buffer;  // Exposé
    size_t buffer_size;      // Exposé
    FILE *log_file;          // Exposé
} mylib_context_t;

// L'utilisateur peut faire n'importe quoi :
mylib_context_t ctx;
ctx.internal_buffer = NULL;  // Cassé !
```

---

### ❌ Anti-pattern 2 : Noms génériques

```c
// MAUVAIS - Noms trop génériques
void init(void);
int process(void *data);
typedef struct context context_t;

// Collision garantie avec d'autres bibliothèques !
```

---

### ❌ Anti-pattern 3 : État global caché

```c
// mylib.c - MAUVAIS
static FILE *global_log_file = NULL;
static int global_counter = 0;

void mylib_init(void) {
    global_log_file = fopen("log.txt", "w");
}

// Problèmes :
// - Pas thread-safe
// - Impossible d'avoir plusieurs instances
// - État caché difficile à déboguer
```

---

### ❌ Anti-pattern 4 : Retour de pointeurs vers des données temporaires

```c
// MAUVAIS
const char* mylib_get_message(void) {
    char buffer[100];
    sprintf(buffer, "Message: %d", get_code());
    return buffer;  // ❌ Pointeur vers pile locale !
}

// BON
const char* mylib_get_message(void) {
    static char buffer[100];  // Ou allouer dynamiquement
    sprintf(buffer, "Message: %d", get_code());
    return buffer;
}
```

---

### ❌ Anti-pattern 5 : Pas de gestion des NULL

```c
// MAUVAIS - Crash si ctx est NULL
int mylib_process(mylib_context_t *ctx) {
    return ctx->value * 2;  // Crash si ctx == NULL
}

// BON - Vérification défensive
int mylib_process(mylib_context_t *ctx) {
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }
    return ctx->value * 2;
}
```

---

## Outils pour valider votre API

### 1. Analyse statique

```bash
# cppcheck
$ cppcheck --enable=all mylib.c

# clang-tidy
$ clang-tidy mylib.c -- -I.
```

---

### 2. Vérification des symboles exportés

```bash
# Lister les symboles exportés
$ nm -D libmylib.so | grep " T "

# Vérifier qu'aucun symbole interne n'est exporté
$ nm -D libmylib.so | grep internal
# Devrait être vide
```

---

### 3. Test de l'API

Créez un fichier de test minimal :

```c
// test_api.c - Vérifie que l'API est utilisable
#include "mylib.h"

int main(void) {
    // Test de compilation : toutes les fonctions sont-elles accessibles ?
    mylib_context_t *ctx = mylib_create();
    if (!ctx) return 1;

    mylib_process(ctx, "test");
    mylib_destroy(&ctx);

    return 0;
}
```

```bash
$ gcc test_api.c -L. -lmylib -o test_api
$ ./test_api
```

---

## Exemples d'APIs bien conçues à étudier

### Bibliothèques standard C

- **SQLite** : API extrêmement propre et stable
- **zlib** : API minimaliste et efficace
- **libcurl** : Gestion cohérente des options

### Étude de cas : SQLite

```c
// API SQLite - Exemple de clarté
sqlite3 *db;
int rc;

// Ouverture
rc = sqlite3_open("database.db", &db);
if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    return 1;
}

// Utilisation
sqlite3_stmt *stmt;
rc = sqlite3_prepare_v2(db, "SELECT * FROM users", -1, &stmt, NULL);

// Fermeture
sqlite3_finalize(stmt);
sqlite3_close(db);
```

**Ce qui est bien** :
- Préfixe `sqlite3_` partout
- Types opaques (`sqlite3`, `sqlite3_stmt`)
- Codes d'erreur clairs (`SQLITE_OK`)
- Fonction d'erreur (`sqlite3_errmsg`)
- Paires ouverture/fermeture évidentes

---

## Résumé : Principes d'une API propre

| Principe | Description | Exemple |
|----------|-------------|---------|
| **Préfixe** | Tout doit avoir le préfixe de la bibliothèque | `mylib_create()`, `MYLIB_VERSION` |
| **Opacité** | Cacher l'implémentation | `typedef struct mylib mylib_t;` |
| **Cohérence** | Même pattern partout | `_create()` / `_destroy()` |
| **Clarté** | Noms explicites | `queue_push()` pas `q_p()` |
| **Sécurité** | Validation des paramètres | `if (!ptr) return ERROR;` |
| **Documentation** | Chaque fonction documentée | `/** @brief ... */` |
| **Erreurs** | Gestion cohérente | Codes d'erreur enum |
| **Constance** | Utiliser const | `const vector_t *v` |
| **Versioning** | Numéro de version accessible | `MYLIB_VERSION` |

---

## Conclusion

Créer une API propre en C demande de la discipline et de la réflexion, mais les bénéfices sont énormes :

✅ **Pour vous** :
- Code plus maintenable
- Moins de bugs
- Refactoring plus facile

✅ **Pour les utilisateurs** :
- Apprentissage rapide
- Moins d'erreurs
- Code plus fiable

💡 **Règle d'or** : Concevez votre API comme si vous alliez l'utiliser vous-même dans 5 ans sans vous souvenir de comment elle fonctionne. Si c'est clair et intuitif pour ce "vous du futur", c'est une bonne API.

---

## Pour aller plus loin

**Sections connexes recommandées** :
- **12.1** : Fichiers d'en-tête et gardes d'inclusion
- **12.4** : Organisation d'un projet
- **14.2** : Bibliothèques dynamiques (.so) - SONAME et versioning
- **14.7** : Symbol visibility
- **32.1** : Documentation du code avec Doxygen

**Ressources externes** :
- *API Design for C++* (Martin Reddy) : Principes applicables au C
- *The Art of API Design* : Conférences et articles
- Documentation SQLite : Exemple d'excellente API C

**Projets à étudier** :
- **SQLite** : sqlite.org - API de référence
- **zlib** : zlib.net - API minimaliste
- **libcurl** : curl.se - Gestion d'options élégante

---

**🎯 Points clés à retenir** :
1. Séparez interface (.h) et implémentation (.c)
2. Utilisez des types opaques pour l'encapsulation
3. Préfixez TOUT avec le nom de votre bibliothèque
4. Documentez chaque fonction publique
5. Gérez les erreurs de manière cohérente
6. Utilisez const pour indiquer l'immutabilité
7. Pensez à la rétrocompatibilité dès le début
8. Testez votre API comme si vous étiez l'utilisateur

⏭️ [Symbol visibility](/14-bibliotheques/07-symbol-visibility.md)
