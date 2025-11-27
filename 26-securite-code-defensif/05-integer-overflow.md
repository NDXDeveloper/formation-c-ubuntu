🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.5 Integer overflow

## Introduction

Les **integer overflows** (débordements d'entiers) sont une classe de vulnérabilités subtiles mais dangereuses en C. Contrairement aux buffer overflows qui sont souvent détectés rapidement par un crash, les integer overflows peuvent passer inaperçus pendant longtemps tout en causant des dégâts considérables.

### Qu'est-ce qu'un integer overflow ?

Un integer overflow se produit lorsqu'une opération arithmétique produit un résultat qui **dépasse la capacité de stockage** du type entier utilisé.

**Analogie simple** : Imaginez un compteur mécanique à 3 chiffres qui affiche 999. Si vous ajoutez 1, au lieu d'afficher 1000, il revient à 000 (car il ne peut afficher que 3 chiffres). C'est exactement ce qui se passe avec les entiers en C !

---

## Représentation des entiers en mémoire

### Taille limitée

En C, les entiers ont une taille fixe qui dépend du type :

| Type | Taille typique | Valeur min (signé) | Valeur max (signé) | Valeur max (non signé) |
|------|----------------|--------------------|--------------------|------------------------|
| `char` | 1 octet (8 bits) | -128 | 127 | 255 |
| `short` | 2 octets (16 bits) | -32,768 | 32,767 | 65,535 |
| `int` | 4 octets (32 bits) | -2,147,483,648 | 2,147,483,647 | 4,294,967,295 |
| `long` | 4 ou 8 octets | Dépend de l'architecture | | |
| `long long` | 8 octets (64 bits) | -9,223,372,036,854,775,808 | 9,223,372,036,854,775,807 | 18,446,744,073,709,551,615 |

### Représentation en bits (exemple avec 8 bits)

```
Unsigned (0 à 255) :
0        = 00000000
1        = 00000001
127      = 01111111
255      = 11111111

Signed (-128 à 127, complément à deux) :
0        = 00000000
1        = 00000001
127      = 01111111
-1       = 11111111
-128     = 10000000
```

---

## Comment se produisent les overflows ?

### 1. Overflow sur entiers non signés (unsigned)

Les entiers non signés utilisent l'**arithmétique modulaire** : ils "bouclent" (wraparound).

```c
#include <stdio.h>
#include <limits.h>

int main(void) {
    unsigned char c = 255;  // Maximum pour unsigned char

    printf("c = %u\n", c);           // Affiche : 255
    c = c + 1;                       // Overflow !
    printf("c + 1 = %u\n", c);       // Affiche : 0 (bouclage)

    return 0;
}
```

**Visualisation** :
```
255 (11111111) + 1 = 256 (100000000)
                      ^
                      Ce bit déborde (9 bits nécessaires)

Résultat sur 8 bits : 00000000 = 0
```

### 2. Overflow sur entiers signés (signed)

Pour les entiers signés, l'overflow est un **comportement indéfini** en C !

```c
#include <stdio.h>
#include <limits.h>

int main(void) {
    int i = INT_MAX;  // 2,147,483,647 sur 32 bits

    printf("i = %d\n", i);           // Affiche : 2147483647
    i = i + 1;                       // Comportement indéfini !
    printf("i + 1 = %d\n", i);       // Peut afficher : -2147483648

    return 0;
}
```

**Pourquoi comportement indéfini ?** Le standard C ne spécifie pas ce qui doit se passer. En pratique, avec le complément à deux :

```
INT_MAX  = 01111111 11111111 11111111 11111111 (2,147,483,647)
+ 1      = 00000000 00000000 00000000 00000001
         = 10000000 00000000 00000000 00000000 (INT_MIN = -2,147,483,648)
```

Le bit de signe bascule, transformant le plus grand nombre positif en plus petit nombre négatif !

### 3. Underflow (débordement vers le bas)

```c
unsigned int u = 0;
u = u - 1;  // Underflow : devient UINT_MAX (4,294,967,295)

int i = INT_MIN;
i = i - 1;  // Comportement indéfini
```

---

## Les dangers des integer overflows

### 1. Allocation mémoire insuffisante

**Scénario le plus dangereux** : L'overflow est utilisé pour calculer la taille d'une allocation.

```c
// ❌ CODE VULNÉRABLE
void allouer_tableau(size_t nb_elements) {
    // Sur un système 32 bits, size_t est unsigned int (4 octets)
    size_t taille = nb_elements * sizeof(int);  // Peut overflower !

    int *tableau = malloc(taille);
    if (tableau == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return;
    }

    // Remplissage du tableau
    for (size_t i = 0; i < nb_elements; i++) {
        tableau[i] = i;  // Buffer overflow si taille a overflowé !
    }

    free(tableau);
}

// Attaque :
// Sur système 32 bits, si nb_elements = 1073741824 (2^30)
// taille = 1073741824 * 4 = 4294967296 = 0 (overflow !)
// malloc(0) réussit souvent et alloue un petit buffer
// Puis on écrit 1073741824 éléments → énorme buffer overflow !
```

**Conséquences** :
- Corruption massive de la mémoire
- Crash du programme
- Exploitation possible par un attaquant

### 2. Contournement de vérifications de sécurité

```c
// ❌ CODE VULNÉRABLE
bool copier_donnees(char *dest, size_t dest_size,
                    const char *source, size_t source_size) {
    // Vérification de sécurité... mais vulnérable !
    if (source_size + 100 > dest_size) {
        fprintf(stderr, "Buffer trop petit\n");
        return false;
    }

    memcpy(dest, source, source_size);
    return true;
}

// Attaque :
// Si source_size = SIZE_MAX (4,294,967,295 sur 32 bits)
// source_size + 100 = 99 (overflow !)
// 99 > dest_size est probablement false
// La vérification est contournée !
```

### 3. Boucles infinies

```c
// ❌ BOUCLE INFINIE potentielle
void traiter_elements(unsigned int debut, unsigned int fin) {
    for (unsigned int i = debut; i <= fin; i++) {
        traiter(i);
    }
}

// Si debut = 10 et fin = UINT_MAX (4294967295)
// Quand i atteint UINT_MAX :
// i++ → overflow → i devient 0
// 0 <= UINT_MAX est toujours vrai → boucle infinie !
```

### 4. Logique métier incorrecte

```c
// ❌ LOGIQUE CASSÉE
int calculer_prix_total(int prix_unitaire, int quantite) {
    int total = prix_unitaire * quantite;

    if (total < 0) {
        fprintf(stderr, "Erreur : prix négatif\n");
        return -1;
    }

    return total;
}

// Si prix_unitaire = 100000 et quantite = 30000
// total = 3000000000
// Sur un int 32 bits, cela dépasse INT_MAX (2147483647)
// Overflow → total devient négatif !
// Un attaquant pourrait payer un prix négatif (recevoir de l'argent)
```

---

## Types d'overflow selon le contexte

### 1. Overflow dans les calculs de taille

```c
// ❌ Vulnérable
size_t taille = largeur * hauteur * sizeof(pixel);
void *buffer = malloc(taille);
```

### 2. Overflow dans les indices de tableau

```c
// ❌ Vulnérable
unsigned int index = user_input + offset;
if (index < array_size) {  // Peut être contourné par overflow
    array[index] = value;
}
```

### 3. Overflow dans les comparaisons

```c
// ❌ Vulnérable
if (a + b < a) {  // Détection d'overflow ? Pas toujours !
    // Pour unsigned c'est OK, pour signed c'est UB
}
```

### 4. Overflow dans les conversions de type

```c
// ❌ Vulnérable : conversion avec perte
long long big_number = 3000000000LL;
int small_number = (int)big_number;  // Overflow dans la conversion
```

---

## Prévention des integer overflows

### 1. Utiliser les types appropriés

```c
// ❌ Mauvais : int pour des tailles
int taille = longueur * largeur;

// ✅ Bon : size_t pour des tailles
size_t taille = longueur * largeur;
```

```c
// ❌ Mauvais : unsigned pour des compteurs qui peuvent devenir négatifs
unsigned int compteur = 10;
compteur -= 20;  // Underflow → très grand nombre positif

// ✅ Bon : signed si des valeurs négatives sont possibles
int compteur = 10;
compteur -= 20;  // -10 (comportement attendu)
```

### 2. Vérifier avant l'opération

#### Addition

```c
// ✅ Vérification avant addition (unsigned)
bool addition_secure_unsigned(unsigned int a, unsigned int b, unsigned int *result) {
    if (a > UINT_MAX - b) {
        fprintf(stderr, "Erreur : overflow dans l'addition\n");
        return false;
    }
    *result = a + b;
    return true;
}

// Utilisation
unsigned int somme;
if (addition_secure_unsigned(a, b, &somme)) {
    printf("Somme : %u\n", somme);
}
```

**Explication** : Si `a > UINT_MAX - b`, alors `a + b > UINT_MAX` (overflow).

```c
// ✅ Vérification avant addition (signed)
bool addition_secure_signed(int a, int b, int *result) {
    // Overflow positif
    if (a > 0 && b > 0 && a > INT_MAX - b) {
        fprintf(stderr, "Erreur : overflow positif\n");
        return false;
    }

    // Overflow négatif
    if (a < 0 && b < 0 && a < INT_MIN - b) {
        fprintf(stderr, "Erreur : overflow négatif\n");
        return false;
    }

    *result = a + b;
    return true;
}
```

#### Multiplication

```c
// ✅ Vérification avant multiplication (unsigned)
bool multiplication_secure_unsigned(size_t a, size_t b, size_t *result) {
    if (a != 0 && b > SIZE_MAX / a) {
        fprintf(stderr, "Erreur : overflow dans la multiplication\n");
        return false;
    }
    *result = a * b;
    return true;
}
```

**Explication** : Si `b > SIZE_MAX / a`, alors `a * b > SIZE_MAX` (overflow).

```c
// ✅ Vérification avant multiplication (signed, simplifié)
bool multiplication_secure_signed(int a, int b, int *result) {
    // Cas spéciaux
    if (a == 0 || b == 0) {
        *result = 0;
        return true;
    }

    if (a == -1 && b == INT_MIN) return false;
    if (b == -1 && a == INT_MIN) return false;

    // Vérification générale
    if (a > 0 && b > 0 && a > INT_MAX / b) return false;
    if (a > 0 && b < 0 && b < INT_MIN / a) return false;
    if (a < 0 && b > 0 && a < INT_MIN / b) return false;
    if (a < 0 && b < 0 && a < INT_MAX / b) return false;

    *result = a * b;
    return true;
}
```

#### Soustraction

```c
// ✅ Vérification avant soustraction (unsigned)
bool soustraction_secure_unsigned(unsigned int a, unsigned int b, unsigned int *result) {
    if (a < b) {
        fprintf(stderr, "Erreur : underflow dans la soustraction\n");
        return false;
    }
    *result = a - b;
    return true;
}
```

### 3. Utiliser les fonctions sécurisées (GCC builtins)

GCC fournit des fonctions intégrées qui détectent automatiquement les overflows :

```c
#include <stdio.h>
#include <stdbool.h>

bool addition_builtin(int a, int b, int *result) {
    if (__builtin_add_overflow(a, b, result)) {
        fprintf(stderr, "Erreur : overflow détecté\n");
        return false;
    }
    return true;
}

bool multiplication_builtin(int a, int b, int *result) {
    if (__builtin_mul_overflow(a, b, result)) {
        fprintf(stderr, "Erreur : overflow détecté\n");
        return false;
    }
    return true;
}

bool soustraction_builtin(int a, int b, int *result) {
    if (__builtin_sub_overflow(a, b, result)) {
        fprintf(stderr, "Erreur : overflow détecté\n");
        return false;
    }
    return true;
}

// Utilisation
int main(void) {
    int result;

    if (addition_builtin(INT_MAX, 1, &result)) {
        printf("Résultat : %d\n", result);
    } else {
        printf("Overflow détecté et géré\n");
    }

    return 0;
}
```

**Avantages** :
- Efficace (compilé en quelques instructions)
- Portable entre types (fonctionne avec int, long, long long, etc.)
- Détection précise

### 4. Utiliser C23 checked arithmetic (si disponible)

C23 introduit `<stdckdint.h>` avec des fonctions standardisées :

```c
#ifdef __STDC_VERSION__ >= 202311L
#include <stdckdint.h>

bool addition_c23(int a, int b, int *result) {
    if (ckd_add(result, a, b)) {
        fprintf(stderr, "Overflow détecté\n");
        return false;
    }
    return true;
}
#endif
```

### 5. Limiter les valeurs d'entrée

```c
// ✅ Validation stricte
bool allouer_tableau_securise(size_t nb_elements) {
    // Limite raisonnable (par exemple, 10 millions d'éléments)
    const size_t MAX_ELEMENTS = 10000000;

    if (nb_elements == 0 || nb_elements > MAX_ELEMENTS) {
        fprintf(stderr, "Erreur : nombre d'éléments invalide\n");
        return false;
    }

    // Vérifier l'overflow avant multiplication
    if (nb_elements > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Erreur : taille trop grande\n");
        return false;
    }

    size_t taille = nb_elements * sizeof(int);
    int *tableau = malloc(taille);

    if (tableau == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return false;
    }

    // Utilisation sécurisée du tableau...

    free(tableau);
    return true;
}
```

---

## Exemples complets : Avant/Après

### Exemple 1 : Calcul de taille pour allocation

```c
// ❌ VERSION VULNÉRABLE
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int largeur;
    int hauteur;
} Image;

void* creer_image(int largeur, int hauteur) {
    // Calcul de la taille en octets
    size_t taille = largeur * hauteur * sizeof(int);  // Overflow possible !

    void *pixels = malloc(taille);
    if (pixels == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return NULL;
    }

    return pixels;
}

// Si largeur = 65536 et hauteur = 65536 (sur système 32 bits)
// taille = 65536 * 65536 * 4 = 17179869184 = overflow sur 32 bits
// Résultat : malloc alloue très peu de mémoire
// Utilisation ultérieure → buffer overflow massif
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

typedef struct {
    int largeur;
    int hauteur;
} Image;

void* creer_image_securise(int largeur, int hauteur) {
    // Validation des dimensions
    if (largeur <= 0 || hauteur <= 0) {
        fprintf(stderr, "Erreur : dimensions invalides\n");
        return NULL;
    }

    // Limites raisonnables
    const int MAX_DIM = 10000;
    if (largeur > MAX_DIM || hauteur > MAX_DIM) {
        fprintf(stderr, "Erreur : dimensions trop grandes\n");
        return NULL;
    }

    // Vérification d'overflow pour largeur * hauteur
    if ((size_t)largeur > SIZE_MAX / (size_t)hauteur) {
        fprintf(stderr, "Erreur : overflow dans largeur * hauteur\n");
        return NULL;
    }

    size_t nb_pixels = (size_t)largeur * (size_t)hauteur;

    // Vérification d'overflow pour nb_pixels * sizeof(int)
    if (nb_pixels > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Erreur : overflow dans le calcul de taille\n");
        return NULL;
    }

    size_t taille = nb_pixels * sizeof(int);

    void *pixels = malloc(taille);
    if (pixels == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return NULL;
    }

    return pixels;
}
```

### Exemple 2 : Calcul de prix

```c
// ❌ VERSION VULNÉRABLE
#include <stdio.h>

typedef struct {
    int prix_unitaire;  // En centimes
    int quantite;
} Article;

int calculer_total(Article articles[], int nb_articles) {
    int total = 0;

    for (int i = 0; i < nb_articles; i++) {
        int sous_total = articles[i].prix_unitaire * articles[i].quantite;
        total += sous_total;  // Overflow possible !
    }

    return total;
}

// Si prix_unitaire = 1000000 (10000€) et quantite = 3000
// sous_total = 3000000000 → overflow sur int 32 bits
// Peut devenir négatif !
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

typedef struct {
    int64_t prix_unitaire;  // En centimes, type plus grand
    int64_t quantite;
} Article;

bool calculer_total_securise(Article articles[], int nb_articles, int64_t *total) {
    if (articles == NULL || total == NULL || nb_articles <= 0) {
        return false;
    }

    *total = 0;

    for (int i = 0; i < nb_articles; i++) {
        // Validation des valeurs
        if (articles[i].prix_unitaire < 0 || articles[i].quantite < 0) {
            fprintf(stderr, "Erreur : valeurs négatives\n");
            return false;
        }

        // Vérification overflow pour la multiplication
        if (articles[i].quantite > 0 &&
            articles[i].prix_unitaire > INT64_MAX / articles[i].quantite) {
            fprintf(stderr, "Erreur : overflow dans le calcul du sous-total\n");
            return false;
        }

        int64_t sous_total = articles[i].prix_unitaire * articles[i].quantite;

        // Vérification overflow pour l'addition
        if (sous_total > INT64_MAX - *total) {
            fprintf(stderr, "Erreur : overflow dans le total\n");
            return false;
        }

        *total += sous_total;
    }

    return true;
}

// Utilisation
int main(void) {
    Article articles[] = {
        {100000, 3000},  // 10000€ × 3000
        {50000, 1000}    // 5000€ × 1000
    };

    int64_t total;
    if (calculer_total_securise(articles, 2, &total)) {
        printf("Total : %lld centimes (%.2f €)\n",
               (long long)total, total / 100.0);
    } else {
        fprintf(stderr, "Erreur dans le calcul\n");
    }

    return 0;
}
```

### Exemple 3 : Boucle avec unsigned

```c
// ❌ BOUCLE INFINIE
void afficher_tableau(unsigned int *tableau, unsigned int debut, unsigned int fin) {
    for (unsigned int i = debut; i <= fin; i++) {
        printf("%u ", tableau[i]);
    }
    printf("\n");
}

// Si fin = UINT_MAX, la condition i <= fin est toujours vraie !
// Même après i++, l'overflow fait que i revient à 0
```

```c
// ✅ VERSION SÉCURISÉE - Option 1 : Compteur inversé
void afficher_tableau_securise_v1(unsigned int *tableau, unsigned int debut, unsigned int fin) {
    if (debut > fin) {
        fprintf(stderr, "Erreur : debut > fin\n");
        return;
    }

    // Boucle descendante
    unsigned int nb_elements = fin - debut + 1;
    for (unsigned int i = 0; i < nb_elements; i++) {
        printf("%u ", tableau[debut + i]);
    }
    printf("\n");
}

// ✅ VERSION SÉCURISÉE - Option 2 : Condition différente
void afficher_tableau_securise_v2(unsigned int *tableau, unsigned int debut, unsigned int fin) {
    if (debut > fin) {
        fprintf(stderr, "Erreur : debut > fin\n");
        return;
    }

    for (unsigned int i = debut; i < fin; i++) {
        printf("%u ", tableau[i]);
    }
    // Dernier élément traité séparément si nécessaire
    if (debut <= fin) {
        printf("%u ", tableau[fin]);
    }
    printf("\n");
}

// ✅ VERSION SÉCURISÉE - Option 3 : Test avant incrémentation
void afficher_tableau_securise_v3(unsigned int *tableau, unsigned int debut, unsigned int fin) {
    if (debut > fin) {
        fprintf(stderr, "Erreur : debut > fin\n");
        return;
    }

    unsigned int i = debut;
    while (true) {
        printf("%u ", tableau[i]);

        if (i == fin) break;  // Test avant l'incrémentation

        i++;
    }
    printf("\n");
}
```

### Exemple 4 : Conversion de types

```c
// ❌ CONVERSION DANGEREUSE
#include <stdio.h>

void traiter_donnees(long long taille_fichier) {
    int buffer_size = (int)taille_fichier;  // Overflow si taille > INT_MAX

    char *buffer = malloc(buffer_size);
    // Si taille_fichier = 3000000000, buffer_size peut devenir négatif
    // malloc(-xxx) retourne NULL, mais on ne le vérifie pas toujours...
}
```

```c
// ✅ CONVERSION SÉCURISÉE
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

bool traiter_donnees_securise(int64_t taille_fichier) {
    if (taille_fichier <= 0) {
        fprintf(stderr, "Erreur : taille invalide\n");
        return false;
    }

    // Vérification avant conversion
    if (taille_fichier > INT_MAX) {
        fprintf(stderr, "Erreur : taille trop grande pour int\n");
        return false;
    }

    // Limite raisonnable (par exemple, 100 Mo)
    const int64_t MAX_SIZE = 100 * 1024 * 1024;
    if (taille_fichier > MAX_SIZE) {
        fprintf(stderr, "Erreur : fichier trop gros\n");
        return false;
    }

    int buffer_size = (int)taille_fichier;  // Maintenant sûr

    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return false;
    }

    // Traitement...

    free(buffer);
    return true;
}
```

---

## Détection des integer overflows

### 1. Sanitizers : UndefinedBehaviorSanitizer (UBSan)

UBSan détecte les overflows sur entiers signés au runtime.

```bash
# Compilation avec UBSan
gcc -fsanitize=undefined -g test.c -o test

# Exécution
./test
```

**Exemple de sortie** :
```
test.c:10:15: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
```

### 2. Sanitizers : IntegerSanitizer

Pour une détection plus complète (signés ET non signés) :

```bash
gcc -fsanitize=integer -g test.c -o test
```

### 3. Flags de compilation

```bash
# Activer tous les warnings pertinents
gcc -Wall -Wextra -Wconversion -Wsign-conversion \
    -Wstrict-overflow=5 -Wno-unused-parameter \
    test.c -o test
```

### 4. Analyse statique

#### Clang Static Analyzer

```bash
clang --analyze -Xanalyzer -analyzer-checker=core,security test.c
```

#### cppcheck

```bash
cppcheck --enable=all test.c
```

### 5. Outils spécialisés

#### Frama-C (analyse formelle)

```bash
frama-c -val test.c
```

#### KLEE (exécution symbolique)

Teste automatiquement tous les chemins possibles pour trouver des overflows.

---

## Pattern : Bibliothèque d'arithmétique sécurisée

Créez une bibliothèque réutilisable :

```c
// safe_math.h
#ifndef SAFE_MATH_H
#define SAFE_MATH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Addition sécurisée
bool safe_add_size(size_t a, size_t b, size_t *result);
bool safe_add_int(int a, int b, int *result);

// Multiplication sécurisée
bool safe_mul_size(size_t a, size_t b, size_t *result);
bool safe_mul_int(int a, int b, int *result);

// Soustraction sécurisée
bool safe_sub_size(size_t a, size_t b, size_t *result);
bool safe_sub_int(int a, int b, int *result);

#endif
```

```c
// safe_math.c
#include "safe_math.h"
#include <limits.h>
#include <stdio.h>

bool safe_add_size(size_t a, size_t b, size_t *result) {
    if (a > SIZE_MAX - b) {
        return false;
    }
    *result = a + b;
    return true;
}

bool safe_mul_size(size_t a, size_t b, size_t *result) {
    if (a != 0 && b > SIZE_MAX / a) {
        return false;
    }
    *result = a * b;
    return true;
}

bool safe_add_int(int a, int b, int *result) {
    // Utiliser les builtins GCC si disponibles
    #ifdef __GNUC__
        return !__builtin_add_overflow(a, b, result);
    #else
        if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
            return false;
        }
        *result = a + b;
        return true;
    #endif
}

bool safe_mul_int(int a, int b, int *result) {
    #ifdef __GNUC__
        return !__builtin_mul_overflow(a, b, result);
    #else
        // Implémentation manuelle (simplifié)
        if (a > 0 && b > 0 && a > INT_MAX / b) return false;
        if (a > 0 && b < 0 && b < INT_MIN / a) return false;
        if (a < 0 && b > 0 && a < INT_MIN / b) return false;
        if (a < 0 && b < 0 && a < INT_MAX / b) return false;

        *result = a * b;
        return true;
    #endif
}

// Implémentations des autres fonctions...
```

**Utilisation** :

```c
#include "safe_math.h"

int main(void) {
    size_t a = 1000000;
    size_t b = 5000000;
    size_t resultat;

    if (safe_mul_size(a, b, &resultat)) {
        printf("Résultat : %zu\n", resultat);
    } else {
        fprintf(stderr, "Overflow détecté\n");
    }

    return 0;
}
```

---

## Cas particuliers

### 1. Division par zéro

Bien que ce ne soit pas strictement un overflow, c'est un problème connexe :

```c
// ❌ Dangereux
int resultat = dividende / diviseur;  // Crash si diviseur == 0

// ✅ Sécurisé
bool division_securisee(int dividende, int diviseur, int *resultat) {
    if (diviseur == 0) {
        fprintf(stderr, "Erreur : division par zéro\n");
        return false;
    }

    // Cas spécial : INT_MIN / -1 = overflow
    if (dividende == INT_MIN && diviseur == -1) {
        fprintf(stderr, "Erreur : overflow dans la division\n");
        return false;
    }

    *resultat = dividende / diviseur;
    return true;
}
```

### 2. Opérations de décalage (shift)

```c
// ❌ Comportement indéfini
int a = 1;
int b = a << 32;  // UB si int est 32 bits (décalage >= largeur du type)

int c = -1;
int d = c << 5;   // UB (décalage d'un nombre négatif)

// ✅ Sécurisé
bool shift_left_secure(unsigned int valeur, unsigned int decalage, unsigned int *result) {
    // Vérifier que le décalage n'est pas trop grand
    if (decalage >= sizeof(unsigned int) * 8) {
        return false;
    }

    // Vérifier l'overflow
    if (valeur > UINT_MAX >> decalage) {
        return false;
    }

    *result = valeur << decalage;
    return true;
}
```

### 3. Modulo négatif

```c
// Comportement défini mais peut surprendre
int a = -10;
int b = 3;
int c = a % b;  // c = -1 (pas 2 !)

// Pour un modulo toujours positif :
int modulo_positif(int a, int b) {
    if (b <= 0) return -1;  // Erreur

    int r = a % b;
    return (r < 0) ? r + b : r;
}
```

---

## Checklist de prévention

### ✅ Design et Conception

- [ ] Utiliser les types appropriés (`size_t` pour tailles, `int64_t` si nécessaire)
- [ ] Limiter les valeurs d'entrée à des plages raisonnables
- [ ] Documenter les limites acceptables pour chaque fonction

### ✅ Implémentation

- [ ] Vérifier les overflows avant les opérations critiques
- [ ] Utiliser les builtins GCC (`__builtin_*_overflow`) quand possible
- [ ] Utiliser une bibliothèque d'arithmétique sécurisée
- [ ] Éviter les conversions de types non vérifiées

### ✅ Compilation

- [ ] Activer UBSan (`-fsanitize=undefined`)
- [ ] Activer IntegerSanitizer (`-fsanitize=integer`)
- [ ] Activer les warnings (`-Wconversion`, `-Wsign-conversion`)

### ✅ Tests

- [ ] Tester avec les valeurs limites (0, MAX, MIN)
- [ ] Tester avec des valeurs juste au-dessus des limites
- [ ] Tester les cas négatifs et positifs
- [ ] Utiliser le fuzzing pour découvrir des cas non prévus

---

## Résumé : Les règles d'or

### 🎯 Principes fondamentaux

1. **Les entiers ont des limites** : Connaître les limites de chaque type
2. **Vérifier avant de calculer** : Toujours valider avant les opérations critiques
3. **Utiliser les bons types** : `size_t` pour tailles, `int64_t` pour grands nombres
4. **Overflow signé = UB** : Comportement indéfini, imprévisible, dangereux
5. **Unsigned wraparound = défini** : Mais peut quand même causer des bugs

### 📋 Opérations à risque

**Haute priorité** (critiques pour la sécurité) :
- Calculs de taille pour `malloc`/`calloc`
- Indices de tableaux
- Tailles de copies mémoire
- Calculs financiers

**Moyenne priorité** :
- Boucles avec compteurs
- Conversions de types
- Calculs arithmétiques généraux

### 💡 Solutions rapides

```c
// Pour les calculs de taille malloc :
if (nb_elements > SIZE_MAX / sizeof(element)) {
    // Overflow, refuser
}

// Pour les additions :
if (a > TYPE_MAX - b) {
    // Overflow, refuser
}

// Pour les multiplications :
if (a != 0 && b > TYPE_MAX / a) {
    // Overflow, refuser
}

// Ou utiliser les builtins GCC :
if (__builtin_mul_overflow(a, b, &result)) {
    // Overflow détecté
}
```

### 🔧 Outils essentiels

1. **UBSan** : Détection au runtime (développement)
2. **Analyse statique** : Détection avant exécution
3. **Bibliothèques sécurisées** : Code réutilisable et testé
4. **Tests limites** : Valider avec MIN, MAX, 0

### 📊 Hiérarchie de sécurité

```
Niveau 1 (Minimal) :
└─ Limiter les entrées utilisateur
└─ Utiliser size_t pour les tailles

Niveau 2 (Recommandé) :
└─ Vérifier avant malloc (taille > SIZE_MAX / sizeof())
└─ Activer UBSan en développement
└─ Warnings de conversion activés

Niveau 3 (Paranoïaque) :
└─ Bibliothèque d'arithmétique sécurisée
└─ Analyse statique systématique
└─ Fuzzing régulier
└─ Vérifications sur toutes les opérations arithmétiques critiques
```

---

## Comparaison avec d'autres vulnérabilités

| Vulnérabilité | Détection | Exploitation | Prévention |
|---------------|-----------|--------------|------------|
| **Buffer Overflow** | Facile (crash) | Directe | Vérifier longueurs |
| **Format String** | Moyenne | Complexe | `printf("%s", var)` |
| **Integer Overflow** | **Difficile** (silencieux) | Indirecte | Vérifier calculs |

**Particularité des integer overflows** : Ils sont souvent **silencieux** et ne causent pas de crash immédiat, ce qui les rend plus difficiles à détecter mais tout aussi dangereux.

---

**🔐 Conclusion** : Les integer overflows sont subtils mais peuvent avoir des conséquences graves. La clé est de :
1. **Anticiper** les overflows dans les calculs critiques (malloc, indices)
2. **Vérifier** avant les opérations à risque
3. **Utiliser** les bons outils (UBSan, analyse statique)
4. **Tester** avec des valeurs limites

Contrairement aux buffer overflows qui crashent souvent bruyamment, les integer overflows peuvent passer inaperçus pendant longtemps. Soyez vigilants !

**Prochaine étape** : La section suivante sur le "Principe du moindre privilège" vous montrera comment limiter l'impact des vulnérabilités même quand elles existent.

⏭️ [Principe du moindre privilège](/26-securite-code-defensif/06-moindre-privilege.md)
