🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.7 Pointeurs restreints (restrict)

## Introduction

Le mot-clé **`restrict`** est une fonctionnalité introduite en **C99** qui permet d'améliorer les performances du code en aidant le compilateur à mieux optimiser. Bien que moins connu que `const`, `restrict` est très utilisé dans le code haute performance et les bibliothèques système.

## Qu'est-ce que restrict ?

Le mot-clé **`restrict`** est une **promesse** que vous faites au compilateur : vous lui garantissez que, pendant la durée de vie du pointeur, **aucun autre pointeur** n'accédera à la même zone mémoire.

### Analogie

Imaginez deux bibliothécaires (pointeurs) travaillant sur des livres :

**Sans restrict :**
- Les bibliothécaires peuvent travailler sur les **mêmes livres**
- Ils doivent constamment vérifier si l'autre a modifié un livre
- C'est plus lent car ils doivent se coordonner

**Avec restrict :**
- Vous garantissez que chaque bibliothécaire travaille sur des **livres différents**
- Ils peuvent travailler en parallèle sans se soucier de l'autre
- C'est plus rapide

## Syntaxe de base

```c
int *restrict ptr;
```

Ou dans les paramètres de fonction :

```c
void fonction(int *restrict ptr1, int *restrict ptr2);
```

## Le problème de l'aliasing

### Qu'est-ce que l'aliasing ?

L'**aliasing** se produit quand deux pointeurs différents pointent vers la même zone mémoire.

```c
int valeur = 10;
int *ptr1 = &valeur;
int *ptr2 = &valeur;    // ptr1 et ptr2 sont des alias

*ptr1 = 20;    // Modifie valeur via ptr1
// ptr2 pointe vers la même chose, donc *ptr2 vaut maintenant 20
```

### Visualisation

```
Sans aliasing (pointeurs distincts) :
    x = 10        y = 20
    [  10  ]      [  20  ]
      ↑             ↑
      │             │
    ptr1          ptr2

Avec aliasing (même zone mémoire) :
    x = 10
    [  10  ]
      ↑  ↑
      │  │
    ptr1 ptr2  ← Les deux pointent vers x
```

## Pourquoi l'aliasing pose problème ?

Le compilateur doit être **prudent** car il ne sait pas si deux pointeurs sont des alias. Cela l'empêche de faire certaines optimisations.

### Exemple sans restrict

```c
void ajouter(int *a, int *b, int *resultat) {
    *resultat = *a + *b;
}
```

Le compilateur doit considérer que `a`, `b` et `resultat` **pourraient** pointer vers la même zone. Il doit donc :
1. Lire `*a` → stockage temporaire
2. Lire `*b` → stockage temporaire
3. Additionner
4. Écrire dans `*resultat`
5. **Relire** `*a` et `*b` si nécessaire (au cas où `resultat` serait un alias)

### Avec restrict

```c
void ajouter(int *restrict a, int *restrict b, int *restrict resultat) {
    *resultat = *a + *b;
}
```

Avec `restrict`, vous **garantissez** que `a`, `b` et `resultat` pointent vers des zones différentes. Le compilateur peut :
- Mettre `*a` et `*b` dans des registres
- Ne pas relire après l'écriture
- Réorganiser les opérations librement

## Exemple concret d'optimisation

### Code sans restrict

```c
void copier(int *dest, int *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
```

**Problème :** Le compilateur doit considérer que `dest` et `src` pourraient se chevaucher.

```
Si dest et src se chevauchent :
    src  ──→ [1][2][3][4][5]
    dest ──────→ [2][3][4][5][6]
                  ↑
                  Chevauchement !
```

Le compilateur doit lire et écrire élément par élément prudemment.

### Code avec restrict

```c
void copier(int *restrict dest, int *restrict src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
```

**Garantie :** `dest` et `src` ne se chevauchent **jamais**.

Le compilateur peut :
- Utiliser des instructions SIMD (vectorisation)
- Charger plusieurs éléments à la fois
- Réorganiser les lectures/écritures
- Utiliser des registres plus efficacement

## Exemple de performance

### Fonction de copie de mémoire

```c
#include <stddef.h>

// Sans restrict
void memcpy_simple(void *dest, void *src, size_t n) {
    char *d = dest;
    char *s = src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

// Avec restrict
void memcpy_optimise(void *restrict dest, void *restrict src, size_t n) {
    char *restrict d = dest;
    char *restrict s = src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}
```

**Différence de performance :** Avec `restrict`, le compilateur peut générer du code **2 à 3 fois plus rapide** sur certaines architectures grâce à la vectorisation.

### Fonction memcpy de la bibliothèque standard

```c
// Signature réelle dans <string.h>
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
```

**Note :** La vraie `memcpy` utilise `restrict` ! C'est pourquoi elle est si rapide.

## Règles d'utilisation de restrict

### Règle 1 : Pas de chevauchement

Les pointeurs `restrict` ne doivent **jamais** pointer vers des zones qui se chevauchent.

```c
void fonction(int *restrict a, int *restrict b) {
    // Vous garantissez que a et b pointent vers des zones différentes
    *a = 10;
    *b = 20;    // N'affecte pas *a
}
```

### Règle 2 : Valide pendant la portée

La promesse est valable **pendant toute la portée** du pointeur.

```c
void fonction(int *restrict ptr) {
    // Pendant toute l'exécution de fonction(),
    // aucun autre pointeur n'accède à la zone pointée par ptr
}
```

### Règle 3 : S'applique au pointeur, pas à la zone

```c
void traiter(int *restrict ptr, int taille) {
    // ptr est restrict, mais on peut créer d'autres pointeurs
    // TANT QU'ils ne pointent pas vers la même zone

    int *autre = ptr;    // ⚠️ Attention : pas vraiment restrict ici

    // Mieux :
    for (int i = 0; i < taille; i++) {
        ptr[i] = i;    // OK : accès via ptr
    }
}
```

## Que se passe-t-il si on viole restrict ?

### Comportement indéfini

Si vous violez la promesse `restrict`, vous obtenez un **comportement indéfini**.

```c
void mauvais_exemple(int *restrict a, int *restrict b) {
    *a = 10;
    *b = 20;
}

int main(void) {
    int x = 5;

    // ⚠️ VIOLATION : a et b pointent vers la même zone !
    mauvais_exemple(&x, &x);

    // Comportement indéfini : peut afficher 10, 20, ou autre chose !
    printf("%d\n", x);

    return 0;
}
```

**Résultat possible :**
- Le compilateur a optimisé en supposant que `a` et `b` sont différents
- Les opérations peuvent être réordonnées
- `x` peut avoir n'importe quelle valeur

### Le compilateur vous fait confiance

```
restrict est une PROMESSE, pas une VÉRIFICATION
                ↓
Le compilateur ne vérifie PAS que vous respectez la promesse
                ↓
C'est VOTRE responsabilité
```

## Cas d'usage typiques

### 1. Fonctions de copie mémoire

```c
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memmove(void *dest, const void *src, size_t n);    // PAS restrict !
```

**Différence :**
- `memcpy` : **suppose** que `dest` et `src` ne se chevauchent pas (restrict)
- `memmove` : **gère** le chevauchement (pas restrict, plus lent)

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[] = "Hello World";

    // memcpy : NE PAS utiliser si chevauchement
    // memcpy(buffer + 3, buffer, 5);    // ⚠️ Comportement indéfini !

    // memmove : gère le chevauchement
    memmove(buffer + 3, buffer, 5);    // ✓ OK
    printf("%s\n", buffer);

    return 0;
}
```

### 2. Opérations mathématiques sur tableaux

```c
void ajouter_tableaux(double *restrict resultat,
                      const double *restrict a,
                      const double *restrict b,
                      size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        resultat[i] = a[i] + b[i];
    }
}
```

**Garantie :** `resultat`, `a` et `b` pointent vers des tableaux distincts.

### 3. Traitement d'images

```c
void appliquer_filtre(unsigned char *restrict sortie,
                      const unsigned char *restrict entree,
                      int largeur, int hauteur) {
    // Traitement pixel par pixel
    for (int i = 0; i < largeur * hauteur; i++) {
        sortie[i] = entree[i] * 2;    // Exemple simplifié
    }
}
```

**Bénéfice :** Le compilateur peut vectoriser le traitement.

### 4. Fonctions de la bibliothèque standard

Plusieurs fonctions standard utilisent `restrict` :

```c
char *strcpy(char *restrict dest, const char *restrict src);
char *strcat(char *restrict dest, const char *restrict src);
int sprintf(char *restrict str, const char *restrict format, ...);
```

## Restrict avec const

On peut combiner `restrict` et `const` :

```c
void fonction(const int *restrict ptr) {
    // ptr est restrict : accès unique
    // const : ne peut pas modifier la valeur
    printf("%d\n", *ptr);    // ✓ Lecture OK
    // *ptr = 10;    // ✗ ERREUR : const
}
```

### Ordre des qualificateurs

```c
// Les trois sont valides et équivalents :
const int *restrict ptr;
int const *restrict ptr;
restrict const int *ptr;    // Moins courant

// Pointeur restrict constant vers int constant
const int *restrict const ptr;
```

## Exemple complet : produit scalaire

### Version sans restrict

```c
#include <stdio.h>

double produit_scalaire(double *a, double *b, size_t n) {
    double somme = 0.0;
    for (size_t i = 0; i < n; i++) {
        somme += a[i] * b[i];
    }
    return somme;
}

int main(void) {
    double v1[] = {1.0, 2.0, 3.0, 4.0};
    double v2[] = {5.0, 6.0, 7.0, 8.0};

    double resultat = produit_scalaire(v1, v2, 4);
    printf("Produit scalaire : %.1f\n", resultat);

    return 0;
}
```

### Version avec restrict (optimisée)

```c
#include <stdio.h>

double produit_scalaire(const double *restrict a,
                        const double *restrict b,
                        size_t n) {
    double somme = 0.0;
    for (size_t i = 0; i < n; i++) {
        somme += a[i] * b[i];
    }
    return somme;
}

int main(void) {
    double v1[] = {1.0, 2.0, 3.0, 4.0};
    double v2[] = {5.0, 6.0, 7.0, 8.0};

    double resultat = produit_scalaire(v1, v2, 4);
    printf("Produit scalaire : %.1f\n", resultat);

    return 0;
}
```

**Sortie :**
```
Produit scalaire : 70.0
```

**Amélioration :** Le compilateur peut :
- Garder `somme` dans un registre
- Dérouler la boucle (loop unrolling)
- Utiliser les instructions FMA (Fused Multiply-Add)

## Restrict et structures

### Pointeur restrict vers une structure

```c
typedef struct {
    int x;
    int y;
} Point;

void deplacer(Point *restrict p, int dx, int dy) {
    p->x += dx;
    p->y += dy;
}

int main(void) {
    Point pt = {10, 20};
    deplacer(&pt, 5, 3);
    printf("Point : (%d, %d)\n", pt.x, pt.y);
    return 0;
}
```

### Membres restrict dans une structure

```c
typedef struct {
    int *restrict data;    // data est un pointeur restrict
    size_t taille;
} Tableau;

void traiter(Tableau *t) {
    // t->data garantit un accès unique
    for (size_t i = 0; i < t->taille; i++) {
        t->data[i] *= 2;
    }
}
```

## Restrict dans les déclarations complexes

### Tableau de pointeurs restrict

```c
int *restrict tableau_ptrs[10];    // Tableau de 10 pointeurs restrict
```

Chaque pointeur dans le tableau est `restrict`.

### Pointeur restrict vers un tableau

```c
int (*restrict ptr_tableau)[10];    // Pointeur restrict vers tableau de 10 int
```

### Fonction retournant un pointeur restrict

```c
int *restrict allouer_buffer(size_t taille) {
    return malloc(taille * sizeof(int));
}
```

**Note :** Le pointeur retourné garantit un accès unique.

## Comparaison : const vs restrict

| Aspect | `const` | `restrict` |
|--------|---------|------------|
| **But** | Empêcher la modification | Optimiser les performances |
| **Vérifié par** | Compilateur (erreur si violation) | Programmeur (comportement indéfini si violation) |
| **S'applique à** | La valeur ou le pointeur | L'aliasing |
| **Depuis** | ANSI C (C89) | C99 |
| **Usage** | Documentation + sécurité | Performance |

### Utilisation combinée

```c
void traiter(const int *restrict input, int *restrict output, size_t n) {
    // input : ne peut pas être modifié (const) + accès unique (restrict)
    // output : peut être modifié + accès unique (restrict)
    for (size_t i = 0; i < n; i++) {
        output[i] = input[i] * 2;
    }
}
```

## Support et compatibilité

### C99 et versions ultérieures

`restrict` est standard depuis **C99**.

```c
#if __STDC_VERSION__ >= 199901L
    // restrict est disponible
    void fonction(int *restrict ptr);
#endif
```

### C++ et restrict

**Important :** `restrict` n'existe **pas** en C++ standard.

Cependant, certains compilateurs offrent des extensions :
- GCC/Clang : `__restrict__` ou `__restrict`
- MSVC : `__restrict`

```c
// Pour compatibilité C/C++
#ifdef __cplusplus
    #define RESTRICT __restrict
#else
    #define RESTRICT restrict
#endif

void fonction(int *RESTRICT ptr);
```

### Vérification du support

```c
#include <stdio.h>

int main(void) {
#ifdef __STDC_NO_RESTRICT__
    printf("restrict n'est pas supporté\n");
#else
    printf("restrict est supporté\n");
#endif
    return 0;
}
```

## Quand utiliser restrict ?

### ✅ Utilisez restrict quand :

1. **Performance critique** : Boucles intensives, traitement de données
2. **Garantie naturelle** : Vous savez que les pointeurs sont distincts
3. **API publique** : Bibliothèques, fonctions réutilisables
4. **Traitement de tableaux** : Opérations mathématiques, filtres

### ❌ N'utilisez PAS restrict si :

1. **Incertitude** : Vous n'êtes pas sûr du non-chevauchement
2. **Chevauchement possible** : Les pointeurs peuvent se chevaucher
3. **Code simple** : L'optimisation n'est pas nécessaire
4. **Compatibilité C++** : Code partagé entre C et C++

## Erreurs courantes

### ❌ Erreur 1 : Chevauchement accidentel

```c
void mauvais(int *restrict a, int *restrict b) {
    *a = *b + 10;
}

int main(void) {
    int x = 5;
    mauvais(&x, &x);    // ⚠️ VIOLATION : a et b pointent vers x
    return 0;
}
```

### ❌ Erreur 2 : Oublier restrict dans les fonctions internes

```c
void interne(int *ptr, size_t n) {    // Pas restrict
    for (size_t i = 0; i < n; i++) {
        ptr[i] = 0;
    }
}

void externe(int *restrict ptr, size_t n) {
    interne(ptr, n);    // Perd l'information restrict
}
```

**Solution :**
```c
void interne(int *restrict ptr, size_t n) {    // Ajouter restrict
    for (size_t i = 0; i < n; i++) {
        ptr[i] = 0;
    }
}
```

### ❌ Erreur 3 : Utiliser restrict sans raison

```c
// Inutile : fonction simple, pas de boucle
int obtenir_valeur(const int *restrict ptr) {
    return *ptr;
}
```

`restrict` n'apporte rien ici car il n'y a pas d'optimisation possible.

## Exemple avancé : multiplication de matrices

```c
#include <stdio.h>

void multiplier_matrices(double *restrict C,
                         const double *restrict A,
                         const double *restrict B,
                         int n) {
    // C = A × B (matrices n×n)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double somme = 0.0;
            for (int k = 0; k < n; k++) {
                somme += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = somme;
        }
    }
}

int main(void) {
    double A[4] = {1, 2, 3, 4};         // Matrice 2×2
    double B[4] = {5, 6, 7, 8};
    double C[4] = {0};

    multiplier_matrices(C, A, B, 2);

    printf("Résultat C :\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%.0f ", C[i * 2 + j]);
        }
        printf("\n");
    }

    return 0;
}
```

**Sortie :**
```
Résultat C :
19 22
43 50
```

**Bénéfice de restrict :**
- Le compilateur peut optimiser les boucles internes
- Meilleure utilisation du cache
- Vectorisation possible

## Outils pour vérifier

### GCC avec warnings

```bash
gcc -O2 -Wall -Wextra -Wstrict-aliasing=3 programme.c
```

Le flag `-Wstrict-aliasing` peut détecter certains problèmes d'aliasing.

### Analyse statique

```bash
# cppcheck
cppcheck --enable=all programme.c

# clang-tidy
clang-tidy programme.c -- -std=c99
```

### Tests avec sanitizers

```bash
gcc -fsanitize=undefined programme.c -o programme
./programme
```

## Résumé visuel

```
Sans restrict :
    ptr1 ──→ [DATA]
    ptr2 ──→ [DATA]  ← Peut-être la même zone ?
                        Compilateur prudent
                        Moins d'optimisations

Avec restrict :
    ptr1 ──→ [DATA_1]
    ptr2 ──→ [DATA_2]  ← Garantie : zones différentes
                          Compilateur confiant
                          Optimisations agressives
```

## Ce qu'il faut retenir

✅ **`restrict`** est une **promesse** au compilateur (depuis C99)

✅ Garantit que **deux pointeurs restrict ne pointent pas vers la même zone**

✅ Permet au compilateur de faire des **optimisations agressives** :
   - Vectorisation SIMD
   - Réorganisation d'instructions
   - Meilleure utilisation des registres

✅ **Comportement indéfini** si la promesse est violée

✅ Utilisé dans les **fonctions standard** : `memcpy`, `strcpy`, etc.

✅ Combine avec `const` pour plus de clarté

✅ **Le compilateur ne vérifie PAS** : c'est votre responsabilité

✅ Gain de performance : **jusqu'à 2-3x** dans certains cas

✅ N'existe **pas** en C++ standard (extensions compilateur uniquement)

## Bonnes pratiques

### ✅ À faire

1. Utiliser restrict pour les **fonctions critiques**
2. **Documenter** la garantie de non-chevauchement
3. Combiner avec `const` pour les paramètres en lecture seule
4. Tester avec différents niveaux d'optimisation

### ❌ À éviter

1. Utiliser restrict sans être **absolument sûr**
2. Oublier restrict dans les fonctions internes
3. Utiliser restrict inutilement (fonctions simples)
4. Ignorer les warnings du compilateur

## Prochaine étape

Vous avez maintenant complété le chapitre sur les pointeurs ! Vous maîtrisez :
- Les concepts d'adresse mémoire
- La déclaration et le déréférencement
- L'arithmétique des pointeurs
- Les pointeurs void* et le casting
- Les pointeurs NULL et dangling
- Les pointeurs constants (const)
- Les pointeurs restreints (restrict)

La prochaine section explorera les **tableaux et chaînes de caractères**, en approfondissant la relation étroite entre tableaux et pointeurs.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je comprends ce qu'est l'aliasing
- [ ] Je sais ce que garantit restrict
- [ ] Je comprends que restrict est une promesse, pas une vérification
- [ ] Je connais les cas d'usage typiques (memcpy, boucles)
- [ ] Je sais combiner restrict et const
- [ ] Je comprends les gains de performance possibles
- [ ] Je sais que violer restrict = comportement indéfini
- [ ] Je connais les différences avec const

---

**Prochaine section :** Chapitre 8 : Tableaux et Chaînes de caractères

⏭️ [Tableaux et Chaînes de caractères](/08-tableaux-et-chaines/README.md)
