🔝 Retour au [Sommaire](/SOMMAIRE.md)

# A.1 Standards C officiels

## Introduction aux standards C

Le langage C, contrairement à ce que l'on pourrait penser, n'est pas figé dans le temps. Depuis sa création dans les années 1970, il a évolué à travers différentes versions standardisées. Comprendre ces standards est essentiel pour écrire du code portable et moderne.

Un **standard** est une spécification officielle qui définit ce qui fait partie du langage C : sa syntaxe, ses fonctionnalités, son comportement. Il permet d'assurer que votre code fonctionnera de la même manière, quel que soit le compilateur ou le système d'exploitation utilisé.

---

## Les différents standards C

### K&R C (1978) - Le C originel

**Contexte historique :**
- Première spécification du langage dans le livre "The C Programming Language" par Brian Kernighan et Dennis Ritchie
- Non standardisé officiellement, mais référence de facto
- Utilisé pour développer UNIX

**Caractéristiques :**
- Syntaxe simple et minimaliste
- Pas de prototypes de fonctions obligatoires
- Bibliothèque standard limitée

**Pourquoi le connaître en 2025 ?**
- Vous le rencontrerez dans du code très ancien
- Comprendre l'évolution du langage
- Certains exemples de livres anciens l'utilisent encore

**Exemple K&R C :**
```c
/* Déclaration K&R - syntaxe ancienne */
int add(a, b)
int a, b;
{
    return a + b;
}
```

⚠️ **Ne pas utiliser cette syntaxe aujourd'hui !**

---

### ANSI C / C89 / C90 (1989-1990) - Le premier standard officiel

**Nom officiel :** ISO/IEC 9899:1990

**Pourquoi plusieurs noms ?**
- **C89** : Adopté par l'ANSI (American National Standards Institute) en 1989
- **C90** : Adopté par l'ISO (International Organization for Standardization) en 1990
- **ANSI C** : Terme générique encore utilisé aujourd'hui

**Apports majeurs :**
- Prototypes de fonctions obligatoires
- Mots-clés `const` et `volatile`
- Bibliothèque standard définie (stdio.h, stdlib.h, string.h, etc.)
- Préprocesseur amélioré
- Types de données standardisés

**Exemple C89/C90 :**
```c
#include <stdio.h>

/* Prototype obligatoire */
int add(int a, int b);

int add(int a, int b) {
    return a + b;
}

int main(void) {
    printf("Résultat : %d\n", add(3, 4));
    return 0;
}
```

**Impact :**
- Base de compatibilité pour tous les compilateurs modernes
- Code C89 compilera partout

---

### C99 (1999) - Modernisation du langage

**Nom officiel :** ISO/IEC 9899:1999

**Apports majeurs :**

#### 1. Déclarations de variables flexibles
```c
/* C89 : toutes les variables en début de bloc */
int main(void) {
    int i;
    int sum = 0;

    for (i = 0; i < 10; i++) {
        sum += i;
    }
    return 0;
}

/* C99 : déclarations n'importe où */
int main(void) {
    int sum = 0;

    for (int i = 0; i < 10; i++) {  // Variable déclarée dans le for
        sum += i;
    }
    return 0;
}
```

#### 2. Nouveaux types de données
- `long long` : Entiers 64 bits
- `_Bool` : Type booléen natif
- `_Complex` : Nombres complexes
- Types entiers de taille fixe (`stdint.h`) : `int32_t`, `uint64_t`, etc.

```c
#include <stdint.h>
#include <stdbool.h>

int main(void) {
    int64_t big_number = 9223372036854775807LL;
    bool is_active = true;
    return 0;
}
```

#### 3. Commentaires de style C++
```c
// Commentaire sur une ligne (style C++)
/* Commentaire multiligne
   toujours supporté */
```

#### 4. Tableaux de longueur variable (VLA)
```c
void process_array(int n) {
    int array[n];  // Taille déterminée au runtime
    // ...
}
```

#### 5. Fonctions inline
```c
inline int square(int x) {
    return x * x;
}
```

#### 6. Macros variadiques
```c
#define debug_print(fmt, ...) \
    fprintf(stderr, fmt, __VA_ARGS__)
```

**Adoption :**
- Support complet dans GCC depuis 2001
- Clang supporte C99 complètement
- MSVC (Microsoft) a mis du temps (support partiel seulement)

---

### C11 (2011) - Concurrence et sécurité

**Nom officiel :** ISO/IEC 9899:2011

**Apports majeurs :**

#### 1. Support natif du multithreading
```c
#include <threads.h>

int thread_function(void* arg) {
    printf("Hello from thread!\n");
    return 0;
}

int main(void) {
    thrd_t thread;
    thrd_create(&thread, thread_function, NULL);
    thrd_join(thread, NULL);
    return 0;
}
```

#### 2. Opérations atomiques
```c
#include <stdatomic.h>

atomic_int counter = 0;

void increment(void) {
    atomic_fetch_add(&counter, 1);  // Thread-safe
}
```

#### 3. Assertions statiques
```c
_Static_assert(sizeof(int) == 4, "int doit faire 4 octets");
```

#### 4. Fonctions sécurisées (Annexe K - optionnelle)
```c
// Versions sécurisées de fonctions standard
strcpy_s(dest, dest_size, src);  // Au lieu de strcpy()
```

#### 5. Types génériques avec `_Generic`
```c
#define print_type(x) _Generic((x), \
    int: printf("int: %d\n", x),    \
    float: printf("float: %f\n", x), \
    default: printf("unknown\n"))
```

#### 6. Spécificateur d'alignement
```c
_Alignas(16) int aligned_array[4];
```

#### 7. Suppression de fonctions obsolètes
- `gets()` officiellement retirée (dangereuse)

**Statut :**
- **C11 est le standard de production recommandé en 2025**
- Excellent support dans GCC, Clang
- Support moderne de la concurrence
- Balance entre modernité et portabilité

---

### C17 / C18 (2018) - Corrections et clarifications

**Nom officiel :** ISO/IEC 9899:2018

**Nature :**
- Pas de nouvelles fonctionnalités majeures
- Corrections de bugs dans la spécification C11
- Clarifications et améliorations de la rédaction
- Souvent appelé "C11 bug fix release"

**Pourquoi important ?**
- Corrige des ambiguïtés dans C11
- C'est techniquement la version "stable" de C11
- Compilateurs récents ciblent C17 par défaut

**Utilisation :**
```bash
# Compiler en C17
gcc -std=c17 program.c -o program
```

---

### C23 (2023) - Le standard moderne

**Nom officiel :** ISO/IEC 9899:2023

**Apports majeurs :**

#### 1. Mot-clé `typeof` et `typeof_unqual`
```c
int x = 5;
typeof(x) y = 10;  // y est de type int
```

#### 2. Amélioration des attributs
```c
[[nodiscard]] int important_function(void);
[[deprecated]] void old_function(void);
```

#### 3. Littéraux binaires
```c
int flags = 0b10101010;  // Notation binaire
```

#### 4. Séparateurs de chiffres
```c
long big_number = 1'000'000'000;  // Plus lisible
```

#### 5. `auto` pour déduction de type
```c
auto x = 5;  // x est int
```

#### 6. Fonctions `strdup()` et `strndup()` standardisées
```c
char* copy = strdup("Hello");  // Officiellement dans C23
```

#### 7. `constexpr` (limité)
```c
constexpr int max_size = 100;
```

#### 8. Amélioration de `_Static_assert`
```c
static_assert(sizeof(int) == 4);  // Plus besoin de message
```

#### 9. Amélioration de l'UTF-8
```c
char8_t utf8_string[] = u8"Hello 世界";
```

#### 10. `nullptr` officiellement dans le standard
```c
int* ptr = nullptr;  // Meilleur que NULL
```

**État du support (2025) :**
- ⚠️ Support partiel dans GCC 13+, Clang 16+
- ⚠️ Beaucoup de fonctionnalités encore en développement
- ⚠️ **Pas recommandé pour la production actuellement**
- ✅ Intéressant pour expérimenter et apprendre

**Vérifier le support :**
```bash
# GCC
gcc --version
gcc -std=c23 -E -dM - < /dev/null | grep __STDC_VERSION__

# Clang
clang --version
clang -std=c23 -E -dM - < /dev/null | grep __STDC_VERSION__
```

---

## Quel standard choisir en 2025 ?

### Recommandations par cas d'usage

#### Pour la production et les projets professionnels
**→ C11 ou C17**

**Raisons :**
- Support universel dans tous les compilateurs modernes
- Fonctionnalités modernes (threads, atomics)
- Stabilité éprouvée
- Portabilité maximale

```bash
# Compilation recommandée
gcc -std=c11 -Wall -Wextra -O2 program.c -o program
```

#### Pour l'apprentissage
**→ C11 ou C17**

**Raisons :**
- Toute la documentation utilise C11 comme base
- Vous pourrez lire et comprendre 99% du code moderne
- Pas de piège avec des fonctionnalités non supportées

#### Pour l'embarqué ou le code legacy
**→ C99 (parfois C89)**

**Raisons :**
- Certains compilateurs embarqués ne supportent que C99
- Code legacy peut être en C89
- Contraintes matérielles strictes

#### Pour expérimenter les nouveautés
**→ C23**

**Raisons :**
- Découvrir les évolutions futures du langage
- Projets personnels sans contraintes
- **Attention :** Compiler avec GCC 13+ ou Clang 16+ minimum

---

## Vérifier le standard supporté par votre compilateur

### Macro `__STDC_VERSION__`

Chaque standard définit une valeur pour cette macro :

```c
#include <stdio.h>

int main(void) {
#ifdef __STDC_VERSION__
    printf("Standard C détecté : ");

    #if __STDC_VERSION__ == 199409L
        printf("C94\n");
    #elif __STDC_VERSION__ == 199901L
        printf("C99\n");
    #elif __STDC_VERSION__ == 201112L
        printf("C11\n");
    #elif __STDC_VERSION__ == 201710L
        printf("C17\n");
    #elif __STDC_VERSION__ > 201710L
        printf("C23 (ou plus récent)\n");
    #else
        printf("Version inconnue\n");
    #endif
#else
    printf("Standard C89/C90 (pré-C99)\n");
#endif

    return 0;
}
```

### Options de compilation

```bash
# GCC : Spécifier le standard
gcc -std=c89 program.c    # Force C89
gcc -std=c99 program.c    # Force C99
gcc -std=c11 program.c    # Force C11 (recommandé)
gcc -std=c17 program.c    # Force C17 (recommandé)
gcc -std=c23 program.c    # Force C23 (expérimental)

# Extensions GNU (non portable)
gcc -std=gnu11 program.c  # C11 + extensions GNU

# Clang : Même syntaxe
clang -std=c11 program.c
```

### Vérifier les fonctionnalités disponibles

```c
// Vérifier si les threads C11 sont disponibles
#ifdef __STDC_NO_THREADS__
    #error "Les threads C11 ne sont pas supportés"
#endif

// Vérifier si les atomics sont disponibles
#ifdef __STDC_NO_ATOMICS__
    #error "Les atomics C11 ne sont pas supportés"
#endif

// Vérifier si les VLA sont supportés
#ifdef __STDC_NO_VLA__
    #error "Les tableaux de longueur variable ne sont pas supportés"
#endif
```

---

## Bonnes pratiques pour le choix du standard

### 1. Documentez votre choix

Dans votre `CMakeLists.txt` ou `Makefile` :

```cmake
# CMake
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # Désactive les extensions GNU
```

```makefile
# Makefile
CFLAGS = -std=c11 -Wall -Wextra -pedantic
```

### 2. Soyez cohérent dans tout le projet

❌ **Mauvais :**
```bash
gcc -std=c99 file1.c -c
gcc -std=c11 file2.c -c
gcc file1.o file2.o -o program  # Mélange de standards !
```

✅ **Bon :**
```bash
gcc -std=c11 file1.c -c
gcc -std=c11 file2.c -c
gcc file1.o file2.o -o program
```

### 3. Activez les avertissements stricts

```bash
gcc -std=c11 -Wall -Wextra -pedantic program.c
```

- `-Wall` : Active les avertissements courants
- `-Wextra` : Active des avertissements supplémentaires
- `-pedantic` : Rejette les extensions non standard

### 4. Testez avec plusieurs compilateurs

Si possible, testez votre code avec :
- GCC
- Clang
- MSVC (si compatibilité Windows nécessaire)

---

## Compatibilité ascendante et descendante

### Compatibilité ascendante (forward compatibility)

Un code écrit en C99 devrait compiler en C11/C17/C23 sans modification.

✅ **Garanti par le standard :** Les nouvelles versions ne cassent (presque) jamais l'ancien code.

**Exception notable :** `gets()` retirée en C11 (mais elle était dangereuse).

### Compatibilité descendante (backward compatibility)

Un code C23 ne compilera pas forcément en C99.

❌ **Exemple :**
```c
// C23
constexpr int size = 100;  // Ne compile pas en C11
```

---

## Ressources officielles

### Spécifications officielles (payantes via ISO)

Les standards ISO sont vendus, mais voici les références officielles :

- **C89/C90** : ISO/IEC 9899:1990
- **C99** : ISO/IEC 9899:1999
- **C11** : ISO/IEC 9899:2011
- **C17** : ISO/IEC 9899:2018
- **C23** : ISO/IEC 9899:2023

💡 **Astuce :** Les brouillons de travail (drafts) sont souvent disponibles gratuitement et contiennent quasiment le texte final.

### Brouillons publics (gratuits)

- **C11 draft** : [N1570](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
- **C17 draft** : [N2176](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2176.pdf)
- **C23 draft** : Disponible sur [open-std.org](http://www.open-std.org/jtc1/sc22/wg14/)

### Sites de référence

- **cppreference.com/c** : Documentation de référence (gratuite, excellente)
- **ISO C Working Group (WG14)** : [open-std.org/jtc1/sc22/wg14](http://www.open-std.org/jtc1/sc22/wg14/)
- **GCC C dialect options** : [gcc.gnu.org/onlinedocs](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)

---

## Résumé visuel

```
┌────────────────────────────────────────────────────────────────────┐
│                    Évolution des Standards C                       │
├──────────┬──────────┬──────────────────────────────────────────────┤
│ Année    │ Standard │ Caractéristiques principales                 │
├──────────┼──────────┼──────────────────────────────────────────────┤
│ 1978     │ K&R C    │ C originel, non standardisé                  │
├──────────┼──────────┼──────────────────────────────────────────────┤
│ 1989-90  │ C89/C90  │ Premier standard officiel                    │
│          │          │ Base de compatibilité universelle            │
├──────────┼──────────┼──────────────────────────────────────────────┤
│ 1999     │ C99      │ Modernisation : //, stdint.h, inline, VLA    │
├──────────┼──────────┼──────────────────────────────────────────────┤
│ 2011     │ C11      │ ⭐ Threads, atomics, _Generic                │
│          │          │ ✅ RECOMMANDÉ EN PRODUCTION                  │
├──────────┼──────────┼──────────────────────────────────────────────┤
│ 2018     │ C17/C18  │ Corrections de C11, pas de nouvelles feat    │
│          │          │ ✅ RECOMMANDÉ EN PRODUCTION                  │
├──────────┼──────────┼──────────────────────────────────────────────┤
│ 2023     │ C23      │ typeof, [[]], nullptr, constexpr             │
│          │          │ ⚠️ EXPÉRIMENTAL, support partiel             │
└──────────┴──────────┴──────────────────────────────────────────────┘
```

---

## Conclusion

En 2025, voici ce qu'il faut retenir :

✅ **Pour 99% des projets : Utilisez C11 ou C17**
- Support universel
- Fonctionnalités modernes et stabilité
- Documentation abondante

✅ **Connaître C89/C99 pour lire du code ancien**
- Beaucoup de code legacy existe encore
- Certains projets embarqués sont bloqués sur ces versions

✅ **Explorer C23 pour la culture générale**
- Comprendre où va le langage
- Ne pas l'utiliser en production encore

❌ **Éviter K&R C et C89 pour du nouveau code**
- Obsolètes et limitants
- Sauf contraintes matérielles extrêmes

**Le standard C évolue lentement, mais sûrement. Cette stabilité est une force : votre code C11 d'aujourd'hui fonctionnera encore dans 20 ans.**

---


⏭️ [Man pages Linux essentielles](/annexes/references-essentielles/02-man-pages-linux.md)
