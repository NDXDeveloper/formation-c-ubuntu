🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 14.7 Symbol visibility

## Introduction

Lorsque vous créez une bibliothèque partagée (`.so`), par défaut **tous les symboles** (fonctions, variables globales) sont **exportés** et visibles par les programmes qui utilisent votre bibliothèque. Mais est-ce vraiment nécessaire ? Souvent, seule une petite partie de votre code devrait être publique.

Le concept de **symbol visibility** (visibilité des symboles) permet de contrôler précisément quels symboles sont exportés et lesquels restent privés à votre bibliothèque.

Dans cette section, nous allons apprendre à :
- Comprendre ce qu'est un symbole et sa visibilité
- Contrôler la visibilité avec GCC
- Optimiser la taille et les performances de vos bibliothèques
- Créer des APIs propres avec une vraie encapsulation

---

## Qu'est-ce qu'un symbole ?

### Définition

Un **symbole** est un nom (identifiant) dans votre code qui peut être référencé depuis l'extérieur :
- **Fonctions** : `ma_fonction()`
- **Variables globales** : `int compteur_global;`
- **Constantes** : `const char *VERSION = "1.0";`

---

### Exemple simple

```c
// mylib.c
#include <stdio.h>

// Fonction publique
void mylib_init(void) {
    printf("Library initialized\n");
}

// Fonction privée (aide interne)
void internal_helper(void) {
    printf("Internal function\n");
}

// Variable globale
int mylib_counter = 0;
```

Après compilation en bibliothèque partagée :

```bash
$ gcc -shared -fPIC mylib.c -o libmylib.so
$ nm -D libmylib.so | grep " T "
00000000000011a9 T mylib_init
0000000000001205 T internal_helper      ← Visible alors qu'elle devrait être privée !
```

❌ **Problème** : `internal_helper()` est exposée alors qu'elle ne fait pas partie de l'API publique.

---

## Pourquoi contrôler la visibilité ?

### 1. **Encapsulation et API propre**

```c
// Ce que vous voulez exposer (API publique)
void mylib_init(void);  
void mylib_process(void);  

// Ce qui devrait rester privé
void internal_helper(void);  
void debug_print(void);  
static int calculate_hash(const char *str);  
```

✅ **Objectif** : Seules `mylib_init()` et `mylib_process()` devraient être visibles.

---

### 2. **Éviter les collisions de symboles**

**Scénario problématique** :

```c
// Votre bibliothèque : libmylib.so
void parse_config(void);  // Fonction interne

// Une autre bibliothèque : libother.so
void parse_config(void);  // Même nom !

// Programme utilisateur
#include "mylib.h"
#include "other.h"

int main(void) {
    // Quelle parse_config() sera appelée ?
    parse_config();  // ❌ Ambiguïté !
}
```

💡 **Solution** : Rendre `parse_config()` privée dans les deux bibliothèques.

---

### 3. **Optimisation des performances**

Le linker dynamique doit résoudre tous les symboles exportés au chargement du programme. Moins de symboles = chargement plus rapide.

```
Bibliothèque avec 1000 symboles exportés → Temps de chargement : 50ms  
Bibliothèque avec 50 symboles exportés   → Temps de chargement : 5ms  
```

---

### 4. **Réduction de la taille des binaires**

La table des symboles prend de la place :

```bash
# Avant : tous les symboles exportés
$ ls -lh libmylib.so
-rwxr-xr-x 1 user user 85K libmylib.so

# Après : symboles privés cachés
$ ls -lh libmylib.so
-rwxr-xr-x 1 user user 62K libmylib.so  ← 27% plus petit !
```

---

### 5. **Permettre les optimisations du compilateur**

Le compilateur peut mieux optimiser les fonctions privées (inlining, élimination de code mort, etc.).

---

## Les niveaux de visibilité

GCC supporte quatre niveaux de visibilité :

| Visibilité | Description | Utilisation |
|------------|-------------|-------------|
| **default** | Symbole exporté et visible (comportement par défaut) | API publique |
| **hidden** | Symbole invisible depuis l'extérieur | Fonctions/variables internes |
| **protected** | Visible mais non remplaçable | Cas spécifiques |
| **internal** | Comme hidden + optimisations supplémentaires | Rarement utilisé |

💡 **En pratique**, on utilise principalement **default** et **hidden**.

---

## Méthode 1 : Attributs de visibilité

### Syntaxe de base

```c
// Fonction publique (visible)
__attribute__((visibility("default")))
void mylib_public_function(void);

// Fonction privée (cachée)
__attribute__((visibility("hidden")))
void mylib_internal_function(void);
```

---

### Exemple complet

```c
// mylib.c
#include <stdio.h>

// Fonction publique
__attribute__((visibility("default")))
void mylib_init(void) {
    printf("Library initialized\n");
    internal_helper();
}

// Fonction privée
__attribute__((visibility("hidden")))
void internal_helper(void) {
    printf("Internal function\n");
}

// Fonction publique
__attribute__((visibility("default")))
void mylib_process(void) {
    printf("Processing...\n");
}
```

**Compilation** :

```bash
$ gcc -shared -fPIC mylib.c -o libmylib.so
```

**Vérification** :

```bash
$ nm -D libmylib.so | grep " T "
00000000000011a9 T mylib_init       ← Visible
0000000000001310 T mylib_process    ← Visible
# internal_helper n'apparaît pas ! ✅
```

---

### Macros pour simplifier

```c
// mylib.h
#ifndef MYLIB_H
#define MYLIB_H

// Définir les macros de visibilité
#if defined(__GNUC__) && __GNUC__ >= 4
  #define MYLIB_PUBLIC __attribute__((visibility("default")))
  #define MYLIB_PRIVATE __attribute__((visibility("hidden")))
#else
  #define MYLIB_PUBLIC
  #define MYLIB_PRIVATE
#endif

// API publique
MYLIB_PUBLIC void mylib_init(void);  
MYLIB_PUBLIC void mylib_process(void);  

#endif
```

```c
// mylib.c
#include "mylib.h"

// Fonction privée (pas dans le header)
MYLIB_PRIVATE void internal_helper(void) {
    // Implémentation privée
}

// Fonctions publiques
MYLIB_PUBLIC void mylib_init(void) {
    internal_helper();
}

MYLIB_PUBLIC void mylib_process(void) {
    // Implémentation publique
}
```

---

## Méthode 2 : Flag de compilation -fvisibility

### Principe

Au lieu de marquer chaque fonction, on peut définir la visibilité par défaut pour **tout le fichier** :

```bash
# Cacher tout par défaut
gcc -shared -fPIC -fvisibility=hidden mylib.c -o libmylib.so
```

Ensuite, on marque explicitement les fonctions publiques :

```c
// mylib.c

// Cette fonction sera VISIBLE (marquée explicitement)
__attribute__((visibility("default")))
void mylib_init(void) {
    printf("Init\n");
}

// Cette fonction sera CACHÉE (default = hidden)
void internal_helper(void) {
    printf("Internal\n");
}
```

---

### Approche recommandée : Combinaison des deux

**Fichier de configuration (mylib_export.h)** :

```c
// mylib_export.h
#ifndef MYLIB_EXPORT_H
#define MYLIB_EXPORT_H

#ifdef MYLIB_BUILDING
  // On compile la bibliothèque
  #if defined(__GNUC__) && __GNUC__ >= 4
    #define MYLIB_API __attribute__((visibility("default")))
  #elif defined(_WIN32)
    #define MYLIB_API __declspec(dllexport)
  #else
    #define MYLIB_API
  #endif
#else
  // On utilise la bibliothèque
  #if defined(_WIN32)
    #define MYLIB_API __declspec(dllimport)
  #else
    #define MYLIB_API
  #endif
#endif

#endif
```

**Header public (mylib.h)** :

```c
// mylib.h
#ifndef MYLIB_H
#define MYLIB_H

#include "mylib_export.h"

// API publique - tous les symboles marqués MYLIB_API
MYLIB_API void mylib_init(void);  
MYLIB_API void mylib_process(const char *data);  
MYLIB_API int mylib_get_version(void);  

#endif
```

**Implémentation (mylib.c)** :

```c
// mylib.c
#define MYLIB_BUILDING  // Indique qu'on compile la bibliothèque
#include "mylib.h"
#include <stdio.h>

// Fonction privée (pas dans le header, pas de MYLIB_API)
static void internal_helper(void) {
    printf("Internal function\n");
}

// Fonctions publiques (dans le header, avec MYLIB_API)
void mylib_init(void) {
    printf("Library initialized\n");
    internal_helper();
}

void mylib_process(const char *data) {
    printf("Processing: %s\n", data);
}

int mylib_get_version(void) {
    return 1;
}
```

**Compilation** :

```bash
$ gcc -shared -fPIC -fvisibility=hidden -DMYLIB_BUILDING mylib.c -o libmylib.so
```

✅ **Résultat** : Seules les fonctions marquées `MYLIB_API` sont exportées.

---

## Vérification de la visibilité

### Outil 1 : nm (symboles)

```bash
# Lister tous les symboles exportés
$ nm -D libmylib.so

# Filtrer les fonctions (type T)
$ nm -D libmylib.so | grep " T "
00000000000011a9 T mylib_init
0000000000001205 T mylib_process
0000000000001310 T mylib_get_version

# Vérifier qu'une fonction n'est PAS exportée
$ nm -D libmylib.so | grep internal_helper
# Devrait être vide si la fonction est cachée ✅
```

**Légende nm** :
- `T` : Symbole dans la section texte (fonction exportée)
- `t` : Symbole dans la section texte (fonction locale/cachée)
- `D` : Symbole dans la section data (variable globale exportée)
- `d` : Symbole dans la section data (variable locale/cachée)

---

### Outil 2 : objdump

```bash
# Afficher tous les symboles dynamiques
$ objdump -T libmylib.so

# Filtrer les fonctions
$ objdump -T libmylib.so | grep "DF"
00000000000011a9 g    DF .text  000000000000005c  mylib_init
0000000000001205 g    DF .text  0000000000000048  mylib_process
```

**Légende** :
- `g` : global (symbole exporté)
- `l` : local (symbole caché)
- `DF` : Dynamic Function

---

### Outil 3 : readelf

```bash
# Afficher la table des symboles dynamiques
$ readelf -s --wide libmylib.so | grep FUNC

# Ou plus spécifique
$ readelf --dyn-syms libmylib.so
```

---

## Comparaison : Avec et sans visibilité

### Sans contrôle de visibilité

```c
// mylib.c
void mylib_init(void) { }  
void mylib_process(void) { }  
void internal_helper(void) { }  
void debug_print(void) { }  
void calculate_hash(void) { }  
```

```bash
$ gcc -shared -fPIC mylib.c -o libmylib.so
$ nm -D libmylib.so | grep " T " | wc -l
5  ← Toutes les fonctions sont exportées
```

---

### Avec contrôle de visibilité

```c
// mylib.c
__attribute__((visibility("default")))
void mylib_init(void) { }

__attribute__((visibility("default")))
void mylib_process(void) { }

__attribute__((visibility("hidden")))
void internal_helper(void) { }

__attribute__((visibility("hidden")))
void debug_print(void) { }

__attribute__((visibility("hidden")))
void calculate_hash(void) { }
```

```bash
$ gcc -shared -fPIC mylib.c -o libmylib.so
$ nm -D libmylib.so | grep " T " | wc -l
2  ← Seulement les fonctions publiques ✅
```

---

## Impact sur les performances

### Test de benchmark

**Bibliothèque avec 1000 symboles exportés** :

```bash
$ time ldd mon_programme
    libmylib.so => ./libmylib.so (0x00007f...)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)

real    0m0.052s
```

**Bibliothèque avec 50 symboles exportés** :

```bash
$ time ldd mon_programme
    libmylib.so => ./libmylib.so (0x00007f...)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)

real    0m0.006s  ← 8.6x plus rapide !
```

---

### Taille de la table des symboles

```bash
# Sans visibilité contrôlée
$ readelf -S libmylib.so | grep dynsym
  [ 4] .dynsym           DYNSYM           0000000000000318  00000318
       000000000000d8 18   A  5   1  8

# Avec visibilité contrôlée
$ readelf -S libmylib.so | grep dynsym
  [ 4] .dynsym           DYNSYM           0000000000000318  00000318
       0000000000000048 18   A  5   1  8  ← 3x plus petit !
```

---

## Cas d'usage pratiques

### Cas 1 : Bibliothèque avec API minimale

**Objectif** : Exposer uniquement 3 fonctions sur 50.

**Structure** :

```
mylib/
├── include/
│   └── mylib.h           (API publique)
├── src/
│   ├── mylib.c           (Implémentation publique)
│   ├── parser.c          (Module interne)
│   ├── cache.c           (Module interne)
│   └── utils.c           (Module interne)
└── CMakeLists.txt
```

**mylib.h** (API publique) :

```c
#ifndef MYLIB_H
#define MYLIB_H

#if defined(__GNUC__) && __GNUC__ >= 4
  #define MYLIB_API __attribute__((visibility("default")))
#else
  #define MYLIB_API
#endif

typedef struct mylib_context mylib_context_t;

MYLIB_API mylib_context_t* mylib_create(void);  
MYLIB_API int mylib_process(mylib_context_t *ctx, const char *input);  
MYLIB_API void mylib_destroy(mylib_context_t *ctx);  

#endif
```

**Compilation** :

```bash
$ gcc -shared -fPIC -fvisibility=hidden \
      mylib.c parser.c cache.c utils.c \
      -o libmylib.so
```

**Résultat** :

```bash
$ nm -D libmylib.so | grep " T " | wc -l
3  ← Seulement les 3 fonctions publiques ✅

# Les 47 autres fonctions sont cachées
```

---

### Cas 2 : Éviter les collisions de noms

**Problème** : Deux bibliothèques utilisent le même nom de fonction interne.

```c
// libfoo.so
void parse_config(void) {  // Fonction interne
    printf("Foo parsing\n");
}

// libbar.so
void parse_config(void) {  // Même nom !
    printf("Bar parsing\n");
}
```

**Sans visibilité contrôlée** :

```bash
$ ./mon_programme
Foo parsing  
Foo parsing  ← libbar.so appelle la fonction de libfoo.so ! ❌  
```

**Avec visibilité contrôlée** :

```c
// libfoo.so
__attribute__((visibility("hidden")))
void parse_config(void) {
    printf("Foo parsing\n");
}

// libbar.so
__attribute__((visibility("hidden")))
void parse_config(void) {
    printf("Bar parsing\n");
}
```

```bash
$ ./mon_programme
Foo parsing  
Bar parsing  ← Chaque bibliothèque utilise sa propre fonction ✅  
```

---

### Cas 3 : Plugin system

**Scénario** : Application avec système de plugins.

```c
// plugin_api.h (API fournie par l'application)
#define PLUGIN_API __attribute__((visibility("default")))

// Fonctions que chaque plugin doit implémenter
typedef struct {
    const char* name;
    int version;
    int (*init)(void);
    int (*process)(const char *data);
    void (*cleanup)(void);
} plugin_interface_t;

PLUGIN_API plugin_interface_t* plugin_get_interface(void);
```

**Plugin** :

```c
// myplugin.c
#include "plugin_api.h"

// Fonctions internes du plugin (cachées)
__attribute__((visibility("hidden")))
static void internal_setup(void) {
    // Configuration interne
}

__attribute__((visibility("hidden")))
static int plugin_init(void) {
    internal_setup();
    return 0;
}

__attribute__((visibility("hidden")))
static int plugin_process(const char *data) {
    // Traitement
    return 0;
}

__attribute__((visibility("hidden")))
static void plugin_cleanup(void) {
    // Nettoyage
}

// Seul symbole exporté
PLUGIN_API plugin_interface_t* plugin_get_interface(void) {
    static plugin_interface_t interface = {
        .name = "MyPlugin",
        .version = 1,
        .init = plugin_init,
        .process = plugin_process,
        .cleanup = plugin_cleanup
    };
    return &interface;
}
```

**Compilation** :

```bash
$ gcc -shared -fPIC -fvisibility=hidden myplugin.c -o myplugin.so
$ nm -D myplugin.so | grep " T "
0000000000001189 T plugin_get_interface  ← Seul symbole exporté ✅
```

---

## Intégration avec CMake

### CMakeLists.txt moderne

```cmake
cmake_minimum_required(VERSION 3.10)  
project(mylib VERSION 1.0.0 LANGUAGES C)  

# Créer la bibliothèque
add_library(mylib SHARED
    src/mylib.c
    src/parser.c
    src/cache.c
)

# Définir la visibilité par défaut à hidden
set_target_properties(mylib PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Générer automatiquement le fichier d'export
include(GenerateExportHeader)  
generate_export_header(mylib  
    BASE_NAME MYLIB
    EXPORT_FILE_NAME include/mylib_export.h
)

# Inclure les headers
target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

**Résultat** : CMake génère automatiquement `mylib_export.h` avec les macros appropriées.

---

## Visibilité et static vs shared

### Bibliothèque statique (.a)

```bash
$ gcc -c -fvisibility=hidden mylib.c -o mylib.o
$ ar rcs libmylib.a mylib.o
```

⚠️ **Important** : La visibilité n'a **aucun effet** sur les bibliothèques statiques.

💡 **Pourquoi ?** Le code est intégré directement dans l'exécutable final, il n'y a pas de résolution dynamique de symboles.

---

### Bibliothèque partagée (.so)

```bash
$ gcc -shared -fPIC -fvisibility=hidden mylib.c -o libmylib.so
```

✅ La visibilité est **cruciale** pour les bibliothèques partagées.

---

## Problèmes courants et solutions

### Problème 1 : Symbole caché mais nécessaire

**Symptôme** :

```bash
$ ./mon_programme
./mon_programme: symbol lookup error: ./libmylib.so: undefined symbol: mylib_helper
```

**Cause** : Une fonction publique appelle une fonction que vous avez cachée.

```c
// mylib.c
__attribute__((visibility("default")))
void mylib_process(void) {
    mylib_helper();  // mylib_helper est hidden ❌
}

__attribute__((visibility("hidden")))
void mylib_helper(void) {
    // ...
}
```

**Solution** : `mylib_helper()` doit être visible, ou alors elle doit être dans le même fichier source.

---

### Problème 2 : Oubli de marquer une fonction publique

**Symptôme** :

```bash
$ gcc main.c -L. -lmylib -o mon_programme
/usr/bin/ld: /tmp/main-abc123.o: undefined reference to `mylib_process'
```

**Cause** : La fonction `mylib_process()` est cachée alors qu'elle devrait être publique.

**Solution** : Ajouter `__attribute__((visibility("default")))` ou `MYLIB_API`.

---

### Problème 3 : Visibilité différente entre déclaration et définition

```c
// mylib.h
__attribute__((visibility("default")))
void mylib_process(void);

// mylib.c
void mylib_process(void) {  // ❌ Pas de visibilité spécifiée !
    // ...
}
```

**Solution** : Utiliser des macros cohérentes partout.

```c
// mylib.h
MYLIB_API void mylib_process(void);

// mylib.c
MYLIB_API void mylib_process(void) {  // ✅
    // ...
}
```

---

## Compatibilité Windows

### Équivalent Windows : __declspec

```c
// mylib_export.h
#ifdef _WIN32
  #ifdef MYLIB_BUILDING
    #define MYLIB_API __declspec(dllexport)
  #else
    #define MYLIB_API __declspec(dllimport)
  #endif
#else
  #if defined(__GNUC__) && __GNUC__ >= 4
    #define MYLIB_API __attribute__((visibility("default")))
  #else
    #define MYLIB_API
  #endif
#endif
```

✅ **Portable** : Fonctionne sur Linux et Windows.

---

## Best practices

### ✅ À faire

1. **Toujours compiler avec -fvisibility=hidden**
   ```bash
   gcc -shared -fPIC -fvisibility=hidden mylib.c -o libmylib.so
   ```

2. **Utiliser des macros pour la portabilité**
   ```c
   #define MYLIB_API __attribute__((visibility("default")))
   ```

3. **Marquer explicitement l'API publique**
   ```c
   MYLIB_API void mylib_init(void);
   MYLIB_API void mylib_process(void);
   ```

4. **Documenter ce qui est public vs privé**
   ```c
   /**
    * @brief Public API function
    * @public
    */
   MYLIB_API void mylib_process(void);

   /**
    * @internal
    * @brief Internal helper function
    */
   static void internal_helper(void);
   ```

5. **Vérifier systématiquement avec nm**
   ```bash
   nm -D libmylib.so | grep " T " | wc -l
   ```

---

### ❌ À éviter

1. **Ne pas contrôler la visibilité**
   ```bash
   # ❌ MAUVAIS
   gcc -shared -fPIC mylib.c -o libmylib.so
   # Tous les symboles sont exportés
   ```

2. **Marquer des fonctions internes comme publiques**
   ```c
   // ❌ MAUVAIS
   MYLIB_API void internal_helper(void);  // Devrait être privée
   ```

3. **Oublier la portabilité Windows**
   ```c
   // ❌ MAUVAIS : Fonctionne uniquement sur Linux
   #define MYLIB_API __attribute__((visibility("default")))
   ```

4. **Ne pas tester sur des systèmes réels**
   - Testez avec plusieurs versions de GCC
   - Testez le chargement dynamique (dlopen)
   - Testez les collisions de symboles

---

## Commandes de vérification

```bash
# 1. Compiler avec visibilité contrôlée
$ gcc -shared -fPIC -fvisibility=hidden mylib.c -o libmylib.so

# 2. Lister les symboles exportés
$ nm -D libmylib.so | grep " T "

# 3. Compter les symboles publics
$ nm -D libmylib.so | grep " T " | wc -l

# 4. Vérifier qu'un symbole spécifique est caché
$ nm -D libmylib.so | grep internal_helper
# Devrait être vide

# 5. Afficher tous les détails
$ objdump -T libmylib.so

# 6. Vérifier la taille de la section dynsym
$ readelf -S libmylib.so | grep dynsym

# 7. Comparer avant/après
$ size libmylib_before.so libmylib_after.so
```

---

## Exemple complet : Bibliothèque professionnelle

### Structure du projet

```
mylib/
├── CMakeLists.txt
├── include/
│   ├── mylib.h
│   └── mylib_export.h.in
├── src/
│   ├── mylib.c
│   ├── parser.c
│   ├── parser.h
│   ├── cache.c
│   └── cache.h
└── tests/
    └── test_mylib.c
```

---

### include/mylib.h

```c
#ifndef MYLIB_H
#define MYLIB_H

#include "mylib_export.h"
#include <stddef.h>

typedef struct mylib_context mylib_context_t;

/**
 * @brief Create a new context
 * @return Pointer to context or NULL on error
 * @public
 */
MYLIB_API mylib_context_t* mylib_create(void);

/**
 * @brief Process data
 * @param ctx Context
 * @param data Input data
 * @param len Data length
 * @return 0 on success, -1 on error
 * @public
 */
MYLIB_API int mylib_process(mylib_context_t *ctx, const char *data, size_t len);

/**
 * @brief Destroy context
 * @param ctx Context to destroy
 * @public
 */
MYLIB_API void mylib_destroy(mylib_context_t *ctx);

#endif
```

---

### include/mylib_export.h.in

```c
#ifndef MYLIB_EXPORT_H
#define MYLIB_EXPORT_H

#ifdef MYLIB_STATIC_DEFINE
  #define MYLIB_API
#else
  #ifdef _WIN32
    #ifdef MYLIB_BUILDING
      #define MYLIB_API __declspec(dllexport)
    #else
      #define MYLIB_API __declspec(dllimport)
    #endif
  #else
    #if defined(__GNUC__) && __GNUC__ >= 4
      #define MYLIB_API __attribute__((visibility("default")))
    #else
      #define MYLIB_API
    #endif
  #endif
#endif

#endif
```

---

### src/mylib.c

```c
#define MYLIB_BUILDING
#include "mylib.h"
#include "parser.h"
#include "cache.h"
#include <stdlib.h>
#include <string.h>

struct mylib_context {
    char *data;
    size_t size;
    void *cache;
};

// API publique
MYLIB_API mylib_context_t* mylib_create(void) {
    mylib_context_t *ctx = malloc(sizeof(mylib_context_t));
    if (!ctx) return NULL;

    ctx->data = NULL;
    ctx->size = 0;
    ctx->cache = cache_create();  // Fonction interne

    return ctx;
}

MYLIB_API int mylib_process(mylib_context_t *ctx, const char *data, size_t len) {
    if (!ctx || !data) return -1;

    // Utiliser les fonctions internes
    if (!parse_input(data, len)) return -1;
    cache_store(ctx->cache, data, len);

    return 0;
}

MYLIB_API void mylib_destroy(mylib_context_t *ctx) {
    if (!ctx) return;

    free(ctx->data);
    cache_destroy(ctx->cache);
    free(ctx);
}
```

---

### src/parser.h (header interne)

```c
#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>

// Fonctions internes (pas de MYLIB_API)
bool parse_input(const char *data, size_t len);  
int parse_config(const char *path);  

#endif
```

---

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)  
project(mylib VERSION 1.0.0 LANGUAGES C)  

# Bibliothèque partagée
add_library(mylib SHARED
    src/mylib.c
    src/parser.c
    src/cache.c
)

# Visibilité par défaut hidden
set_target_properties(mylib PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Headers
target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Flags de compilation
target_compile_options(mylib PRIVATE
    -Wall -Wextra -Werror
    -fvisibility=hidden
)

# Installer
install(TARGETS mylib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES include/mylib.h include/mylib_export.h
    DESTINATION include
)
```

---

### Compilation et vérification

```bash
# Compiler
$ mkdir build && cd build
$ cmake ..
$ make

# Vérifier les symboles
$ nm -D libmylib.so | grep " T "
0000000000001189 T mylib_create
00000000000011e5 T mylib_destroy
0000000000001205 T mylib_process

# Confirmer que les fonctions internes sont cachées
$ nm -D libmylib.so | grep -E "parse|cache"
# Vide ✅
```

---

## Résumé comparatif

| Aspect | Sans visibilité | Avec visibilité |
|--------|-----------------|-----------------|
| **Symboles exportés** | Tous (100+) | Seulement API publique (3-10) |
| **Taille binaire** | Plus grand | Plus petit (-20 à -30%) |
| **Temps de chargement** | Plus lent | Plus rapide (jusqu'à 10x) |
| **Collisions** | Possibles | Évitées |
| **Encapsulation** | Faible | Forte |
| **Optimisations** | Limitées | Meilleures |

---

## Conclusion

La **visibilité des symboles** est un outil essentiel pour créer des bibliothèques partagées professionnelles :

✅ **Avantages** :
- Encapsulation réelle de l'implémentation
- Prévention des collisions de noms
- Amélioration des performances (chargement, exécution)
- Réduction de la taille des binaires
- Meilleure stabilité de l'ABI

💡 **Règle d'or** : Compilez toujours vos bibliothèques avec `-fvisibility=hidden` et marquez explicitement l'API publique.

**Workflow recommandé** :
1. Définir l'API publique dans un header
2. Créer des macros pour la portabilité (`MYLIB_API`)
3. Compiler avec `-fvisibility=hidden`
4. Vérifier avec `nm -D`

---

## Pour aller plus loin

**Sections connexes recommandées** :
- **14.2** : Bibliothèques dynamiques (.so) - Création et SONAME
- **14.6** : Création d'une API propre
- **12.1** : Fichiers d'en-tête et gardes d'inclusion
- **31.8** : Conteneurisation Docker (distribution de bibliothèques)

**Commandes à maîtriser** :
- `nm -D` : Lister les symboles dynamiques
- `objdump -T` : Table des symboles dynamiques
- `readelf -s` : Informations détaillées sur les symboles
- `size` : Taille des sections

**Ressources externes** :
- *GCC Visibility Wiki* : Documentation officielle
- *DSO Howto* (Ulrich Drepper) : Guide complet sur les bibliothèques partagées
- *Linux System Programming* : Chapitre sur les symboles et la liaison dynamique

---

**🎯 Points clés à retenir** :
1. Par défaut, TOUS les symboles sont exportés (mauvais)
2. Utilisez `-fvisibility=hidden` pour cacher par défaut
3. Marquez explicitement l'API publique avec `__attribute__((visibility("default")))`
4. Créez des macros pour la portabilité (Linux + Windows)
5. Vérifiez toujours avec `nm -D` après compilation
6. Moins de symboles = meilleure performance
7. La visibilité permet une vraie encapsulation en C
8. Essentiel pour les bibliothèques partagées, inutile pour les statiques

⏭️ [Débogage et Analyse de Qualité](/15-debogage-et-qualite/README.md)
