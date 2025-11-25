🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.1 Concept d'adresse mémoire

## Introduction

Avant de plonger dans le monde des pointeurs, il est essentiel de comprendre un concept fondamental : **l'adresse mémoire**. C'est la base qui vous permettra de maîtriser les pointeurs et la gestion de la mémoire en C.

## Qu'est-ce que la mémoire ?

La mémoire de votre ordinateur (RAM) peut être vue comme un immense tableau de cases numérotées. Chaque case peut contenir une petite quantité de données (généralement 1 octet, soit 8 bits).

```
Visualisation simplifiée de la mémoire :

Adresse     Contenu (en hexadécimal)
-------     ------------------------
0x1000      [  42  ]
0x1001      [  00  ]
0x1002      [  FF  ]
0x1003      [  A5  ]
0x1004      [  7C  ]
0x1005      [  12  ]
...         ...
```

## Qu'est-ce qu'une adresse mémoire ?

Une **adresse mémoire** est simplement le numéro d'une case dans ce grand tableau. C'est comme une adresse postale qui permet de localiser précisément où se trouve une donnée dans la mémoire.

### Analogie avec une rue

Imaginez une rue avec des maisons :
- Chaque maison a un **numéro** (l'adresse)
- Chaque maison peut contenir des **habitants** (les données)
- Pour rendre visite à quelqu'un, vous avez besoin de son **adresse**

En programmation C, c'est exactement pareil :
- Chaque variable a une **adresse mémoire**
- Chaque variable contient une **valeur**
- Pour accéder à une variable, le programme utilise son **adresse**

## Les variables et leurs adresses

Quand vous déclarez une variable en C, le compilateur lui réserve un emplacement en mémoire. Cette variable possède alors :
1. **Un nom** (celui que vous lui donnez)
2. **Une adresse** (son emplacement en mémoire)
3. **Une valeur** (le contenu stocké à cette adresse)

### Exemple simple

```c
int age = 25;
```

Cette simple ligne fait trois choses :
- Réserve 4 octets en mémoire (la taille d'un `int` sur la plupart des systèmes modernes)
- Stocke la valeur `25` dans cet emplacement
- Associe le nom `age` à cette adresse

```
Représentation en mémoire :

Adresse     Nom        Valeur
-------     ----       ------
0x7FFF12A4  age        25
```

## L'opérateur d'adresse : &

En C, pour obtenir l'adresse d'une variable, on utilise l'opérateur **`&`** (esperluette, ou "ampersand" en anglais).

### Syntaxe

```c
&nom_de_variable
```

### Exemple complet

```c
#include <stdio.h>

int main(void) {
    int age = 25;

    printf("La valeur de age est : %d\n", age);
    printf("L'adresse de age est : %p\n", (void*)&age);

    return 0;
}
```

**Sortie possible :**
```
La valeur de age est : 25
L'adresse de age est : 0x7ffd4c8b3a4c
```

### Décortiquons cet exemple

1. `age` : accède à la **valeur** stockée dans la variable
2. `&age` : accède à l'**adresse** de la variable
3. `%p` : format d'affichage pour une adresse (en hexadécimal)
4. `(void*)` : conversion de type nécessaire pour `%p` (nous y reviendrons plus tard)

> **Note importante :** L'adresse exacte affichée sera différente à chaque exécution du programme. C'est normal ! Le système d'exploitation place les variables à des emplacements qui peuvent varier.

## Pourquoi les adresses sont en hexadécimal ?

Vous avez remarqué que l'adresse s'affiche avec des lettres et des chiffres comme `0x7ffd4c8b3a4c` ?

- Le préfixe `0x` indique une notation **hexadécimale** (base 16)
- L'hexadécimal utilise les chiffres 0-9 et les lettres A-F
- C'est plus compact et plus lisible que le binaire pour les humains
- Chaque chiffre hexadécimal représente exactement 4 bits

### Exemple de conversion

```
Hexadécimal : 0x2A
Décimal     : 42
Binaire     : 00101010
```

## Taille des adresses

Sur un système 32 bits, une adresse fait 4 octets (32 bits).
Sur un système 64 bits, une adresse fait 8 octets (64 bits).

```c
#include <stdio.h>

int main(void) {
    int nombre = 42;

    printf("Taille d'un int : %zu octets\n", sizeof(int));
    printf("Taille d'une adresse : %zu octets\n", sizeof(&nombre));

    return 0;
}
```

**Sortie possible (système 64 bits) :**
```
Taille d'un int : 4 octets
Taille d'une adresse : 8 octets
```

## Plusieurs variables et leurs adresses

Voyons comment plusieurs variables sont disposées en mémoire :

```c
#include <stdio.h>

int main(void) {
    int a = 10;
    int b = 20;
    int c = 30;

    printf("Adresse de a : %p (valeur : %d)\n", (void*)&a, a);
    printf("Adresse de b : %p (valeur : %d)\n", (void*)&b, b);
    printf("Adresse de c : %p (valeur : %d)\n", (void*)&c, c);

    return 0;
}
```

**Sortie possible :**
```
Adresse de a : 0x7ffc8b2a4a0c (valeur : 10)
Adresse de b : 0x7ffc8b2a4a08 (valeur : 20)
Adresse de c : 0x7ffc8b2a4a04 (valeur : 30)
```

### Observations importantes

1. Les adresses sont **proches les unes des autres** (différence de 4 octets = taille d'un int)
2. Les variables locales sont souvent placées dans l'ordre **inverse** de leur déclaration sur la pile (d'où les adresses décroissantes)
3. L'ordre exact dépend du compilateur et des optimisations

```
Visualisation de la pile (croissance vers le bas) :

Adresse plus haute
    ↓
0x7ffc8b2a4a0c  → a = 10
0x7ffc8b2a4a08  → b = 20
0x7ffc8b2a4a04  → c = 30
    ↓
Adresse plus basse
```

## Types de données et adresses

Tous les types de variables ont une adresse, quelle que soit leur taille :

```c
#include <stdio.h>

int main(void) {
    char    lettre = 'A';        // 1 octet
    int     nombre = 42;          // 4 octets (typiquement)
    float   decimal = 3.14f;      // 4 octets
    double  grand = 2.718281828;  // 8 octets

    printf("Adresse de lettre (char)   : %p\n", (void*)&lettre);
    printf("Adresse de nombre (int)    : %p\n", (void*)&nombre);
    printf("Adresse de decimal (float) : %p\n", (void*)&decimal);
    printf("Adresse de grand (double)  : %p\n", (void*)&grand);

    return 0;
}
```

**Point clé :** Chaque variable a une adresse, mais la **taille** de la variable détermine combien d'octets consécutifs elle occupe.

```
Exemple de disposition en mémoire :

Adresse     Type      Octets occupés
-------     ----      --------------
0x1000      char      [A]
0x1001      (vide)    [  ]
0x1002      (vide)    [  ]
0x1003      (vide)    [  ]
0x1004      int       [42][00][00][00]  (4 octets)
0x1008      float     [..][..][..][..]  (4 octets)
0x100C      double    [..][..][..][..]  (8 octets)
                      [..][..][..][..]
```

## Pourquoi les adresses sont-elles importantes ?

Comprendre les adresses mémoire est crucial car :

1. **Pointeurs** : Les pointeurs stockent des adresses (chapitre suivant)
2. **Passage par référence** : Permet de modifier des variables dans les fonctions
3. **Allocation dynamique** : `malloc()` retourne une adresse
4. **Débogage** : Identifier les problèmes de mémoire
5. **Performance** : Comprendre la localité des données

## Résumé des concepts clés

| Concept | Description | Exemple |
|---------|-------------|---------|
| **Adresse mémoire** | Numéro identifiant un emplacement en mémoire | `0x7ffd4c8b3a4c` |
| **Opérateur &** | Obtient l'adresse d'une variable | `&age` |
| **Format %p** | Affiche une adresse en hexadécimal | `printf("%p", &var)` |
| **sizeof()** | Retourne la taille en octets | `sizeof(int)` |

## Ce qu'il faut retenir

✅ Chaque variable en mémoire a :
   - Un nom (pour le programmeur)
   - Une adresse (pour le processeur)
   - Une valeur (les données stockées)

✅ L'opérateur `&` permet d'obtenir l'adresse d'une variable

✅ Les adresses sont affichées en hexadécimal avec `%p`

✅ La taille d'une adresse dépend de l'architecture (32 ou 64 bits)

✅ Comprendre les adresses est la clé pour maîtriser les pointeurs

## Transition vers les pointeurs

Maintenant que vous comprenez ce qu'est une adresse mémoire, vous êtes prêt à découvrir les **pointeurs** : des variables spéciales qui **stockent des adresses**.

La prochaine section (7.2) vous montrera comment déclarer des pointeurs, les utiliser, et pourquoi ils sont si puissants en C.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je sais ce qu'est une adresse mémoire
- [ ] Je comprends l'analogie avec les adresses postales
- [ ] Je peux utiliser l'opérateur `&` pour obtenir une adresse
- [ ] Je sais afficher une adresse avec `printf` et `%p`
- [ ] Je comprends pourquoi les adresses sont en hexadécimal

---

**Prochaine étape :** 7.2 Déclaration, déréférencement et adresse

⏭️ [Déclaration, déréférencement et adresse](/07-pointeurs/02-declaration-dereferencement.md)
