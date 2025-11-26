🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 8.1 Relation tableaux et pointeurs

## Introduction

La relation entre les tableaux et les pointeurs est l'un des concepts les plus fondamentaux — et souvent les plus déroutants — du langage C. Comprendre cette relation est essentiel pour maîtriser la manipulation de données en C et éviter de nombreuses erreurs courantes.

**Ce que vous devez savoir avant de commencer :**
- Les bases des pointeurs (déclaration, déréférencement)
- Le concept d'adresse mémoire
- L'arithmétique simple des pointeurs

## Déclaration et initialisation d'un tableau

Commençons par la base : qu'est-ce qu'un tableau en C ?

```c
int nombres[5];  // Déclare un tableau de 5 entiers
```

Cette déclaration réserve un bloc contigu de mémoire capable de stocker 5 entiers. Si un `int` occupe 4 octets, ce tableau occupera 20 octets consécutifs en mémoire.

```c
int nombres[5] = {10, 20, 30, 40, 50};
```

En mémoire, cela ressemble à :

```
Adresse :    0x1000   0x1004   0x1008   0x100C   0x1010
            ┌────────┬────────┬────────┬────────┬────────┐
nombres --> │   10   │   20   │   30   │   40   │   50   │
            └────────┴────────┴────────┴────────┴────────┘
Index :        [0]      [1]      [2]      [3]      [4]
```

## Le nom du tableau est une adresse

**Règle fondamentale :** En C, le nom d'un tableau (sans les crochets) est **équivalent à l'adresse de son premier élément**.

```c
int nombres[5] = {10, 20, 30, 40, 50};

// Ces trois expressions sont IDENTIQUES :
printf("%p\n", nombres);        // Adresse du tableau
printf("%p\n", &nombres[0]);    // Adresse du premier élément
printf("%p\n", &nombres);       // Adresse du tableau lui-même (subtilité à voir plus tard)
```

**Important :** `nombres` et `&nombres[0]` donnent exactement la même adresse.

## Accès aux éléments : notation tableau vs pointeur

Vous connaissez déjà l'accès classique par index :

```c
int valeur = nombres[2];  // Accède au 3ème élément (30)
```

Mais vous pouvez aussi utiliser la notation pointeur :

```c
int valeur = *(nombres + 2);  // Exactement équivalent à nombres[2]
```

**Comment ça fonctionne ?**

1. `nombres` est l'adresse du premier élément
2. `nombres + 2` calcule l'adresse du 3ème élément (arithmétique de pointeurs)
3. `*(nombres + 2)` déréférence cette adresse pour obtenir la valeur

### Équivalences importantes

```c
nombres[i]  ≡  *(nombres + i)
&nombres[i] ≡  nombres + i
```

Ces équivalences sont **toujours vraies** et le compilateur les traite de manière identique.

## Exemple concret : parcourir un tableau

### Méthode classique (avec index)

```c
int nombres[5] = {10, 20, 30, 40, 50};

for (int i = 0; i < 5; i++) {
    printf("%d ", nombres[i]);
}
```

### Méthode avec pointeur

```c
int nombres[5] = {10, 20, 30, 40, 50};
int *ptr = nombres;  // ptr pointe sur le premier élément

for (int i = 0; i < 5; i++) {
    printf("%d ", *(ptr + i));
}
```

### Méthode avec pointeur itératif

```c
int nombres[5] = {10, 20, 30, 40, 50};
int *ptr = nombres;

for (int i = 0; i < 5; i++) {
    printf("%d ", *ptr);
    ptr++;  // Passe à l'élément suivant
}
```

**Note :** Après cette boucle, `ptr` ne pointe plus sur le début du tableau !

## Passage de tableaux aux fonctions

Lorsque vous passez un tableau à une fonction, **vous passez en réalité un pointeur** vers le premier élément.

```c
void afficher_tableau(int tab[], int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tab[i]);
    }
}

int main() {
    int nombres[5] = {10, 20, 30, 40, 50};
    afficher_tableau(nombres, 5);  // Passe l'adresse du premier élément
}
```

**Ces déclarations sont équivalentes :**

```c
void afficher_tableau(int tab[], int taille)
void afficher_tableau(int *tab, int taille)
```

Le compilateur traite `int tab[]` comme `int *tab` dans les paramètres de fonction.

### Conséquence importante : la taille n'est pas préservée

```c
void fonction(int tab[]) {
    int taille = sizeof(tab) / sizeof(tab[0]);  // ❌ ERREUR !
    // sizeof(tab) renvoie sizeof(int*), PAS la taille du tableau
}
```

**C'est pourquoi on passe toujours la taille du tableau séparément !**

```c
void fonction(int tab[], int taille) {
    // Maintenant on connaît la taille
}
```

## Différences subtiles entre tableaux et pointeurs

Bien que les tableaux se comportent souvent comme des pointeurs, **ils ne sont pas identiques**.

### Différence 1 : sizeof

```c
int nombres[5];
int *ptr = nombres;

printf("%zu\n", sizeof(nombres));  // 20 (5 × 4 octets)
printf("%zu\n", sizeof(ptr));      // 8 (taille d'un pointeur sur système 64 bits)
```

### Différence 2 : Adresse de l'opérateur &

```c
int nombres[5];

// Ces expressions donnent la MÊME valeur numérique mais des types différents :
nombres      // Type : int*
&nombres[0]  // Type : int*
&nombres     // Type : int(*)[5]  (pointeur vers tableau de 5 int)
```

`&nombres` est un pointeur vers le tableau entier, pas vers un élément. Cela a des implications pour l'arithmétique des pointeurs :

```c
int nombres[5];

nombres + 1     // Avance de 4 octets (un int)
(&nombres) + 1  // Avance de 20 octets (un tableau entier de 5 int) ⚠️
```

### Différence 3 : Modification

```c
int nombres[5];
int *ptr = nombres;

ptr = ptr + 1;  // ✅ OK : ptr est une variable, on peut la modifier
nombres = nombres + 1;  // ❌ ERREUR : nombres est une constante
```

**Le nom d'un tableau est une constante**, vous ne pouvez pas le réassigner.

## La "décomposition" (array decay)

Lorsqu'un tableau est utilisé dans une expression, il est automatiquement converti en pointeur vers son premier élément. C'est ce qu'on appelle le **array decay** (décomposition de tableau).

**Exceptions :** La décomposition ne se produit PAS avec :
- L'opérateur `sizeof()`
- L'opérateur `&` (adresse-de)
- L'initialisation d'un tableau de caractères avec une chaîne littérale

```c
int nombres[5] = {1, 2, 3, 4, 5};

// Décomposition automatique :
int *ptr = nombres;  // nombres devient un pointeur

// Pas de décomposition :
size_t taille = sizeof(nombres);  // Renvoie 20, pas 8
```

## Arithmétique des pointeurs sur les tableaux

L'arithmétique des pointeurs prend tout son sens avec les tableaux :

```c
int nombres[5] = {10, 20, 30, 40, 50};
int *ptr = nombres;

printf("%d\n", *ptr);       // 10
printf("%d\n", *(ptr + 1)); // 20
printf("%d\n", *(ptr + 2)); // 30

ptr = ptr + 3;
printf("%d\n", *ptr);       // 40
```

### Soustraction de pointeurs

Vous pouvez soustraire deux pointeurs pour obtenir le nombre d'éléments qui les séparent :

```c
int nombres[5] = {10, 20, 30, 40, 50};
int *debut = &nombres[0];
int *fin = &nombres[4];

ptrdiff_t distance = fin - debut;  // Résultat : 4
printf("Distance : %td éléments\n", distance);
```

**Note :** `ptrdiff_t` est le type approprié pour stocker la différence entre deux pointeurs.

## Tableaux multidimensionnels et pointeurs

Un tableau 2D est en réalité un tableau de tableaux :

```c
int matrice[3][4] = {
    {1,  2,  3,  4},
    {5,  6,  7,  8},
    {9, 10, 11, 12}
};
```

- `matrice` est un tableau de 3 éléments
- Chaque élément est un tableau de 4 entiers
- `matrice[0]` est un pointeur vers le premier sous-tableau
- `matrice[0][0]` est le premier entier

```c
int *ptr = matrice[0];  // Pointe sur le premier élément du premier sous-tableau

// Ces expressions sont équivalentes :
matrice[1][2]
*(matrice[1] + 2)
*(*(matrice + 1) + 2)
```

**Visualisation mémoire :**

```
matrice --> [1][2][3][4][5][6][7][8][9][10][11][12]
            └───matrice[0]────┘
                              └───matrice[1]────┘
                                                └────matrice[2]────┘
```

## Pointeurs vs tableaux : tableau récapitulatif

| Aspect | Tableau | Pointeur |
|--------|---------|----------|
| **Déclaration** | `int tab[10];` | `int *ptr;` |
| **Mémoire** | Alloue un bloc contigu | Stocke une adresse |
| **sizeof()** | Taille totale du tableau | Taille du pointeur (8 octets) |
| **Modification** | Constante, non modifiable | Variable, peut être modifié |
| **Initialisation** | `int tab[] = {1,2,3};` | `int *ptr = tab;` |
| **Arithmétique** | Possible avec décomposition | Naturelle |
| **Passage en fonction** | Se transforme en pointeur | Reste un pointeur |

## Conseils et pièges à éviter

### ✅ Bonnes pratiques

1. **Utilisez `const` pour les tableaux en lecture seule**
   ```c
   void afficher(const int tab[], int taille) {
       // tab ne peut pas être modifié
   }
   ```

2. **Passez toujours la taille avec le tableau**
   ```c
   void traiter(int tab[], size_t taille) { /* ... */ }
   ```

3. **Préférez la notation tableau pour la clarté**
   ```c
   tab[i]  // Plus lisible que *(tab + i)
   ```

### ❌ Erreurs courantes

1. **Ne pas confondre l'adresse et la valeur**
   ```c
   int tab[5] = {1, 2, 3, 4, 5};
   printf("%d", tab);    // ❌ Affiche une adresse, pas une valeur
   printf("%d", tab[0]); // ✅ Affiche la première valeur
   ```

2. **Oublier que sizeof() sur un paramètre tableau renvoie la taille du pointeur**
   ```c
   void fonction(int tab[]) {
       int n = sizeof(tab) / sizeof(tab[0]);  // ❌ FAUX !
   }
   ```

3. **Modifier le pointeur d'un tableau passé en paramètre**
   ```c
   void fonction(int tab[]) {
       tab++;  // ⚠️ Modifie la copie locale, pas le tableau original
   }
   ```

4. **Retourner l'adresse d'un tableau local**
   ```c
   int* creer_tableau() {
       int tab[5] = {1, 2, 3, 4, 5};
       return tab;  // ❌ DANGEREUX : le tableau sera détruit
   }
   ```

## Exemple complet : manipulation d'un tableau avec pointeurs

```c
#include <stdio.h>

void afficher_valeurs(const int *tab, size_t taille) {
    printf("Valeurs : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%d ", tab[i]);  // Ou : *(tab + i)
    }
    printf("\n");
}

void afficher_adresses(const int *tab, size_t taille) {
    printf("Adresses : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%p ", (void*)(tab + i));
    }
    printf("\n");
}

void doubler_valeurs(int *tab, size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        tab[i] *= 2;  // Ou : *(tab + i) *= 2
    }
}

int main(void) {
    int nombres[5] = {10, 20, 30, 40, 50};

    printf("Tableau initial :\n");
    afficher_valeurs(nombres, 5);
    afficher_adresses(nombres, 5);

    printf("\nNom du tableau : %p\n", (void*)nombres);
    printf("Adresse 1er élément : %p\n", (void*)&nombres[0]);
    printf("Taille du tableau : %zu octets\n", sizeof(nombres));

    doubler_valeurs(nombres, 5);

    printf("\nAprès doublement :\n");
    afficher_valeurs(nombres, 5);

    // Parcours avec pointeur
    printf("\nParcours avec pointeur itératif : ");
    int *ptr = nombres;
    for (size_t i = 0; i < 5; i++) {
        printf("%d ", *ptr);
        ptr++;
    }
    printf("\n");

    return 0;
}
```

**Sortie possible :**
```
Tableau initial :
Valeurs : 10 20 30 40 50
Adresses : 0x7ffd8c2a1a40 0x7ffd8c2a1a44 0x7ffd8c2a1a48 0x7ffd8c2a1a4c 0x7ffd8c2a1a50

Nom du tableau : 0x7ffd8c2a1a40
Adresse 1er élément : 0x7ffd8c2a1a40
Taille du tableau : 20 octets

Après doublement :
Valeurs : 20 40 60 80 100

Parcours avec pointeur itératif : 20 40 60 80 100
```

## Résumé des points clés

1. **Le nom d'un tableau est équivalent à l'adresse de son premier élément**
   - `tab` ≡ `&tab[0]`

2. **Les notations tableau et pointeur sont interchangeables**
   - `tab[i]` ≡ `*(tab + i)`
   - `&tab[i]` ≡ `tab + i`

3. **Un tableau passé en paramètre devient un pointeur**
   - Perte de l'information de taille
   - Nécessité de passer la taille séparément

4. **Différences importantes entre tableaux et pointeurs**
   - `sizeof()` retourne des valeurs différentes
   - Un nom de tableau ne peut pas être réassigné
   - Un pointeur est modifiable

5. **L'arithmétique des pointeurs est naturelle sur les tableaux**
   - `ptr + 1` avance d'un élément (pas d'un octet)
   - La soustraction de pointeurs donne le nombre d'éléments

6. **Attention aux pièges courants**
   - Ne jamais retourner l'adresse d'un tableau local
   - Toujours passer la taille avec le tableau
   - Vérifier les accès hors limites

## Pour aller plus loin

Dans les sections suivantes, nous verrons :
- **Section 8.2** : Tableaux multidimensionnels en détail
- **Section 8.3** : Les chaînes de caractères (tableaux de `char`)
- **Section 8.4** : Manipulation avancée de chaînes
- **Section 8.5** : Buffer overflows et sécurité

La compréhension de la relation tableaux/pointeurs est la **fondation** de toute manipulation de données en C. Prenez le temps de bien assimiler ces concepts !

---

**Note :** N'hésitez pas à expérimenter avec du code pour visualiser les adresses mémoire et comprendre comment les tableaux et pointeurs fonctionnent concrètement. Utilisez `printf()` avec `%p` pour afficher les adresses et observez les patterns !

⏭️ [Tableaux multidimensionnels](/08-tableaux-et-chaines/02-tableaux-multidimensionnels.md)
