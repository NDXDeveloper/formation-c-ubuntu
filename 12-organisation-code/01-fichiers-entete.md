🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 12.1 Fichiers d'en-tête et gardes d'inclusion

## Introduction

Lorsqu'un projet C dépasse quelques dizaines de lignes, il devient essentiel de **structurer le code** en plusieurs fichiers. Les **fichiers d'en-tête** (ou *header files*, extension `.h`) jouent un rôle central dans cette organisation : ils permettent de **déclarer** les fonctions, structures et constantes qui seront **définies** ailleurs.

Dans cette section, nous allons découvrir :
- Ce qu'est un fichier d'en-tête et pourquoi l'utiliser
- Le problème de l'inclusion multiple
- Les gardes d'inclusion (include guards)
- Les conventions et bonnes pratiques

---

## Qu'est-ce qu'un fichier d'en-tête ?

### Définition

Un **fichier d'en-tête** (`.h`) contient des **déclarations** :
- Prototypes de fonctions
- Définitions de structures (`struct`)
- Définitions de types (`typedef`)
- Déclarations de constantes et macros
- Déclarations de variables externes (`extern`)

Il **ne contient généralement pas** d'implémentation de fonctions (sauf cas particuliers comme les fonctions `inline` ou `static inline`).

### Pourquoi séparer déclarations et définitions ?

Imaginez que vous écrivez un projet avec plusieurs fichiers `.c` qui utilisent les mêmes fonctions. Sans fichier d'en-tête, vous devriez **recopier les prototypes** dans chaque fichier :

```c
// fichier1.c
int addition(int a, int b);  // Prototype recopié

int main() {
    int resultat = addition(5, 3);
    return 0;
}
```

```c
// fichier2.c
int addition(int a, int b);  // Même prototype recopié !

void autre_fonction() {
    int x = addition(10, 20);
}
```

**Problèmes de cette approche :**
- ❌ Duplication de code (DRY : *Don't Repeat Yourself* violé)  
- ❌ Risque d'incohérence (si on modifie le prototype dans un fichier mais pas l'autre)  
- ❌ Maintenance difficile

**Solution : fichier d'en-tête**

```c
// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int addition(int a, int b);

#endif // MATH_UTILS_H
```

```c
// fichier1.c
#include "math_utils.h"

int main() {
    int resultat = addition(5, 3);
    return 0;
}
```

```c
// fichier2.c
#include "math_utils.h"

void autre_fonction() {
    int x = addition(10, 20);
}
```

- ✅ Un seul endroit où déclarer le prototype  
- ✅ Cohérence garantie  
- ✅ Facilité de maintenance

---

## Le problème de l'inclusion multiple

### Scénario typique

Supposons que nous ayons trois fichiers :

```c
// point.h
struct Point {
    int x;
    int y;
};
```

```c
// rectangle.h
#include "point.h"

struct Rectangle {
    struct Point coin_haut_gauche;
    struct Point coin_bas_droit;
};
```

```c
// main.c
#include "point.h"
#include "rectangle.h"

int main() {
    struct Rectangle rect;
    return 0;
}
```

### Que se passe-t-il lors de la compilation ?

Le préprocesseur C remplace les `#include` par le contenu des fichiers. Après préprocessing, `main.c` devient :

```c
// Contenu de point.h
struct Point {
    int x;
    int y;
};

// Contenu de rectangle.h
// qui inclut point.h à nouveau !
struct Point {    // ❌ ERREUR : redéfinition !
    int x;
    int y;
};
struct Rectangle {
    struct Point coin_haut_gauche;
    struct Point coin_bas_droit;
};

int main() {
    struct Rectangle rect;
    return 0;
}
```

**Résultat :** Le compilateur voit la définition de `struct Point` **deux fois** et génère une erreur :

```
error: redefinition of 'struct Point'
```

Ce problème survient dès qu'un fichier d'en-tête est inclus **plusieurs fois** dans la même unité de compilation (directement ou indirectement via d'autres includes).

---

## Les gardes d'inclusion : La solution

### Principe

Les **gardes d'inclusion** (ou *include guards*) sont des directives du préprocesseur qui garantissent qu'un fichier d'en-tête n'est traité **qu'une seule fois** par unité de compilation, même s'il est inclus plusieurs fois.

### Syntaxe standard

```c
#ifndef NOM_UNIQUE_H
#define NOM_UNIQUE_H

// Contenu du fichier d'en-tête
// (déclarations, structures, prototypes, etc.)

#endif // NOM_UNIQUE_H
```

**Explication :**
1. `#ifndef NOM_UNIQUE_H` : "Si le symbole `NOM_UNIQUE_H` n'est **pas défini**..."
2. `#define NOM_UNIQUE_H` : "...alors définissons-le maintenant"
3. Contenu du fichier
4. `#endif` : Fin du bloc conditionnel

**Lors de la première inclusion :**
- Le symbole `NOM_UNIQUE_H` n'existe pas
- Le préprocesseur entre dans le bloc `#ifndef`
- Le symbole est défini avec `#define`
- Le contenu est traité

**Lors d'une inclusion ultérieure :**
- Le symbole `NOM_UNIQUE_H` existe déjà
- Le préprocesseur **saute tout le bloc** jusqu'au `#endif`
- Le contenu n'est pas retraité → pas de redéfinition

---

## Exemple complet avec gardes d'inclusion

### Fichier `point.h`

```c
#ifndef POINT_H
#define POINT_H

struct Point {
    int x;
    int y;
};

// Prototype d'une fonction
void afficher_point(struct Point p);

#endif // POINT_H
```

### Fichier `rectangle.h`

```c
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "point.h"  // Inclusion de point.h

struct Rectangle {
    struct Point coin_haut_gauche;
    struct Point coin_bas_droit;
};

int aire_rectangle(struct Rectangle r);

#endif // RECTANGLE_H
```

### Fichier `main.c`

```c
#include <stdio.h>
#include "point.h"
#include "rectangle.h"  // Inclut indirectement point.h

int main() {
    struct Point p = {10, 20};
    struct Rectangle rect = {{0, 0}, {100, 50}};

    afficher_point(p);
    printf("Aire : %d\n", aire_rectangle(rect));

    return 0;
}
```

**Que se passe-t-il lors de la compilation ?**

1. `main.c` inclut `point.h` → `POINT_H` est défini, contenu traité
2. `main.c` inclut `rectangle.h`
   - `rectangle.h` inclut `point.h` à nouveau
   - Mais `POINT_H` est déjà défini → contenu de `point.h` ignoré
3. ✅ Pas de redéfinition, compilation réussie

---

## Conventions de nommage pour les gardes

### Règle générale

Le nom du symbole de garde doit être **unique** dans tout le projet. La convention la plus courante est :

```
NOM_DU_FICHIER_H
```

**Exemple :** Pour un fichier `math_utils.h` :

```c
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

// ...

#endif // MATH_UTILS_H
```

### Cas particuliers

**Projet avec plusieurs sous-répertoires :**

```
projet/
├── include/
│   ├── core/
│   │   └── engine.h
│   └── utils/
│       └── engine.h  ⚠️ Même nom !
```

Pour éviter les collisions, on peut utiliser le chemin complet :

```c
// include/core/engine.h
#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H
// ...
#endif
```

```c
// include/utils/engine.h
#ifndef UTILS_ENGINE_H
#define UTILS_ENGINE_H
// ...
#endif
```

**Ou avec le nom du projet :**

```c
#ifndef MONPROJET_CORE_ENGINE_H
#define MONPROJET_CORE_ENGINE_H
// ...
#endif
```

### Utiliser des underscores doubles (`__`) ?

**❌ À éviter :** Les symboles commençant par `__` (double underscore) ou `_` suivi d'une majuscule sont **réservés** par le standard C pour l'implémentation (compilateur, bibliothèque standard).

```c
// ❌ MAUVAIS
#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

// ✅ BON
#ifndef MATH_UTILS_H
#define MATH_UTILS_H
```

---

## Alternative moderne : `#pragma once`

### Présentation

Certains compilateurs modernes (GCC, Clang, MSVC) supportent une directive plus simple :

```c
#pragma once

// Contenu du fichier d'en-tête
```

**Avantages :**
- ✅ Plus concis (une seule ligne)  
- ✅ Pas de risque de collision de noms de gardes  
- ✅ Légèrement plus rapide (le compilateur peut optimiser)

**Inconvénients :**
- ⚠️ **Non standard** (pas dans le standard C)  
- ⚠️ Peut poser problème avec certains systèmes de fichiers (liens symboliques, systèmes de fichiers réseau)  
- ⚠️ Moins portable (compilateurs anciens ou exotiques)

### Comparaison

**Avec gardes traditionnelles :**

```c
#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    double x, y, z;
} Vector3D;

#endif // VECTOR_H
```

**Avec `#pragma once` :**

```c
#pragma once

typedef struct {
    double x, y, z;
} Vector3D;
```

### Recommandation

Pour un **maximum de portabilité**, préférez les gardes d'inclusion traditionnelles (`#ifndef`/`#define`/`#endif`). Pour des projets modernes avec des compilateurs récents et sans contraintes de portabilité extrême, `#pragma once` est acceptable.

**Certains projets utilisent les deux** (défense en profondeur) :

```c
#pragma once
#ifndef VECTOR_H
#define VECTOR_H

// Contenu

#endif
```

Cela garantit la compatibilité avec les compilateurs ne supportant pas `#pragma once` tout en bénéficiant de l'optimisation sur ceux qui le supportent.

---

## Que mettre dans un fichier d'en-tête ?

### ✅ À inclure

**1. Prototypes de fonctions publiques**

```c
// string_utils.h
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

char* copier_chaine(const char* source);  
int comparer_chaines(const char* s1, const char* s2);  

#endif
```

**2. Définitions de structures**

```c
// utilisateur.h
#ifndef UTILISATEUR_H
#define UTILISATEUR_H

typedef struct {
    char nom[50];
    char email[100];
    int age;
} Utilisateur;

#endif
```

**3. Définitions de types (`typedef`)**

```c
// types.h
#ifndef TYPES_H
#define TYPES_H

typedef unsigned char byte;  
typedef unsigned int uint;  

#endif
```

**4. Constantes et macros**

```c
// config.h
#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "1.0.0"
#define MAX_CONNEXIONS 100
#define PI 3.14159265359

#endif
```

**5. Déclarations de variables externes**

```c
// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

extern int compteur_global;  // Déclaration (défini dans globals.c)  
extern const char* nom_application;  

#endif
```

### ❌ À éviter

**1. Définitions de fonctions** (sauf `static inline`)

```c
// ❌ MAUVAIS (dans un .h)
int addition(int a, int b) {
    return a + b;
}
```

**Pourquoi ?** Si ce fichier est inclus dans plusieurs `.c`, la fonction sera **définie plusieurs fois** → erreur de linkage (*multiple definition*).

**✅ Solution :** Mettre le prototype dans le `.h` et la définition dans un `.c`

```c
// math_utils.h
int addition(int a, int b);  // Prototype uniquement
```

```c
// math_utils.c
#include "math_utils.h"

int addition(int a, int b) {  // Définition
    return a + b;
}
```

**Exception :** Fonctions `static inline` (pour l'optimisation)

```c
// utils.h
#ifndef UTILS_H
#define UTILS_H

static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

#endif
```

**2. Définitions de variables globales**

```c
// ❌ MAUVAIS
int compteur = 0;  // Définition dans le .h
```

**✅ BON :**

```c
// globals.h
extern int compteur;  // Déclaration

// globals.c
int compteur = 0;  // Définition
```

---

## Ordre des includes et dépendances

### Convention d'ordre

Pour éviter les problèmes de dépendances, il est recommandé d'inclure les fichiers dans cet ordre :

```c
// mon_module.c

// 1. En-tête correspondant (si applicable)
#include "mon_module.h"

// 2. En-têtes système (bibliothèque standard)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 3. En-têtes de bibliothèques tierces
#include <sqlite3.h>
#include <curl/curl.h>

// 4. En-têtes du projet
#include "utils.h"
#include "config.h"
```

### Auto-suffisance des fichiers d'en-tête

**Règle d'or :** Chaque fichier `.h` doit pouvoir être inclus **seul** sans erreur de compilation.

**❌ Mauvais exemple :**

```c
// rectangle.h (incomplet)
#ifndef RECTANGLE_H
#define RECTANGLE_H

// ⚠️ Utilise struct Point sans l'inclure !
struct Rectangle {
    struct Point coin1;
    struct Point coin2;
};

#endif
```

Si on fait `#include "rectangle.h"` seul, compilation échoue : `struct Point` inconnu.

**✅ Bon exemple :**

```c
// rectangle.h (auto-suffisant)
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "point.h"  // Dépendance explicite

struct Rectangle {
    struct Point coin1;
    struct Point coin2;
};

#endif
```

Maintenant, `#include "rectangle.h"` fonctionne seul.

---

## Includes circulaires : Le piège à éviter

### Le problème

Deux fichiers s'incluent mutuellement :

```c
// a.h
#ifndef A_H
#define A_H

#include "b.h"  // A inclut B

struct A {
    struct B* lien_vers_b;
};

#endif
```

```c
// b.h
#ifndef B_H
#define B_H

#include "a.h"  // B inclut A

struct B {
    struct A* lien_vers_a;
};

#endif
```

**Résultat :** Même avec les gardes, le compilateur ne peut pas résoudre les dépendances (une des structures est toujours inconnue au moment de la compilation de l'autre).

### Solution : Forward declaration

On peut **déclarer l'existence** d'une structure sans la définir complètement :

```c
// a.h
#ifndef A_H
#define A_H

struct B;  // Forward declaration (on dit "struct B existe")

struct A {
    struct B* lien_vers_b;  // Pointeur OK (taille connue)
};

#endif
```

```c
// b.h
#ifndef B_H
#define B_H

struct A;  // Forward declaration

struct B {
    struct A* lien_vers_a;
};

#endif
```

✅ Pas d'inclusion circulaire  
✅ Compilation réussie

**Limitation :** On ne peut utiliser que des **pointeurs** vers le type forward-déclaré (on ne connaît pas sa taille complète).

---

## Commentaire de fermeture du `#endif`

Pour améliorer la lisibilité, surtout dans les gros fichiers, il est courant de commenter le `#endif` final :

```c
#ifndef GROS_MODULE_H
#define GROS_MODULE_H

// ... 500 lignes de code ...

#endif // GROS_MODULE_H
```

ou

```c
#endif /* GROS_MODULE_H */
```

Cela permet de savoir rapidement quel `#ifndef` est fermé, surtout si le fichier contient d'autres directives conditionnelles (`#ifdef`, `#if defined`, etc.).

---

## Récapitulatif des bonnes pratiques

| ✅ À FAIRE | ❌ À ÉVITER |
|-----------|------------|
| Utiliser des gardes d'inclusion systématiquement | Oublier les gardes → redéfinitions |
| Nommer les gardes avec `NOM_FICHIER_H` en majuscules | Utiliser `__NOM__` (réservé) |
| Déclarer uniquement (prototypes, structures, types) | Définir des fonctions dans les `.h` |
| Rendre les `.h` auto-suffisants (inclure les dépendances) | Forcer l'utilisateur à inclure dans un ordre précis |
| Utiliser `extern` pour les variables globales | Définir des variables globales dans les `.h` |
| Commenter le `#endif` de fermeture | Laisser des `#endif` anonymes dans les gros fichiers |
| Utiliser forward declarations pour éviter les inclusions circulaires | Créer des dépendances circulaires entre `.h` |

---

## Exemple complet d'organisation

### Structure du projet

```
projet/
├── include/
│   ├── math_utils.h
│   ├── point.h
│   └── rectangle.h
├── src/
│   ├── math_utils.c
│   ├── point.c
│   ├── rectangle.c
│   └── main.c
└── Makefile
```

### `include/point.h`

```c
#ifndef POINT_H
#define POINT_H

typedef struct {
    int x;
    int y;
} Point;

void afficher_point(Point p);  
double distance_origine(Point p);  

#endif // POINT_H
```

### `include/rectangle.h`

```c
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "point.h"  // Dépendance nécessaire

typedef struct {
    Point coin_haut_gauche;
    Point coin_bas_droit;
} Rectangle;

int aire(Rectangle r);  
int perimetre(Rectangle r);  

#endif // RECTANGLE_H
```

### `include/math_utils.h`

```c
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#define PI 3.14159265359

int carre(int n);  
double racine_carree_approx(double x);  

#endif // MATH_UTILS_H
```

### `src/main.c`

```c
#include <stdio.h>
#include "point.h"
#include "rectangle.h"
#include "math_utils.h"

int main(void) {
    Point p = {3, 4};
    Rectangle rect = {{0, 0}, {10, 5}};

    printf("Distance du point à l'origine : %.2f\n", distance_origine(p));
    printf("Aire du rectangle : %d\n", aire(rect));
    printf("Carré de 7 : %d\n", carre(7));

    return 0;
}
```

**Compilation :**

```bash
gcc -I./include -o programme src/*.c
```

---

## Conclusion

Les **fichiers d'en-tête** et les **gardes d'inclusion** sont des mécanismes fondamentaux pour organiser du code C propre et maintenable. En suivant les conventions présentées, vous éviterez les erreurs de compilation courantes et faciliterez la collaboration sur des projets de taille moyenne à grande.

**Points clés à retenir :**
1. Les `.h` contiennent des **déclarations**, les `.c` des **définitions**
2. Les gardes d'inclusion (`#ifndef`/`#define`/`#endif`) préviennent les inclusions multiples
3. Respectez les conventions de nommage (majuscules, pas de `__`)
4. Rendez vos fichiers d'en-tête **auto-suffisants**
5. Évitez les inclusions circulaires avec des *forward declarations*

Dans la section suivante, nous explorerons la **portée des variables** et comment organiser efficacement les fichiers sources d'un projet.

⏭️ [La portée des variables](/12-organisation-code/02-portee-variables.md)
