🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.6 Macros prédéfinies utiles

## Introduction

Les **macros prédéfinies** sont des macros automatiquement définies par le préprocesseur C et le compilateur. Elles fournissent des informations précieuses sur :
- Le fichier source en cours de compilation
- La ligne de code actuelle
- La date et l'heure de compilation
- Le compilateur utilisé
- Le système d'exploitation cible
- Les fonctionnalités disponibles

Ces macros sont extrêmement utiles pour le débogage, le logging, la portabilité et la gestion de versions.

> **Bonne pratique** : Ces macros sont votre source d'information fiable sur l'environnement de compilation. Utilisez-les abondamment pour créer des outils de développement robustes.

---

## 1. Macros standards du langage C

Ces macros sont définies par le standard C et disponibles sur tous les compilateurs conformes.

### __FILE__ - Nom du fichier source

Retourne le nom du fichier source actuel sous forme de chaîne de caractères.

```c
#include <stdio.h>

int main(void) {
    printf("Ce code est dans le fichier : %s\n", __FILE__);

    return 0;
}
```

**Sortie** (si le fichier s'appelle `main.c`) :
```
Ce code est dans le fichier : main.c
```

**Utilisation pratique** : Logging avec le nom du fichier

```c
#include <stdio.h>

#define LOG(msg) \
    printf("[%s] %s\n", __FILE__, msg)

int main(void) {
    LOG("Démarrage du programme");
    LOG("Initialisation terminée");

    return 0;
}
```

### __LINE__ - Numéro de ligne

Retourne le numéro de ligne actuel dans le fichier source (entier).

```c
#include <stdio.h>

int main(void) {
    printf("Ligne actuelle : %d\n", __LINE__);  // Affiche le numéro de cette ligne
    printf("Ligne actuelle : %d\n", __LINE__);  // Affiche le numéro de cette ligne
    printf("Ligne actuelle : %d\n", __LINE__);  // Affiche le numéro de cette ligne

    return 0;
}
```

**Sortie** :
```
Ligne actuelle : 4  
Ligne actuelle : 5  
Ligne actuelle : 6  
```

**Utilisation pratique** : Traçage précis des erreurs

```c
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion échouée à %s:%d: %s\n", \
                    __FILE__, __LINE__, message); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

int diviser(int a, int b) {
    ASSERT(b != 0, "Division par zéro");
    return a / b;
}

int main(void) {
    printf("10 / 2 = %d\n", diviser(10, 2));

    // Décommentez pour tester l'assertion
    // printf("10 / 0 = %d\n", diviser(10, 0));

    return 0;
}
```

### __DATE__ - Date de compilation

Retourne la date de compilation sous forme de chaîne (format "Mmm dd yyyy").

```c
#include <stdio.h>

int main(void) {
    printf("Programme compilé le : %s\n", __DATE__);

    return 0;
}
```

**Sortie** (exemple) :
```
Programme compilé le : Dec 25 2024
```

### __TIME__ - Heure de compilation

Retourne l'heure de compilation sous forme de chaîne (format "hh:mm:ss").

```c
#include <stdio.h>

int main(void) {
    printf("Programme compilé à : %s\n", __TIME__);

    return 0;
}
```

**Sortie** (exemple) :
```
Programme compilé à : 14:32:45
```

### Combinaison DATE et TIME

```c
#include <stdio.h>

int main(void) {
    printf("Version du programme\n");
    printf("====================\n");
    printf("Compilé le : %s à %s\n", __DATE__, __TIME__);

    return 0;
}
```

**Sortie** :
```
Version du programme
====================
Compilé le : Dec 25 2024 à 14:32:45
```

### __func__ - Nom de la fonction (C99)

Retourne le nom de la fonction actuelle. C'est une variable prédéfinie, pas une macro du préprocesseur.

```c
#include <stdio.h>

void fonction_exemple(void) {
    printf("Dans la fonction : %s\n", __func__);
}

int main(void) {
    printf("Dans la fonction : %s\n", __func__);
    fonction_exemple();

    return 0;
}
```

**Sortie** :
```
Dans la fonction : main  
Dans la fonction : fonction_exemple  
```

**Note** : Avant C99, certains compilateurs utilisaient `__FUNCTION__` (GCC, MSVC).

---

## 2. Macros de version du standard C

Ces macros indiquent quelle version du standard C est utilisée.

### __STDC__ - Conformité au standard

Définie si le compilateur est conforme au standard C.

```c
#include <stdio.h>

int main(void) {
#ifdef __STDC__
    printf("Compilateur conforme au standard C\n");
#else
    printf("Compilateur non standard\n");
#endif

    return 0;
}
```

### __STDC_VERSION__ - Version du standard

Indique quelle version du standard C est supportée :

| Valeur | Standard |
|--------|----------|
| `199409L` | C95 |
| `199901L` | C99 |
| `201112L` | C11 |
| `201710L` | C17 |
| `202311L` | C23 |

```c
#include <stdio.h>

int main(void) {
    printf("Version du standard C : ");

#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 202311L
        printf("C23 ou supérieur\n");
    #elif __STDC_VERSION__ >= 201710L
        printf("C17\n");
    #elif __STDC_VERSION__ >= 201112L
        printf("C11\n");
    #elif __STDC_VERSION__ >= 199901L
        printf("C99\n");
    #else
        printf("C95\n");
    #endif
#else
    printf("C89/C90 (ou pre-ANSI)\n");
#endif

    return 0;
}
```

### Utilisation pour la compatibilité

```c
#include <stdio.h>

// Utiliser des fonctionnalités C11 si disponibles
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #include <stdatomic.h>
    #define HAS_ATOMICS 1
    typedef atomic_int atomic_counter_t;
#else
    #define HAS_ATOMICS 0
    typedef int atomic_counter_t;
#endif

int main(void) {
    atomic_counter_t compteur = 0;

#if HAS_ATOMICS
    printf("Utilisation des atomics C11\n");
#else
    printf("Atomics non disponibles, utilisation d'int standard\n");
#endif

    compteur++;
    printf("Compteur : %d\n", compteur);

    return 0;
}
```

---

## 3. Macros de détection du compilateur

### GCC

```c
#include <stdio.h>

int main(void) {
#ifdef __GNUC__
    printf("Compilateur : GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    return 0;
}
```

**Macros GCC** :
- `__GNUC__` : Version majeure
- `__GNUC_MINOR__` : Version mineure
- `__GNUC_PATCHLEVEL__` : Patch level

### Clang

```c
#include <stdio.h>

int main(void) {
#ifdef __clang__
    printf("Compilateur : Clang %d.%d.%d\n",
           __clang_major__, __clang_minor__, __clang_patchlevel__);
#endif

    return 0;
}
```

**Macros Clang** :
- `__clang__` : Définie si c'est Clang
- `__clang_major__` : Version majeure
- `__clang_minor__` : Version mineure
- `__clang_patchlevel__` : Patch level

### MSVC (Microsoft Visual C++)

```c
#include <stdio.h>

int main(void) {
#ifdef _MSC_VER
    printf("Compilateur : MSVC version %d\n", _MSC_VER);
#endif

    return 0;
}
```

**Macros MSVC** :
- `_MSC_VER` : Version du compilateur (ex: 1900 pour VS 2015)
- `_MSC_FULL_VER` : Version complète

### Exemple de détection universelle

```c
#include <stdio.h>

void afficher_compilateur(void) {
    printf("Compilateur : ");

#if defined(__clang__)
    printf("Clang %d.%d.%d\n",
           __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("MSVC %d\n", _MSC_VER);
#elif defined(__INTEL_COMPILER)
    printf("Intel C++ %d\n", __INTEL_COMPILER);
#else
    printf("Inconnu\n");
#endif
}

int main(void) {
    afficher_compilateur();
    return 0;
}
```

---

## 4. Macros de détection de plateforme

### Système d'exploitation

```c
#include <stdio.h>

void afficher_os(void) {
    printf("Système d'exploitation : ");

#if defined(_WIN32) || defined(_WIN64)
    printf("Windows");
    #ifdef _WIN64
        printf(" (64 bits)");
    #else
        printf(" (32 bits)");
    #endif
    printf("\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("macOS\n");
#elif defined(__linux__)
    printf("Linux\n");
#elif defined(__FreeBSD__)
    printf("FreeBSD\n");
#elif defined(__unix__) || defined(__unix)
    printf("Unix\n");
#else
    printf("Inconnu\n");
#endif
}

int main(void) {
    afficher_os();
    return 0;
}
```

**Macros principales** :
- Windows : `_WIN32`, `_WIN64`
- Linux : `__linux__`, `__gnu_linux__`
- macOS : `__APPLE__` et `__MACH__`
- Unix : `__unix__`, `__unix`
- FreeBSD : `__FreeBSD__`

### Architecture du processeur

```c
#include <stdio.h>

void afficher_architecture(void) {
    printf("Architecture : ");

#if defined(__x86_64__) || defined(_M_X64)
    printf("x86_64 (64 bits)\n");
#elif defined(__i386__) || defined(_M_IX86)
    printf("x86 (32 bits)\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("ARM64\n");
#elif defined(__arm__) || defined(_M_ARM)
    printf("ARM (32 bits)\n");
#elif defined(__mips__)
    printf("MIPS\n");
#elif defined(__powerpc__) || defined(__ppc__)
    printf("PowerPC\n");
#else
    printf("Inconnue\n");
#endif
}

int main(void) {
    afficher_architecture();
    printf("Taille du pointeur : %zu octets\n", sizeof(void*));

    return 0;
}
```

---

## 5. Macros d'assertion et de débogage

### __COUNTER__ (Extension GCC/Clang/MSVC)

Génère un compteur qui s'incrémente à chaque utilisation.

```c
#include <stdio.h>

int main(void) {
    printf("Compteur 1 : %d\n", __COUNTER__);  // 0
    printf("Compteur 2 : %d\n", __COUNTER__);  // 1
    printf("Compteur 3 : %d\n", __COUNTER__);  // 2
    printf("Compteur 4 : %d\n", __COUNTER__);  // 3

    return 0;
}
```

**Utilisation** : Générer des identifiants uniques automatiquement.

```c
#include <stdio.h>

// Macros d'indirection nécessaires : ## empêche l'expansion de ses opérandes
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define DECLARE_VAR(prefix) \
    int CONCAT(prefix, __COUNTER__)

int main(void) {
    DECLARE_VAR(var);  // int var0
    DECLARE_VAR(var);  // int var1
    DECLARE_VAR(var);  // int var2

    var0 = 10;
    var1 = 20;
    var2 = 30;

    printf("%d %d %d\n", var0, var1, var2);

    return 0;
}
```

### NDEBUG - Désactiver les assertions

La macro `NDEBUG` désactive les assertions de `<assert.h>`.

```c
#include <stdio.h>
#include <assert.h>

// Décommentez pour désactiver les assertions
// #define NDEBUG

int diviser(int a, int b) {
    assert(b != 0 && "Division par zéro");
    return a / b;
}

int main(void) {
#ifdef NDEBUG
    printf("Mode RELEASE : Assertions désactivées\n");
#else
    printf("Mode DEBUG : Assertions activées\n");
#endif

    printf("10 / 2 = %d\n", diviser(10, 2));

    // Décommentez pour tester l'assertion
    // printf("10 / 0 = %d\n", diviser(10, 0));

    return 0;
}
```

**Compilation** :
```bash
# Mode debug (assertions activées)
gcc main.c -o main_debug

# Mode release (assertions désactivées)
gcc -DNDEBUG main.c -o main_release
```

---

## 6. Création de macros de logging avancées

### Système de logging complet

```c
#include <stdio.h>
#include <time.h>

// Niveaux de log
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

// Niveau de log global
static LogLevel g_log_level = LOG_DEBUG;

// Fonction d'aide pour obtenir l'heure actuelle
static void get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Macros de logging avec toutes les informations
#define LOG(level, format, ...) \
    do { \
        if (level >= g_log_level) { \
            char timestamp[20]; \
            get_timestamp(timestamp, sizeof(timestamp)); \
            const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"}; \
            fprintf(stderr, "[%s] [%s] %s:%d:%s() - " format "\n", \
                    timestamp, level_str[level], \
                    __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        } \
    } while(0)

#define LOG_DEBUG(format, ...)   LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    LOG(LOG_INFO, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)   LOG(LOG_ERROR, format, ##__VA_ARGS__)

// Fonction de démonstration
void fonction_exemple(int valeur) {
    LOG_DEBUG("Entrée dans la fonction avec valeur=%d", valeur);

    if (valeur < 0) {
        LOG_WARNING("Valeur négative détectée : %d", valeur);
    } else if (valeur == 0) {
        LOG_ERROR("Valeur nulle non autorisée");
        return;
    } else {
        LOG_INFO("Traitement de la valeur %d", valeur);
    }

    LOG_DEBUG("Sortie de la fonction");
}

int main(void) {
    LOG_INFO("Démarrage du programme");

    fonction_exemple(10);
    fonction_exemple(-5);
    fonction_exemple(0);

    LOG_INFO("Programme terminé");

    return 0;
}
```

**Sortie exemple** :
```
[2024-12-25 14:35:20] [INFO] main.c:50:main() - Démarrage du programme
[2024-12-25 14:35:20] [DEBUG] main.c:35:fonction_exemple() - Entrée dans la fonction avec valeur=10
[2024-12-25 14:35:20] [INFO] main.c:42:fonction_exemple() - Traitement de la valeur 10
[2024-12-25 14:35:20] [DEBUG] main.c:45:fonction_exemple() - Sortie de la fonction
[2024-12-25 14:35:20] [DEBUG] main.c:35:fonction_exemple() - Entrée dans la fonction avec valeur=-5
[2024-12-25 14:35:20] [WARN] main.c:38:fonction_exemple() - Valeur négative détectée : -5
[2024-12-25 14:35:20] [DEBUG] main.c:45:fonction_exemple() - Sortie de la fonction
[2024-12-25 14:35:20] [DEBUG] main.c:35:fonction_exemple() - Entrée dans la fonction avec valeur=0
[2024-12-25 14:35:20] [ERROR] main.c:40:fonction_exemple() - Valeur nulle non autorisée
[2024-12-25 14:35:20] [INFO] main.c:55:main() - Programme terminé
```

---

## 7. Macros pour la gestion des erreurs

### Système de gestion d'erreurs avec contexte

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Macro pour vérifier les pointeurs
#define CHECK_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "ERREUR [%s:%d:%s()] - Pointeur NULL\n", \
                    __FILE__, __LINE__, __func__); \
            return -1; \
        } \
    } while(0)

// Macro pour vérifier les codes de retour
#define CHECK_ERROR(expr, msg) \
    do { \
        if ((expr) != 0) { \
            fprintf(stderr, "ERREUR [%s:%d:%s()] - %s: %s\n", \
                    __FILE__, __LINE__, __func__, msg, strerror(errno)); \
            return -1; \
        } \
    } while(0)

// Macro pour les erreurs fatales
#define FATAL_ERROR(msg) \
    do { \
        fprintf(stderr, "ERREUR FATALE [%s:%d:%s()] - %s\n", \
                __FILE__, __LINE__, __func__, msg); \
        exit(EXIT_FAILURE); \
    } while(0)

// Exemple d'utilisation
int traiter_fichier(const char* nom_fichier) {
    CHECK_NULL(nom_fichier);

    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "ERREUR [%s:%d:%s()] - Impossible d'ouvrir '%s': %s\n",
                __FILE__, __LINE__, __func__, nom_fichier, strerror(errno));
        return -1;
    }

    // Traitement...
    printf("Fichier '%s' ouvert avec succès\n", nom_fichier);

    fclose(fichier);
    return 0;
}

int main(void) {
    printf("Test de gestion d'erreurs\n\n");

    // Test avec un fichier qui n'existe probablement pas
    if (traiter_fichier("fichier_inexistant.txt") != 0) {
        printf("Le traitement a échoué (comportement attendu)\n");
    }

    return 0;
}
```

---

## 8. Macros de profilage et mesure de performance

### Mesure du temps d'exécution

```c
#include <stdio.h>
#include <time.h>

// Macro pour mesurer le temps d'exécution d'un bloc de code
#define TIME_IT(name, code) \
    do { \
        clock_t start = clock(); \
        code; \
        clock_t end = clock(); \
        double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC; \
        printf("[PERF] %s pris %.6f secondes (ligne %d)\n", \
               name, cpu_time, __LINE__); \
    } while(0)

// Fonction à profiler
void calcul_intensif(int iterations) {
    volatile long long somme = 0;
    for (int i = 0; i < iterations; i++) {
        somme += i;
    }
}

int main(void) {
    printf("Tests de performance\n\n");

    TIME_IT("Calcul 1M", calcul_intensif(1000000));
    TIME_IT("Calcul 10M", calcul_intensif(10000000));
    TIME_IT("Calcul 100M", calcul_intensif(100000000));

    return 0;
}
```

**Sortie exemple** :
```
Tests de performance

[PERF] Calcul 1M pris 0.002345 secondes (ligne 24)
[PERF] Calcul 10M pris 0.023456 secondes (ligne 25)
[PERF] Calcul 100M pris 0.234567 secondes (ligne 26)
```

---

## 9. Informations de build complètes

### Macro pour afficher toutes les informations

```c
#include <stdio.h>

void afficher_build_info(void) {
    printf("========================================\n");
    printf("Informations de Build\n");
    printf("========================================\n\n");

    // Informations de compilation
    printf("Date de compilation : %s\n", __DATE__);
    printf("Heure de compilation : %s\n", __TIME__);
    printf("Fichier : %s\n", __FILE__);
    printf("\n");

    // Standard C
    printf("Standard C : ");
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 202311L
        printf("C23+\n");
    #elif __STDC_VERSION__ >= 201710L
        printf("C17\n");
    #elif __STDC_VERSION__ >= 201112L
        printf("C11\n");
    #elif __STDC_VERSION__ >= 199901L
        printf("C99\n");
    #else
        printf("C95\n");
    #endif
#else
    printf("C89/C90\n");
#endif
    printf("\n");

    // Compilateur
    printf("Compilateur : ");
#if defined(__clang__)
    printf("Clang %d.%d.%d\n", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("MSVC %d\n", _MSC_VER);
#else
    printf("Inconnu\n");
#endif
    printf("\n");

    // Système d'exploitation
    printf("OS cible : ");
#if defined(_WIN32)
    printf("Windows");
    #ifdef _WIN64
        printf(" 64-bit\n");
    #else
        printf(" 32-bit\n");
    #endif
#elif defined(__APPLE__)
    printf("macOS\n");
#elif defined(__linux__)
    printf("Linux\n");
#elif defined(__unix__)
    printf("Unix\n");
#else
    printf("Inconnu\n");
#endif
    printf("\n");

    // Architecture
    printf("Architecture : ");
#if defined(__x86_64__) || defined(_M_X64)
    printf("x86_64\n");
#elif defined(__i386__) || defined(_M_IX86)
    printf("x86\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("ARM64\n");
#elif defined(__arm__) || defined(_M_ARM)
    printf("ARM\n");
#else
    printf("Inconnue\n");
#endif

    printf("Taille du pointeur : %zu octets\n", sizeof(void*));
    printf("\n");

    // Configuration de build
    printf("Configuration : ");
#ifdef NDEBUG
    printf("Release (optimisé)\n");
#else
    printf("Debug\n");
#endif

    printf("========================================\n");
}

int main(void) {
    afficher_build_info();
    return 0;
}
```

**Sortie exemple** :
```
========================================
Informations de Build
========================================

Date de compilation : Dec 25 2024  
Heure de compilation : 14:42:18  
Fichier : main.c  

Standard C : C11

Compilateur : GCC 11.4.0

OS cible : Linux

Architecture : x86_64  
Taille du pointeur : 8 octets  

Configuration : Debug
========================================
```

---

## 10. Macros conditionnelles pratiques

### Détection de fonctionnalités

```c
#include <stdio.h>

int main(void) {
    printf("Fonctionnalités disponibles :\n\n");

    // Threads POSIX
#ifdef __STDC_NO_THREADS__
    printf("❌ Threads C11 NON disponibles\n");
#else
    printf("✓ Threads C11 disponibles\n");
#endif

    // Atomics
#ifdef __STDC_NO_ATOMICS__
    printf("❌ Atomics NON disponibles\n");
#else
    printf("✓ Atomics disponibles\n");
#endif

    // VLA (Variable Length Arrays)
#ifdef __STDC_NO_VLA__
    printf("❌ VLA NON disponibles\n");
#else
    printf("✓ VLA disponibles\n");
#endif

    // Complex numbers
#ifdef __STDC_NO_COMPLEX__
    printf("❌ Nombres complexes NON disponibles\n");
#else
    printf("✓ Nombres complexes disponibles\n");
#endif

    return 0;
}
```

---

## 11. Création d'une macro VERSION

### Système de version automatique

```c
#include <stdio.h>

// Définir les informations de version
#define VERSION_MAJOR 2
#define VERSION_MINOR 5
#define VERSION_PATCH 3

// Créer une chaîne de version
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define VERSION_STRING \
    TO_STRING(VERSION_MAJOR) "." \
    TO_STRING(VERSION_MINOR) "." \
    TO_STRING(VERSION_PATCH)

// Numéro de version sous forme d'entier
#define VERSION_NUMBER \
    ((VERSION_MAJOR * 10000) + (VERSION_MINOR * 100) + VERSION_PATCH)

// Build info complète
#define BUILD_INFO \
    "v" VERSION_STRING " (compilé le " __DATE__ " à " __TIME__ ")"

void afficher_version(void) {
    printf("Programme version %s\n", VERSION_STRING);
    printf("Build: %s\n", BUILD_INFO);
    printf("Version numérique: %d\n", VERSION_NUMBER);
}

// Vérification de version minimale
#define VERSION_AT_LEAST(major, minor, patch) \
    (VERSION_NUMBER >= ((major) * 10000 + (minor) * 100 + (patch)))

int main(void) {
    afficher_version();
    printf("\n");

    // Tests de version
    if (VERSION_AT_LEAST(2, 5, 0)) {
        printf("Version 2.5.0 ou supérieure\n");
    }

    if (VERSION_AT_LEAST(3, 0, 0)) {
        printf("Version 3.0.0 ou supérieure\n");
    } else {
        printf("Version inférieure à 3.0.0\n");
    }

    return 0;
}
```

**Sortie** :
```
Programme version 2.5.3  
Build: v2.5.3 (compilé le Dec 25 2024 à 14:45:30)  
Version numérique: 20503  

Version 2.5.0 ou supérieure  
Version inférieure à 3.0.0  
```

---

## 12. Tableau récapitulatif des macros essentielles

### Macros standards du langage

| Macro | Type | Description | Exemple |
|-------|------|-------------|---------|
| `__FILE__` | Chaîne | Nom du fichier source | `"main.c"` |
| `__LINE__` | Entier | Numéro de ligne | `42` |
| `__DATE__` | Chaîne | Date de compilation | `"Dec 25 2024"` |
| `__TIME__` | Chaîne | Heure de compilation | `"14:32:45"` |
| `__func__` | Chaîne | Nom de la fonction (C99) | `"main"` |
| `__STDC__` | Définie | Conformité au standard | `1` |
| `__STDC_VERSION__` | Entier | Version du standard | `201112L` (C11) |

### Macros de compilateur

| Compilateur | Macros principales |
|-------------|-------------------|
| **GCC** | `__GNUC__`, `__GNUC_MINOR__`, `__GNUC_PATCHLEVEL__` |
| **Clang** | `__clang__`, `__clang_major__`, `__clang_minor__` |
| **MSVC** | `_MSC_VER`, `_MSC_FULL_VER` |
| **Intel** | `__INTEL_COMPILER` |

### Macros de plateforme

| Plateforme | Macros principales |
|------------|-------------------|
| **Windows** | `_WIN32`, `_WIN64` |
| **Linux** | `__linux__`, `__gnu_linux__` |
| **macOS** | `__APPLE__`, `__MACH__` |
| **Unix** | `__unix__`, `__unix` |
| **FreeBSD** | `__FreeBSD__` |

### Macros d'architecture

| Architecture | Macros principales |
|--------------|-------------------|
| **x86_64** | `__x86_64__`, `_M_X64` |
| **x86** | `__i386__`, `_M_IX86` |
| **ARM64** | `__aarch64__`, `_M_ARM64` |
| **ARM** | `__arm__`, `_M_ARM` |

---

## 13. Bonnes pratiques

### 1. Créer un fichier d'en-tête centralisé

**build_info.h** :
```c
#ifndef BUILD_INFO_H
#define BUILD_INFO_H

#include <stdio.h>

// Version
#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 0
#define APP_VERSION_PATCH 0

// Informations de build
#define APP_BUILD_DATE __DATE__
#define APP_BUILD_TIME __TIME__

// Fonction pour afficher les infos
static inline void print_build_info(void) {
    printf("Version: %d.%d.%d\n",
           APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);
    printf("Build: %s %s\n", APP_BUILD_DATE, APP_BUILD_TIME);

    #ifdef __GNUC__
        printf("Compilateur: GCC %d.%d.%d\n",
               __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    #endif
}

#endif  // BUILD_INFO_H
```

### 2. Utiliser pour le débogage conditionnel

```c
#include <stdio.h>

// Activer avec -DDEBUG_VERBOSE lors de la compilation
#ifdef DEBUG_VERBOSE
    #define TRACE() \
        printf("[TRACE] %s:%d:%s()\n", __FILE__, __LINE__, __func__)
    #define TRACE_VAR(var, fmt) \
        printf("[TRACE] %s:%d:%s() - " #var " = " fmt "\n", \
               __FILE__, __LINE__, __func__, var)
#else
    #define TRACE() ((void)0)
    #define TRACE_VAR(var, fmt) ((void)0)
#endif

void fonction_exemple(int x) {
    TRACE();
    TRACE_VAR(x, "%d");

    x *= 2;

    TRACE_VAR(x, "%d");
}

int main(void) {
    TRACE();
    fonction_exemple(5);
    return 0;
}
```

### 3. Documentation automatique

```c
#include <stdio.h>

#define PRINT_MACRO_INFO(name) \
    printf(#name " = %s\n", name)

#define PRINT_MACRO_VALUE(name) \
    printf(#name " = %d\n", name)

int main(void) {
    printf("Informations de configuration\n\n");

    PRINT_MACRO_INFO(__DATE__);
    PRINT_MACRO_INFO(__TIME__);
    PRINT_MACRO_INFO(__FILE__);
    PRINT_MACRO_VALUE(__LINE__);

    return 0;
}
```

---

## 14. Exemple complet : Framework de diagnostic

```c
#include <stdio.h>
#include <string.h>
#include <time.h>

// ========== Configuration ==========
#define APP_NAME "MonApplication"
#define APP_VERSION "1.2.3"

// ========== Macros de diagnostic ==========

// Timestamp
#define TIMESTAMP() \
    ({ \
        time_t now = time(NULL); \
        char* ts = ctime(&now); \
        ts[strlen(ts)-1] = '\0'; \
        ts; \
    })

// Log avec contexte complet
#define LOG_FULL(level, msg) \
    printf("[%s] [%s] %s:%d:%s() - %s\n", \
           TIMESTAMP(), level, __FILE__, __LINE__, __func__, msg)

#define INFO(msg)    LOG_FULL("INFO", msg)
#define WARNING(msg) LOG_FULL("WARN", msg)
#define ERROR(msg)   LOG_FULL("ERROR", msg)

// Afficher les infos de build
#define PRINT_BUILD_INFO() \
    do { \
        printf("\n"); \
        printf("╔════════════════════════════════════════╗\n"); \
        printf("║  %s v%s\n", APP_NAME, APP_VERSION); \
        printf("╠════════════════════════════════════════╣\n"); \
        printf("║  Build: %s %s\n", __DATE__, __TIME__); \
        printf("║  Fichier: %s\n", __FILE__); \
        printf("╚════════════════════════════════════════╝\n"); \
        printf("\n"); \
    } while(0)

// ========== Programme principal ==========

void traiter_donnees(int valeur) {
    INFO("Début du traitement");

    if (valeur < 0) {
        WARNING("Valeur négative détectée");
    }

    if (valeur == 0) {
        ERROR("Valeur zéro non autorisée");
        return;
    }

    printf("Traitement de la valeur: %d\n", valeur);

    INFO("Traitement terminé");
}

int main(void) {
    PRINT_BUILD_INFO();

    INFO("Démarrage du programme");

    traiter_donnees(10);
    traiter_donnees(-5);
    traiter_donnees(0);

    INFO("Programme terminé");

    return 0;
}
```

---

## Résumé

### Points clés

🔑 **Macros essentielles** :
- `__FILE__` et `__LINE__` : Débogage et traçabilité
- `__DATE__` et `__TIME__` : Informations de build
- `__func__` : Identification de fonction
- `__STDC_VERSION__` : Compatibilité de version

🔑 **Détection** :
- Utilisez les macros de compilateur pour la compatibilité
- Utilisez les macros de plateforme pour la portabilité
- Créez vos propres macros pour centraliser la détection

🔑 **Logging et débogage** :
- Combinez les macros pour créer des systèmes de logging riches
- Utilisez la compilation conditionnelle pour activer/désactiver
- Incluez toujours le contexte (fichier, ligne, fonction)

### Recommandations

✅ **À faire** :
- Créer un fichier d'en-tête centralisé pour les infos de build
- Utiliser les macros prédéfinies pour le logging
- Documenter vos macros personnalisées
- Tester sur différentes plateformes

❌ **À éviter** :
- Redéfinir les macros standard
- Créer des macros avec des noms trop génériques
- Oublier que ces macros sont évaluées à la compilation

Les macros prédéfinies sont vos alliées pour créer du code robuste, traçable et portable !

---

**Prochaine section** : 23.7 X-Macros (technique avancée de génération de code)

⏭️ [X-Macros](/23-macros-preprocesseur/07-x-macros.md)
