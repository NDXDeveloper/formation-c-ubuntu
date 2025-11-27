🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 22.3 Pointeurs multi-niveaux

## Introduction

Après avoir maîtrisé les pointeurs simples et les tableaux de pointeurs, nous allons maintenant explorer les **pointeurs multi-niveaux**. Ce concept peut sembler intimidant, mais avec une approche progressive, vous verrez qu'il s'agit simplement de pointeurs qui pointent vers d'autres pointeurs.

### Qu'est-ce qu'un pointeur multi-niveaux ?

Un pointeur multi-niveaux est un pointeur qui pointe vers un autre pointeur, qui lui-même peut pointer vers un autre pointeur, et ainsi de suite. Le niveau le plus courant est le **pointeur double** (ou pointeur de pointeur), noté avec deux astérisques `**`.

**Analogie** : Imaginez un système de directions pour trouver un trésor :
- **Pointeur simple (`*`)** : Une carte qui vous dit "Le trésor est à cette adresse"
- **Pointeur double (`**`)** : Une carte qui dit "Allez à cette adresse pour trouver une autre carte qui indique où est le trésor"
- **Pointeur triple (`***`)** : Une carte qui dit "Allez à cette adresse pour trouver une carte qui vous donnera une troisième carte..."

### Pourquoi utiliser des pointeurs multi-niveaux ?

Les pointeurs multi-niveaux sont utiles dans plusieurs situations :

1. **Tableaux dynamiques 2D et 3D** : Matrices, images, volumes de données
2. **Modification de pointeurs** : Fonctions qui modifient un pointeur passé en paramètre
3. **Structures de données complexes** : Listes chaînées de listes, graphes
4. **Tableaux de chaînes** : Comme `argv` dans `main()`
5. **Allocation dynamique de structures imbriquées**

---

## Rappel : Pointeur simple (niveau 1)

Avant d'aller plus loin, révisons rapidement les pointeurs simples :

```c
#include <stdio.h>

int main(void) {
    int valeur = 42;        // Variable normale
    int *ptr = &valeur;     // Pointeur vers valeur

    printf("valeur = %d\n", valeur);           // Affiche : 42
    printf("*ptr = %d\n", *ptr);               // Affiche : 42
    printf("Adresse de valeur : %p\n", (void*)&valeur);
    printf("Contenu de ptr : %p\n", (void*)ptr);

    return 0;
}
```

**Visualisation mémoire** :
```
Mémoire :
┌──────────┬───────────┐
│  valeur  │    ptr    │
│    42    │  [0x1000] │  ← ptr contient l'adresse de valeur
└──────────┴───────────┘
   0x1000     0x2000

ptr pointe vers valeur
```

---

## Pointeur double (niveau 2)

### Concept de base

Un **pointeur double** (ou pointeur de pointeur) est un pointeur qui contient l'adresse d'un autre pointeur.

```c
#include <stdio.h>

int main(void) {
    int valeur = 42;           // Variable normale
    int *ptr = &valeur;        // Pointeur vers valeur
    int **pptr = &ptr;         // Pointeur vers ptr

    printf("Valeur directe : %d\n", valeur);
    printf("Via ptr : %d\n", *ptr);
    printf("Via pptr : %d\n", **pptr);

    // Affichage des adresses
    printf("\nAdresses :\n");
    printf("Adresse de valeur : %p\n", (void*)&valeur);
    printf("Contenu de ptr : %p (pointe vers valeur)\n", (void*)ptr);
    printf("Adresse de ptr : %p\n", (void*)&ptr);
    printf("Contenu de pptr : %p (pointe vers ptr)\n", (void*)pptr);

    return 0;
}
```

**Visualisation mémoire** :
```
Mémoire :
┌──────────┬──────────┬──────────┐
│  valeur  │   ptr    │   pptr   │
│    42    │ [0x1000] │ [0x2000] │
└──────────┴──────────┴──────────┘
   0x1000    0x2000     0x3000

Chaîne de pointage :
pptr → ptr → valeur
      (2x)   (1x)    (0x)
```

**Déréférencement** :
- `pptr` : adresse de `ptr`
- `*pptr` : contenu de `ptr` (qui est l'adresse de `valeur`)
- `**pptr` : contenu de `valeur` (42)

### Syntaxe et déclaration

```c
int valeur = 42;
int *ptr = &valeur;      // Pointeur simple
int **pptr = &ptr;       // Pointeur double
int ***ppptr = &pptr;    // Pointeur triple

// Déclaration multiple
int **p1, **p2, **p3;    // Trois pointeurs doubles

// Avec typedef (plus lisible)
typedef int* IntPtr;
typedef IntPtr* IntPtrPtr;

IntPtrPtr pptr2 = &ptr;  // Équivalent à int **pptr2
```

---

## Cas d'usage 1 : Modifier un pointeur dans une fonction

L'un des usages les plus importants des pointeurs doubles est de permettre à une fonction de modifier un pointeur passé en paramètre.

### Problème avec un pointeur simple

```c
#include <stdio.h>
#include <stdlib.h>

// ❌ INCORRECT : Ne modifie pas le pointeur original
void allouer_incorrect(int *ptr) {
    ptr = malloc(sizeof(int));  // Modifie la copie locale
    if (ptr != NULL) {
        *ptr = 100;
    }
}

int main(void) {
    int *mon_ptr = NULL;

    allouer_incorrect(mon_ptr);

    if (mon_ptr == NULL) {
        printf("mon_ptr est toujours NULL !\n");  // ← Affiche ceci
    }

    return 0;
}
```

**Pourquoi ça ne marche pas ?** En C, les paramètres sont passés **par valeur**. La fonction reçoit une **copie** du pointeur, donc modifier cette copie ne change pas l'original.

### Solution avec un pointeur double

```c
#include <stdio.h>
#include <stdlib.h>

// ✅ CORRECT : Modifie le pointeur original
void allouer_correct(int **ptr) {
    *ptr = malloc(sizeof(int));  // Modifie le pointeur via son adresse
    if (*ptr != NULL) {
        **ptr = 100;
    }
}

int main(void) {
    int *mon_ptr = NULL;

    allouer_correct(&mon_ptr);  // On passe l'adresse du pointeur

    if (mon_ptr != NULL) {
        printf("Allocation réussie ! Valeur : %d\n", *mon_ptr);  // Affiche : 100
        free(mon_ptr);
    }

    return 0;
}
```

**Explication** :
- On passe `&mon_ptr` : l'**adresse** du pointeur
- La fonction reçoit `int **ptr` : un pointeur vers notre pointeur
- `*ptr = malloc(...)` modifie le pointeur original
- `**ptr = 100` modifie la valeur pointée

**Visualisation** :
```
main()                      allouer_correct()
┌──────────┐               ┌──────────┐
│ mon_ptr  │  ←────────────│   ptr    │
│  [0x0]   │  (adresse)    │ [0x5000] │
└──────────┘               └──────────┘
  0x5000

Après malloc :
┌──────────┐
│ mon_ptr  │  pointe vers  ┌──────────┐
│ [0x7000] │ ─────────────→│   100    │ (mémoire allouée)
└──────────┘               └──────────┘
```

---

## Cas d'usage 2 : Tableaux dynamiques 2D

### Création d'une matrice dynamique

```c
#include <stdio.h>
#include <stdlib.h>

// Allocation d'une matrice 2D dynamique
int** creer_matrice(int lignes, int colonnes) {
    // Allocation du tableau de pointeurs (les lignes)
    int **matrice = malloc(lignes * sizeof(int*));
    if (matrice == NULL) {
        return NULL;
    }

    // Allocation de chaque ligne
    for (int i = 0; i < lignes; i++) {
        matrice[i] = malloc(colonnes * sizeof(int));
        if (matrice[i] == NULL) {
            // En cas d'erreur, libérer ce qui a été alloué
            for (int j = 0; j < i; j++) {
                free(matrice[j]);
            }
            free(matrice);
            return NULL;
        }
    }

    return matrice;
}

// Libération de la matrice
void liberer_matrice(int **matrice, int lignes) {
    for (int i = 0; i < lignes; i++) {
        free(matrice[i]);  // Libérer chaque ligne
    }
    free(matrice);  // Libérer le tableau de pointeurs
}

// Initialisation de la matrice
void initialiser_matrice(int **matrice, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            matrice[i][j] = i * colonnes + j;
        }
    }
}

// Affichage de la matrice
void afficher_matrice(int **matrice, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%3d ", matrice[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int lignes = 3, colonnes = 4;

    // Création de la matrice
    int **matrice = creer_matrice(lignes, colonnes);
    if (matrice == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return 1;
    }

    // Utilisation
    initialiser_matrice(matrice, lignes, colonnes);
    printf("Matrice %dx%d :\n", lignes, colonnes);
    afficher_matrice(matrice, lignes, colonnes);

    // Libération
    liberer_matrice(matrice, lignes);

    return 0;
}
```

**Sortie** :
```
Matrice 3x4 :
  0   1   2   3
  4   5   6   7
  8   9  10  11
```

**Visualisation mémoire** :
```
matrice (int**) : Tableau de pointeurs
┌──────┐
│ [P1] │ ─→ [0][1][2][3]  ← Ligne 0
├──────┤
│ [P2] │ ─→ [4][5][6][7]  ← Ligne 1
├──────┤
│ [P3] │ ─→ [8][9][10][11] ← Ligne 2
└──────┘

Accès : matrice[i][j]
        = *(*(matrice + i) + j)
        = *(matrice[i] + j)
```

### Alternative : Allocation contiguë

Pour de meilleures performances (cache-friendly), on peut allouer toute la mémoire d'un bloc :

```c
#include <stdio.h>
#include <stdlib.h>

int** creer_matrice_contigue(int lignes, int colonnes) {
    // Allocation du tableau de pointeurs
    int **matrice = malloc(lignes * sizeof(int*));
    if (matrice == NULL) {
        return NULL;
    }

    // Allocation de toutes les données en un seul bloc
    int *data = malloc(lignes * colonnes * sizeof(int));
    if (data == NULL) {
        free(matrice);
        return NULL;
    }

    // Configuration des pointeurs de lignes
    for (int i = 0; i < lignes; i++) {
        matrice[i] = data + i * colonnes;
    }

    return matrice;
}

void liberer_matrice_contigue(int **matrice) {
    if (matrice != NULL) {
        free(matrice[0]);  // Libère toutes les données
        free(matrice);     // Libère le tableau de pointeurs
    }
}

int main(void) {
    int **mat = creer_matrice_contigue(3, 4);

    // Utilisation normale
    mat[1][2] = 42;
    printf("mat[1][2] = %d\n", mat[1][2]);

    liberer_matrice_contigue(mat);
    return 0;
}
```

**Avantages** :
- ✅ Meilleure localité spatiale (cache-friendly)
- ✅ Un seul appel à `malloc` pour les données
- ✅ Libération simplifiée

---

## Cas d'usage 3 : Tableaux de chaînes de caractères

### Le paramètre argv de main()

Le paramètre `argv` de la fonction `main()` est un pointeur double vers des chaînes de caractères :

```c
#include <stdio.h>

int main(int argc, char **argv) {
    // argv est un tableau de chaînes de caractères
    // Équivalent à : char *argv[]

    printf("Nombre d'arguments : %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    // Accès caractère par caractère
    if (argc > 1) {
        printf("\nPremier caractère du 1er argument : %c\n", argv[1][0]);
        // argv[1][0] = *(*(argv + 1) + 0)
    }

    return 0;
}
```

**Exécution** : `./programme hello world`

**Sortie** :
```
Nombre d'arguments : 3
argv[0] = ./programme
argv[1] = hello
argv[2] = world

Premier caractère du 1er argument : h
```

**Visualisation** :
```
argv (char**) :
┌──────┐
│ [P0] │ ─→ "./programme\0"
├──────┤
│ [P1] │ ─→ "hello\0"
├──────┤
│ [P2] │ ─→ "world\0"
├──────┤
│ NULL │    ← Terminateur
└──────┘

Accès :
argv[1]     → pointeur vers "hello"
argv[1][0]  → 'h'
argv[1][1]  → 'e'
```

### Création d'un tableau de chaînes dynamique

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** creer_tableau_chaines(int nb_chaines) {
    char **tableau = malloc((nb_chaines + 1) * sizeof(char*));
    if (tableau == NULL) {
        return NULL;
    }
    tableau[nb_chaines] = NULL;  // Terminateur
    return tableau;
}

void ajouter_chaine(char **tableau, int index, const char *chaine) {
    tableau[index] = malloc(strlen(chaine) + 1);
    if (tableau[index] != NULL) {
        strcpy(tableau[index], chaine);
    }
}

void liberer_tableau_chaines(char **tableau) {
    if (tableau == NULL) return;

    for (int i = 0; tableau[i] != NULL; i++) {
        free(tableau[i]);
    }
    free(tableau);
}

void afficher_chaines(char **tableau) {
    for (int i = 0; tableau[i] != NULL; i++) {
        printf("%d: %s\n", i, tableau[i]);
    }
}

int main(void) {
    // Création d'un tableau de 3 chaînes
    char **langages = creer_tableau_chaines(3);
    if (langages == NULL) {
        return 1;
    }

    // Ajout de chaînes
    ajouter_chaine(langages, 0, "C");
    ajouter_chaine(langages, 1, "Python");
    ajouter_chaine(langages, 2, "Rust");

    // Affichage
    printf("Langages de programmation :\n");
    afficher_chaines(langages);

    // Libération
    liberer_tableau_chaines(langages);

    return 0;
}
```

---

## Pointeur triple et au-delà

### Concept

Un pointeur triple (`***`) pointe vers un pointeur double, qui pointe vers un pointeur simple, qui pointe vers une valeur.

```c
#include <stdio.h>

int main(void) {
    int valeur = 42;
    int *p1 = &valeur;      // Niveau 1
    int **p2 = &p1;         // Niveau 2
    int ***p3 = &p2;        // Niveau 3

    printf("Valeur via p1 : %d\n", *p1);      // 1 déréférencement
    printf("Valeur via p2 : %d\n", **p2);     // 2 déréférencements
    printf("Valeur via p3 : %d\n", ***p3);    // 3 déréférencements

    // Modification via p3
    ***p3 = 100;
    printf("Nouvelle valeur : %d\n", valeur);  // Affiche : 100

    return 0;
}
```

**Visualisation** :
```
┌────────┬────────┬────────┬────────┐
│ valeur │   p1   │   p2   │   p3   │
│  42    │[0x100] │[0x200] │[0x300] │
└────────┴────────┴────────┴────────┘
  0x100    0x200    0x300    0x400

p3 ──→ p2 ──→ p1 ──→ valeur
(***) (***) (***) (***)
```

### Cas d'usage : Tableau 3D dynamique

```c
#include <stdio.h>
#include <stdlib.h>

// Allocation d'un cube 3D
int*** creer_cube(int profondeur, int lignes, int colonnes) {
    int ***cube = malloc(profondeur * sizeof(int**));
    if (cube == NULL) return NULL;

    for (int i = 0; i < profondeur; i++) {
        cube[i] = malloc(lignes * sizeof(int*));
        if (cube[i] == NULL) {
            // Gestion d'erreur (simplifiée)
            return NULL;
        }

        for (int j = 0; j < lignes; j++) {
            cube[i][j] = malloc(colonnes * sizeof(int));
            if (cube[i][j] == NULL) {
                // Gestion d'erreur (simplifiée)
                return NULL;
            }
        }
    }

    return cube;
}

void liberer_cube(int ***cube, int profondeur, int lignes) {
    for (int i = 0; i < profondeur; i++) {
        for (int j = 0; j < lignes; j++) {
            free(cube[i][j]);
        }
        free(cube[i]);
    }
    free(cube);
}

int main(void) {
    int ***cube = creer_cube(2, 3, 4);  // 2x3x4

    if (cube != NULL) {
        // Utilisation : cube[profondeur][ligne][colonne]
        cube[0][1][2] = 42;
        printf("cube[0][1][2] = %d\n", cube[0][1][2]);

        liberer_cube(cube, 2, 3);
    }

    return 0;
}
```

**Visualisation d'un cube 2x3x4** :
```
cube[0][0] → [0][1][2][3]
cube[0][1] → [0][1][2][3]
cube[0][2] → [0][1][2][3]

cube[1][0] → [0][1][2][3]
cube[1][1] → [0][1][2][3]
cube[1][2] → [0][1][2][3]
```

---

## Arithmétique des pointeurs multi-niveaux

### Avec des pointeurs doubles

```c
#include <stdio.h>

int main(void) {
    int valeurs[] = {10, 20, 30, 40, 50};
    int *ptrs[5];

    // Initialisation : chaque pointeur pointe vers un élément
    for (int i = 0; i < 5; i++) {
        ptrs[i] = &valeurs[i];
    }

    int **pptr = ptrs;  // Pointe vers le début du tableau de pointeurs

    // Parcours avec arithmétique
    for (int i = 0; i < 5; i++) {
        printf("Via pptr[%d] : %d\n", i, *pptr[i]);
        // Équivalent à : **(pptr + i)
    }

    // Incrémentation
    printf("\nAprès pptr++ :\n");
    pptr++;
    printf("Valeur pointée : %d\n", **pptr);  // Affiche : 20

    return 0;
}
```

### Attention aux priorités

```c
int **pptr;

*pptr++    // Incrémente pptr, retourne l'ancienne valeur déréférencée
(*pptr)++  // Incrémente ce que pptr pointe
**pptr++   // Déréférence deux fois, puis incrémente pptr
(*(*pptr)) // Valeur finale
```

---

## Fonctions avec pointeurs multi-niveaux

### Exemple : Tri de chaînes

```c
#include <stdio.h>
#include <string.h>

void trier_chaines(char **chaines, int nb) {
    char *temp;

    for (int i = 0; i < nb - 1; i++) {
        for (int j = i + 1; j < nb; j++) {
            if (strcmp(chaines[i], chaines[j]) > 0) {
                // Échange des pointeurs
                temp = chaines[i];
                chaines[i] = chaines[j];
                chaines[j] = temp;
            }
        }
    }
}

int main(void) {
    char *fruits[] = {
        "banane",
        "pomme",
        "orange",
        "fraise",
        "kiwi"
    };

    int nb_fruits = sizeof(fruits) / sizeof(fruits[0]);

    printf("Avant tri :\n");
    for (int i = 0; i < nb_fruits; i++) {
        printf("  %s\n", fruits[i]);
    }

    trier_chaines(fruits, nb_fruits);

    printf("\nAprès tri :\n");
    for (int i = 0; i < nb_fruits; i++) {
        printf("  %s\n", fruits[i]);
    }

    return 0;
}
```

**Sortie** :
```
Avant tri :
  banane
  pomme
  orange
  fraise
  kiwi

Après tri :
  banane
  fraise
  kiwi
  orange
  pomme
```

---

## Pièges courants et erreurs

### 1. Confusion entre niveaux de déréférencement

```c
int valeur = 42;
int *p1 = &valeur;
int **p2 = &p1;

// ❌ ERREUR : trop de déréférencements
printf("%d\n", ***p2);  // p2 n'est qu'un double pointeur !

// ✅ CORRECT
printf("%d\n", **p2);   // Accède à valeur
```

### 2. Oubli de l'esperluette lors du passage

```c
void modifier_pointeur(int **ptr) {
    *ptr = malloc(sizeof(int));
}

int main(void) {
    int *mon_ptr = NULL;

    // ❌ ERREUR : on passe le pointeur, pas son adresse
    // modifier_pointeur(mon_ptr);

    // ✅ CORRECT : on passe l'adresse du pointeur
    modifier_pointeur(&mon_ptr);

    free(mon_ptr);
    return 0;
}
```

### 3. Libération incorrecte de structures imbriquées

```c
// ❌ ERREUR : on libère d'abord le tableau principal
int **mat = creer_matrice(3, 4);
free(mat);           // Les lignes sont perdues (fuite mémoire) !
// On ne peut plus accéder à mat[i] pour les libérer

// ✅ CORRECT : libérer dans l'ordre inverse de l'allocation
for (int i = 0; i < 3; i++) {
    free(mat[i]);    // D'abord les lignes
}
free(mat);           // Puis le tableau principal
```

### 4. Pointeurs pendants après libération

```c
int **pptr;
int *ptr = malloc(sizeof(int));
*ptr = 42;
pptr = &ptr;

free(ptr);
// ptr est maintenant un pointeur pendant (dangling pointer)
// *pptr pointe toujours vers ptr, qui est invalide

ptr = NULL;    // ✅ Bonne pratique : mettre à NULL après free
```

### 5. Tableau de pointeurs non initialisés

```c
int *tableau[10];  // Contient des valeurs aléatoires !

// ❌ DANGEREUX
*tableau[0] = 42;  // Déréférence une adresse aléatoire

// ✅ CORRECT : initialiser
int *tableau[10] = {NULL};  // Tous à NULL
// Ou
for (int i = 0; i < 10; i++) {
    tableau[i] = NULL;
}
```

---

## Équivalences et syntaxes alternatives

### Tableaux vs pointeurs

```c
// Ces déclarations sont équivalentes pour les paramètres de fonction :
void fonction1(char **argv);
void fonction1(char *argv[]);

// Mais PAS pour les variables locales :
char **argv;     // Pointeur double non initialisé
char *argv[10];  // Tableau de 10 pointeurs

// Pour les matrices :
void traiter1(int **mat);           // Pointeur double
void traiter2(int *mat[]);          // Tableau de pointeurs
void traiter3(int mat[][4]);        // Tableau 2D avec taille fixe colonne
```

### Syntaxes de déréférencement

```c
int **pptr;

// Ces expressions sont équivalentes :
**pptr
*(*pptr)
*(pptr[0])
pptr[0][0]

// Pour pptr[i][j] :
pptr[i][j]
*(*(pptr + i) + j)
*(pptr[i] + j)
(*pptr + i)[j]  // Moins courant mais valide
```

---

## Typedef pour simplifier

Les pointeurs multi-niveaux peuvent rendre le code illisible. Utilisez `typedef` :

```c
// Sans typedef (difficile à lire)
int*** creer_cube(int d, int l, int c);
void traiter(int ***cube);

// Avec typedef (plus clair)
typedef int** Matrice;
typedef Matrice* Cube;

Cube creer_cube(int d, int l, int c);
void traiter(Cube cube);

// Ou pour les chaînes :
typedef char* String;
typedef String* StringArray;

StringArray creer_tableau(int taille);
```

---

## Cas d'usage avancés

### 1. Liste de listes chaînées

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct NodeList {
    Node *head;
    struct NodeList *next;
} NodeList;

// NodeList** est nécessaire pour modifier la tête de liste
void ajouter_liste(NodeList **liste_de_listes, Node *nouvelle_liste) {
    NodeList *nouveau = malloc(sizeof(NodeList));
    if (nouveau == NULL) return;

    nouveau->head = nouvelle_liste;
    nouveau->next = *liste_de_listes;
    *liste_de_listes = nouveau;
}
```

### 2. Table de hachage avec chaînage

```c
typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

// Tableau de listes chaînées
Entry **table = malloc(TABLE_SIZE * sizeof(Entry*));
for (int i = 0; i < TABLE_SIZE; i++) {
    table[i] = NULL;  // Initialisation
}

// Accès : table[hash] pointe vers une liste chaînée
```

### 3. Graphe représenté par liste d'adjacence

```c
typedef struct Node {
    int vertex;
    struct Node *next;
} Node;

// Graphe : tableau de listes d'adjacence
Node **graph = malloc(num_vertices * sizeof(Node*));

// graph[i] pointe vers la liste des voisins du sommet i
```

---

## Debugging des pointeurs multi-niveaux

### Avec GDB

```bash
(gdb) print pptr
$1 = (int **) 0x555555756260

(gdb) print *pptr
$2 = (int *) 0x555555756280

(gdb) print **pptr
$3 = 42

# Afficher la structure
(gdb) print *pptr@5  # Affiche 5 éléments du tableau pointé
```

### Avec printf

```c
int **pptr;

printf("pptr = %p\n", (void*)pptr);           // Adresse du pointeur double
printf("*pptr = %p\n", (void*)*pptr);         // Adresse du pointeur simple
printf("**pptr = %d\n", **pptr);              // Valeur finale

// Pour les tableaux 2D
for (int i = 0; i < lignes; i++) {
    printf("mat[%d] = %p\n", i, (void*)mat[i]);
}
```

---

## Bonnes pratiques

### ✅ À FAIRE

1. **Documenter les niveaux de pointage**
```c
/**
 * @param matrix Pointeur vers un tableau de pointeurs vers des lignes
 * @param rows Nombre de lignes
 * @param cols Nombre de colonnes
 */
void process_matrix(int **matrix, int rows, int cols);
```

2. **Toujours initialiser à NULL**
```c
int **pptr = NULL;
char **argv = NULL;
```

3. **Vérifier avant déréférencement**
```c
if (pptr != NULL && *pptr != NULL) {
    printf("%d\n", **pptr);
}
```

4. **Libérer dans l'ordre inverse**
```c
// Allocation : tableau → lignes
// Libération : lignes → tableau
for (int i = 0; i < n; i++) {
    free(tab[i]);
}
free(tab);
```

5. **Utiliser typedef pour la clarté**
```c
typedef int** Matrix;
Matrix mat = create_matrix(3, 4);
```

### ❌ À ÉVITER

1. **Trop de niveaux**
```c
// ❌ Difficile à maintenir
int *****ptr;  // Très rarement nécessaire !

// ✅ Réévaluer la conception
```

2. **Mélanger types de données**
```c
// ❌ Dangereux
int **pptr = (int**)&double_value;
```

3. **Oublier la gestion d'erreurs**
```c
// ❌ Pas de vérification
int **mat = malloc(n * sizeof(int*));
mat[0] = malloc(m * sizeof(int));  // Et si malloc échoue ?
```

---

## Alternatives et considérations

### Quand éviter les pointeurs multi-niveaux

1. **Matrices petites et fixes** : Utilisez des tableaux statiques
```c
// Au lieu de int **
int matrice[10][10];  // Plus simple et plus rapide
```

2. **Tableaux 1D indexés** : Simulez la 2D
```c
// Au lieu de mat[i][j] avec **
int *mat = malloc(lignes * colonnes * sizeof(int));
// Accès : mat[i * colonnes + j]
```

3. **Structures avec dimensions**
```c
typedef struct {
    int *data;
    int lignes;
    int colonnes;
} Matrice;

int get(Matrice *m, int i, int j) {
    return m->data[i * m->colonnes + j];
}
```

### Considérations de performance

- ➕ **Pointeurs multi-niveaux** : Flexibilité, redimensionnement
- ➖ **Pointeurs multi-niveaux** : Indirection multiple, mauvaise localité cache
- ➕ **Allocation contiguë** : Meilleure performance cache
- ➖ **Allocation contiguë** : Moins flexible

---

## Comparaison avec d'autres langages

| Concept | C | Python | Java |
|---------|---|--------|------|
| Pointeur double | `int **p` | Pas de notion directe | Pas de pointeurs |
| Tableau 2D | `int **mat` ou `int mat[][]` | `list of lists` | `int[][] mat` |
| argv | `char **argv` | `sys.argv` (liste) | `String[] args` |
| Gestion mémoire | Manuelle | Automatique (GC) | Automatique (GC) |

---

## Résumé

Les pointeurs multi-niveaux sont un outil puissant mais qui demande de la rigueur :

**Niveaux courants** :
- `int *` : Pointeur simple vers un entier
- `int **` : Pointeur vers un pointeur d'entier (tableaux 2D, argv, modification de pointeurs)
- `int ***` : Pointeur triple (tableaux 3D, structures complexes)

**Usages principaux** :
- ✅ Matrices et tableaux dynamiques multi-dimensionnels
- ✅ Modification de pointeurs dans des fonctions
- ✅ Tableaux de chaînes de caractères
- ✅ Structures de données complexes (graphes, listes de listes)

**Points clés** :
- Chaque `*` ajoute un niveau d'indirection
- Toujours vérifier NULL à chaque niveau
- Libérer la mémoire dans l'ordre inverse de l'allocation
- Documenter clairement les niveaux de pointage
- Privilégier la simplicité quand possible

**Quand utiliser** :
- Tableaux dynamiques multi-dimensionnels
- Fonctions devant modifier un pointeur
- Arguments de ligne de commande
- Structures de données nécessitant plusieurs niveaux

**Quand éviter** :
- Plus de 3 niveaux (revoir la conception)
- Alternatives plus simples disponibles
- Performance critique (préférer allocation contiguë)

---


⏭️ [Pointeurs opaques](/22-pointeurs-avances/04-pointeurs-opaques.md)
