🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.3 L'arithmétique des pointeurs

## Introduction

Dans les sections précédentes, vous avez appris à déclarer des pointeurs et à les déréférencer. Maintenant, nous allons découvrir une fonctionnalité puissante du C : **l'arithmétique des pointeurs**, qui permet de naviguer efficacement dans la mémoire, notamment dans les tableaux.

## Qu'est-ce que l'arithmétique des pointeurs ?

L'arithmétique des pointeurs consiste à effectuer des opérations mathématiques (addition, soustraction) sur des pointeurs pour se déplacer dans la mémoire.

### Analogie

Imaginez une rue avec des maisons numérotées :
- Vous êtes devant la **maison numéro 100** (pointeur actuel)
- Vous voulez aller à la **maison suivante** (pointeur + 1)
- Vous ne dites pas "allez au numéro 101", mais plutôt "**avancez d'une maison**"

C'est exactement ce que fait l'arithmétique des pointeurs : elle permet de "sauter" d'un élément à l'autre.

## Opérations autorisées

Voici les opérations valides sur les pointeurs :

| Opération | Syntaxe | Description |
|-----------|---------|-------------|
| Addition | `ptr + n` | Avance de n éléments |
| Soustraction | `ptr - n` | Recule de n éléments |
| Incrémentation | `ptr++` ou `++ptr` | Avance d'un élément |
| Décrémentation | `ptr--` ou `--ptr` | Recule d'un élément |
| Différence | `ptr2 - ptr1` | Nombre d'éléments entre deux pointeurs |
| Comparaison | `ptr1 < ptr2` | Compare les adresses |

## Addition de pointeurs : ptr + n

Quand vous ajoutez un nombre à un pointeur, vous ne modifiez pas l'adresse d'un octet, mais **d'un nombre d'éléments**.

### Exemple fondamental

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr = tableau;    // ptr pointe vers le premier élément

    printf("Adresse de ptr     : %p\n", (void*)ptr);
    printf("Adresse de ptr + 1 : %p\n", (void*)(ptr + 1));
    printf("Adresse de ptr + 2 : %p\n", (void*)(ptr + 2));

    printf("\nDifférence en octets : %ld\n",
           (char*)(ptr + 1) - (char*)ptr);

    return 0;
}
```

**Sortie possible :**
```
Adresse de ptr     : 0x7ffd4c8b3a40  
Adresse de ptr + 1 : 0x7ffd4c8b3a44  
Adresse de ptr + 2 : 0x7ffd4c8b3a48  

Différence en octets : 4
```

### Observation importante

`ptr + 1` n'ajoute **pas 1 à l'adresse**, mais **4 octets** (la taille d'un `int`) !

**Visualisation en mémoire :**

```
Tableau en mémoire :

Adresse      Valeur    Position
---------    ------    --------
0x1000       [  10 ]   ← ptr (ptr + 0)
0x1004       [  20 ]   ← ptr + 1
0x1008       [  30 ]   ← ptr + 2
0x100C       [  40 ]   ← ptr + 3
0x1010       [  50 ]   ← ptr + 4

Chaque int occupe 4 octets
```

## La règle d'or de l'arithmétique des pointeurs

```
ptr + n  =  adresse_de_ptr + (n × sizeof(type_pointé))
```

### Exemples selon les types

```c
#include <stdio.h>

int main(void) {
    char   tableau_char[5];
    int    tableau_int[5];
    double tableau_double[5];

    char   *ptr_char   = tableau_char;
    int    *ptr_int    = tableau_int;
    double *ptr_double = tableau_double;

    printf("sizeof(char)   = %zu octet\n", sizeof(char));
    printf("sizeof(int)    = %zu octets\n", sizeof(int));
    printf("sizeof(double) = %zu octets\n\n", sizeof(double));

    printf("char*   : %p → %p (différence : %ld octets)\n",
           (void*)ptr_char, (void*)(ptr_char + 1),
           (char*)(ptr_char + 1) - (char*)ptr_char);

    printf("int*    : %p → %p (différence : %ld octets)\n",
           (void*)ptr_int, (void*)(ptr_int + 1),
           (char*)(ptr_int + 1) - (char*)ptr_int);

    printf("double* : %p → %p (différence : %ld octets)\n",
           (void*)ptr_double, (void*)(ptr_double + 1),
           (char*)(ptr_double + 1) - (char*)ptr_double);

    return 0;
}
```

**Sortie possible :**
```
sizeof(char)   = 1 octet  
sizeof(int)    = 4 octets  
sizeof(double) = 8 octets  

char*   : 0x7ffd4c8b3a40 → 0x7ffd4c8b3a41 (différence : 1 octets)  
int*    : 0x7ffd4c8b3a50 → 0x7ffd4c8b3a54 (différence : 4 octets)  
double* : 0x7ffd4c8b3a60 → 0x7ffd4c8b3a68 (différence : 8 octets)  
```

### Visualisation par type

```
char* (1 octet par élément) :  
Adresse: 0x1000  0x1001  0x1002  0x1003  0x1004  
         [  A ]  [  B ]  [  C ]  [  D ]  [  E ]
         ↑       ↑
         ptr     ptr+1  (avance de 1 octet)

int* (4 octets par élément) :  
Adresse: 0x1000           0x1004           0x1008  
         [ int1:10      ] [ int2:20      ] [ int3:30      ]
         ↑                ↑
         ptr              ptr+1  (avance de 4 octets)

double* (8 octets par élément) :  
Adresse: 0x1000                   0x1008  
         [ double1: 3.14        ] [ double2: 2.71        ]
         ↑                        ↑
         ptr                      ptr+1  (avance de 8 octets)
```

## Pointeurs et tableaux : la relation

En C, le nom d'un tableau est essentiellement un pointeur vers son premier élément.

```c
int tableau[5] = {10, 20, 30, 40, 50};  
int *ptr = tableau;    // Équivalent à : int *ptr = &tableau[0];  
```

### Deux notations équivalentes

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};

    // Notation avec crochets []
    printf("tableau[0] = %d\n", tableau[0]);
    printf("tableau[1] = %d\n", tableau[1]);
    printf("tableau[2] = %d\n\n", tableau[2]);

    // Notation avec arithmétique de pointeurs
    printf("*(tableau + 0) = %d\n", *(tableau + 0));
    printf("*(tableau + 1) = %d\n", *(tableau + 1));
    printf("*(tableau + 2) = %d\n", *(tableau + 2));

    return 0;
}
```

**Sortie :**
```
tableau[0] = 10  
tableau[1] = 20  
tableau[2] = 30  

*(tableau + 0) = 10
*(tableau + 1) = 20
*(tableau + 2) = 30
```

### L'équivalence fondamentale

```c
tableau[i]  ≡  *(tableau + i)
```

Cette équivalence est au cœur du C !

**Explication :**
- `tableau[2]` signifie : "Va au début de `tableau`, avance de 2 éléments, et récupère la valeur"
- `*(tableau + 2)` fait exactement la même chose, mais explicitement

## Parcourir un tableau avec un pointeur

### Méthode 1 : Incrémenter le pointeur

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr = tableau;

    printf("Parcours du tableau avec un pointeur :\n");
    for (int i = 0; i < 5; i++) {
        printf("Adresse : %p, Valeur : %d\n", (void*)ptr, *ptr);
        ptr++;    // Avance au prochain élément
    }

    return 0;
}
```

**Sortie possible :**
```
Parcours du tableau avec un pointeur :  
Adresse : 0x7ffd4c8b3a40, Valeur : 10  
Adresse : 0x7ffd4c8b3a44, Valeur : 20  
Adresse : 0x7ffd4c8b3a48, Valeur : 30  
Adresse : 0x7ffd4c8b3a4c, Valeur : 40  
Adresse : 0x7ffd4c8b3a50, Valeur : 50  
```

### Méthode 2 : Arithmétique sans modifier le pointeur

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr = tableau;

    printf("Parcours avec arithmétique de pointeurs :\n");
    for (int i = 0; i < 5; i++) {
        printf("*(ptr + %d) = %d\n", i, *(ptr + i));
    }

    return 0;
}
```

**Sortie :**
```
Parcours avec arithmétique de pointeurs :
*(ptr + 0) = 10
*(ptr + 1) = 20
*(ptr + 2) = 30
*(ptr + 3) = 40
*(ptr + 4) = 50
```

## Incrémentation et décrémentation

Les pointeurs supportent les opérateurs `++` et `--` comme les variables normales.

### Les quatre variantes

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr = tableau + 2;    // Commence au 3ème élément (30)

    printf("Position initiale : *ptr = %d\n\n", *ptr);

    // Post-incrémentation : utilise puis incrémente
    printf("*ptr++ = %d\n", *ptr++);    // Affiche 30, puis ptr avance
    printf("Maintenant *ptr = %d\n\n", *ptr);

    ptr = tableau + 2;    // Réinitialise

    // Pré-incrémentation : incrémente puis utilise
    printf("*++ptr = %d\n", *++ptr);    // Avance d'abord, puis affiche 40
    printf("Maintenant *ptr = %d\n", *ptr);

    return 0;
}
```

**Sortie :**
```
Position initiale : *ptr = 30

*ptr++ = 30
Maintenant *ptr = 40

*++ptr = 40
Maintenant *ptr = 40
```

### Tableau de référence

| Expression | Action | Résultat |
|------------|--------|----------|
| `ptr++` | Retourne `*ptr`, puis incrémente `ptr` | Valeur actuelle, ptr avance |
| `++ptr` | Incrémente `ptr`, puis retourne `*ptr` | Valeur suivante |
| `ptr--` | Retourne `*ptr`, puis décrémente `ptr` | Valeur actuelle, ptr recule |
| `--ptr` | Décrémente `ptr`, puis retourne `*ptr` | Valeur précédente |
| `*ptr++` | Déréférence, puis incrémente | Comme `ptr++` |
| `(*ptr)++` | Incrémente la **valeur** pointée | Modifie la valeur ! |

### Attention à la priorité !

```c
#include <stdio.h>

int main(void) {
    int tableau[3] = {10, 20, 30};
    int *ptr = tableau;

    printf("Valeur initiale : *ptr = %d\n", *ptr);

    int val = *ptr++;   // Équivalent à *(ptr++) : lit la valeur, puis avance
    printf("Valeur lue par *ptr++ : %d\n", val);
    printf("Après *ptr++ : *ptr = %d\n", *ptr);

    ptr = tableau;    // Réinitialise

    (*ptr)++;   // Incrémente la VALEUR pointée
    printf("Après (*ptr)++ : *ptr = %d, tableau[0] = %d\n",
           *ptr, tableau[0]);

    return 0;
}
```

**Sortie :**
```
Valeur initiale : *ptr = 10  
Valeur lue par *ptr++ : 10  
Après *ptr++ : *ptr = 20  
Après (*ptr)++ : *ptr = 11, tableau[0] = 11
```

## Soustraction de pointeurs

On peut soustraire deux pointeurs pour obtenir le **nombre d'éléments** qui les séparent.

### Exemple

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr1 = &tableau[1];    // Pointe vers 20
    int *ptr2 = &tableau[4];    // Pointe vers 50

    printf("ptr1 pointe vers tableau[1] (%d)\n", *ptr1);
    printf("ptr2 pointe vers tableau[4] (%d)\n", *ptr2);

    printf("\nptr2 - ptr1 = %ld éléments\n", ptr2 - ptr1);
    printf("ptr1 - ptr2 = %ld éléments\n", ptr1 - ptr2);

    return 0;
}
```

**Sortie :**
```
ptr1 pointe vers tableau[1] (20)  
ptr2 pointe vers tableau[4] (50)  

ptr2 - ptr1 = 3 éléments  
ptr1 - ptr2 = -3 éléments  
```

### Visualisation

```
Tableau :  
Index:    0     1     2     3     4  
Valeur:  [10]  [20]  [30]  [40]  [50]  
                ↑                 ↑
               ptr1              ptr2

Distance : ptr2 - ptr1 = 4 - 1 = 3 éléments
```

### Attention : type du résultat

Le résultat de `ptr2 - ptr1` est de type **`ptrdiff_t`** (défini dans `<stddef.h>`), qui est généralement un `long` signé.

## Comparaison de pointeurs

On peut comparer des pointeurs avec les opérateurs relationnels classiques.

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *ptr1 = &tableau[1];
    int *ptr2 = &tableau[3];

    if (ptr1 < ptr2) {
        printf("ptr1 est avant ptr2 en mémoire\n");
    }

    if (ptr2 > ptr1) {
        printf("ptr2 est après ptr1 en mémoire\n");
    }

    if (ptr1 == &tableau[1]) {
        printf("ptr1 pointe bien vers tableau[1]\n");
    }

    return 0;
}
```

**Sortie :**
```
ptr1 est avant ptr2 en mémoire  
ptr2 est après ptr1 en mémoire  
ptr1 pointe bien vers tableau[1]  
```

### Opérateurs de comparaison valides

| Opérateur | Signification |
|-----------|---------------|
| `==` | Égalité (même adresse) |
| `!=` | Différence |
| `<` | Avant en mémoire |
| `>` | Après en mémoire |
| `<=` | Avant ou égal |
| `>=` | Après ou égal |

## Exemple pratique : fonction pour trouver un élément

```c
#include <stdio.h>

// Recherche une valeur dans un tableau
// Retourne un pointeur vers l'élément ou NULL si non trouvé
int* trouver(int *debut, int *fin, int valeur) {
    while (debut < fin) {
        if (*debut == valeur) {
            return debut;    // Trouvé !
        }
        debut++;
    }
    return NULL;    // Non trouvé
}

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};
    int *resultat;

    resultat = trouver(tableau, tableau + 5, 30);

    if (resultat != NULL) {
        printf("Valeur trouvée à l'index : %ld\n", resultat - tableau);
        printf("Valeur : %d\n", *resultat);
    } else {
        printf("Valeur non trouvée\n");
    }

    return 0;
}
```

**Sortie :**
```
Valeur trouvée à l'index : 2  
Valeur : 30  
```

### Explication de la fonction

1. `debut < fin` : Parcourt le tableau
2. `*debut` : Déréférence pour comparer la valeur
3. `debut++` : Avance au prochain élément
4. `resultat - tableau` : Calcule l'index (distance du début)

## Les limites et précautions

### ⚠️ Ne pas dépasser les limites du tableau

```c
int tableau[5] = {10, 20, 30, 40, 50};  
int *ptr = tableau;  

// ✓ Valide : pointe vers tableau[2]
ptr = ptr + 2;

// ⚠️ DANGER : hors limites !
ptr = ptr + 10;    // Pointe en dehors du tableau
```

**Comportement indéfini :** Accéder à la mémoire hors d'un tableau peut :
- Crasher le programme (segmentation fault)
- Lire/écrire des données aléatoires
- Corrompre d'autres variables

### ⚠️ Pointeurs vers des tableaux différents

```c
int tableau1[5] = {1, 2, 3, 4, 5};  
int tableau2[5] = {6, 7, 8, 9, 10};  

int *ptr1 = tableau1;  
int *ptr2 = tableau2;  

// ✗ NON DÉFINI : comparer des pointeurs de tableaux différents
if (ptr1 < ptr2) {  // Résultat non prévisible
    // ...
}

// ✗ NON DÉFINI : soustraire des pointeurs de tableaux différents
int diff = ptr2 - ptr1;  // Résultat non prévisible
```

**Règle :** Ne comparez ou ne soustrayez que des pointeurs qui pointent vers le **même tableau** (ou bloc mémoire).

## Arithmétique sur void*

Les pointeurs `void*` ne supportent **pas** l'arithmétique standard car le compilateur ne connaît pas la taille de l'élément pointé.

```c
void *ptr_void;
// ptr_void + 1;    // ✗ ERREUR de compilation !
```

Pour faire de l'arithmétique sur `void*`, il faut d'abord le convertir :

```c
char *ptr_char = (char*)ptr_void;  
ptr_char = ptr_char + 1;    // ✓ OK, avance de 1 octet  
```

## Exemple récapitulatif complet

```c
#include <stdio.h>

void afficher_tableau(int *ptr, size_t taille) {
    printf("[ ");
    for (size_t i = 0; i < taille; i++) {
        printf("%d ", *(ptr + i));    // Ou ptr[i]
    }
    printf("]\n");
}

int main(void) {
    int nombres[6] = {100, 200, 300, 400, 500, 600};
    int *ptr = nombres;

    printf("=== Tableau initial ===\n");
    afficher_tableau(nombres, 6);

    printf("\n=== Navigation avec arithmétique ===\n");
    printf("Premier élément (*ptr)       : %d\n", *ptr);
    printf("Troisième élément (*(ptr+2)) : %d\n", *(ptr + 2));
    printf("Dernier élément (*(ptr+5))   : %d\n", *(ptr + 5));

    printf("\n=== Avancer le pointeur ===\n");
    ptr = ptr + 2;    // Avance de 2 éléments
    printf("Après ptr + 2, *ptr = %d\n", *ptr);

    printf("\n=== Incrémenter ===\n");
    ptr++;
    printf("Après ptr++, *ptr = %d\n", *ptr);

    printf("\n=== Distance ===\n");
    printf("Distance de ptr au début : %ld éléments\n", ptr - nombres);

    printf("\n=== Modification via pointeur ===\n");
    *ptr = 999;
    afficher_tableau(nombres, 6);

    return 0;
}
```

**Sortie :**
```
=== Tableau initial ===
[ 100 200 300 400 500 600 ]

=== Navigation avec arithmétique ===
Premier élément (*ptr)       : 100  
Troisième élément (*(ptr+2)) : 300  
Dernier élément (*(ptr+5))   : 600  

=== Avancer le pointeur ===
Après ptr + 2, *ptr = 300

=== Incrémenter ===
Après ptr++, *ptr = 400

=== Distance ===
Distance de ptr au début : 3 éléments

=== Modification via pointeur ===
[ 100 200 300 999 500 600 ]
```

## Cas d'usage courants

### 1. Parcourir un tableau

```c
int *ptr = tableau;  
while (ptr < tableau + taille) {  
    // Traiter *ptr
    ptr++;
}
```

### 2. Accéder au dernier élément

```c
int *dernier = tableau + (taille - 1);  
printf("Dernier : %d\n", *dernier);  
```

### 3. Inverser un tableau

```c
int *debut = tableau;  
int *fin = tableau + taille - 1;  

while (debut < fin) {
    int temp = *debut;
    *debut = *fin;
    *fin = temp;
    debut++;
    fin--;
}
```

## Résumé des concepts clés

| Concept | Formule | Signification |
|---------|---------|---------------|
| **Addition** | `ptr + n` | Avance de n éléments |
| **Taille** | `ptr + n = ptr + (n × sizeof(type))` | En octets |
| **Équivalence** | `tableau[i] ≡ *(tableau + i)` | Deux notations identiques |
| **Incrémentation** | `ptr++` | Avance d'un élément |
| **Soustraction** | `ptr2 - ptr1` | Nombre d'éléments entre eux |
| **Comparaison** | `ptr1 < ptr2` | Position relative en mémoire |

## Ce qu'il faut retenir

✅ **`ptr + n`** avance de **n éléments**, pas de n octets

✅ Le nombre d'octets ajouté dépend du **type** pointé

✅ **`tableau[i]`** est équivalent à **`*(tableau + i)`**

✅ **`ptr++`** et **`ptr--`** permettent de naviguer élément par élément

✅ **`ptr2 - ptr1`** donne le nombre d'éléments qui les séparent

✅ On peut comparer des pointeurs avec `<`, `>`, `==`, etc.

✅ **Attention** : ne pas sortir des limites du tableau !

✅ L'arithmétique des pointeurs ne fonctionne **que sur des pointeurs vers le même tableau**

## Visualisation finale

```
Mémoire :
         +----+----+----+----+----+
tableau: | 10 | 20 | 30 | 40 | 50 |
         +----+----+----+----+----+
           ↑    ↑    ↑    ↑    ↑
           |    |    |    |    |
        ptr   +1   +2   +3   +4

Opérations :  
ptr + 0  → tableau[0] → 10  
ptr + 1  → tableau[1] → 20  
ptr + 2  → tableau[2] → 30  
ptr + 3  → tableau[3] → 40  
ptr + 4  → tableau[4] → 50  

*(ptr + i) ≡ tableau[i]
```

## Prochaine étape

Maintenant que vous maîtrisez l'arithmétique des pointeurs, vous êtes prêt à découvrir les **pointeurs void*** et le **casting** dans la section suivante, qui vous donneront encore plus de flexibilité dans la manipulation de la mémoire.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je comprends que `ptr + 1` avance d'un élément, pas d'un octet
- [ ] Je sais que `tableau[i]` équivaut à `*(tableau + i)`
- [ ] Je maîtrise `ptr++` et `ptr--`
- [ ] Je peux calculer la distance entre deux pointeurs
- [ ] Je comprends les limites : ne pas sortir du tableau
- [ ] Je peux parcourir un tableau avec un pointeur
- [ ] Je connais la différence entre `*ptr++` et `(*ptr)++`

---

**Prochaine étape :** 7.4 Pointeurs void* et casting

⏭️ [Pointeurs void* et casting](/07-pointeurs/04-pointeurs-void.md)
