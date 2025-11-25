🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4.6 Nouveautés C23

## Introduction

Le standard **C23** (ISO/IEC 9899:2024, publié en octobre 2024) est la plus récente révolution du langage C. Après C11 (2011) et C17 (2018, qui était principalement une correction de bugs), C23 apporte de **nombreuses améliorations** pour moderniser le langage tout en préservant sa philosophie.

Cette section présente les nouveautés de C23 qui concernent les **types de données** et la **représentation mémoire**, en complément de ce que nous avons vu dans ce chapitre.

**Important** : C23 est très récent et le support par les compilateurs est encore **en cours de déploiement**. Nous indiquerons l'état du support pour chaque fonctionnalité.

## État du support C23 en 2025

### GCC (GNU Compiler Collection)

- **GCC 13** : Support partiel de C23
- **GCC 14** (2024) : Support amélioré
- **GCC 15** (attendu 2025) : Support étendu

### Clang/LLVM

- **Clang 16-18** : Support progressif
- Support actif en développement

### MSVC (Microsoft Visual C++)

- Support limité pour l'instant
- Progression attendue dans les prochaines versions

### Comment vérifier le support ?

```bash
# GCC
gcc --version
gcc -std=c23 test.c  # Essayez de compiler avec -std=c23

# Clang
clang --version
clang -std=c23 test.c
```

**Recommandation pour 2025** : Utilisez **C11** ou **C17** en production, et expérimentez avec **C23** pour l'avenir.

## Nouvelles constantes de types

### 1. `true` et `false` comme mots-clés

**Avant C23** (C99/C11) :

```c
#include <stdbool.h>  // Nécessaire pour bool, true, false

bool est_pair(int n) {
    return n % 2 == 0;
}
```

**Avec C23** :

```c
// Plus besoin d'inclure <stdbool.h> !
bool est_pair(int n) {
    return n % 2 == 0;
}

int main(void) {
    bool resultat = true;  // true et false sont des mots-clés

    if (resultat) {
        printf("C'est vrai !\n");
    }

    return 0;
}
```

**Avantages** :
- `bool`, `true`, `false` sont maintenant des **mots-clés natifs**
- Plus besoin d'inclure `<stdbool.h>`
- Cohérence avec d'autres langages modernes

### 2. `nullptr` pour les pointeurs nuls

**Avant C23** :

```c
int* ptr = NULL;  // NULL est une macro définie comme 0 ou ((void*)0)
```

**Avec C23** :

```c
int* ptr = nullptr;  // nullptr est un mot-clé typé

if (ptr == nullptr) {
    printf("Pointeur nul\n");
}
```

**Différence avec `NULL`** :
- `nullptr` a un **type spécifique** : `nullptr_t`
- Plus sûr dans les surcharges de fonctions (comme en C++)
- Évite certaines ambiguïtés avec `NULL` qui peut être `0`

**Exemple de clarté** :

```c
#include <stdio.h>

void traiter(int valeur) {
    printf("Traitement d'un entier : %d\n", valeur);
}

void traiter_ptr(int* ptr) {
    printf("Traitement d'un pointeur\n");
}

int main(void) {
    // Avec NULL (0), il y a ambiguïté
    // traiter(NULL);  // Compile, mais appelle traiter(int) !

    // Avec nullptr, c'est clair
    traiter_ptr(nullptr);  // Sans ambiguïté

    return 0;
}
```

## Littéraux binaires (préfixe `0b`)

**Avant C23** :

```c
int masque = 0xF0;  // Hexadécimal : obligation de convertir mentalement
```

**Avec C23** :

```c
int masque = 0b11110000;  // Binaire : 240 en décimal

unsigned char flags = 0b00001111;  // Plus lisible pour les bits
unsigned int permissions = 0b101;   // rwx → r-x
```

**Avantages** :
- **Clarté** : voir directement les bits
- **Manipulation de bits** : plus naturel
- **Moins d'erreurs** : pas de conversion mentale

**Exemples pratiques** :

```c
// Configuration de registres matériels
#define GPIO_PIN_0  0b00000001
#define GPIO_PIN_1  0b00000010
#define GPIO_PIN_2  0b00000100
#define GPIO_PIN_3  0b00001000

unsigned char gpio_config = GPIO_PIN_0 | GPIO_PIN_2;  // 0b00000101

// Masques de bits
unsigned int READ_PERM  = 0b100;  // 4
unsigned int WRITE_PERM = 0b010;  // 2
unsigned int EXEC_PERM  = 0b001;  // 1

unsigned int permissions = READ_PERM | WRITE_PERM;  // 0b110 = 6 (rw-)
```

## Séparateur de chiffres (`'`) dans les littéraux

**Avant C23** :

```c
long population = 67000000;      // Difficile à lire
int grand_nombre = 1234567890;   // Combien de zéros ?
```

**Avec C23** :

```c
long population = 67'000'000;          // 67 millions
int grand_nombre = 1'234'567'890;      // Plus lisible !

// Fonctionne aussi avec hexadécimal et binaire
unsigned int couleur = 0xFF'AA'BB;     // RGB
unsigned int masque = 0b1111'0000'1010'0101;
```

**Règles** :
- Les apostrophes (`'`) sont **ignorées** par le compilateur
- Purement pour la **lisibilité**
- Peuvent être placées n'importe où dans le nombre

**Comparaison** :

```c
// Difficile à lire
long budget = 1500000000;

// Facile à lire
long budget = 1'500'000'000;  // 1.5 milliard
```

## Type `_BitInt(N)` : Entiers de taille arbitraire

C23 introduit des entiers de **taille arbitraire** avec `_BitInt(N)` où `N` est le nombre de bits.

**Syntaxe** :

```c
_BitInt(N)           // Entier signé de N bits
unsigned _BitInt(N)  // Entier non signé de N bits
```

**Exemples** :

```c
_BitInt(17) nombre17bits = 50000;         // Entier signé 17 bits
unsigned _BitInt(24) rgb = 0xFF00AA;      // 24 bits non signés

_BitInt(128) tres_grand = 12345678901234567890;  // 128 bits !
```

**Avantages** :
- Contrôle **précis** de la taille
- Économie de mémoire pour les valeurs bornées
- Support des très grands entiers (au-delà de 64 bits)

**Cas d'usage** :

```c
// Protocole avec champ de 12 bits
unsigned _BitInt(12) longueur_paquet = 4095;  // Max : 2^12 - 1

// Couleur RGB sur 24 bits exactement
unsigned _BitInt(24) couleur = 0xFF'AA'BB;

// Cryptographie avec 256 bits
unsigned _BitInt(256) cle_publique = /* ... */;
```

**Limites** :
- Support compilateur **encore limité** en 2025
- Performance variable selon la taille choisie
- Les opérations sur des tailles non standards peuvent être plus lentes

## Attribut `[[nodiscard]]`

Cet attribut avertit si une valeur de retour est ignorée.

**Syntaxe** :

```c
[[nodiscard]] int fonction_importante(void);
```

**Exemple** :

```c
#include <stdio.h>

[[nodiscard]] int ouvrir_fichier(const char* nom) {
    FILE* f = fopen(nom, "r");
    if (f == NULL) {
        return -1;  // Erreur
    }
    fclose(f);
    return 0;  // Succès
}

int main(void) {
    // AVERTISSEMENT du compilateur : valeur de retour ignorée !
    ouvrir_fichier("test.txt");

    // Correct : on vérifie le résultat
    if (ouvrir_fichier("test.txt") != 0) {
        printf("Erreur d'ouverture\n");
    }

    return 0;
}
```

**Avantages** :
- Évite d'oublier de vérifier les codes d'erreur
- Sécurité accrue
- Documentation claire de l'intention

## Attribut `[[maybe_unused]]`

Supprime les avertissements pour les variables/fonctions inutilisées.

**Exemple** :

```c
void fonction_debug([[maybe_unused]] int param_debug) {
    #ifdef DEBUG
        printf("Debug : %d\n", param_debug);
    #endif
    // param_debug n'est utilisé qu'en mode DEBUG
    // Sans [[maybe_unused]], avertissement en mode Release
}

int main(void) {
    [[maybe_unused]] int variable_future = 42;
    // Variable préparée pour une fonctionnalité future

    return 0;
}
```

## Amélioration de `typeof`

C23 standardise l'opérateur `typeof` (auparavant extension GNU).

**Syntaxe** :

```c
typeof(expression)  // Retourne le type de l'expression
```

**Exemples** :

```c
int x = 10;
typeof(x) y = 20;  // y est de type int

double pi = 3.14;
typeof(pi) autre_reel = 2.71;  // autre_reel est de type double

// Utile dans les macros
#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

int max = MAX(10, 20);  // max = 20
```

**Avantages** :
- Macros **génériques** plus sûres
- Évite de répéter les types
- Code plus maintenable

## Initialisation améliorée des structures

### Initialisation partielle plus permissive

**C11/C17** :

```c
struct Point {
    int x;
    int y;
    int z;
};

// Obligation d'initialiser dans l'ordre
struct Point p = {.x = 10, .y = 20, .z = 30};
```

**C23** : Plus de flexibilité

```c
struct Point {
    int x;
    int y;
    int z;
};

// Ordre libre, champs omis initialisés à zéro
struct Point p1 = {.z = 30, .x = 10};  // y = 0 automatiquement
struct Point p2 = {.y = 20};            // x = 0, z = 0
```

### Initialisation de tableaux améliorée

**C23** permet des initialisations plus flexibles :

```c
int tableau[10] = {
    [0] = 100,
    [5] = 500,
    [9] = 900
};
// Les éléments non spécifiés sont à 0
```

## Fonction `memset_explicit()` (C23)

Garantit que l'effacement de mémoire sensible ne sera **pas optimisé**.

**Problème avec `memset()` classique** :

```c
void fonction_securisee(void) {
    char mot_de_passe[100];
    // ... utilisation du mot de passe ...

    memset(mot_de_passe, 0, sizeof(mot_de_passe));  // Peut être optimisé !
    // Le compilateur peut supprimer cet appel s'il détecte que
    // mot_de_passe n'est plus utilisé après
}
```

**Solution C23** :

```c
#include <string.h>

void fonction_securisee(void) {
    char mot_de_passe[100];
    // ... utilisation du mot de passe ...

    memset_explicit(mot_de_passe, 0, sizeof(mot_de_passe));
    // Garantit que la mémoire est bien effacée, jamais optimisé
}
```

**Usage** :
- Données sensibles (mots de passe, clés crypto)
- Sécurité critique

## Type `char8_t` pour UTF-8

C23 introduit un type dédié pour les caractères UTF-8.

**Avant C23** :

```c
char texte_utf8[] = u8"Bonjour 世界";  // u8 prefix, mais type char
```

**Avec C23** :

```c
#include <uchar.h>

char8_t texte_utf8[] = u8"Bonjour 世界";  // Type dédié : char8_t
```

**Avantages** :
- Distinction claire entre ASCII et UTF-8
- Meilleure sûreté de type
- Prépare pour l'Unicode moderne

## Comparaison des standards

| Fonctionnalité | C89/C90 | C99 | C11 | C17 | C23 |
|----------------|---------|-----|-----|-----|-----|
| `bool`, `true`, `false` | ❌ | ✅ (via `<stdbool.h>`) | ✅ (via `<stdbool.h>`) | ✅ | ✅ (mots-clés) |
| `nullptr` | ❌ | ❌ | ❌ | ❌ | ✅ |
| Types à largeur fixe (`int32_t`) | ❌ | ✅ | ✅ | ✅ | ✅ |
| Littéraux binaires (`0b`) | ❌ | ❌ | ❌ | ❌ | ✅ |
| Séparateur de chiffres (`'`) | ❌ | ❌ | ❌ | ❌ | ✅ |
| `_BitInt(N)` | ❌ | ❌ | ❌ | ❌ | ✅ |
| `[[nodiscard]]` | ❌ | ❌ | ❌ | ❌ | ✅ |
| `typeof` standard | ❌ | ❌ | ❌ (extension) | ❌ (extension) | ✅ |
| `memset_explicit()` | ❌ | ❌ | ❌ | ❌ | ✅ |

## Exemple complet utilisant les nouveautés C23

```c
#include <stdio.h>

// Fonction avec attribut [[nodiscard]]
[[nodiscard]] bool valider_age(int age) {
    return age >= 18 && age <= 120;
}

// Fonction avec [[maybe_unused]]
void debug_info([[maybe_unused]] const char* message) {
    #ifdef DEBUG
        printf("DEBUG: %s\n", message);
    #endif
}

int main(void) {
    printf("=== Démo des nouveautés C23 ===\n\n");

    // 1. Mots-clés bool, true, false (sans <stdbool.h>)
    bool est_majeur = true;
    printf("1. Booléens natifs: %s\n", est_majeur ? "vrai" : "faux");

    // 2. nullptr pour les pointeurs
    int* ptr = nullptr;
    if (ptr == nullptr) {
        printf("2. Pointeur est null\n");
    }

    // 3. Littéraux binaires
    unsigned char flags = 0b1010'1010;  // Avec séparateur !
    printf("3. Flags binaires: 0x%02X\n", flags);

    // 4. Séparateurs de chiffres
    long population = 67'000'000;
    printf("4. Population: %ld habitants\n", population);

    // 5. Fonction avec [[nodiscard]]
    if (valider_age(25)) {
        printf("5. Âge valide\n");
    }
    // valider_age(30);  // Avertissement : valeur ignorée !

    // 6. typeof
    int x = 42;
    typeof(x) y = 100;  // y est automatiquement de type int
    printf("6. typeof: x=%d, y=%d\n", x, y);

    // 7. Initialisation flexible de structure
    struct Point {
        int x;
        int y;
        int z;
    };

    struct Point p = {.z = 30, .x = 10};  // y = 0 automatiquement
    printf("7. Point: x=%d, y=%d, z=%d\n", p.x, p.y, p.z);

    debug_info("Fin du programme");

    return 0;
}
```

**Compilation** :

```bash
gcc -std=c23 -Wall -Wextra demo_c23.c -o demo_c23
./demo_c23
```

**Sortie** :

```
=== Démo des nouveautés C23 ===

1. Booléens natifs: vrai
2. Pointeur est null
3. Flags binaires: 0xAA
4. Population: 67000000 habitants
5. Âge valide
6. typeof: x=42, y=100
7. Point: x=10, y=0, z=30
```

## Quand utiliser C23 ?

### ✅ Utilisez C23 si :

- Vous démarrez un **nouveau projet**
- Vous avez un compilateur récent (GCC 14+, Clang 18+)
- Vous voulez les **dernières fonctionnalités**
- Vous pouvez tester sur votre environnement cible

### ⚠️ Restez en C11/C17 si :

- Vous maintenez du **code legacy**
- Vous ciblez des **systèmes embarqués** avec compilateurs anciens
- La **compatibilité maximale** est critique
- Vous travaillez dans un environnement **contraint**

### 🔄 Migration progressive :

```c
// Détection du standard à la compilation
#if __STDC_VERSION__ >= 202311L
    // Code C23
    bool valeur = true;
#else
    // Code C11
    #include <stdbool.h>
    bool valeur = true;
#endif
```

## Ressources pour suivre C23

### Documentation officielle

- **ISO/IEC 9899:2024** : Standard officiel (payant)
- **WG14** : Groupe de travail C (documents gratuits)
  - https://www.open-std.org/jtc1/sc22/wg14/

### Support des compilateurs

- **GCC C23 Status** : https://gcc.gnu.org/projects/c23-status.html
- **Clang C23 Status** : https://clang.llvm.org/c_status.html

### Tutoriels et articles

- **cppreference** : https://en.cppreference.com/w/c/23
- **Modern C (2nd edition)** : Livre à jour avec C23

## Points clés à retenir

🔹 **C23** : Standard le plus récent (2024), modernise le langage

🔹 **`bool`/`true`/`false`** : mots-clés natifs, plus besoin de `<stdbool.h>`

🔹 **`nullptr`** : pointeur nul typé, plus sûr que `NULL`

🔹 **Littéraux binaires** : `0b1010`, plus clairs pour les bits

🔹 **Séparateur `'`** : `1'000'000`, améliore la lisibilité

🔹 **`_BitInt(N)`** : entiers de taille arbitraire

🔹 **`[[nodiscard]]`** : force la vérification des valeurs de retour

🔹 **Support limité** : GCC 14+, Clang 18+ recommandés

🔹 **Production 2025** : C11/C17 reste la référence, C23 pour l'avenir

## Ce que nous verrons ensuite

Après avoir terminé le **Chapitre 4 : Types de données et Représentation mémoire**, vous passerez au :

**Chapitre 5 : Opérateurs et Structures de contrôle**

Où nous apprendrons à :
- Manipuler les données avec des opérateurs (arithmétiques, logiques, bit-à-bit)
- Contrôler le flux d'exécution (if, switch, boucles)
- Formater le code proprement
- Utiliser clang-format pour automatiser le style

---

**🎯 Vous connaissez maintenant les nouveautés de C23 ! Le langage C continue d'évoluer tout en restant fidèle à ses principes de performance et de contrôle bas niveau.**

⏭️ [Opérateurs et Structures de contrôle](/05-operateurs-et-controle/README.md)
