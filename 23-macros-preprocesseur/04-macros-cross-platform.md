🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.4 Macros cross-platform

## Introduction

Écrire du code C qui fonctionne sur plusieurs systèmes d'exploitation (Windows, Linux, macOS, etc.) et avec différents compilateurs est un défi important. Les **macros cross-platform** permettent de détecter automatiquement l'environnement d'exécution et d'adapter le code en conséquence.

Dans ce chapitre, nous allons apprendre à :
- Détecter le système d'exploitation
- Détecter le compilateur utilisé
- Gérer les différences entre plateformes
- Créer des abstractions portables
- Utiliser les bonnes pratiques pour un code vraiment portable

---

## Pourquoi la portabilité est importante ?

### Problèmes courants sans code portable

```c
// ❌ Code non portable - Ne fonctionne que sur Windows
#include <windows.h>

void effacer_ecran(void) {
    system("cls");  // "cls" n'existe pas sur Unix
}

void dormir(int ms) {
    Sleep(ms);  // Fonction Windows uniquement
}
```

Sur Linux, ce code ne compilera même pas car `windows.h` et `Sleep()` n'existent pas !

### Avantages du code portable

- ✅ **Un seul code source** pour toutes les plateformes
- ✅ **Maintenance simplifiée** : pas de multiples versions à gérer
- ✅ **Plus d'utilisateurs** : votre logiciel fonctionne partout
- ✅ **Facilite les tests** sur différents environnements

---

## Détection du système d'exploitation

### Macros prédéfinies principales

Les compilateurs définissent automatiquement des macros qui identifient le système d'exploitation :

| Système | Macros prédéfinies |
|---------|-------------------|
| **Windows** | `_WIN32`, `_WIN64`, `__WINDOWS__` |
| **Linux** | `__linux__`, `__gnu_linux__` |
| **macOS** | `__APPLE__`, `__MACH__` |
| **Unix (général)** | `__unix__`, `__unix` |
| **FreeBSD** | `__FreeBSD__` |
| **Android** | `__ANDROID__` |

### Exemple de détection

```c
#include <stdio.h>

int main(void) {
    printf("Système d'exploitation détecté : ");

#if defined(_WIN32) || defined(_WIN64)
    printf("Windows\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("macOS\n");
#elif defined(__linux__)
    printf("Linux\n");
#elif defined(__FreeBSD__)
    printf("FreeBSD\n");
#elif defined(__unix__) || defined(__unix)
    printf("Unix (autre)\n");
#else
    printf("Inconnu\n");
#endif

    return 0;
}
```

### Détection détaillée de Windows

Windows a plusieurs macros selon l'architecture :

```c
#include <stdio.h>

void detecter_windows(void) {
#ifdef _WIN32
    printf("Windows détecté\n");

    #ifdef _WIN64
        printf("  Architecture : 64 bits\n");
    #else
        printf("  Architecture : 32 bits\n");
    #endif

    #ifdef _MSC_VER
        printf("  Compilateur : MSVC\n");
    #elif defined(__MINGW32__) || defined(__MINGW64__)
        printf("  Compilateur : MinGW\n");
    #elif defined(__CYGWIN__)
        printf("  Compilateur : Cygwin\n");
    #endif
#else
    printf("Pas Windows\n");
#endif
}

int main(void) {
    detecter_windows();
    return 0;
}
```

### Création de macros personnalisées

Il est recommandé de créer vos propres macros pour simplifier la détection :

```c
// platform.h
#ifndef PLATFORM_H
#define PLATFORM_H

// Détection du système d'exploitation
#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define OS_MACOS
#elif defined(__linux__)
    #define OS_LINUX
#elif defined(__FreeBSD__)
    #define OS_FREEBSD
#elif defined(__unix__) || defined(__unix)
    #define OS_UNIX
#else
    #define OS_UNKNOWN
#endif

// Groupement Unix-like
#if defined(OS_LINUX) || defined(OS_MACOS) || defined(OS_FREEBSD) || defined(OS_UNIX)
    #define OS_UNIX_LIKE
#endif

#endif  // PLATFORM_H
```

**Utilisation** :

```c
#include <stdio.h>
#include "platform.h"

int main(void) {
#ifdef OS_WINDOWS
    printf("Code spécifique Windows\n");
#endif

#ifdef OS_UNIX_LIKE
    printf("Code pour systèmes Unix-like\n");
#endif

    return 0;
}
```

---

## Détection du compilateur

### Macros des compilateurs courants

| Compilateur | Macro prédéfinie |
|------------|------------------|
| **GCC** | `__GNUC__` |
| **Clang** | `__clang__` |
| **MSVC** | `_MSC_VER` |
| **Intel C++** | `__INTEL_COMPILER` |
| **MinGW** | `__MINGW32__`, `__MINGW64__` |

### Exemple de détection

```c
#include <stdio.h>

int main(void) {
    printf("Compilateur : ");

#if defined(__clang__)
    printf("Clang %d.%d.%d\n",
           __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("MSVC version %d\n", _MSC_VER);
#elif defined(__INTEL_COMPILER)
    printf("Intel C++ version %d\n", __INTEL_COMPILER);
#else
    printf("Inconnu\n");
#endif

    return 0;
}
```

### Note importante sur Clang

⚠️ **Attention** : Clang définit aussi `__GNUC__` pour la compatibilité ! Toujours tester `__clang__` en premier :

```c
// ✅ Bon ordre
#if defined(__clang__)
    // Code Clang
#elif defined(__GNUC__)
    // Code GCC
#endif

// ❌ Mauvais ordre - Clang sera détecté comme GCC
#if defined(__GNUC__)
    // Code GCC (Clang entre ici aussi !)
#elif defined(__clang__)
    // Ce code ne sera jamais atteint avec Clang
#endif
```

---

## Gestion des différences entre plateformes

### 1. Chemins de fichiers

Les systèmes Windows et Unix utilisent des séparateurs différents :

```c
#include <stdio.h>

// Définition du séparateur de chemin
#ifdef _WIN32
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STR "\\"
#else
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STR "/"
#endif

void afficher_chemin(const char* dossier, const char* fichier) {
    printf("%s%c%s\n", dossier, PATH_SEPARATOR, fichier);
}

int main(void) {
    // Fonctionne sur toutes les plateformes
    afficher_chemin("Documents", "rapport.txt");

#ifdef _WIN32
    printf("Chemin Windows : C:\\Users\\Alice\\Documents\\fichier.txt\n");
#else
    printf("Chemin Unix : /home/alice/documents/fichier.txt\n");
#endif

    return 0;
}
```

### 2. Fonctions de pause

```c
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

int main(void) {
    printf("Attente de 2 secondes...\n");
    SLEEP_MS(2000);  // Portable !
    printf("Terminé !\n");

    return 0;
}
```

### 3. Effacement de l'écran

```c
#include <stdio.h>
#include <stdlib.h>

void effacer_ecran(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main(void) {
    printf("Appuyez sur Entrée pour effacer l'écran...\n");
    getchar();

    effacer_ecran();
    printf("Écran effacé !\n");

    return 0;
}
```

### 4. Couleurs dans le terminal

```c
#include <stdio.h>

// Codes ANSI pour les couleurs (Unix/Linux/macOS)
// Windows 10+ supporte aussi les codes ANSI
#ifdef _WIN32
    #include <windows.h>

    void activer_couleurs_windows(void) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    #define INIT_COLORS() activer_couleurs_windows()
#else
    #define INIT_COLORS() ((void)0)
#endif

// Codes couleurs ANSI (fonctionnent sur tous les systèmes modernes)
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"

int main(void) {
    INIT_COLORS();

    printf(COLOR_RED "Texte rouge\n" COLOR_RESET);
    printf(COLOR_GREEN "Texte vert\n" COLOR_RESET);
    printf(COLOR_BLUE "Texte bleu\n" COLOR_RESET);

    return 0;
}
```

### 5. Gestion des DLL/Shared Libraries

```c
// library.h
#ifndef LIBRARY_H
#define LIBRARY_H

// Export de symboles pour bibliothèques partagées
#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define API_EXPORT __declspec(dllexport)
    #else
        #define API_EXPORT __declspec(dllimport)
    #endif
#else
    #define API_EXPORT __attribute__((visibility("default")))
#endif

// Fonction exportée
API_EXPORT int ma_fonction(int x);

#endif  // LIBRARY_H
```

### 6. Types de données portables

```c
#include <stdio.h>
#include <stdint.h>  // Types de taille fixe

// ❌ Problème : int peut être 16, 32 ou 64 bits selon la plateforme
void fonction_non_portable(int valeur) {
    (void)valeur;
    printf("Taille de int : %zu octets\n", sizeof(int));
}

// ✅ Solution : utiliser des types de taille fixe
void fonction_portable(int32_t valeur) {
    (void)valeur;
    printf("Taille de int32_t : %zu octets (toujours 4)\n", sizeof(int32_t));
}

int main(void) {
    // Types portables de stdint.h
    int8_t   petit = 127;          // 8 bits signé
    uint8_t  u_petit = 255;        // 8 bits non signé
    int16_t  moyen = 32767;        // 16 bits signé
    int32_t  grand = 2147483647;   // 32 bits signé
    int64_t  tres_grand = 9223372036854775807LL;  // 64 bits signé

    printf("Types à taille fixe :\n");
    printf("int8_t  : %zu octets\n", sizeof(int8_t));
    printf("int16_t : %zu octets\n", sizeof(int16_t));
    printf("int32_t : %zu octets\n", sizeof(int32_t));
    printf("int64_t : %zu octets\n", sizeof(int64_t));

    return 0;
}
```

---

## Abstractions portables complètes

### Bibliothèque de compatibilité

Créons une petite bibliothèque qui abstrait les différences de plateformes :

**compat.h** :
```c
#ifndef COMPAT_H
#define COMPAT_H

#include <stdint.h>
#include <stdio.h>

// ============ Détection de plateforme ============
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#else
    #define PLATFORM_UNIX
#endif

// ============ Détection du compilateur ============
#if defined(__clang__)
    #define COMPILER_CLANG
#elif defined(__GNUC__)
    #define COMPILER_GCC
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#endif

// ============ Séparateurs de chemin ============
#ifdef PLATFORM_WINDOWS
    #define PATH_SEP '\\'
    #define PATH_SEP_STR "\\"
#else
    #define PATH_SEP '/'
    #define PATH_SEP_STR "/"
#endif

// ============ Fonctions de temps ============
#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    static inline void sleep_ms(uint32_t milliseconds) {
        Sleep(milliseconds);
    }
#else
    #include <unistd.h>
    static inline void sleep_ms(uint32_t milliseconds) {
        usleep(milliseconds * 1000);
    }
#endif

// ============ Effacement d'écran ============
#ifdef PLATFORM_WINDOWS
    #define clear_screen() system("cls")
#else
    #define clear_screen() system("clear")
#endif

// ============ Fonctions pour les fichiers ============
#ifdef PLATFORM_WINDOWS
    #include <io.h>
    #include <direct.h>
    #define access _access
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// ============ Export de symboles ============
#ifdef PLATFORM_WINDOWS
    #ifdef BUILDING_DLL
        #define EXPORT __declspec(dllexport)
    #else
        #define EXPORT __declspec(dllimport)
    #endif
#else
    #if defined(__GNUC__) && __GNUC__ >= 4
        #define EXPORT __attribute__((visibility("default")))
    #else
        #define EXPORT
    #endif
#endif

// ============ Attributs de fonction ============
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    #define UNUSED __attribute__((unused))
    #define NORETURN __attribute__((noreturn))
#elif defined(COMPILER_MSVC)
    #define UNUSED
    #define NORETURN __declspec(noreturn)
#else
    #define UNUSED
    #define NORETURN
#endif

// ============ Inline ============
#ifdef COMPILER_MSVC
    #define FORCE_INLINE __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

// ============ Alignement mémoire ============
#ifdef COMPILER_MSVC
    #define ALIGN(n) __declspec(align(n))
#else
    #define ALIGN(n) __attribute__((aligned(n)))
#endif

// ============ Détection du boutisme (endianness) ============
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define IS_BIG_ENDIAN 1
    #define IS_LITTLE_ENDIAN 0
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define IS_BIG_ENDIAN 0
    #define IS_LITTLE_ENDIAN 1
#else
    // Détection à l'exécution si nécessaire
    static inline int is_little_endian(void) {
        uint32_t test = 0x01020304;
        return ((uint8_t*)&test)[0] == 0x04;
    }
#endif

#endif  // COMPAT_H
```

### Exemple d'utilisation

```c
#include <stdio.h>
#include "compat.h"

int main(void) {
    // Informations de plateforme
    printf("=== Informations de plateforme ===\n");

#ifdef PLATFORM_WINDOWS
    printf("Système : Windows\n");
#elif defined(PLATFORM_LINUX)
    printf("Système : Linux\n");
#elif defined(PLATFORM_MACOS)
    printf("Système : macOS\n");
#else
    printf("Système : Autre Unix\n");
#endif

#ifdef COMPILER_GCC
    printf("Compilateur : GCC\n");
#elif defined(COMPILER_CLANG)
    printf("Compilateur : Clang\n");
#elif defined(COMPILER_MSVC)
    printf("Compilateur : MSVC\n");
#endif

    printf("Séparateur de chemin : '%c'\n", PATH_SEP);

    // Utilisation des fonctions portables
    printf("\nTest des fonctions portables...\n");
    printf("Attente de 1 seconde...\n");
    sleep_ms(1000);
    printf("Terminé !\n");

    // Construction de chemin portable
    char chemin[256];
    snprintf(chemin, sizeof(chemin), "dossier%cfichier.txt", PATH_SEP);
    printf("Chemin construit : %s\n", chemin);

    return 0;
}
```

---

## Gestion des extensions spécifiques

### Extensions GCC/Clang

```c
#include <stdio.h>

// Utiliser les extensions GCC si disponibles
#if defined(__GNUC__) || defined(__clang__)
    // Optimisation : indiquer au compilateur les branches probables
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)

    // Désactiver les warnings pour une fonction
    #define DISABLE_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define DISABLE_WARNING_POP  _Pragma("GCC diagnostic pop")
    #define DISABLE_WARNING(warningName) \
        _Pragma(#warningName)
#else
    // Fallback pour autres compilateurs
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
    #define DISABLE_WARNING_PUSH
    #define DISABLE_WARNING_POP
    #define DISABLE_WARNING(warningName)
#endif

int fonction_avec_optimisation(int* ptr) {
    // Indique que ptr == NULL est très improbable
    if (UNLIKELY(ptr == NULL)) {
        fprintf(stderr, "Erreur : pointeur NULL\n");
        return -1;
    }

    // Cas normal
    return *ptr * 2;
}

int main(void) {
    int valeur = 42;
    int resultat = fonction_avec_optimisation(&valeur);
    printf("Résultat : %d\n", resultat);

    return 0;
}
```

### Extensions MSVC

```c
#ifdef _MSC_VER
    // Désactiver un warning spécifique MSVC
    #pragma warning(disable : 4996)  // Fonction "unsafe"

    // Alternative à __func__ pour MSVC ancien
    #define __func__ __FUNCTION__
#endif
```

---

## Détection de l'architecture

### Architecture du processeur

```c
#include <stdio.h>

int main(void) {
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

    printf("Taille du pointeur : %zu octets\n", sizeof(void*));

    return 0;
}
```

### Détection 32 vs 64 bits

```c
#include <stdio.h>
#include <stdint.h>

// Macro générique pour détecter 64 bits
#if defined(__LP64__) || defined(_WIN64) || \
    (defined(__WORDSIZE) && __WORDSIZE == 64)
    #define ARCH_64BIT
#else
    #define ARCH_32BIT
#endif

int main(void) {
#ifdef ARCH_64BIT
    printf("Architecture 64 bits détectée\n");
    printf("Utilisation de pointeurs 64 bits\n");
#else
    printf("Architecture 32 bits détectée\n");
    printf("Utilisation de pointeurs 32 bits\n");
#endif

    printf("sizeof(void*) = %zu\n", sizeof(void*));
    printf("sizeof(size_t) = %zu\n", sizeof(size_t));

    return 0;
}
```

---

## Gestion de l'endianness (boutisme)

```c
#include <stdio.h>
#include <stdint.h>

// Détection de l'endianness
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define IS_BIG_ENDIAN 1
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define IS_LITTLE_ENDIAN 1
#else
    // Détection à l'exécution
    static inline int is_little_endian(void) {
        uint32_t test = 0x01020304;
        return ((uint8_t*)&test)[0] == 0x04;
    }
    #define IS_LITTLE_ENDIAN is_little_endian()
#endif

// Fonctions de conversion d'endianness
#if defined(__GNUC__) || defined(__clang__)
    #define SWAP16(x) __builtin_bswap16(x)
    #define SWAP32(x) __builtin_bswap32(x)
    #define SWAP64(x) __builtin_bswap64(x)
#elif defined(_MSC_VER)
    #include <stdlib.h>
    #define SWAP16(x) _byteswap_ushort(x)
    #define SWAP32(x) _byteswap_ulong(x)
    #define SWAP64(x) _byteswap_uint64(x)
#else
    // Implémentation portable
    static inline uint16_t swap16(uint16_t x) {
        return (x >> 8) | (x << 8);
    }
    static inline uint32_t swap32(uint32_t x) {
        return ((x >> 24) & 0xff) |
               ((x >> 8) & 0xff00) |
               ((x << 8) & 0xff0000) |
               ((x << 24) & 0xff000000);
    }
    #define SWAP16(x) swap16(x)
    #define SWAP32(x) swap32(x)
#endif

int main(void) {
#ifdef IS_BIG_ENDIAN
    printf("Système Big Endian\n");
#elif defined(IS_LITTLE_ENDIAN)
    if (IS_LITTLE_ENDIAN) {
        printf("Système Little Endian\n");
    } else {
        printf("Système Big Endian\n");
    }
#endif

    // Test de conversion
    uint32_t valeur = 0x12345678;
    printf("Valeur originale : 0x%08X\n", valeur);
    printf("Après swap : 0x%08X\n", SWAP32(valeur));

    return 0;
}
```

---

## Exemple complet : Programme portable

Voici un exemple de programme complet qui fonctionne sur Windows, Linux et macOS :

**portable_app.c** :
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ========== Configuration plateforme ==========
#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS
    #include <windows.h>
    #include <direct.h>
    #define PATH_SEP '\\'
    #define mkdir(path) _mkdir(path)
    #define sleep_seconds(s) Sleep((s) * 1000)
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #define PATH_SEP '/'
    #define sleep_seconds(s) sleep(s)
#endif

// ========== Détection compilateur ==========
#if defined(__clang__)
    #define COMPILER_NAME "Clang"
#elif defined(__GNUC__)
    #define COMPILER_NAME "GCC"
#elif defined(_MSC_VER)
    #define COMPILER_NAME "MSVC"
#else
    #define COMPILER_NAME "Inconnu"
#endif

// ========== Architecture ==========
#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH_NAME "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH_NAME "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH_NAME "ARM64"
#else
    #define ARCH_NAME "Inconnue"
#endif

// ========== Fonctions portables ==========

void afficher_info_systeme(void) {
    printf("=== Informations système ===\n");

#ifdef OS_WINDOWS
    printf("OS : Windows\n");
#elif defined(__linux__)
    printf("OS : Linux\n");
#elif defined(__APPLE__)
    printf("OS : macOS\n");
#else
    printf("OS : Unix/Autre\n");
#endif

    printf("Compilateur : %s\n", COMPILER_NAME);
    printf("Architecture : %s\n", ARCH_NAME);
    printf("sizeof(void*) : %zu octets\n", sizeof(void*));
    printf("sizeof(size_t) : %zu octets\n", sizeof(size_t));
    printf("Séparateur : '%c'\n", PATH_SEP);
}

void effacer_ecran_portable(void) {
#ifdef OS_WINDOWS
    system("cls");
#else
    system("clear");
#endif
}

int creer_dossier_portable(const char* nom) {
#ifdef OS_WINDOWS
    return _mkdir(nom);
#else
    return mkdir(nom, 0755);
#endif
}

void construire_chemin(char* buffer, size_t taille,
                      const char* dossier, const char* fichier) {
    snprintf(buffer, taille, "%s%c%s", dossier, PATH_SEP, fichier);
}

// ========== Programme principal ==========

int main(void) {
    printf("Programme Portable - Démonstration\n");
    printf("===================================\n\n");

    // Afficher les informations système
    afficher_info_systeme();
    printf("\n");

    // Test de temporisation
    printf("Test de temporisation (2 secondes)...\n");
    sleep_seconds(2);
    printf("Terminé !\n\n");

    // Test de construction de chemin
    char chemin[256];
    construire_chemin(chemin, sizeof(chemin), "documents", "rapport.pdf");
    printf("Chemin construit : %s\n\n", chemin);

    // Test de création de dossier
    const char* nom_dossier = "test_portable";
    printf("Création du dossier '%s'...\n", nom_dossier);
    if (creer_dossier_portable(nom_dossier) == 0) {
        printf("Dossier créé avec succès !\n");
    } else {
        printf("Le dossier existe déjà ou erreur\n");
    }

    printf("\nAppuyez sur Entrée pour effacer l'écran...");
    getchar();

    effacer_ecran_portable();
    printf("Écran effacé ! Programme terminé.\n");

    return 0;
}
```

**Compilation** :

```bash
# Linux / macOS
$ gcc portable_app.c -o portable_app
$ ./portable_app

# Windows (MinGW)
$ gcc portable_app.c -o portable_app.exe
$ portable_app.exe

# Windows (MSVC)
$ cl portable_app.c
$ portable_app.exe
```

---

## Fichiers de configuration cross-platform

### Approche avec fichiers de configuration séparés

**config_windows.h** :
```c
#ifndef CONFIG_WINDOWS_H
#define CONFIG_WINDOWS_H

#define USE_WINDOWS_API 1
#define MAX_PATH_LEN 260
#define LINE_ENDING "\r\n"
#define DEFAULT_CONFIG_PATH "C:\\ProgramData\\MyApp\\config.ini"

#endif
```

**config_unix.h** :
```c
#ifndef CONFIG_UNIX_H
#define CONFIG_UNIX_H

#define USE_POSIX_API 1
#define MAX_PATH_LEN 4096
#define LINE_ENDING "\n"
#define DEFAULT_CONFIG_PATH "/etc/myapp/config.ini"

#endif
```

**config.h** (fichier principal) :
```c
#ifndef CONFIG_H
#define CONFIG_H

#if defined(_WIN32) || defined(_WIN64)
    #include "config_windows.h"
#else
    #include "config_unix.h"
#endif

// Configuration commune à toutes les plateformes
#define APP_NAME "MyApp"
#define APP_VERSION "1.0.0"
#define BUFFER_SIZE 4096

#endif
```

---

## Bonnes pratiques

### 1. Centraliser les détections de plateforme

```c
// ✅ Bon - Détections dans un seul fichier header
// platform.h
#ifndef PLATFORM_H
#define PLATFORM_H

// Toutes les détections ici
#ifdef _WIN32
    #define OS_WINDOWS
#endif

// Toutes les abstractions ici
#ifdef OS_WINDOWS
    #define sleep_ms(x) Sleep(x)
#else
    #define sleep_ms(x) usleep((x) * 1000)
#endif

#endif

// ❌ Mauvais - Détections dispersées dans tout le code
```

### 2. Tester sur toutes les plateformes cibles

- Utilisez des machines virtuelles
- Configurez des pipelines CI/CD multi-plateformes (GitHub Actions, GitLab CI)
- Testez avec différents compilateurs

### 3. Documenter les dépendances de plateforme

```c
/**
 * @brief Fonction de temporisation portable
 * @param milliseconds Durée en millisecondes
 *
 * @note Windows: utilise Sleep()
 * @note Unix/Linux/macOS: utilise usleep()
 *
 * @warning Sur Windows, la précision est limitée à ~15ms
 */
void sleep_ms(uint32_t milliseconds);
```

### 4. Utiliser les standards autant que possible

```c
// ✅ Préférer les fonctions standards
#include <stdio.h>   // fopen, fclose, etc.
#include <stdlib.h>  // malloc, free, etc.
#include <string.h>  // strcpy, strlen, etc.
#include <stdint.h>  // int32_t, uint64_t, etc.

// Éviter les fonctions spécifiques sauf nécessité absolue
```

### 5. Prévoir des fallbacks

```c
// Si une fonctionnalité n'est pas disponible, prévoir une alternative
#if defined(__GNUC__) && __GNUC__ >= 4
    #define LIKELY(x) __builtin_expect(!!(x), 1)
#else
    #define LIKELY(x) (x)  // Fallback : pas d'optimisation
#endif
```

---

## Outils pour faciliter la portabilité

### 1. Bibliothèques cross-platform

Plutôt que réinventer la roue, utilisez des bibliothèques éprouvées :

- **SDL2** : graphique, audio, input
- **GLFW** : fenêtrage et contexte OpenGL
- **libuv** : I/O asynchrone, réseau, threads
- **Boost** : nombreuses fonctionnalités (si vous pouvez utiliser C++)

### 2. Générateurs de build

- **CMake** : le standard de facto pour le C/C++
- **Meson** : moderne et rapide
- **Autotools** : traditionnel pour Unix

**Exemple CMakeLists.txt** :
```cmake
cmake_minimum_required(VERSION 3.10)  
project(MonProjet C)  

# Options selon la plateforme
if(WIN32)
    add_definitions(-DOS_WINDOWS)
    set(PLATFORM_LIBS ws2_32)  # Winsock
elseif(UNIX)
    add_definitions(-DOS_UNIX)
    set(PLATFORM_LIBS pthread m)  # Threads et math
endif()

add_executable(monapp main.c)  
target_link_libraries(monapp ${PLATFORM_LIBS})  
```

---

## Checklist de portabilité

Avant de déclarer votre code "portable", vérifiez :

✅ **Compilation** :
- [ ] Compile avec GCC sur Linux
- [ ] Compile avec Clang sur macOS
- [ ] Compile avec MSVC sur Windows
- [ ] Compile avec MinGW sur Windows
- [ ] Pas de warnings avec `-Wall -Wextra`

✅ **Exécution** :
- [ ] Fonctionne correctement sur Windows
- [ ] Fonctionne correctement sur Linux
- [ ] Fonctionne correctement sur macOS
- [ ] Gère correctement les chemins de fichiers
- [ ] Gère correctement les fins de ligne

✅ **Types de données** :
- [ ] Utilise `stdint.h` pour les types de taille fixe
- [ ] Évite les assomptions sur sizeof(int)
- [ ] Gère l'endianness si nécessaire

✅ **API système** :
- [ ] Abstractions pour les fonctions spécifiques
- [ ] Fallbacks pour fonctionnalités non disponibles
- [ ] Détection de plateforme correcte

---

## Résumé

### Points clés

🔑 **Détection** : Utilisez les macros prédéfinies pour détecter OS, compilateur, architecture

🔑 **Abstraction** : Créez des macros/fonctions portables pour les différences de plateforme

🔑 **Standards** : Préférez les fonctions standard C aux extensions spécifiques

🔑 **Tests** : Testez sur toutes les plateformes cibles

🔑 **Documentation** : Documentez les dépendances et limitations de plateforme

### Macros essentielles à connaître

| Catégorie | Macros |
|-----------|--------|
| **Windows** | `_WIN32`, `_WIN64`, `_MSC_VER` |
| **Linux** | `__linux__`, `__gnu_linux__` |
| **macOS** | `__APPLE__`, `__MACH__` |
| **Unix** | `__unix__`, `__unix` |
| **GCC** | `__GNUC__`, `__GNUC_MINOR__` |
| **Clang** | `__clang__`, `__clang_major__` |
| **x86_64** | `__x86_64__`, `_M_X64` |
| **ARM** | `__arm__`, `__aarch64__` |

### Approche recommandée

1. **Créer un fichier platform.h** avec toutes les détections
2. **Définir des macros portables** pour les opérations communes
3. **Isoler le code spécifique** dans des fichiers séparés si nécessaire
4. **Tester régulièrement** sur toutes les plateformes
5. **Documenter** les limitations et dépendances

Le code portable demande un effort initial, mais facilite grandement la maintenance et l'extension de votre projet !

---

**Prochaine section** : 23.5 Dangers et pièges des macros (erreurs courantes et comment les éviter)

⏭️ [Dangers et pièges des macros](/23-macros-preprocesseur/05-dangers-macros.md)
