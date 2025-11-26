🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 8.2 Tableaux multidimensionnels

## Introduction

Jusqu'à présent, nous avons travaillé avec des tableaux unidimensionnels : une simple liste d'éléments. Mais que faire si vous voulez représenter une grille, une matrice mathématique, ou un plateau de jeu ? C'est là qu'interviennent les **tableaux multidimensionnels**.

**Ce que vous devez maîtriser avant cette section :**
- Les tableaux unidimensionnels
- La relation entre tableaux et pointeurs (section 8.1)
- L'arithmétique des pointeurs

## Qu'est-ce qu'un tableau multidimensionnel ?

Un tableau multidimensionnel est simplement un **tableau de tableaux**. Le cas le plus courant est le tableau bidimensionnel (2D), mais on peut aller jusqu'à 3D, 4D et plus.

**Exemples d'utilisation :**
- Matrice mathématique (lignes × colonnes)
- Grille de pixels d'une image
- Plateau de jeu (échecs, morpion)
- Données tabulaires (tableau de scores par équipe et par match)

## Tableaux bidimensionnels (2D)

### Déclaration et initialisation

```c
int matrice[3][4];  // 3 lignes, 4 colonnes
```

Cette déclaration crée un tableau de **3 tableaux de 4 entiers chacun**.

**Visualisation conceptuelle :**

```
        Colonne 0  Colonne 1  Colonne 2  Colonne 3
         ↓          ↓          ↓          ↓
Ligne 0 [ ?  ]    [ ?  ]    [ ?  ]    [ ?  ]
Ligne 1 [ ?  ]    [ ?  ]    [ ?  ]    [ ?  ]
Ligne 2 [ ?  ]    [ ?  ]    [ ?  ]    [ ?  ]
```

### Initialisation avec valeurs

```c
int matrice[3][4] = {
    {1,  2,  3,  4},   // Ligne 0
    {5,  6,  7,  8},   // Ligne 1
    {9, 10, 11, 12}    // Ligne 2
};
```

**Initialisation sans spécifier toutes les valeurs :**

```c
int matrice[3][4] = {
    {1, 2},        // Ligne 0 : {1, 2, 0, 0}
    {5},           // Ligne 1 : {5, 0, 0, 0}
    {9, 10, 11}    // Ligne 2 : {9, 10, 11, 0}
};
// Les valeurs non spécifiées sont initialisées à 0
```

**Initialisation plate (moins recommandée mais possible) :**

```c
int matrice[3][4] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
// Le compilateur remplit ligne par ligne
```

### Représentation en mémoire

**Point crucial :** Les tableaux multidimensionnels sont stockés en mémoire de manière **contiguë**, ligne par ligne (row-major order en anglais).

```c
int matrice[3][4] = {
    {1,  2,  3,  4},
    {5,  6,  7,  8},
    {9, 10, 11, 12}
};
```

**En mémoire :**

```
Adresse : 0x1000  0x1004  0x1008  0x100C  0x1010  0x1014  0x1018  0x101C  0x1020  0x1024  0x1028  0x102C
         ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
         │   1   │   2   │   3   │   4   │   5   │   6   │   7   │   8   │   9   │  10   │  11   │  12   │
         └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
         └─────── Ligne 0 ──────────────┘ └────── Ligne 1 ──────────────┘ └────── Ligne 2 ───────────────┘
```

C'est un **bloc mémoire continu** de 12 entiers (3 × 4 × 4 octets = 48 octets total).

### Accès aux éléments

**Syntaxe standard :**

```c
int matrice[3][4] = {
    {1,  2,  3,  4},
    {5,  6,  7,  8},
    {9, 10, 11, 12}
};

int valeur = matrice[1][2];  // Ligne 1, Colonne 2 → 7
```

**Comment lire `matrice[i][j]` :**
1. `matrice[i]` : accède à la ligne `i` (qui est elle-même un tableau)
2. `[j]` : accède à l'élément `j` de cette ligne

**Exemple de modification :**

```c
matrice[0][0] = 100;  // Change la première case
matrice[2][3] = 200;  // Change la dernière case
```

### Parcourir un tableau 2D

**Méthode classique avec boucles imbriquées :**

```c
int matrice[3][4] = {
    {1,  2,  3,  4},
    {5,  6,  7,  8},
    {9, 10, 11, 12}
};

// Parcours ligne par ligne
for (int i = 0; i < 3; i++) {           // Pour chaque ligne
    for (int j = 0; j < 4; j++) {       // Pour chaque colonne
        printf("%3d ", matrice[i][j]);
    }
    printf("\n");  // Retour à la ligne après chaque ligne
}
```

**Sortie :**
```
  1   2   3   4
  5   6   7   8
  9  10  11  12
```

**Parcours colonne par colonne :**

```c
for (int j = 0; j < 4; j++) {           // Pour chaque colonne
    for (int i = 0; i < 3; i++) {       // Pour chaque ligne
        printf("%3d ", matrice[i][j]);
    }
    printf("\n");
}
```

**Sortie :**
```
  1   5   9
  2   6  10
  3   7  11
  4   8  12
```

## Tableaux 2D et pointeurs

### Structure du tableau 2D

Un tableau 2D est un **tableau de tableaux** :

```c
int matrice[3][4];
```

- `matrice` est un tableau de **3 éléments**
- Chaque élément est lui-même un **tableau de 4 entiers**
- `matrice[0]`, `matrice[1]`, `matrice[2]` sont des pointeurs vers chaque ligne

**Décomposition :**

```c
matrice       // Type : int (*)[4]  → pointeur vers un tableau de 4 int
matrice[0]    // Type : int*         → pointeur vers le premier int de la ligne 0
matrice[0][0] // Type : int          → le premier entier
```

### Équivalences avec pointeurs

```c
int matrice[3][4];

// Ces expressions sont équivalentes :
matrice[i][j]
*(matrice[i] + j)        // matrice[i] est un pointeur, on avance de j positions
*(*(matrice + i) + j)    // Version complète avec double déréférencement
```

**Décomposition pas à pas :**

```c
matrice[1][2]
// Étape 1 : matrice[1] devient *(matrice + 1)
*(matrice + 1)[2]
// Étape 2 : [2] devient *(... + 2)
*(*(matrice + 1) + 2)
```

### Visualisation des niveaux de déréférencement

```c
int matrice[3][4] = {
    {1,  2,  3,  4},
    {5,  6,  7,  8},
    {9, 10, 11, 12}
};

printf("%p\n", matrice);        // Adresse de la première ligne
printf("%p\n", matrice[0]);     // Adresse du premier élément de la ligne 0 (même valeur)
printf("%p\n", &matrice[0][0]); // Adresse du premier élément (même valeur encore)

printf("%p\n", matrice + 1);    // Adresse de la ligne 1 (avance de 16 octets)
printf("%p\n", matrice[1]);     // Adresse du premier élément de la ligne 1 (même valeur)

printf("%d\n", **matrice);      // Valeur : 1
printf("%d\n", *(*matrice + 1));// Valeur : 2
printf("%d\n", *(*(matrice + 1) + 2)); // Valeur : 7
```

## Passage de tableaux 2D aux fonctions

Le passage de tableaux multidimensionnels aux fonctions est plus subtil que pour les tableaux 1D.

### Méthode 1 : Spécifier toutes les dimensions sauf la première

```c
void afficher_matrice(int mat[][4], int lignes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    afficher_matrice(matrice, 3);
    return 0;
}
```

**Important :** La deuxième dimension `[4]` est **obligatoire**. Le compilateur en a besoin pour calculer les adresses correctement.

**Pourquoi ?** Pour accéder à `mat[i][j]`, le compilateur calcule :
```
adresse = base + (i × 4 + j) × sizeof(int)
                   ↑
            Nombre de colonnes nécessaire
```

### Méthode 2 : Utiliser un pointeur vers un tableau

```c
void afficher_matrice(int (*mat)[4], int lignes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}
```

**Attention à la syntaxe :**
- `int (*mat)[4]` : pointeur vers un tableau de 4 int ✅
- `int *mat[4]` : tableau de 4 pointeurs vers int ❌

Les parenthèses sont **cruciales** !

### Méthode 3 : Passer les dimensions et utiliser un pointeur simple

```c
void afficher_matrice(int *mat, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            // Calcul manuel de l'index linéaire
            printf("%3d ", mat[i * colonnes + j]);
        }
        printf("\n");
    }
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    afficher_matrice(&matrice[0][0], 3, 4);
    // Ou : afficher_matrice((int*)matrice, 3, 4);
    return 0;
}
```

**Avantages :**
- Plus flexible (fonctionne avec n'importe quelle dimension)
- Permet l'allocation dynamique

**Inconvénient :**
- Perd la notation `mat[i][j]`, il faut calculer l'index manuellement

### Méthode 4 : Allocation dynamique (aperçu)

Pour des tableaux de taille variable déterminée à l'exécution :

```c
void afficher_matrice(int **mat, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

// Note : Cette approche nécessite une allocation dynamique spéciale
// que nous verrons en détail dans la section sur l'allocation dynamique
```

## Tableaux à trois dimensions (3D)

Les tableaux 3D étendent le concept à une dimension supplémentaire. Pensez-y comme un **tableau de matrices** ou un **cube de données**.

### Déclaration et initialisation

```c
int cube[2][3][4];  // 2 matrices de 3 lignes × 4 colonnes
```

**Visualisation :**

```
Niveau 0 :               Niveau 1 :
┌────────────────┐      ┌────────────────┐
│  [0][0][0..3]  │      │  [1][0][0..3]  │
│  [0][1][0..3]  │      │  [1][1][0..3]  │
│  [0][2][0..3]  │      │  [1][2][0..3]  │
└────────────────┘      └────────────────┘
```

**Initialisation :**

```c
int cube[2][3][4] = {
    {   // Niveau 0
        {1,  2,  3,  4},    // Ligne 0
        {5,  6,  7,  8},    // Ligne 1
        {9, 10, 11, 12}     // Ligne 2
    },
    {   // Niveau 1
        {13, 14, 15, 16},   // Ligne 0
        {17, 18, 19, 20},   // Ligne 1
        {21, 22, 23, 24}    // Ligne 2
    }
};
```

### Accès aux éléments

```c
int valeur = cube[1][2][3];  // Niveau 1, Ligne 2, Colonne 3 → 24
```

**Lecture de `cube[i][j][k]` :**
1. `cube[i]` : accède au niveau `i` (une matrice 3×4)
2. `[j]` : accède à la ligne `j` de cette matrice (un tableau de 4 int)
3. `[k]` : accède à l'élément `k` de cette ligne

### Parcourir un tableau 3D

```c
for (int i = 0; i < 2; i++) {           // Pour chaque niveau
    printf("Niveau %d :\n", i);
    for (int j = 0; j < 3; j++) {       // Pour chaque ligne
        for (int k = 0; k < 4; k++) {   // Pour chaque colonne
            printf("%3d ", cube[i][j][k]);
        }
        printf("\n");
    }
    printf("\n");
}
```

### Représentation mémoire

Le tableau 3D est toujours stocké de manière contiguë en mémoire :

```c
int cube[2][3][4];
// Taille totale : 2 × 3 × 4 × sizeof(int) = 96 octets
```

**Ordre en mémoire :**
```
[0][0][0], [0][0][1], [0][0][2], [0][0][3],
[0][1][0], [0][1][1], [0][1][2], [0][1][3],
[0][2][0], [0][2][1], [0][2][2], [0][2][3],
[1][0][0], [1][0][1], ... jusqu'à [1][2][3]
```

## Calcul de l'index linéaire

Pour comprendre comment le compilateur accède aux éléments, il est utile de connaître la formule de calcul de l'index linéaire.

### Pour un tableau 2D

```c
int matrice[LIGNES][COLONNES];
```

L'élément `matrice[i][j]` est à l'index linéaire :
```
index = i × COLONNES + j
```

**Exemple :**

```c
int matrice[3][4];  // 3 lignes, 4 colonnes
// matrice[1][2] → index = 1 × 4 + 2 = 6
// C'est le 7ème élément en mémoire (en comptant à partir de 0)
```

### Pour un tableau 3D

```c
int cube[NIVEAUX][LIGNES][COLONNES];
```

L'élément `cube[i][j][k]` est à l'index :
```
index = (i × LIGNES + j) × COLONNES + k
```

**Exemple :**

```c
int cube[2][3][4];  // 2 niveaux, 3 lignes, 4 colonnes
// cube[1][2][3] → index = (1 × 3 + 2) × 4 + 3 = 5 × 4 + 3 = 23
```

## Tableaux de pointeurs (structure différente)

**Attention :** Un **tableau de pointeurs** est différent d'un tableau multidimensionnel !

### Tableau 2D classique vs tableau de pointeurs

```c
// Tableau 2D classique (bloc mémoire contigu)
int matrice[3][4];

// Tableau de pointeurs (3 pointeurs vers des tableaux séparés)
int *lignes[3];
```

**Visualisation de `int *lignes[3]` :**

```
lignes[0] ──→ [10][20][30][40]
lignes[1] ──→ [50][60][70][80]
lignes[2] ──→ [90][100][110][120]
```

Chaque ligne peut être allouée séparément et être de taille différente (tableaux dentelés ou "jagged arrays").

**Exemple d'utilisation :**

```c
int ligne0[] = {1, 2, 3, 4};
int ligne1[] = {5, 6, 7, 8};
int ligne2[] = {9, 10, 11, 12};

int *lignes[3] = {ligne0, ligne1, ligne2};

// Accès similaire mais structure différente
printf("%d\n", lignes[1][2]);  // 7
```

**Différence cruciale :**
- `int matrice[3][4]` : 48 octets contigus
- `int *lignes[3]` : 3 pointeurs (24 octets) + tableaux séparés ailleurs en mémoire

## Exemples pratiques

### Exemple 1 : Somme des éléments d'une matrice

```c
#include <stdio.h>

int somme_matrice(int mat[][4], int lignes) {
    int somme = 0;
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            somme += mat[i][j];
        }
    }
    return somme;
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    int total = somme_matrice(matrice, 3);
    printf("Somme totale : %d\n", total);  // 78

    return 0;
}
```

### Exemple 2 : Transposition d'une matrice

```c
#include <stdio.h>

void transposer(int mat[][4], int resultat[][3], int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            resultat[j][i] = mat[i][j];
        }
    }
}

void afficher_matrice_3x4(int mat[][4], int lignes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

void afficher_matrice_4x3(int mat[][3], int lignes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    int transposee[4][3];

    printf("Matrice originale (3×4) :\n");
    afficher_matrice_3x4(matrice, 3);

    transposer(matrice, transposee, 3, 4);

    printf("\nMatrice transposée (4×3) :\n");
    afficher_matrice_4x3(transposee, 4);

    return 0;
}
```

**Sortie :**
```
Matrice originale (3×4) :
  1   2   3   4
  5   6   7   8
  9  10  11  12

Matrice transposée (4×3) :
  1   5   9
  2   6  10
  3   7  11
  4   8  12
```

### Exemple 3 : Recherche dans une matrice

```c
#include <stdio.h>
#include <stdbool.h>

bool rechercher(int mat[][4], int lignes, int colonnes, int valeur,
                int *ligne_trouvee, int *col_trouvee) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            if (mat[i][j] == valeur) {
                *ligne_trouvee = i;
                *col_trouvee = j;
                return true;
            }
        }
    }
    return false;
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    int ligne, colonne;
    int cherche = 7;

    if (rechercher(matrice, 3, 4, cherche, &ligne, &colonne)) {
        printf("%d trouvé à la position [%d][%d]\n", cherche, ligne, colonne);
    } else {
        printf("%d non trouvé\n", cherche);
    }

    return 0;
}
```

### Exemple 4 : Matrice d'identité

```c
#include <stdio.h>

void creer_identite(int mat[][5], int taille) {
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            mat[i][j] = (i == j) ? 1 : 0;
        }
    }
}

void afficher_matrice(int mat[][5], int taille) {
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int identite[5][5];

    creer_identite(identite, 5);

    printf("Matrice d'identité 5×5 :\n");
    afficher_matrice(identite, 5);

    return 0;
}
```

**Sortie :**
```
Matrice d'identité 5×5 :
1 0 0 0 0
0 1 0 0 0
0 0 1 0 0
0 0 0 1 0
0 0 0 0 1
```

## Tableaux de taille variable (VLA - C99)

Depuis C99, vous pouvez déclarer des tableaux avec des dimensions déterminées à l'exécution (Variable Length Arrays).

```c
void afficher_matrice_vla(int lignes, int colonnes, int mat[lignes][colonnes]) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int n, m;
    printf("Nombre de lignes : ");
    scanf("%d", &n);
    printf("Nombre de colonnes : ");
    scanf("%d", &m);

    int matrice[n][m];  // VLA : taille déterminée à l'exécution

    // Remplissage
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrice[i][j] = i * m + j + 1;
        }
    }

    afficher_matrice_vla(n, m, matrice);

    return 0;
}
```

**Attention :**
- Les VLA sont alloués sur la pile (stack)
- Attention à la taille : risque de débordement de pile
- Optionnel en C11, support variable selon les compilateurs

## Pièges courants et bonnes pratiques

### ❌ Erreur 1 : Oublier une dimension lors du passage en fonction

```c
void fonction(int mat[][], int lignes) {  // ❌ ERREUR DE COMPILATION
    // ...
}

void fonction(int mat[][4], int lignes) {  // ✅ CORRECT
    // ...
}
```

### ❌ Erreur 2 : Confusion pointeur vers tableau vs tableau de pointeurs

```c
int (*ptr)[4];   // ✅ Pointeur vers un tableau de 4 int
int *ptr[4];     // ✅ Tableau de 4 pointeurs vers int
```

Les parenthèses changent tout !

### ❌ Erreur 3 : Accès hors limites

```c
int matrice[3][4];
matrice[3][0] = 10;  // ❌ Ligne 3 n'existe pas (indices 0-2 seulement)
matrice[0][4] = 10;  // ❌ Colonne 4 n'existe pas (indices 0-3 seulement)
```

### ❌ Erreur 4 : Retourner un tableau local

```c
int (*creer_matrice(void))[4] {
    int mat[3][4] = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}};
    return mat;  // ❌ DANGEREUX : mat sera détruit après le retour
}
```

### ✅ Bonne pratique 1 : Utiliser des constantes pour les dimensions

```c
#define LIGNES 3
#define COLONNES 4

int matrice[LIGNES][COLONNES];

void afficher(int mat[][COLONNES], int lignes) {
    // ...
}
```

### ✅ Bonne pratique 2 : Passer les dimensions en paramètres

```c
void traiter_matrice(int *mat, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%d ", mat[i * colonnes + j]);
        }
        printf("\n");
    }
}
```

### ✅ Bonne pratique 3 : Vérifier les limites

```c
bool est_dans_limites(int ligne, int colonne, int max_lignes, int max_colonnes) {
    return (ligne >= 0 && ligne < max_lignes &&
            colonne >= 0 && colonne < max_colonnes);
}

void modifier(int mat[][4], int i, int j, int valeur) {
    if (est_dans_limites(i, j, 3, 4)) {
        mat[i][j] = valeur;
    } else {
        fprintf(stderr, "Erreur : indices hors limites\n");
    }
}
```

### ✅ Bonne pratique 4 : Documenter les dimensions

```c
/**
 * Calcule la moyenne des éléments d'une matrice
 * @param mat    Matrice d'entiers (lignes × 4 colonnes)
 * @param lignes Nombre de lignes de la matrice
 * @return       Moyenne des éléments
 */
double moyenne_matrice(int mat[][4], int lignes) {
    int somme = 0;
    int total = lignes * 4;

    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            somme += mat[i][j];
        }
    }

    return (double)somme / total;
}
```

## Exemple complet : Jeu du morpion

```c
#include <stdio.h>
#include <stdbool.h>

#define TAILLE 3

void initialiser_plateau(char plateau[][TAILLE]) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            plateau[i][j] = ' ';
        }
    }
}

void afficher_plateau(char plateau[][TAILLE]) {
    printf("\n");
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            printf(" %c ", plateau[i][j]);
            if (j < TAILLE - 1) printf("|");
        }
        printf("\n");
        if (i < TAILLE - 1) printf("---+---+---\n");
    }
    printf("\n");
}

bool verifier_victoire(char plateau[][TAILLE], char joueur) {
    // Vérifier les lignes
    for (int i = 0; i < TAILLE; i++) {
        if (plateau[i][0] == joueur &&
            plateau[i][1] == joueur &&
            plateau[i][2] == joueur) {
            return true;
        }
    }

    // Vérifier les colonnes
    for (int j = 0; j < TAILLE; j++) {
        if (plateau[0][j] == joueur &&
            plateau[1][j] == joueur &&
            plateau[2][j] == joueur) {
            return true;
        }
    }

    // Vérifier les diagonales
    if (plateau[0][0] == joueur &&
        plateau[1][1] == joueur &&
        plateau[2][2] == joueur) {
        return true;
    }

    if (plateau[0][2] == joueur &&
        plateau[1][1] == joueur &&
        plateau[2][0] == joueur) {
        return true;
    }

    return false;
}

bool plateau_plein(char plateau[][TAILLE]) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (plateau[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

int main(void) {
    char plateau[TAILLE][TAILLE];
    char joueur_actuel = 'X';
    int ligne, colonne;

    initialiser_plateau(plateau);

    printf("=== JEU DU MORPION ===\n");
    printf("Positions : ligne (0-2) et colonne (0-2)\n");

    while (true) {
        afficher_plateau(plateau);

        printf("Joueur %c, entrez ligne et colonne : ", joueur_actuel);
        scanf("%d %d", &ligne, &colonne);

        // Vérifier la validité du coup
        if (ligne < 0 || ligne >= TAILLE ||
            colonne < 0 || colonne >= TAILLE) {
            printf("Position invalide !\n");
            continue;
        }

        if (plateau[ligne][colonne] != ' ') {
            printf("Case déjà occupée !\n");
            continue;
        }

        // Jouer le coup
        plateau[ligne][colonne] = joueur_actuel;

        // Vérifier la victoire
        if (verifier_victoire(plateau, joueur_actuel)) {
            afficher_plateau(plateau);
            printf("Le joueur %c a gagné !\n", joueur_actuel);
            break;
        }

        // Vérifier le match nul
        if (plateau_plein(plateau)) {
            afficher_plateau(plateau);
            printf("Match nul !\n");
            break;
        }

        // Changer de joueur
        joueur_actuel = (joueur_actuel == 'X') ? 'O' : 'X';
    }

    return 0;
}
```

## Résumé des points clés

1. **Un tableau multidimensionnel est un tableau de tableaux**
   - `int mat[3][4]` : 3 tableaux de 4 entiers

2. **Stockage contigu en mémoire (row-major order)**
   - Les éléments sont stockés ligne par ligne

3. **Notations équivalentes pour l'accès**
   - `mat[i][j]` ≡ `*(mat[i] + j)` ≡ `*(*(mat + i) + j)`

4. **Passage aux fonctions : dimensions nécessaires**
   - Toutes les dimensions sauf la première doivent être spécifiées
   - `void fonction(int mat[][4], int lignes)`

5. **Calcul d'index linéaire**
   - 2D : `index = i × COLONNES + j`
   - 3D : `index = (i × LIGNES + j) × COLONNES + k`

6. **Tableau 2D ≠ Tableau de pointeurs**
   - `int mat[3][4]` : bloc contigu
   - `int *lignes[3]` : 3 pointeurs séparés

7. **VLA (C99) : dimensions déterminées à l'exécution**
   - Pratique mais attention à la taille de la pile

8. **Toujours vérifier les limites des indices**
   - Prévenir les accès hors limites

## Pour aller plus loin

Dans les sections suivantes, nous verrons :
- **Section 8.3** : Les chaînes de caractères (cas particulier des tableaux)
- **Section 8.4** : Manipulation avancée de chaînes
- **Section 8.5** : Sécurité et buffer overflows
- **Chapitre 9** : Allocation dynamique de tableaux multidimensionnels

La maîtrise des tableaux multidimensionnels est essentielle pour travailler avec des structures de données complexes et manipuler efficacement de grandes quantités de données organisées !

---

**Note :** Les tableaux multidimensionnels peuvent sembler complexes au début. N'hésitez pas à dessiner des schémas sur papier pour visualiser la structure en mémoire et comprendre comment les indices correspondent aux positions réelles en mémoire.

⏭️ [Les chaînes en C](/08-tableaux-et-chaines/03-chaines-en-c.md)
