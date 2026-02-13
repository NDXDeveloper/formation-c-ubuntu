🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.2 Alignement mémoire et padding

## Introduction

Vous avez probablement remarqué quelque chose d'étrange en utilisant `sizeof()` sur une structure :

```c
struct Exemple {
    char c;    // 1 octet
    int i;     // 4 octets
};

printf("Taille : %zu octets\n", sizeof(struct Exemple));
// Attendu : 1 + 4 = 5 octets
// Réalité : 8 octets !! Pourquoi ?
```

Cette "taille mystérieuse" s'explique par un concept fondamental : **l'alignement mémoire** et le **padding**.

---

## Pourquoi l'alignement existe-t-il ?

### Fonctionnement du processeur

Les processeurs modernes lisent la mémoire par **blocs** (appelés "mots mémoire"), généralement de 4 ou 8 octets à la fois, selon l'architecture :

- **Architecture 32 bits** : lit 4 octets à la fois
- **Architecture 64 bits** : lit 8 octets à la fois

### Le problème sans alignement

Imaginons qu'un `int` (4 octets) soit stocké de manière désalignée :

```
Adresse:   0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
Mémoire:  | c  | i0 | i1 | i2 | i3 |    |    |    |
          +----+----+----+----+----+----+----+----+
               ^----- int (4 octets) -----^
```

Pour lire l'`int` qui commence à l'adresse 1, le processeur doit :
1. Lire le premier mot (adresses 0-3) → obtenir `i0`, `i1`, `i2`
2. Lire le deuxième mot (adresses 4-7) → obtenir `i3`
3. Reconstruire l'`int` en assemblant les morceaux

**Résultat** : 2 accès mémoire au lieu d'un seul = **perte de performance**.

### La solution : alignement

Le processeur préfère que chaque type de données soit aligné sur un **multiple de sa taille** :

- `char` (1 octet) : peut être n'importe où
- `short` (2 octets) : adresse multiple de 2
- `int` (4 octets) : adresse multiple de 4
- `double` (8 octets) : adresse multiple de 8
- `pointeur` (8 octets en 64 bits) : adresse multiple de 8

---

## Qu'est-ce que le padding ?

Le **padding** (ou "rembourrage") est l'ajout automatique d'octets vides par le compilateur pour respecter les règles d'alignement.

### Exemple simple

```c
struct Test {
    char c;    // 1 octet
    int i;     // 4 octets
};
```

**Sans padding (ce qui ne se produit PAS)** :
```
Adresse:   0    1    2    3    4
          +----+----+----+----+----+
          | c  | i0 | i1 | i2 | i3 |
          +----+----+----+----+----+
```
Problème : `int` commence à l'adresse 1 (pas un multiple de 4).

**Avec padding (la réalité)** :
```
Adresse:   0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
          | c  | XX | XX | XX | i0 | i1 | i2 | i3 |
          +----+----+----+----+----+----+----+----+
               ^-- 3 octets de padding
```

Le compilateur ajoute **3 octets vides** après `c` pour que `i` commence à l'adresse 4 (multiple de 4).

**Taille totale** : 1 + 3 (padding) + 4 = **8 octets**

---

## Règles d'alignement

### Règle 1 : Alignement des membres

Chaque membre d'une structure est aligné sur un **multiple de sa propre taille** :

```c
struct Alignement {
    char c;      // Alignement : 1 octet (n'importe quelle adresse)
    short s;     // Alignement : 2 octets (adresse paire)
    int i;       // Alignement : 4 octets (adresse multiple de 4)
    double d;    // Alignement : 8 octets (adresse multiple de 8)
};
```

### Règle 2 : Alignement de la structure complète

La structure elle-même doit être alignée sur le **plus grand alignement de ses membres**.

Cela garantit que dans un tableau de structures, tous les membres restent correctement alignés.

```c
struct Test {
    char c;
    int i;
};
// Alignement de la structure : 4 (car int nécessite 4)
// Taille : 8 octets (arrondie au multiple de 4)
```

---

## Exemples détaillés

### Exemple 1 : Ordre des membres important

```c
#include <stdio.h>

struct MauvaisOrdre {
    char c1;    // 1 octet
    int i;      // 4 octets
    char c2;    // 1 octet
};

struct BonOrdre {
    char c1;    // 1 octet
    char c2;    // 1 octet
    int i;      // 4 octets
};

int main() {
    printf("MauvaisOrdre : %zu octets\n", sizeof(struct MauvaisOrdre));
    printf("BonOrdre     : %zu octets\n", sizeof(struct BonOrdre));
    return 0;
}
```

**Sortie** :
```
MauvaisOrdre : 12 octets  
BonOrdre     : 8 octets  
```

**Explication visuelle** :

**MauvaisOrdre** (12 octets) :
```
Adresse:   0    1    2    3    4    5    6    7    8    9   10   11
          +----+----+----+----+----+----+----+----+----+----+----+----+
          | c1 | XX | XX | XX | i0 | i1 | i2 | i3 | c2 | XX | XX | XX |
          +----+----+----+----+----+----+----+----+----+----+----+----+
               ^-- 3 padding        ^-- 4 octets int   ^-- 3 padding final
```

- 3 octets de padding après `c1` (pour aligner `i` sur 4)
- 3 octets de padding après `c2` (pour que la structure totale soit multiple de 4)

**BonOrdre** (8 octets) :
```
Adresse:   0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
          | c1 | c2 | XX | XX | i0 | i1 | i2 | i3 |
          +----+----+----+----+----+----+----+----+
                    ^-- 2 padding
```

- Seulement 2 octets de padding entre les deux `char` et l'`int`
- Gain de 4 octets !

### Exemple 2 : Différents types de données

```c
#include <stdio.h>

struct Complexe {
    char c;       // 1 octet
    double d;     // 8 octets
    short s;      // 2 octets
    int i;        // 4 octets
};

int main() {
    printf("Taille : %zu octets\n", sizeof(struct Complexe));

    struct Complexe obj;
    printf("Adresse de c : %p\n", (void*)&obj.c);
    printf("Adresse de d : %p\n", (void*)&obj.d);
    printf("Adresse de s : %p\n", (void*)&obj.s);
    printf("Adresse de i : %p\n", (void*)&obj.i);

    return 0;
}
```

**Sortie possible** :
```
Taille : 24 octets  
Adresse de c : 0x7ffd...0010  (offset 0)  
Adresse de d : 0x7ffd...0018  (offset 8, décalage de 8 depuis c)  
Adresse de s : 0x7ffd...0020  (offset 16, décalage de 8 depuis d)  
Adresse de i : 0x7ffd...0024  (offset 20, décalage de 4 depuis s)
```

**Représentation mémoire** :
```
Offset:    0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
   0      | c  | XX | XX | XX | XX | XX | XX | XX |  (1 + 7 padding)
          +----+----+----+----+----+----+----+----+
   8      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |  (8 octets)
          +----+----+----+----+----+----+----+----+
  16      | s0 | s1 | XX | XX | i0 | i1 | i2 | i3 |  (2 + 2 padding + 4)
          +----+----+----+----+----+----+----+----+
```

**Taille** : 24 octets (pas de padding final car 24 est déjà multiple de 8)

---

## Optimisation : Réorganiser les membres

### Principe

**Placer les membres du plus grand au plus petit** réduit le padding.

```c
struct ComplexeOptimise {
    double d;     // 8 octets (le plus grand en premier)
    int i;        // 4 octets
    short s;      // 2 octets
    char c;       // 1 octet
};
```

**Représentation mémoire** :
```
Offset:    0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
   0      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |  (8 octets)
          +----+----+----+----+----+----+----+----+
   8      | i0 | i1 | i2 | i3 | s0 | s1 | c  | XX |  (4 + 2 + 1 + 1 padding)
          +----+----+----+----+----+----+----+----+
```

**Taille** : 16 octets (gain de 8 octets par rapport à la version non optimisée !)

### Comparaison avant/après

```c
#include <stdio.h>

struct Avant {
    char c;
    double d;
    short s;
    int i;
};

struct Apres {
    double d;
    int i;
    short s;
    char c;
};

int main() {
    printf("Avant optimisation : %zu octets\n", sizeof(struct Avant));
    printf("Après optimisation : %zu octets\n", sizeof(struct Apres));
    printf("Gain               : %zu octets (%.1f%%)\n",
           sizeof(struct Avant) - sizeof(struct Apres),
           100.0 * (sizeof(struct Avant) - sizeof(struct Apres)) / sizeof(struct Avant));
    return 0;
}
```

**Sortie** :
```
Avant optimisation : 24 octets  
Après optimisation : 16 octets  
Gain               : 8 octets (33.3%)
```

---

## Visualiser le padding avec offsetof

La macro `offsetof` (définie dans `<stddef.h>`) permet de connaître la position d'un membre dans une structure.

```c
#include <stdio.h>
#include <stddef.h>

struct Exemple {
    char c;
    int i;
    char c2;
};

int main() {
    printf("Taille totale : %zu octets\n\n", sizeof(struct Exemple));

    printf("Offset de c  : %zu\n", offsetof(struct Exemple, c));
    printf("Offset de i  : %zu\n", offsetof(struct Exemple, i));
    printf("Offset de c2 : %zu\n", offsetof(struct Exemple, c2));

    printf("\nPadding après c  : %zu octets\n",
           offsetof(struct Exemple, i) - offsetof(struct Exemple, c) - sizeof(char));
    printf("Padding après c2 : %zu octets\n",
           sizeof(struct Exemple) - offsetof(struct Exemple, c2) - sizeof(char));

    return 0;
}
```

**Sortie** :
```
Taille totale : 12 octets

Offset de c  : 0  
Offset de i  : 4  
Offset de c2 : 8  

Padding après c  : 3 octets  
Padding après c2 : 3 octets  
```

---

## Cas particulier : structures vides et tableaux

### Padding pour les tableaux

Dans un tableau de structures, chaque élément doit être correctement aligné :

```c
struct Test {
    char c;
    int i;
};  // Taille : 8 octets

struct Test tableau[3];
```

**Mémoire** :
```
Element [0]:  | c | XX XX XX | i i i i |  
Element [1]:  | c | XX XX XX | i i i i |  
Element [2]:  | c | XX XX XX | i i i i |  
```

Si la structure faisait 5 octets (sans padding final), l'élément [1] commencerait à l'adresse 5, et son `int` serait désaligné.

Le padding final garantit que `sizeof(struct Test)` est un multiple de l'alignement requis.

### Structure avec un seul char

```c
struct UnSeulChar {
    char c;
};

printf("Taille : %zu octet\n", sizeof(struct UnSeulChar));  // 1 octet (pas de padding nécessaire)
```

---

## Architectures et portabilité

### Différences selon l'architecture

L'alignement peut varier selon le processeur et le compilateur :

| Architecture | int | long | pointeur | double |
|--------------|-----|------|----------|--------|
| x86 32-bit   | 4   | 4    | 4        | 8      |
| x86_64       | 4   | 8    | 8        | 8      |
| ARM 32-bit   | 4   | 4    | 4        | 8      |
| ARM 64-bit   | 4   | 8    | 8        | 8      |

### Vérifier l'alignement sur votre système

```c
#include <stdio.h>
#include <stdalign.h>  // C11

int main() {
    printf("Alignement de char   : %zu\n", alignof(char));
    printf("Alignement de short  : %zu\n", alignof(short));
    printf("Alignement de int    : %zu\n", alignof(int));
    printf("Alignement de long   : %zu\n", alignof(long));
    printf("Alignement de double : %zu\n", alignof(double));
    printf("Alignement de void*  : %zu\n", alignof(void*));

    return 0;
}
```

**Sortie typique (x86_64)** :
```
Alignement de char   : 1  
Alignement de short  : 2  
Alignement de int    : 4  
Alignement de long   : 8  
Alignement de double : 8  
Alignement de void*  : 8  
```

---

## Quand optimiser ?

### Optimiser si :

1. **Structures utilisées massivement** : tableaux de milliers d'éléments
2. **Contraintes mémoire** : systèmes embarqués
3. **Performance critique** : accès cache, bande passante mémoire

**Exemple** : Un tableau de 1 million de structures qui passe de 32 à 16 octets économise **16 Mo de RAM**.

### Ne pas optimiser si :

1. **Structures peu utilisées** : quelques instances
2. **Lisibilité prioritaire** : ordre logique des champs plus important
3. **API publique** : ne pas casser la compatibilité

### Règle d'or

> "Optimisez seulement si vous avez mesuré et identifié un problème réel."

---

## Bonnes pratiques

### 1. Ordre recommandé par défaut

```c
struct BienOrganisee {
    // 1. Pointeurs et types 8 octets
    void *ptr;
    double d;

    // 2. Types 4 octets
    int i;
    float f;

    // 3. Types 2 octets
    short s;

    // 4. Types 1 octet
    char c1;
    char c2;
};
```

### 2. Grouper les petits types

```c
// ❌ Mauvais : beaucoup de padding
struct Mauvais {
    char c1;
    int i1;
    char c2;
    int i2;
    char c3;
};

// ✅ Bon : moins de padding
struct Bon {
    char c1;
    char c2;
    char c3;
    int i1;
    int i2;
};
```

### 3. Documenter les choix d'optimisation

```c
/*
 * Structure optimisée pour minimiser le padding.
 * Taille : 16 octets (au lieu de 32 sans optimisation).
 * NE PAS réorganiser sans recalculer les impacts.
 */
struct Optimisee {
    double d;
    int i;
    short s;
    char c;
};
```

### 4. Utiliser des outils de vérification

Le compilateur GCC peut afficher des warnings sur le padding :

```bash
gcc -Wpadded -c fichier.c
```

**Sortie** :
```
warning: padding struct to align 'i' [-Wpadded]  
warning: padding struct size to alignment boundary [-Wpadded]  
```

---

## Calculer le padding manuellement

### Méthode étape par étape

Prenons cette structure :

```c
struct Calcul {
    char c;      // 1 octet, alignement 1
    double d;    // 8 octets, alignement 8
    int i;       // 4 octets, alignement 4
};
```

**Étape 1** : Placer `char c` à l'offset 0
```
Offset 0: [c]
```

**Étape 2** : Placer `double d`
- `d` nécessite un alignement sur 8
- Offset actuel : 1
- Prochain multiple de 8 : 8
- **Padding nécessaire** : 8 - 1 = 7 octets

```
Offset 0: [c] [XX XX XX XX XX XX XX]  
Offset 8: [d d d d d d d d]  
```

**Étape 3** : Placer `int i`
- `i` nécessite un alignement sur 4
- Offset actuel : 16 (déjà multiple de 4)
- **Padding nécessaire** : 0

```
Offset 0:  [c] [XX XX XX XX XX XX XX]  
Offset 8:  [d d d d d d d d]  
Offset 16: [i i i i]  
```

**Étape 4** : Padding final
- Taille actuelle : 20 octets
- Alignement de la structure : 8 (le plus grand)
- Prochain multiple de 8 : 24
- **Padding final** : 24 - 20 = 4 octets

```
Offset 0:  [c] [XX XX XX XX XX XX XX]  
Offset 8:  [d d d d d d d d]  
Offset 16: [i i i i] [XX XX XX XX]  
```

**Taille finale** : 24 octets

### Vérification

```c
#include <stdio.h>

struct Calcul {
    char c;
    double d;
    int i;
};

int main() {
    printf("Taille calculée : 24 octets\n");
    printf("Taille réelle   : %zu octets\n", sizeof(struct Calcul));
    return 0;
}
```

---

## Impact sur les performances

### Exemple de benchmark

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000000

struct NonOptimise {
    char c1;
    int i1;
    char c2;
    int i2;
};

struct Optimise {
    char c1;
    char c2;
    int i1;
    int i2;
};

int main() {
    struct NonOptimise *tab1 = malloc(N * sizeof(struct NonOptimise));
    struct Optimise *tab2 = malloc(N * sizeof(struct Optimise));

    printf("NonOptimise : %zu octets × %d = %.2f Mo\n",
           sizeof(struct NonOptimise), N,
           (sizeof(struct NonOptimise) * N) / (1024.0 * 1024.0));

    printf("Optimise    : %zu octets × %d = %.2f Mo\n",
           sizeof(struct Optimise), N,
           (sizeof(struct Optimise) * N) / (1024.0 * 1024.0));

    free(tab1);
    free(tab2);

    return 0;
}
```

**Sortie** :
```
NonOptimise : 16 octets × 10000000 = 152.59 Mo  
Optimise    : 12 octets × 10000000 = 114.44 Mo  
```

**Gain** : 38 Mo économisés (25% de réduction) !

---

## Résumé

### Points clés

1. **L'alignement** garantit des accès mémoire efficaces
2. **Le padding** est ajouté automatiquement par le compilateur
3. **L'ordre des membres** influence la taille totale de la structure
4. **Règle d'optimisation** : placer les types du plus grand au plus petit
5. **Utiliser `sizeof()` et `offsetof()`** pour vérifier la disposition mémoire

### Formule de calcul rapide

Pour une structure, la taille est :
```
Taille = (somme des tailles des membres) + padding interne + padding final
```

Où :
- **Padding interne** : ajusté pour respecter l'alignement de chaque membre
- **Padding final** : pour que la taille totale soit un multiple de l'alignement le plus grand

### Commandes utiles

```c
sizeof(struct T)           // Taille totale  
offsetof(struct T, membre) // Position d'un membre  
alignof(type)              // Alignement requis (C11)  
```

---

## À retenir

```c
// ❌ Mauvais : beaucoup de padding
struct Mauvais {
    char c;    // 1 + 3 padding
    int i;     // 4
    char c2;   // 1 + 3 padding
};  // Total : 12 octets

// ✅ Bon : padding minimal
struct Bon {
    int i;     // 4
    char c;    // 1
    char c2;   // 1 + 2 padding
};  // Total : 8 octets
```

**Prochaine étape** : Nous allons découvrir les **packed structures** (section 10.3) qui permettent de forcer le compilateur à supprimer le padding (avec des compromis).

⏭️ [Packed structures](/10-types-utilisateur/03-packed-structures.md)
