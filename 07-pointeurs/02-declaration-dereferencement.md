🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.2 Déclaration, déréférencement et adresse

## Introduction

Dans la section précédente, vous avez découvert ce qu'est une **adresse mémoire**. Maintenant, nous allons apprendre à manipuler ces adresses en utilisant les **pointeurs** : des variables spéciales qui stockent des adresses.

## Qu'est-ce qu'un pointeur ?

Un **pointeur** est une variable qui contient l'adresse d'une autre variable.

### Analogie

Imaginez que vous avez :
- Une **maison** (la variable originale)
- Un **post-it avec l'adresse de la maison** (le pointeur)

Le post-it ne contient pas la maison elle-même, mais l'information qui permet de la trouver.

```
Variable normale :     age = 25
                       ┌────────┐
                       │   25   │  Contient directement la valeur
                       └────────┘

Pointeur :            ptr = &age
                       ┌────────┐
                       │ 0x1000 │  Contient une adresse (qui pointe vers age)
                       └────────┘
                           ↓
                       ┌────────┐
                       │   25   │  La variable pointée (age)
                       └────────┘
```

## Déclaration d'un pointeur

Pour déclarer un pointeur, on utilise l'astérisque `*` dans la déclaration.

### Syntaxe générale

```c
type *nom_du_pointeur;
```

### Exemples de déclarations

```c
int *ptr_int;       // Pointeur vers un int  
char *ptr_char;     // Pointeur vers un char  
float *ptr_float;   // Pointeur vers un float  
double *ptr_double; // Pointeur vers un double  
```

### Point important sur la syntaxe

L'astérisque peut être placé de différentes manières (toutes équivalentes) :

```c
int *ptr;    // Style recommandé (proche du nom de variable)  
int* ptr;    // Style alternatif (proche du type)  
int * ptr;   // Avec espaces des deux côtés
```

**Recommandation :** Utilisez `int *ptr` car cela évite les confusions lors de déclarations multiples :

```c
int* a, b;     // ⚠️ ATTENTION : a est un pointeur, b est un int !  
int *a, b;     // Plus clair : a est un pointeur, b est un int  
int *a, *b;    // a et b sont tous les deux des pointeurs  
```

## Initialisation d'un pointeur

Un pointeur doit pointer vers quelque chose pour être utilisé de manière sûre.

### Méthode 1 : Pointer vers une variable existante

```c
#include <stdio.h>

int main(void) {
    int age = 25;           // Variable normale
    int *ptr_age;           // Déclaration du pointeur

    ptr_age = &age;         // Le pointeur stocke l'adresse de age

    printf("Valeur de age : %d\n", age);
    printf("Adresse de age : %p\n", (void*)&age);
    printf("Valeur de ptr_age (l'adresse stockée) : %p\n", (void*)ptr_age);

    return 0;
}
```

**Sortie possible :**
```
Valeur de age : 25  
Adresse de age : 0x7ffd4c8b3a4c  
Valeur de ptr_age (l'adresse stockée) : 0x7ffd4c8b3a4c  
```

### Décortiquons cet exemple

```c
int age = 25;        // Variable normale contenant 25  
int *ptr_age;        // Pointeur non initialisé (dangereux !)  
ptr_age = &age;      // Maintenant ptr_age contient l'adresse de age  
```

**Visualisation mémoire :**

```
Adresse      Variable     Valeur
--------     --------     ------
0x7FFF1000   age          25
0x7FFF1008   ptr_age      0x7FFF1000  (adresse de age)
```

### Méthode 2 : Initialisation à la déclaration

Il est préférable d'initialiser le pointeur dès sa déclaration :

```c
int age = 25;  
int *ptr_age = &age;    // Déclaration + initialisation en une ligne  
```

### Pointeur non initialisé (DANGER !)

```c
int *ptr;    // ⚠️ DANGEREUX : ptr contient une adresse aléatoire !
```

Un pointeur non initialisé est **extrêmement dangereux** car il pointe vers un emplacement mémoire aléatoire. L'utiliser peut :
- Causer un **segmentation fault** (crash)
- Corrompre des données
- Créer des bugs très difficiles à trouver

## Le déréférencement : opérateur *

Le **déréférencement** permet d'accéder à la valeur stockée à l'adresse contenue dans le pointeur.

### Syntaxe

```c
*nom_du_pointeur
```

L'astérisque `*` a **deux rôles différents** en C :
1. **À la déclaration** : indique que la variable est un pointeur
2. **À l'utilisation** : déréférence le pointeur (accède à la valeur pointée)

### Exemple complet

```c
#include <stdio.h>

int main(void) {
    int age = 25;
    int *ptr_age = &age;    // ptr_age pointe vers age

    printf("Valeur de age : %d\n", age);
    printf("Valeur via le pointeur : %d\n", *ptr_age);  // Déréférencement

    return 0;
}
```

**Sortie :**
```
Valeur de age : 25  
Valeur via le pointeur : 25  
```

### Comprendre le déréférencement

```c
int age = 25;  
int *ptr_age = &age;  

// Différentes manières d'accéder à la même valeur :
printf("%d\n", age);        // Accès direct par le nom  
printf("%d\n", *ptr_age);   // Accès indirect via le pointeur  
```

**Visualisation :**

```
    age                  ptr_age              *ptr_age
┌─────────┐          ┌───────────┐              │
│   25    │  ←───────│  adresse  │──────────────┘
└─────────┘          └───────────┘
   Valeur          Adresse stockée         Valeur obtenue
  directe            dans le             par déréférencement
                     pointeur
```

## Modification via un pointeur

Le déréférencement permet non seulement de **lire** mais aussi de **modifier** la valeur pointée :

```c
#include <stdio.h>

int main(void) {
    int age = 25;
    int *ptr_age = &age;

    printf("Avant : age = %d\n", age);

    *ptr_age = 30;    // Modification via le pointeur

    printf("Après : age = %d\n", age);

    return 0;
}
```

**Sortie :**
```
Avant : age = 25  
Après : age = 30  
```

### Que s'est-il passé ?

```c
*ptr_age = 30;
```

Cette ligne signifie : "Va à l'adresse stockée dans `ptr_age` et change la valeur qui s'y trouve en 30".

**Visualisation de l'opération :**

```
Avant :
    age                  ptr_age
┌─────────┐          ┌───────────┐
│   25    │  ←───────│  0x1000   │
└─────────┘          └───────────┘

Après (*ptr_age = 30) :
    age                  ptr_age
┌─────────┐          ┌───────────┐
│   30    │  ←───────│  0x1000   │  (l'adresse n'a pas changé)
└─────────┘          └───────────┘
```

## Tableau récapitulatif des opérateurs

| Opérateur | Nom | Signification | Exemple |
|-----------|-----|---------------|---------|
| `*` | Astérisque (déclaration) | Déclare un pointeur | `int *ptr;` |
| `&` | Esperluette (adresse) | Obtient l'adresse d'une variable | `ptr = &age;` |
| `*` | Astérisque (déréférencement) | Accède à la valeur pointée | `*ptr = 30;` |

## Exemple complet avec tous les concepts

```c
#include <stdio.h>

int main(void) {
    // Déclaration et initialisation d'une variable normale
    int nombre = 42;

    // Déclaration et initialisation d'un pointeur
    int *ptr_nombre = &nombre;

    // Affichage des informations
    printf("=== Variable 'nombre' ===\n");
    printf("Valeur de nombre : %d\n", nombre);
    printf("Adresse de nombre : %p\n", (void*)&nombre);
    printf("Taille de nombre : %zu octets\n\n", sizeof(nombre));

    printf("=== Pointeur 'ptr_nombre' ===\n");
    printf("Valeur de ptr_nombre (adresse stockée) : %p\n", (void*)ptr_nombre);
    printf("Adresse de ptr_nombre lui-même : %p\n", (void*)&ptr_nombre);
    printf("Taille de ptr_nombre : %zu octets\n", sizeof(ptr_nombre));
    printf("Valeur pointée (*ptr_nombre) : %d\n\n", *ptr_nombre);

    // Vérification que c'est bien la même adresse
    printf("=== Vérification ===\n");
    if (&nombre == ptr_nombre) {
        printf("✓ ptr_nombre pointe bien vers nombre\n\n");
    }

    // Modification via le pointeur
    printf("=== Modification via le pointeur ===\n");
    printf("Avant : nombre = %d\n", nombre);
    *ptr_nombre = 100;
    printf("Après (*ptr_nombre = 100) : nombre = %d\n", nombre);

    return 0;
}
```

**Sortie possible :**
```
=== Variable 'nombre' ===
Valeur de nombre : 42  
Adresse de nombre : 0x7ffd4c8b3a4c  
Taille de nombre : 4 octets  

=== Pointeur 'ptr_nombre' ===
Valeur de ptr_nombre (adresse stockée) : 0x7ffd4c8b3a4c  
Adresse de ptr_nombre lui-même : 0x7ffd4c8b3a40  
Taille de ptr_nombre : 8 octets  
Valeur pointée (*ptr_nombre) : 42  

=== Vérification ===
✓ ptr_nombre pointe bien vers nombre

=== Modification via le pointeur ===
Avant : nombre = 42  
Après (*ptr_nombre = 100) : nombre = 100  
```

### Observations importantes

1. **Le pointeur a sa propre adresse** : `ptr_nombre` est lui-même une variable stockée quelque part en mémoire
2. **Taille du pointeur** : 8 octets sur un système 64 bits (indépendamment du type pointé)
3. **Même adresse** : `&nombre` et `ptr_nombre` contiennent la même adresse
4. **Modification indirecte** : Changer `*ptr_nombre` modifie `nombre`

## Plusieurs pointeurs vers la même variable

On peut avoir plusieurs pointeurs qui pointent vers la même variable :

```c
#include <stdio.h>

int main(void) {
    int valeur = 50;
    int *ptr1 = &valeur;
    int *ptr2 = &valeur;

    printf("valeur = %d\n", valeur);
    printf("*ptr1 = %d\n", *ptr1);
    printf("*ptr2 = %d\n", *ptr2);

    *ptr1 = 60;    // Modification via ptr1
    printf("\nAprès *ptr1 = 60 :\n");
    printf("valeur = %d\n", valeur);
    printf("*ptr2 = %d\n", *ptr2);

    return 0;
}
```

**Sortie :**
```
valeur = 50
*ptr1 = 50
*ptr2 = 50

Après *ptr1 = 60 :  
valeur = 60  
*ptr2 = 60
```

**Visualisation :**

```
        valeur
      ┌────────┐
      │   60   │
      └────────┘
         ↑  ↑
         │  │
    ptr1 │  │ ptr2
┌────────┘  └────────────┐
│   adresse   │  adresse │
└─────────────┴──────────┘
```

## Pointeur et types

Un pointeur est **typé** : il pointe vers un type spécifique de données.

```c
int entier = 10;  
float reel = 3.14f;  

int *ptr_int = &entier;      // ✓ Correct  
float *ptr_float = &reel;    // ✓ Correct  

// int *ptr_erreur = &reel;  // ✗ ERREUR : incompatibilité de types
```

### Pourquoi les pointeurs sont-ils typés ?

Le type du pointeur indique :
1. **Combien d'octets lire/écrire** lors du déréférencement
2. **Comment interpréter** les octets lus

```c
int entier = 1000;  
int *ptr_int = &entier;  
char *ptr_char = (char*)&entier;  // Cast explicite (avancé)  

printf("Via int* : %d\n", *ptr_int);      // Lit 4 octets comme un int  
printf("Via char* : %d\n", *ptr_char);    // Lit 1 octet comme un char  
```

## Erreurs courantes à éviter

### ❌ Erreur 1 : Déréférencement d'un pointeur non initialisé

```c
int *ptr;           // Pointeur non initialisé  
printf("%d\n", *ptr);  // ⚠️ DANGER : comportement indéfini, probablement un crash  
```

**Solution :**
```c
int valeur = 42;  
int *ptr = &valeur;    // ✓ Initialisation correcte  
printf("%d\n", *ptr);  // ✓ Sûr  
```

### ❌ Erreur 2 : Confusion entre pointeur et valeur

```c
int *ptr;  
int age = 25;  

ptr = age;     // ✗ ERREUR : on assigne une valeur à un pointeur !
```

**Solution :**
```c
int *ptr;  
int age = 25;  

ptr = &age;    // ✓ Correct : on assigne une adresse
```

### ❌ Erreur 3 : Oublier l'astérisque lors du déréférencement

```c
int age = 25;  
int *ptr = &age;  

printf("%d\n", ptr);   // ⚠️ Affiche l'adresse, pas la valeur !  
printf("%d\n", *ptr);  // ✓ Affiche la valeur (25)  
```

### ❌ Erreur 4 : Confusion dans les déclarations multiples

```c
int* ptr1, ptr2;    // ⚠️ ptr1 est un pointeur, ptr2 est un int !
```

**Solution :**
```c
int *ptr1, *ptr2;   // ✓ Les deux sont des pointeurs
// OU mieux :
int *ptr1;  
int *ptr2;          // Une déclaration par ligne (plus clair)  
```

## Résumé visuel

```c
int age = 25;  
int *ptr = &age;  
*ptr = 30;
```

**Étape par étape :**

```
1. int age = 25;
   ┌─────────┐
   │   25    │  age
   └─────────┘
   @ 0x1000

2. int *ptr = &age;
   ┌─────────┐
   │   25    │  age
   └─────────┘
   @ 0x1000
       ↑
       │
   ┌─────────┐
   │ 0x1000  │  ptr
   └─────────┘
   @ 0x1008

3. *ptr = 30;
   ┌─────────┐
   │   30    │  age (modifié !)
   └─────────┘
   @ 0x1000
       ↑
       │
   ┌─────────┐
   │ 0x1000  │  ptr (inchangé)
   └─────────┘
   @ 0x1008
```

## Ce qu'il faut retenir

✅ **Déclaration :** `type *nom_pointeur;` crée un pointeur

✅ **Adresse :** `&variable` donne l'adresse d'une variable

✅ **Initialisation :** `int *ptr = &age;` fait pointer ptr vers age

✅ **Déréférencement :** `*ptr` accède à la valeur pointée

✅ **Modification :** `*ptr = 30;` modifie la valeur pointée

✅ **L'astérisque a deux rôles :**
   - À la déclaration : indique un pointeur
   - À l'utilisation : déréférence le pointeur

✅ **Toujours initialiser les pointeurs** avant de les utiliser

## Tableau de référence rapide

| Expression | Type | Signification |
|------------|------|---------------|
| `int age` | `int` | Variable entière |
| `&age` | `int *` | Adresse de age |
| `int *ptr` | `int *` | Pointeur vers un int |
| `ptr = &age` | - | ptr pointe vers age |
| `*ptr` | `int` | Valeur pointée par ptr |
| `*ptr = 30` | - | Modifie la valeur pointée |

## Analogie finale

Pensez aux pointeurs comme à une **télécommande** :

- La **télécommande** (le pointeur) n'est pas la télévision elle-même
- Elle **pointe** vers la télévision (contient son "adresse")
- Vous pouvez **utiliser la télécommande** pour contrôler la télévision à distance (déréférencement)
- Plusieurs télécommandes peuvent contrôler la même télévision (plusieurs pointeurs vers la même variable)

## Prochaine étape

Maintenant que vous maîtrisez les bases des pointeurs, nous allons découvrir **l'arithmétique des pointeurs** dans la section suivante, qui vous permettra de naviguer dans des tableaux et des structures de données complexes.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je sais déclarer un pointeur : `int *ptr;`
- [ ] Je comprends l'opérateur d'adresse `&`
- [ ] Je sais initialiser un pointeur : `ptr = &variable;`
- [ ] Je maîtrise le déréférencement avec `*ptr`
- [ ] Je peux lire ET modifier une valeur via un pointeur
- [ ] Je comprends que l'astérisque `*` a deux rôles différents
- [ ] Je sais éviter les pointeurs non initialisés

---

**Prochaine étape :** 7.3 L'arithmétique des pointeurs

⏭️ [L'arithmétique des pointeurs](/07-pointeurs/03-arithmetique-pointeurs.md)
