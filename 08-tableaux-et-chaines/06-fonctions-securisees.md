🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 8.6 Fonctions sécurisées

## Introduction

Après avoir compris les dangers des buffer overflows (section 8.5), vous vous demandez sûrement : *"Comment manipuler les chaînes de manière sûre en C ?"* Cette section vous présente les alternatives sécurisées aux fonctions dangereuses et les meilleures pratiques pour protéger vos programmes.

**Ce que vous apprendrez :**
- Les fonctions sécurisées du standard C11 (Annex K)
- Les alternatives BSD : strlcpy/strlcat
- Les bonnes pratiques POSIX
- Comment implémenter vos propres fonctions sécurisées
- Les wrappers et bibliothèques de sécurité

**Objectif :** Vous donner les outils pour écrire du code C robuste et sécurisé.

## Panorama des solutions

Il existe plusieurs approches pour sécuriser la manipulation de chaînes en C :

| Approche | Disponibilité | Avantages | Inconvénients |
|----------|--------------|-----------|---------------|
| **strncpy/strncat** | Standard C | Universelle | Pièges, pas toujours '\0' terminé |
| **snprintf** | C99+ | Largement disponible | Uniquement pour formatage |
| **Annex K (C11)** | C11 optionnel | API cohérente | Peu supportée en pratique |
| **strlcpy/strlcat** | BSD, musl | Simple et sûre | Pas dans glibc standard |
| **Fonctions custom** | Toujours | Contrôle total | À implémenter soi-même |

## Les fonctions "n" : strncpy et strncat

### strncpy() : Utilisation correcte

```c
#include <string.h>

char *strncpy(char *dest, const char *src, size_t n);
```

**Comportement :**
- Copie au maximum `n` caractères de `src` vers `dest`
- **Attention :** Ne garantit PAS la terminaison par '\0' si `src` >= `n` !
- Remplit le reste avec '\0' si `src` < `n`

**❌ Usage incorrect (fréquent) :**

```c
char dest[10];  
strncpy(dest, "Texte très long", sizeof(dest));  // ❌ Pas de '\0' garanti !  
printf("%s\n", dest);  // Comportement indéfini  
```

**✅ Usage correct :**

```c
char dest[10];  
strncpy(dest, "Texte très long", sizeof(dest) - 1);  
dest[sizeof(dest) - 1] = '\0';  // Garantir la terminaison  
printf("%s\n", dest);  // "Texte trè"  
```

**Fonction wrapper sécurisée :**

```c
void safe_strncpy(char *dest, const char *src, size_t dest_size) {
    if (dest_size == 0) return;

    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

// Utilisation
char buffer[20];  
safe_strncpy(buffer, "Bonjour le monde", sizeof(buffer));  
```

### strncat() : Utilisation correcte

```c
char *strncat(char *dest, const char *src, size_t n);
```

**Comportement :**
- Ajoute au maximum `n` caractères de `src` à la fin de `dest`
- **Garantit** la terminaison par '\0' (contrairement à strncpy)
- Suppose que `dest` est déjà une chaîne valide

**❌ Usage incorrect :**

```c
char dest[20] = "Hello";  
strncat(dest, " World", sizeof(dest));  // ❌ Risque de débordement !  
// sizeof(dest) ne tient pas compte de ce qui est déjà dans dest
```

**✅ Usage correct :**

```c
char dest[20] = "Hello";  
size_t espace_restant = sizeof(dest) - strlen(dest) - 1;  
strncat(dest, " World", espace_restant);  
```

**Fonction wrapper sécurisée :**

```c
void safe_strncat(char *dest, const char *src, size_t dest_size) {
    size_t dest_len = strlen(dest);

    if (dest_len >= dest_size - 1) {
        return;  // Pas d'espace
    }

    size_t espace = dest_size - dest_len - 1;
    strncat(dest, src, espace);
}

// Utilisation
char buffer[20] = "Hello";  
safe_strncat(buffer, " World", sizeof(buffer));  
```

### Les limites de strncpy/strncat

**Problèmes :**
1. **strncpy** ne garantit pas '\0'
2. Interface peu intuitive
3. Calculs de taille complexes
4. Performances (strncpy remplit avec '\0')

**Exemple de confusion courante :**

```c
char dest[10];

// Beaucoup pensent que cela fonctionne...
strncpy(dest, "Long texte", sizeof(dest));

// Mais dest n'est pas terminée !
// dest = {'L','o','n','g',' ','t','e','x','t','e'} sans '\0' !
```

## snprintf() : La solution universelle pour le formatage

```c
#include <stdio.h>

int snprintf(char *str, size_t size, const char *format, ...);
```

**Avantages :**
- **Garantit** la terminaison par '\0'
- Retourne le nombre de caractères qui auraient été écrits
- Disponible depuis C99 (largement supporté)
- Sûre par conception

**Utilisation de base :**

```c
char buffer[50];  
int nombre = 42;  
const char *nom = "Alice";  

int written = snprintf(buffer, sizeof(buffer),
                       "Nom: %s, Age: %d", nom, nombre);

if (written >= (int)sizeof(buffer)) {
    printf("Attention : troncature détectée\n");
}

printf("%s\n", buffer);  // "Nom: Alice, Age: 42"
```

**Pour copier une simple chaîne :**

```c
char dest[50];  
const char *source = "Bonjour le monde";  

snprintf(dest, sizeof(dest), "%s", source);
// Équivalent à une copie sécurisée
```

**Vérification de troncature :**

```c
char buffer[10];  
int written = snprintf(buffer, sizeof(buffer), "Texte très long");  

if (written < 0) {
    // Erreur d'encodage (rare)
    fprintf(stderr, "Erreur snprintf\n");
} else if (written >= (int)sizeof(buffer)) {
    // Troncature : written contient la taille nécessaire
    fprintf(stderr, "Troncature : %d caractères nécessaires, %zu disponibles\n",
            written, sizeof(buffer));
}
```

**Construction progressive :**

```c
#include <stdio.h>

void construire_message_securise(char *buffer, size_t size) {
    int offset = 0;

    // Ajouter la première partie
    offset += snprintf(buffer + offset, size - offset, "Ligne 1\n");
    if (offset >= (int)size) return;

    // Ajouter la deuxième partie
    offset += snprintf(buffer + offset, size - offset, "Ligne 2\n");
    if (offset >= (int)size) return;

    // Ajouter la troisième partie
    offset += snprintf(buffer + offset, size - offset, "Ligne 3\n");
}

int main(void) {
    char message[100];
    construire_message_securise(message, sizeof(message));
    printf("%s", message);
    return 0;
}
```

## C11 Annex K : Les fonctions "_s"

Le standard C11 a introduit l'Annexe K (optionnelle) avec des fonctions sécurisées suffixées par `_s`.

**Note importante :** Ces fonctions sont **optionnelles** et peu supportées en pratique (absentes de glibc, musl). Elles sont principalement disponibles sur Windows (Microsoft).

### strcpy_s() et strncpy_s()

```c
// Disponible si __STDC_LIB_EXT1__ est défini
#ifdef __STDC_LIB_EXT1__

errno_t strcpy_s(char *dest, rsize_t dest_size, const char *src);  
errno_t strncpy_s(char *dest, rsize_t dest_size, const char *src, rsize_t count);  

#endif
```

**Caractéristiques :**
- Retourne 0 en cas de succès, une erreur sinon
- Détecte les pointeurs NULL
- Garantit la terminaison par '\0'
- Peut appeler un handler d'erreur en cas de problème

**Exemple (Windows/MSVC) :**

```c
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>

int main(void) {
    char dest[20];
    const char *source = "Bonjour";

    errno_t err = strcpy_s(dest, sizeof(dest), source);

    if (err == 0) {
        printf("Copie réussie : %s\n", dest);
    } else {
        fprintf(stderr, "Erreur de copie\n");
    }

    return 0;
}
```

### strcat_s()

```c
errno_t strcat_s(char *dest, rsize_t dest_size, const char *src);
```

**Exemple :**

```c
char buffer[50] = "Hello";  
errno_t err = strcat_s(buffer, sizeof(buffer), " World");  

if (err != 0) {
    fprintf(stderr, "Erreur de concaténation\n");
}
```

### Autres fonctions Annex K

- `gets_s()` : Alternative à gets (lecture limitée)
- `sprintf_s()` : Version sécurisée de sprintf
- `scanf_s()` : Version sécurisée de scanf
- `fopen_s()` : Version sécurisée de fopen

### Limites de l'Annex K

**Problèmes :**
1. **Faible adoption** : Non disponible dans glibc (GNU/Linux standard)
2. **Portabilité limitée** : Principalement Windows
3. **Critique de la communauté** : Interface jugée complexe
4. **Comportement par défaut** : Peut terminer le programme en cas d'erreur

**Détection de disponibilité :**

```c
#ifdef __STDC_LIB_EXT1__
    // Fonctions _s disponibles
    strcpy_s(dest, sizeof(dest), src);
#else
    // Fallback sur strncpy
    strncpy(dest, src, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';
#endif
```

## strlcpy() et strlcat() : Les alternatives BSD

Originaires de OpenBSD, ces fonctions offrent une interface simple et intuitive pour la manipulation sécurisée de chaînes.

### strlcpy()

```c
size_t strlcpy(char *dst, const char *src, size_t size);
```

**Comportement :**
- Copie au maximum `size - 1` caractères
- **Garantit** toujours la terminaison par '\0'
- Retourne `strlen(src)` (longueur de la source)
- Permet de détecter la troncature

**Avantages :**
- Interface intuitive
- Toujours sûre
- Détection simple de troncature

**Exemple :**

```c
char dest[10];  
const char *source = "Bonjour le monde";  

size_t src_len = strlcpy(dest, source, sizeof(dest));

if (src_len >= sizeof(dest)) {
    printf("Troncature : %zu caractères nécessaires\n", src_len + 1);
} else {
    printf("Copie complète : %s\n", dest);
}
```

### strlcat()

```c
size_t strlcat(char *dst, const char *src, size_t size);
```

**Comportement :**
- Concatène `src` à la fin de `dst`
- `size` est la taille **totale** du buffer (pas l'espace restant)
- Garantit la terminaison par '\0'
- Retourne la longueur finale que la chaîne aurait dû avoir

**Exemple :**

```c
char buffer[20] = "Hello";  
const char *ajout = " World";  

size_t final_len = strlcat(buffer, ajout, sizeof(buffer));

if (final_len >= sizeof(buffer)) {
    printf("Troncature détectée\n");
}

printf("%s\n", buffer);
```

### Implémentation de strlcpy/strlcat

Si ces fonctions ne sont pas disponibles sur votre système (glibc ne les inclut pas), vous pouvez les implémenter :

```c
#include <string.h>

// Implémentation de strlcpy
size_t strlcpy(char *dst, const char *src, size_t size) {
    size_t src_len = strlen(src);

    if (size > 0) {
        size_t copy_len = (src_len < size - 1) ? src_len : size - 1;
        memcpy(dst, src, copy_len);
        dst[copy_len] = '\0';
    }

    return src_len;
}

// Implémentation de strlcat
size_t strlcat(char *dst, const char *src, size_t size) {
    // Trouver la fin de dst sans dépasser size (équivalent de strnlen)
    const char *nul = memchr(dst, '\0', size);
    size_t src_len = strlen(src);

    if (!nul) {
        return size + src_len;  // dst non terminée dans size octets
    }

    size_t dst_len = (size_t)(nul - dst);

    size_t available = size - dst_len - 1;
    size_t copy_len = (src_len < available) ? src_len : available;

    memcpy(dst + dst_len, src, copy_len);
    dst[dst_len + copy_len] = '\0';

    return dst_len + src_len;
}
```

### Disponibilité

**Systèmes incluant strlcpy/strlcat :**
- OpenBSD, FreeBSD, NetBSD
- macOS
- musl libc (Alpine Linux)
- Solaris

**Systèmes ne les incluant PAS :**
- glibc (Linux standard)
- Windows (MSVC)

**Solution pour glibc :**

```c
// Vérifier la disponibilité
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE__)
    // strlcpy/strlcat disponibles
#else
    // Inclure une implémentation ou utiliser libbsd
    #include <bsd/string.h>  // Si libbsd est installée
#endif
```

## Comparaison des approches

### Copie de chaîne : Comparaison complète

```c
#include <stdio.h>
#include <string.h>

void test_copy_functions(void) {
    const char *source = "Bonjour le monde";
    char dest[10];  // Volontairement petit pour tester la troncature

    printf("Source : \"%s\" (longueur: %zu)\n\n", source, strlen(source));

    // Méthode 1 : strncpy (standard C mais piégeuse)
    printf("=== strncpy ===\n");
    strncpy(dest, source, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';
    printf("Résultat : \"%s\"\n", dest);
    printf("Terminaison garantie : OUI (manuel)\n\n");

    // Méthode 2 : snprintf (C99+)
    printf("=== snprintf ===\n");
    int written = snprintf(dest, sizeof(dest), "%s", source);
    printf("Résultat : \"%s\"\n", dest);
    printf("Terminaison garantie : OUI\n");
    printf("Troncature détectée : %s\n\n",
           written >= (int)sizeof(dest) ? "OUI" : "NON");

    // Méthode 3 : strlcpy (BSD)
    #ifdef HAVE_STRLCPY
    printf("=== strlcpy ===\n");
    size_t needed = strlcpy(dest, source, sizeof(dest));
    printf("Résultat : \"%s\"\n", dest);
    printf("Terminaison garantie : OUI\n");
    printf("Taille nécessaire : %zu\n\n", needed + 1);
    #endif

    // Méthode 4 : strcpy_s (C11 Annex K)
    #ifdef __STDC_LIB_EXT1__
    printf("=== strcpy_s ===\n");
    errno_t err = strcpy_s(dest, sizeof(dest), source);
    printf("Résultat : %s\n", err == 0 ? dest : "ERREUR");
    printf("Statut : %s\n\n", err == 0 ? "OK" : "ÉCHEC");
    #endif
}
```

### Tableau comparatif

| Fonction | Terminaison '\0' | Détection troncature | Disponibilité | Complexité |
|----------|------------------|---------------------|---------------|------------|
| **strcpy** | ⚠️ Non sûr | ❌ Non | ✅ Partout | Simple |
| **strncpy** | ❌ Non garanti | ❌ Non | ✅ Partout | Complexe |
| **snprintf** | ✅ Oui | ✅ Oui (retour) | ✅ C99+ | Moyenne |
| **strcpy_s** | ✅ Oui | ✅ Oui (errno_t) | ❌ Rare | Moyenne |
| **strlcpy** | ✅ Oui | ✅ Oui (retour) | ⚠️ BSD/musl | Simple |

## Fonctions personnalisées sécurisées

### Bibliothèque de wrappers sécurisés

```c
#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <string.h>
#include <stdio.h>
#include <errno.h>

// Codes d'erreur
typedef enum {
    STR_SUCCESS = 0,
    STR_ERROR_NULL_PTR,
    STR_ERROR_ZERO_SIZE,
    STR_ERROR_TRUNCATED,
    STR_ERROR_INVALID
} str_error_t;

/**
 * Copie sécurisée de chaîne
 * @param dest Buffer de destination
 * @param dest_size Taille totale du buffer
 * @param src Chaîne source
 * @return STR_SUCCESS ou code d'erreur
 */
str_error_t safe_strcpy(char *dest, size_t dest_size, const char *src) {
    // Validations
    if (dest == NULL || src == NULL) {
        return STR_ERROR_NULL_PTR;
    }

    if (dest_size == 0) {
        return STR_ERROR_ZERO_SIZE;
    }

    // Calculer la longueur à copier
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;

    // Copier
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';

    // Vérifier la troncature
    return (src_len >= dest_size) ? STR_ERROR_TRUNCATED : STR_SUCCESS;
}

/**
 * Concaténation sécurisée
 * @param dest Buffer de destination
 * @param dest_size Taille totale du buffer
 * @param src Chaîne à ajouter
 * @return STR_SUCCESS ou code d'erreur
 */
str_error_t safe_strcat(char *dest, size_t dest_size, const char *src) {
    // Validations
    if (dest == NULL || src == NULL) {
        return STR_ERROR_NULL_PTR;
    }

    if (dest_size == 0) {
        return STR_ERROR_ZERO_SIZE;
    }

    // Trouver la fin de dest (sans dépasser dest_size)
    const char *nul = memchr(dest, '\0', dest_size);

    if (!nul) {
        return STR_ERROR_INVALID;  // dest n'est pas terminée
    }

    size_t dest_len = (size_t)(nul - dest);

    // Calculer l'espace disponible
    size_t available = dest_size - dest_len - 1;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < available) ? src_len : available;

    // Copier
    memcpy(dest + dest_len, src, copy_len);
    dest[dest_len + copy_len] = '\0';

    return (src_len > available) ? STR_ERROR_TRUNCATED : STR_SUCCESS;
}

/**
 * Lecture sécurisée depuis stdin
 * @param buffer Buffer de destination
 * @param size Taille du buffer
 * @return STR_SUCCESS ou code d'erreur
 */
str_error_t safe_read_line(char *buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return STR_ERROR_NULL_PTR;
    }

    if (fgets(buffer, size, stdin) == NULL) {
        return STR_ERROR_INVALID;
    }

    // Retirer le '\n' si présent
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        // Buffer trop petit, nettoyer stdin
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return STR_ERROR_TRUNCATED;
    }

    return STR_SUCCESS;
}

/**
 * Obtient le message d'erreur
 * @param error Code d'erreur
 * @return Message d'erreur
 */
const char *str_error_message(str_error_t error) {
    switch (error) {
        case STR_SUCCESS:        return "Succès";
        case STR_ERROR_NULL_PTR: return "Pointeur NULL";
        case STR_ERROR_ZERO_SIZE: return "Taille nulle";
        case STR_ERROR_TRUNCATED: return "Troncature";
        case STR_ERROR_INVALID:   return "Paramètre invalide";
        default:                  return "Erreur inconnue";
    }
}

#endif // SAFE_STRING_H
```

### Utilisation de la bibliothèque

```c
#include <stdio.h>
#include "safe_string.h"

int main(void) {
    char buffer[50];
    str_error_t err;

    // Copie sécurisée
    err = safe_strcpy(buffer, sizeof(buffer), "Bonjour");
    if (err != STR_SUCCESS) {
        fprintf(stderr, "Erreur de copie : %s\n", str_error_message(err));
        return 1;
    }
    printf("Buffer : %s\n", buffer);

    // Concaténation sécurisée
    err = safe_strcat(buffer, sizeof(buffer), " le monde");
    if (err == STR_ERROR_TRUNCATED) {
        printf("Attention : troncature détectée\n");
    }
    printf("Buffer : %s\n", buffer);

    // Lecture sécurisée
    printf("Entrez votre nom : ");
    char nom[30];
    err = safe_read_line(nom, sizeof(nom));

    if (err == STR_ERROR_TRUNCATED) {
        printf("Nom trop long, tronqué à : %s\n", nom);
    } else if (err == STR_SUCCESS) {
        printf("Bonjour %s !\n", nom);
    }

    return 0;
}
```

## Macros utilitaires

### Macro pour sizeof automatique

```c
#define SAFE_STRCPY(dest, src) \
    safe_strcpy((dest), sizeof(dest), (src))

#define SAFE_STRCAT(dest, src) \
    safe_strcat((dest), sizeof(dest), (src))

// Utilisation
char buffer[100];  
SAFE_STRCPY(buffer, "Bonjour");  // sizeof(buffer) automatique  
SAFE_STRCAT(buffer, " monde");  
```

### Macro de vérification

```c
#define CHECK_STR_ERROR(expr, action) \
    do { \
        str_error_t _err = (expr); \
        if (_err != STR_SUCCESS) { \
            fprintf(stderr, "Erreur ligne %d: %s\n", \
                    __LINE__, str_error_message(_err)); \
            action; \
        } \
    } while(0)

// Utilisation
char dest[20];  
CHECK_STR_ERROR(  
    safe_strcpy(dest, sizeof(dest), source),
    return -1
);
```

## Bonnes pratiques générales

### ✅ 1. Toujours préférer snprintf() pour le formatage

```c
// ❌ Dangereux
sprintf(buffer, "%s %d", str, num);

// ✅ Sûr
snprintf(buffer, sizeof(buffer), "%s %d", str, num);
```

### ✅ 2. Utiliser sizeof() systématiquement

```c
char buffer[100];

// ✅ Bon : s'adapte si la taille change
snprintf(buffer, sizeof(buffer), "...");

// ❌ Mauvais : risque d'incohérence
snprintf(buffer, 100, "...");
```

### ✅ 3. Vérifier les valeurs de retour

```c
int written = snprintf(buffer, sizeof(buffer), "%s", source);

if (written < 0) {
    // Erreur d'encodage
    fprintf(stderr, "Erreur snprintf\n");
    return -1;
}

if (written >= (int)sizeof(buffer)) {
    // Troncature
    fprintf(stderr, "Attention : données tronquées\n");
}
```

### ✅ 4. Encapsuler dans des fonctions réutilisables

```c
// Créer des wrappers spécifiques au projet
void copy_nom_utilisateur(char *dest, size_t size, const char *src) {
    // Validation spécifique
    if (strlen(src) > MAX_NOM) {
        fprintf(stderr, "Nom trop long\n");
        return;
    }

    // Copie sécurisée
    snprintf(dest, size, "%s", src);
}
```

### ✅ 5. Documentation et assertions

```c
/**
 * Copie une chaîne de manière sécurisée
 *
 * @param dest Buffer de destination (ne doit pas être NULL)
 * @param size Taille du buffer (doit être > 0)
 * @param src Chaîne source (ne doit pas être NULL)
 * @return 0 en cas de succès, -1 sinon
 *
 * @pre dest != NULL && src != NULL && size > 0
 * @post dest est terminée par '\0'
 */
int copy_string(char *dest, size_t size, const char *src) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(size > 0);

    // Implémentation...
}
```

## Bibliothèques tierces

### libbsd (GNU/Linux)

Apporte les fonctions BSD (strlcpy/strlcat) à glibc.

**Installation :**
```bash
# Debian/Ubuntu
sudo apt-get install libbsd-dev

# Fedora
sudo dnf install libbsd-devel
```

**Utilisation :**
```c
#include <bsd/string.h>

char dest[50];  
strlcpy(dest, source, sizeof(dest));  // Disponible !  
```

**Compilation :**
```bash
gcc programme.c -lbsd -o programme
```

### SafeStr (SafeStringLibrary)

Bibliothèque créée par Microsoft, compatible multi-plateforme.

```c
#include <safe_lib.h>

char dest[50];  
errno_t err = strcpy_s(dest, sizeof(dest), source);  
```

### GLib (GNOME)

Bibliothèque riche avec fonctions de manipulation de chaînes.

```c
#include <glib.h>

gchar *str = g_strdup("Bonjour");  // Allocation automatique  
g_strlcpy(dest, source, sizeof(dest));  // Copie sécurisée  
```

## Exemple complet : Application sécurisée

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_USERNAME 30
#define MAX_EMAIL 100
#define MAX_BUFFER 200

// Wrapper de copie sécurisé
int safe_copy(char *dest, size_t dest_size, const char *src) {
    if (!dest || !src || dest_size == 0) {
        return -1;
    }

    int written = snprintf(dest, dest_size, "%s", src);

    if (written < 0) {
        return -1;  // Erreur
    }

    if ((size_t)written >= dest_size) {
        fprintf(stderr, "Attention : troncature\n");
        return 1;  // Troncature
    }

    return 0;  // Succès
}

// Lecture sécurisée avec validation
int read_validated_input(char *buffer, size_t size,
                         const char *prompt,
                         int (*validator)(const char*)) {
    printf("%s", prompt);

    if (fgets(buffer, size, stdin) == NULL) {
        return -1;
    }

    // Retirer le '\n'
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        // Nettoyer stdin
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        fprintf(stderr, "Entrée trop longue\n");
        return -1;
    }

    // Validation
    if (validator && !validator(buffer)) {
        fprintf(stderr, "Format invalide\n");
        return -1;
    }

    return 0;
}

// Validateurs
int validate_username(const char *username) {
    size_t len = strlen(username);

    if (len == 0 || len > MAX_USERNAME - 1) {
        return 0;
    }

    for (size_t i = 0; username[i]; i++) {
        if (!isalnum(username[i]) && username[i] != '_') {
            return 0;
        }
    }

    return 1;
}

int validate_email(const char *email) {
    if (strlen(email) > MAX_EMAIL - 1) {
        return 0;
    }

    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    return (at && dot && dot > at);
}

// Fonction principale
int process_user_registration(void) {
    char username[MAX_USERNAME];
    char email[MAX_EMAIL];
    char message[MAX_BUFFER];

    // Lecture du nom d'utilisateur
    if (read_validated_input(username, sizeof(username),
                            "Nom d'utilisateur (alphanum + _) : ",
                            validate_username) != 0) {
        return -1;
    }

    // Lecture de l'email
    if (read_validated_input(email, sizeof(email),
                            "Email : ",
                            validate_email) != 0) {
        return -1;
    }

    // Construction du message de confirmation (sécurisé)
    int written = snprintf(message, sizeof(message),
                          "Inscription réussie !\n"
                          "Utilisateur : %s\n"
                          "Email : %s\n",
                          username, email);

    if (written >= (int)sizeof(message)) {
        fprintf(stderr, "Erreur : message tronqué\n");
        return -1;
    }

    printf("\n%s", message);

    return 0;
}

int main(void) {
    printf("=== Système d'inscription sécurisé ===\n\n");

    if (process_user_registration() != 0) {
        fprintf(stderr, "Échec de l'inscription\n");
        return 1;
    }

    return 0;
}
```

## Checklist de sécurisation

Avant de livrer votre code, vérifiez :

- [ ] **Aucune fonction dangereuse** : pas de `gets()`, `strcpy()`, `strcat()`, `sprintf()`
- [ ] **snprintf() partout** pour le formatage
- [ ] **sizeof() systématique** pour les limites de taille
- [ ] **Terminaison '\0' garantie** dans toutes les opérations
- [ ] **Vérification des retours** de snprintf() et autres fonctions
- [ ] **Validation des entrées** utilisateur
- [ ] **Tests de troncature** gérés proprement
- [ ] **Documentation** des contraintes de taille
- [ ] **Tests avec AddressSanitizer** (`-fsanitize=address`)
- [ ] **Tests avec Valgrind**
- [ ] **Analyse statique** (cppcheck, clang-tidy)

## Résumé des points clés

1. **snprintf() est votre meilleure amie**
   - Disponible partout (C99+)
   - Toujours sûre
   - Détection de troncature intégrée

2. **strncpy/strncat nécessitent de l'attention**
   - Toujours terminer manuellement avec '\0'
   - Calculs de taille complexes
   - Préférer des wrappers

3. **C11 Annex K peu répandue**
   - Principalement Windows
   - Pas dans glibc
   - Vérifier la disponibilité avant utilisation

4. **strlcpy/strlcat excellentes mais limitées**
   - Interface simple et sûre
   - BSD/musl uniquement
   - Implémenter ou utiliser libbsd sur Linux

5. **Créer ses propres wrappers**
   - API cohérente pour votre projet
   - Validation centralisée
   - Meilleure maintenabilité

6. **Toujours valider**
   - Les entrées utilisateur
   - Les tailles de buffer
   - Les valeurs de retour

7. **Défense en profondeur**
   - Plusieurs couches de protection
   - Assertions et vérifications
   - Tests automatisés

## Pour aller plus loin

Dans la section suivante, nous verrons :
- **Section 8.7** : Strings littérales et immutabilité
- **Chapitre 26** : Sécurité et code défensif (approfondi)

Les fonctions sécurisées sont votre première ligne de défense contre les buffer overflows. Prenez l'habitude de les utiliser systématiquement, et votre code sera beaucoup plus robuste et sûr !

---

**💡 Citation :** *"Security is not a product, but a process."* - Bruce Schneier

**🎯 Objectif atteint :** Vous avez maintenant toutes les connaissances pour manipuler les chaînes de manière sécurisée en C, quelle que soit votre plateforme !

⏭️ [Strings littérales et immutabilité](/08-tableaux-et-chaines/07-strings-litterales.md)
