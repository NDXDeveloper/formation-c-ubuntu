🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.3 Compilation conditionnelle

## Introduction

La **compilation conditionnelle** permet d'inclure ou d'exclure des portions de code en fonction de conditions évaluées par le préprocesseur. C'est une technique puissante pour :

- Créer du code multi-plateforme (Windows, Linux, macOS)
- Activer/désactiver des fonctionnalités de débogage
- Gérer différentes versions d'un logiciel
- Optimiser le code pour différentes architectures
- Éviter les inclusions multiples de fichiers d'en-tête

Le préprocesseur évalue les conditions **avant** la compilation et ne garde que le code qui satisfait les conditions.

---

## Les directives de base

### #ifdef - "Si défini"

La directive `#ifdef` vérifie si une macro est définie.

**Syntaxe** :
```c
#ifdef NOM_MACRO
    // Code inclus si NOM_MACRO est défini
#endif
```

**Premier exemple** :

```c
#include <stdio.h>

#define DEBUG

int main(void) {
    int x = 42;

#ifdef DEBUG
    printf("Mode debug activé\n");
    printf("Valeur de x: %d\n", x);
#endif

    printf("Programme en cours d'exécution\n");

    return 0;
}
```

**Sortie (avec DEBUG défini)** :
```
Mode debug activé  
Valeur de x: 42  
Programme en cours d'exécution  
```

**Si on commente `#define DEBUG`** :
```c
// #define DEBUG  ← Commenté ou supprimé
```

**Sortie (sans DEBUG)** :
```
Programme en cours d'exécution
```

Le code de débogage n'est simplement **pas compilé** !

### #ifndef - "Si non défini"

La directive `#ifndef` est l'inverse de `#ifdef` : elle vérifie si une macro **n'est pas** définie.

```c
#include <stdio.h>

int main(void) {
#ifndef VERSION
    printf("Version non définie, utilisation de la version par défaut\n");
    #define VERSION "1.0.0"
#endif

    printf("Version du programme: %s\n", VERSION);

    return 0;
}
```

**Sortie** :
```
Version non définie, utilisation de la version par défaut  
Version du programme: 1.0.0  
```

### #else - "Sinon"

Comme en C normal, on peut avoir une branche alternative :

```c
#include <stdio.h>

#define PRODUCTION  // Commentez cette ligne pour tester

int main(void) {
#ifdef PRODUCTION
    printf("Mode PRODUCTION - Logs désactivés\n");
#else
    printf("Mode DÉVELOPPEMENT - Logs activés\n");
    printf("Informations de débogage détaillées...\n");
#endif

    return 0;
}
```

### #endif - Fin de bloc conditionnel

**Important** : Chaque directive conditionnelle doit être fermée par `#endif`.

```c
#ifdef DEBUG
    // Code debug
#endif  ← Obligatoire !
```

---

## Directives avancées

### #if - Évaluation d'expressions

`#if` permet d'évaluer des expressions constantes (nombres entiers uniquement).

```c
#include <stdio.h>

#define VERSION_MAJEURE 2
#define VERSION_MINEURE 5

int main(void) {
#if VERSION_MAJEURE > 1
    printf("Version majeure supérieure à 1\n");
#endif

#if VERSION_MAJEURE == 2 && VERSION_MINEURE >= 5
    printf("Version 2.5 ou supérieure détectée\n");
#endif

    return 0;
}
```

**Opérateurs supportés dans #if** :
- Arithmétiques : `+`, `-`, `*`, `/`, `%`
- Comparaison : `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logiques : `&&`, `||`, `!`
- Bit à bit : `&`, `|`, `^`, `~`, `<<`, `>>`
- Parenthèses : `(` `)`

### #elif - "Sinon si"

Équivalent de `else if` pour les directives de préprocesseur :

```c
#include <stdio.h>

#define NIVEAU_LOG 2
// 0 = Aucun, 1 = Erreurs, 2 = Warnings, 3 = Info, 4 = Debug

int main(void) {
#if NIVEAU_LOG == 0
    printf("Logs désactivés\n");
#elif NIVEAU_LOG == 1
    printf("Logs: Erreurs uniquement\n");
#elif NIVEAU_LOG == 2
    printf("Logs: Erreurs + Warnings\n");
#elif NIVEAU_LOG == 3
    printf("Logs: Erreurs + Warnings + Info\n");
#elif NIVEAU_LOG >= 4
    printf("Logs: Mode complet (Debug)\n");
#else
    printf("Niveau de log invalide\n");
#endif

    return 0;
}
```

### defined() - Opérateur de test

L'opérateur `defined()` teste si une macro est définie. Utilisable dans `#if` et `#elif`.

```c
#include <stdio.h>

#define FEATURE_A
// #define FEATURE_B  ← Non défini

int main(void) {
#if defined(FEATURE_A)
    printf("Feature A activée\n");
#endif

#if defined(FEATURE_B)
    printf("Feature B activée\n");
#else
    printf("Feature B désactivée\n");
#endif

    // Combinaisons logiques
#if defined(FEATURE_A) && !defined(FEATURE_B)
    printf("Seulement Feature A est active\n");
#endif

    return 0;
}
```

**Équivalences** :
```c
#ifdef MACRO     ≡  #if defined(MACRO)
#ifndef MACRO    ≡  #if !defined(MACRO)
```

---

## Guards d'inclusion (Include Guards)

Un usage **crucial** de la compilation conditionnelle : éviter les inclusions multiples d'un même fichier d'en-tête.

### Le problème sans guards

**fichier.h** :
```c
// Pas de guard
struct Point {
    int x;
    int y;
};
```

**main.c** :
```c
#include "fichier.h"
#include "fichier.h"  // Erreur : struct Point redéfinie !

int main(void) {
    return 0;
}
```

**Erreur de compilation** :
```
error: redefinition of 'struct Point'
```

### Solution : Include guards

**fichier.h** (avec guards) :
```c
#ifndef FICHIER_H
#define FICHIER_H

struct Point {
    int x;
    int y;
};

void afficher_point(struct Point p);

#endif  // FICHIER_H
```

**Comment ça fonctionne ?**

1. **Première inclusion** :
   - `FICHIER_H` n'est pas défini
   - Le contenu est inclus
   - `FICHIER_H` est défini

2. **Inclusions suivantes** :
   - `FICHIER_H` est déjà défini
   - Le contenu est ignoré (sauté)

### Convention de nommage des guards

```c
// Fichier: ma_bibliotheque/utils.h
#ifndef MA_BIBLIOTHEQUE_UTILS_H
#define MA_BIBLIOTHEQUE_UTILS_H

// Contenu du header

#endif  // MA_BIBLIOTHEQUE_UTILS_H
```

**Règles** :
- Tout en MAJUSCULES
- Remplacer `/` et `.` par `_`
- Préfixer avec le nom du projet/module pour éviter les collisions
- Terminer par `_H`

### Alternative moderne : #pragma once

Une alternative plus simple (supportée par GCC, Clang, MSVC) :

```c
#pragma once

struct Point {
    int x;
    int y;
};
```

**Avantages** :
- ✅ Plus simple (une seule ligne)
- ✅ Pas de risque de collision de noms
- ✅ Légèrement plus rapide

**Inconvénients** :
- ⚠️ Pas dans le standard C (extension des compilateurs)
- ⚠️ Peut avoir des problèmes avec les liens symboliques

**Recommandation** : Utilisez les include guards classiques pour une portabilité maximale.

---

## Code multi-plateforme

### Détection du système d'exploitation

Les compilateurs définissent automatiquement des macros pour identifier la plateforme :

```c
#include <stdio.h>

int main(void) {
#ifdef _WIN32
    printf("Système: Windows (32 ou 64 bits)\n");
#elif defined(__linux__)
    printf("Système: Linux\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("Système: macOS\n");
#elif defined(__unix__)
    printf("Système: Unix\n");
#else
    printf("Système: Inconnu\n");
#endif

    return 0;
}
```

### Code spécifique à la plateforme

```c
#include <stdio.h>

// Fonction de pause multi-plateforme
void pause_programme(void) {
#ifdef _WIN32
    system("pause");  // Windows: "Appuyez sur une touche..."
#else
    printf("Appuyez sur Entrée pour continuer...");
    getchar();
#endif
}

// Nettoyage de l'écran
void effacer_ecran(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main(void) {
    printf("Bonjour depuis un programme multi-plateforme !\n");
    pause_programme();
    effacer_ecran();
    printf("Écran effacé !\n");

    return 0;
}
```

### Chemins de fichiers

```c
#include <stdio.h>

#ifdef _WIN32
    #define SEPARATEUR '\\'
    #define FICHIER_CONFIG "C:\\Program Files\\MonApp\\config.txt"
#else
    #define SEPARATEUR '/'
    #define FICHIER_CONFIG "/etc/monapp/config.txt"
#endif

int main(void) {
    printf("Séparateur de chemin: %c\n", SEPARATEUR);
    printf("Fichier de config: %s\n", FICHIER_CONFIG);

    return 0;
}
```

---

## Détection du compilateur

```c
#include <stdio.h>

int main(void) {
#if defined(__clang__)
    // Attention : Clang définit aussi __GNUC__, donc tester __clang__ EN PREMIER
    printf("Compilateur: Clang version %d.%d.%d\n",
           __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("Compilateur: GCC version %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("Compilateur: Microsoft Visual C++ version %d\n", _MSC_VER);
#else
    printf("Compilateur: Inconnu\n");
#endif

    return 0;
}
```

### Fonctionnalités spécifiques au compilateur

```c
// Utilisation d'extensions GCC si disponibles
#ifdef __GNUC__
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

// Utilisation
if (LIKELY(pointeur != NULL)) {
    // Cas le plus fréquent
}
```

---

## Débogage conditionnel

### Mode debug activé/désactivé

```c
#include <stdio.h>

// Décommenter pour activer le mode debug
// #define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        printf("[DEBUG] %s:%d:%s(): " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

int calculer_somme(int a, int b) {
    DEBUG_PRINT("Calcul de %d + %d", a, b);
    int resultat = a + b;
    DEBUG_PRINT("Résultat = %d", resultat);
    return resultat;
}

int main(void) {
    printf("Début du programme\n");

    int somme = calculer_somme(10, 20);
    printf("Somme: %d\n", somme);

    DEBUG_PRINT("Programme terminé");

    return 0;
}
```

**Avec DEBUG défini** :
```
Début du programme
[DEBUG] main.c:15:calculer_somme(): Calcul de 10 + 20
[DEBUG] main.c:17:calculer_somme(): Résultat = 30
Somme: 30
[DEBUG] main.c:26:main(): Programme terminé
```

**Sans DEBUG** :
```
Début du programme  
Somme: 30  
```

### Niveaux de débogage

```c
#include <stdio.h>

#define LOG_LEVEL 2
// 0 = Aucun, 1 = Erreur, 2 = Warning, 3 = Info, 4 = Debug

#if LOG_LEVEL >= 1
    #define LOG_ERROR(fmt, ...) \
        printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_ERROR(fmt, ...) do {} while(0)
#endif

#if LOG_LEVEL >= 2
    #define LOG_WARNING(fmt, ...) \
        printf("[WARNING] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_WARNING(fmt, ...) do {} while(0)
#endif

#if LOG_LEVEL >= 3
    #define LOG_INFO(fmt, ...) \
        printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_INFO(fmt, ...) do {} while(0)
#endif

#if LOG_LEVEL >= 4
    #define LOG_DEBUG(fmt, ...) \
        printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...) do {} while(0)
#endif

int main(void) {
    LOG_DEBUG("Démarrage du programme");
    LOG_INFO("Configuration chargée");
    LOG_WARNING("Mémoire limitée détectée");
    LOG_ERROR("Impossible d'ouvrir le fichier");

    return 0;
}
```

**Sortie (LOG_LEVEL = 2)** :
```
[WARNING] Mémoire limitée détectée
[ERROR] Impossible d'ouvrir le fichier
```

---

## Détection de l'architecture

```c
#include <stdio.h>

int main(void) {
    printf("Architecture: ");

#if defined(__x86_64__) || defined(_M_X64)
    printf("x86_64 (64 bits)\n");
#elif defined(__i386__) || defined(_M_IX86)
    printf("x86 (32 bits)\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("ARM64 (64 bits)\n");
#elif defined(__arm__) || defined(_M_ARM)
    printf("ARM (32 bits)\n");
#else
    printf("Inconnue\n");
#endif

    printf("Taille du pointeur: %zu octets\n", sizeof(void*));

    return 0;
}
```

---

## Gestion de versions

### Vérification de version du standard C

```c
#include <stdio.h>

int main(void) {
    printf("Standard C: ");

#if __STDC_VERSION__ >= 202311L
    printf("C23 ou supérieur\n");
#elif __STDC_VERSION__ >= 201710L
    printf("C17\n");
#elif __STDC_VERSION__ >= 201112L
    printf("C11\n");
#elif __STDC_VERSION__ >= 199901L
    printf("C99\n");
#elif defined(__STDC__)
    printf("C89/C90\n");
#else
    printf("Pre-ANSI C\n");
#endif

    return 0;
}
```

### Compatibilité avec différentes versions

```c
#include <stdio.h>

// Utilisation de fonctionnalités C11+ si disponibles
#if __STDC_VERSION__ >= 201112L
    #include <stdatomic.h>
    #define ATOMIC_AVAILABLE 1
#else
    #define ATOMIC_AVAILABLE 0
#endif

int main(void) {
#if ATOMIC_AVAILABLE
    atomic_int compteur = 0;
    printf("Atomics disponibles (C11+)\n");
#else
    int compteur = 0;
    printf("Atomics non disponibles, utilisation d'int standard\n");
#endif

    compteur++;
    printf("Compteur: %d\n", compteur);

    return 0;
}
```

---

## Optimisation conditionnelle

### Code de production vs développement

```c
#include <stdio.h>
#include <assert.h>

// En production, NDEBUG est défini
// #define NDEBUG

#ifndef NDEBUG
    #define VERIFICATION_ACTIVE 1
#else
    #define VERIFICATION_ACTIVE 0
#endif

void traiter_donnees(int* data, size_t taille) {
#if VERIFICATION_ACTIVE
    // Vérifications coûteuses en mode debug uniquement
    printf("Vérification des données...\n");
    assert(data != NULL);
    assert(taille > 0);

    for (size_t i = 0; i < taille; i++) {
        assert(data[i] >= 0);  // Vérifier que toutes les valeurs sont positives
    }
#endif

    // Traitement principal
    printf("Traitement de %zu éléments\n", taille);
}

int main(void) {
    int donnees[] = {1, 2, 3, 4, 5};
    traiter_donnees(donnees, 5);

    return 0;
}
```

### Features flags

```c
#include <stdio.h>

// Activation/désactivation de fonctionnalités
#define FEATURE_ANALYTICS 1
#define FEATURE_CLOUD_SYNC 0
#define FEATURE_EXPORT 1

void sauvegarder_donnees(void) {
    printf("Sauvegarde locale effectuée\n");

#if FEATURE_CLOUD_SYNC
    printf("Synchronisation cloud en cours...\n");
#endif
}

void generer_rapport(void) {
    printf("Génération du rapport...\n");

#if FEATURE_ANALYTICS
    printf("Ajout des statistiques d'usage\n");
#endif

#if FEATURE_EXPORT
    printf("Export en PDF disponible\n");
#endif
}

int main(void) {
    sauvegarder_donnees();
    generer_rapport();

    return 0;
}
```

---

## Macros prédéfinies utiles

### Macros standards du préprocesseur

```c
#include <stdio.h>

int main(void) {
    printf("Fichier: %s\n", __FILE__);
    printf("Ligne: %d\n", __LINE__);
    printf("Date de compilation: %s\n", __DATE__);
    printf("Heure de compilation: %s\n", __TIME__);
    printf("Fonction: %s\n", __func__);  // C99+

#ifdef __STDC__
    printf("Compilateur conforme au standard C\n");
#endif

#ifdef __STDC_VERSION__
    printf("Version du standard: %ld\n", __STDC_VERSION__);
#endif

    return 0;
}
```

**Sortie exemple** :
```
Fichier: main.c  
Ligne: 5  
Date de compilation: Dec 25 2024  
Heure de compilation: 14:30:15  
Fonction: main  
Compilateur conforme au standard C  
Version du standard: 201112  
```

### Utilisation dans les assertions

```c
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion échouée: %s\n", #condition); \
            fprintf(stderr, "Message: %s\n", message); \
            fprintf(stderr, "Fichier: %s, Ligne: %d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

int diviser(int a, int b) {
    ASSERT_MSG(b != 0, "Division par zéro");
    return a / b;
}

int main(void) {
    int resultat = diviser(10, 2);
    printf("10 / 2 = %d\n", resultat);

    // Décommentez pour tester l'assertion
    // resultat = diviser(10, 0);

    return 0;
}
```

---

## Compilation depuis la ligne de commande

Vous pouvez définir des macros directement lors de la compilation avec l'option `-D` :

```bash
# Compiler avec DEBUG défini
gcc -DDEBUG main.c -o main

# Compiler avec une valeur spécifique
gcc -DLOG_LEVEL=3 main.c -o main

# Compiler en mode production (définir NDEBUG)
gcc -DNDEBUG -O2 main.c -o main

# Multiples définitions
gcc -DDEBUG -DFEATURE_A -DVERSION=2 main.c -o main
```

**Exemple de code** :

```c
#include <stdio.h>

int main(void) {
#ifdef DEBUG
    printf("Mode DEBUG activé\n");
#endif

#ifdef FEATURE_A
    printf("Feature A activée\n");
#endif

#ifndef VERSION
    #define VERSION 1
#endif
    printf("Version: %d\n", VERSION);

    return 0;
}
```

**Compilation et tests** :

```bash
# Compilation basique
$ gcc main.c -o main
$ ./main
Version: 1

# Avec DEBUG
$ gcc -DDEBUG main.c -o main
$ ./main
Mode DEBUG activé  
Version: 1  

# Avec DEBUG et FEATURE_A
$ gcc -DDEBUG -DFEATURE_A main.c -o main
$ ./main
Mode DEBUG activé  
Feature A activée  
Version: 1  

# Avec une version spécifique
$ gcc -DVERSION=5 main.c -o main
$ ./main
Version: 5
```

---

## Exemple complet : Configuration de build

Voici un exemple réaliste montrant comment gérer différentes configurations de build :

**config.h** :
```c
#ifndef CONFIG_H
#define CONFIG_H

// Configuration par défaut si aucun type de build n'est spécifié
#if !defined(BUILD_TYPE_DEBUG) && !defined(BUILD_TYPE_RELEASE) && !defined(BUILD_TYPE_TEST)
    #define BUILD_TYPE_DEBUG
#endif

// Configurations selon le type de build
#ifdef BUILD_TYPE_DEBUG
    #define DEBUG 1
    #define OPTIMISATION 0
    #define LOGGING_LEVEL 4
    #define ASSERTIONS_ENABLED 1
    #define PROFILING_ENABLED 1
#elif defined(BUILD_TYPE_RELEASE)
    #define DEBUG 0
    #define OPTIMISATION 3
    #define LOGGING_LEVEL 1
    #define ASSERTIONS_ENABLED 0
    #define PROFILING_ENABLED 0
#elif defined(BUILD_TYPE_TEST)
    #define DEBUG 1
    #define OPTIMISATION 0
    #define LOGGING_LEVEL 3
    #define ASSERTIONS_ENABLED 1
    #define PROFILING_ENABLED 1
#endif

// Macros de logging conditionnelles
#if LOGGING_LEVEL >= 1
    #define LOG_ERROR(msg) printf("[ERROR] %s\n", msg)
#else
    #define LOG_ERROR(msg) do {} while(0)
#endif

#if LOGGING_LEVEL >= 2
    #define LOG_WARNING(msg) printf("[WARNING] %s\n", msg)
#else
    #define LOG_WARNING(msg) do {} while(0)
#endif

#if LOGGING_LEVEL >= 3
    #define LOG_INFO(msg) printf("[INFO] %s\n", msg)
#else
    #define LOG_INFO(msg) do {} while(0)
#endif

#if LOGGING_LEVEL >= 4
    #define LOG_DEBUG(msg) printf("[DEBUG] %s:%d - %s\n", __FILE__, __LINE__, msg)
#else
    #define LOG_DEBUG(msg) do {} while(0)
#endif

// Assertions conditionnelles
#if ASSERTIONS_ENABLED
    #include <assert.h>
    #define ASSERT(cond) assert(cond)
#else
    #define ASSERT(cond) do {} while(0)
#endif

#endif  // CONFIG_H
```

**main.c** :
```c
#include <stdio.h>
#include "config.h"

void fonction_critique(int* ptr) {
    ASSERT(ptr != NULL);
    LOG_DEBUG("Entrée dans fonction_critique");

    *ptr = 42;

    LOG_INFO("Valeur assignée");
}

int main(void) {
    printf("=== Configuration du build ===\n");

#if DEBUG
    printf("Mode: DEBUG\n");
#else
    printf("Mode: RELEASE\n");
#endif

    printf("Niveau de logging: %d\n", LOGGING_LEVEL);
    printf("Optimisation: %d\n", OPTIMISATION);
    printf("Assertions: %s\n", ASSERTIONS_ENABLED ? "Oui" : "Non");
    printf("\n");

    LOG_INFO("Démarrage du programme");

    int valeur = 0;
    fonction_critique(&valeur);

    printf("Valeur finale: %d\n", valeur);

    LOG_INFO("Programme terminé");

    return 0;
}
```

**Compilation** :

```bash
# Build DEBUG (par défaut)
$ gcc main.c -o main_debug
$ ./main_debug
=== Configuration du build ===
Mode: DEBUG  
Niveau de logging: 4  
Optimisation: 0  
Assertions: Oui  

[INFO] Démarrage du programme
[DEBUG] main.c:8 - Entrée dans fonction_critique
[INFO] Valeur assignée
Valeur finale: 42
[INFO] Programme terminé

# Build RELEASE
$ gcc -DBUILD_TYPE_RELEASE main.c -o main_release -O3
$ ./main_release
=== Configuration du build ===
Mode: RELEASE  
Niveau de logging: 1  
Optimisation: 3  
Assertions: Non  

Valeur finale: 42

# Build TEST
$ gcc -DBUILD_TYPE_TEST main.c -o main_test
$ ./main_test
=== Configuration du build ===
Mode: DEBUG  
Niveau de logging: 3  
Optimisation: 0  
Assertions: Oui  

[INFO] Démarrage du programme
[INFO] Valeur assignée
Valeur finale: 42
[INFO] Programme terminé
```

---

## Bonnes pratiques

### 1. Toujours utiliser des include guards

```c
// ✅ Bon
#ifndef MON_HEADER_H
#define MON_HEADER_H

// Contenu

#endif

// ❌ Mauvais - Sans guard
// Risque de redéfinitions
```

### 2. Nommer clairement les macros de configuration

```c
// ✅ Bon - Noms explicites
#define ENABLE_LOGGING
#define BUILD_MODE_DEBUG
#define FEATURE_CLOUD_SYNC

// ❌ Mauvais - Noms ambigus
#define LOG
#define DBG
#define SYNC
```

### 3. Documenter les configurations

```c
/**
 * Configuration du système de logging
 *
 * Définir LOG_LEVEL pour contrôler la verbosité:
 * - 0: Aucun log
 * - 1: Erreurs uniquement
 * - 2: Erreurs + Warnings
 * - 3: Erreurs + Warnings + Info
 * - 4: Tout (Debug)
 *
 * Exemple: gcc -DLOG_LEVEL=2 main.c
 */
#ifndef LOG_LEVEL
    #define LOG_LEVEL 1  // Par défaut: erreurs uniquement
#endif
```

### 4. Éviter les niveaux d'imbrication excessifs

```c
// ❌ Mauvais - Difficile à lire
#ifdef PLATFORM_WINDOWS
    #ifdef COMPILER_MSVC
        #ifdef BUILD_DEBUG
            #ifdef FEATURE_X
                // Code
            #endif
        #endif
    #endif
#endif

// ✅ Meilleur - Plus plat
#if defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC) && \
    defined(BUILD_DEBUG) && defined(FEATURE_X)
    // Code
#endif
```

### 5. Fournir des valeurs par défaut

```c
// ✅ Bon - Valeur par défaut si non défini
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 1024
#endif

#ifndef MAX_CONNECTIONS
    #define MAX_CONNECTIONS 100
#endif
```

---

## Pièges à éviter

### 1. Oublier #endif

```c
#ifdef DEBUG
    printf("Debug\n");
// ❌ Manque #endif → Erreur de compilation
```

### 2. Confondre #if et #ifdef

```c
#define FLAG 0

// ❌ Mauvais - FLAG est défini (même si = 0)
#ifdef FLAG
    // Ce code est INCLUS car FLAG existe
#endif

// ✅ Bon - Teste la valeur
#if FLAG
    // Ce code est EXCLU car FLAG = 0
#endif
```

### 3. Effets de bord dans les conditions

```c
// ❌ Mauvais - Le préprocesseur n'exécute pas de code
#if fonction_test()  // Erreur : fonction non appelable par le préprocesseur
    // ...
#endif

// ✅ Bon - Seulement des constantes
#if VERSION >= 2
    // ...
#endif
```

---

## Résumé

### Directives principales

| Directive | Description | Exemple |
|-----------|-------------|---------|
| `#ifdef` | Si défini | `#ifdef DEBUG` |
| `#ifndef` | Si non défini | `#ifndef VERSION` |
| `#if` | Si condition vraie | `#if VERSION >= 2` |
| `#elif` | Sinon si | `#elif VERSION == 1` |
| `#else` | Sinon | `#else` |
| `#endif` | Fin de bloc | `#endif` |
| `defined()` | Test d'existence | `#if defined(MACRO)` |

### Cas d'usage principaux

- ✅ **Include guards** : Protection contre les inclusions multiples
- ✅ **Débogage** : Code de debug conditionnel
- ✅ **Multi-plateforme** : Code spécifique OS/compilateur
- ✅ **Feature flags** : Activation/désactivation de fonctionnalités
- ✅ **Optimisation** : Différentes configurations de build
- ✅ **Compatibilité** : Support de différentes versions du standard

### Points clés

- La compilation conditionnelle se fait **avant** la compilation
- Le code exclu n'est **jamais compilé** (zéro overhead)
- Utilisez `-D` en ligne de commande pour définir des macros
- Toujours fermer avec `#endif`
- Préférez `defined()` avec `#if` pour plus de flexibilité

---

**Prochaine section** : 23.4 Macros cross-platform (techniques avancées pour la portabilité)

⏭️ [Macros cross-platform](/23-macros-preprocesseur/04-macros-cross-platform.md)
