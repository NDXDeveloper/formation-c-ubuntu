🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.6 typedef : Simplification de la syntaxe

## Introduction

Vous en avez assez d'écrire `struct Point` ou `enum Couleur` à chaque fois que vous déclarez une variable ? Vous trouvez la syntaxe des pointeurs de fonctions illisible ?

**typedef** est la solution ! Ce mot-clé permet de créer des **alias** (noms alternatifs) pour des types existants, rendant votre code plus concis et plus lisible.

### Avant typedef

```c
struct Point {
    int x;
    int y;
};

struct Point p1;  
struct Point p2;  
struct Point p3;  // Répétitif...  
```

### Après typedef

```c
typedef struct Point {
    int x;
    int y;
} Point;

Point p1;  // ✅ Beaucoup plus simple !  
Point p2;  
Point p3;  
```

---

## Qu'est-ce que typedef ?

### Définition

**typedef** permet de créer un **nouveau nom** (alias) pour un type existant. Le type sous-jacent reste le même, seul le nom change.

### Syntaxe générale

```c
typedef type_existant nouveau_nom;
```

### Exemples simples

```c
// Créer un alias pour int
typedef int Entier;

Entier nombre = 42;  // Équivalent à : int nombre = 42;

// Créer un alias pour unsigned long
typedef unsigned long ULong;

ULong valeur = 1000000;  // Équivalent à : unsigned long valeur = 1000000;

// Créer un alias pour char*
typedef char* String;

String nom = "Alice";  // Équivalent à : char* nom = "Alice";
```

---

## typedef avec les types de base

### Types numériques

```c
#include <stdio.h>

typedef unsigned char Octet;        // 0 à 255  
typedef unsigned short Mot;         // 0 à 65535  
typedef unsigned int Entier32;      // 0 à 2^32-1  
typedef unsigned long long U64;     // 0 à 2^64-1  

int main() {
    Octet age = 25;
    Mot port = 8080;
    Entier32 population = 67000000;
    U64 tres_grand = 18446744073709551615ULL;

    printf("Age : %hhu\n", age);
    printf("Port : %hu\n", port);
    printf("Population : %u\n", population);
    printf("Très grand : %llu\n", tres_grand);

    return 0;
}
```

### Types flottants

```c
typedef float Reel32;  
typedef double Reel64;  

Reel32 temperature = 23.5f;  
Reel64 pi = 3.14159265358979323846;  
```

### Améliorer la lisibilité du code

```c
// Sans typedef : moins explicite
unsigned long long taille_fichier = 1024000;

// Avec typedef : intention claire
typedef unsigned long long TailleFichier;  
TailleFichier taille_fichier = 1024000;  
```

---

## typedef avec struct

### Méthode 1 : typedef séparé

```c
struct Point {
    int x;
    int y;
};

typedef struct Point Point;

// Utilisation
Point p = {10, 20};
```

### Méthode 2 : typedef intégré (recommandé)

```c
typedef struct Point {
    int x;
    int y;
} Point;

// Les deux syntaxes fonctionnent :
struct Point p1 = {10, 20};  
Point p2 = {30, 40};  // Plus simple !  
```

### Méthode 3 : struct anonyme avec typedef

```c
typedef struct {
    int x;
    int y;
} Point;

// On ne peut utiliser que Point (pas de "struct Point")
Point p = {10, 20};
```

### Exemple complet

```c
#include <stdio.h>

typedef struct {
    char nom[50];
    int age;
    float moyenne;
} Etudiant;

void afficher_etudiant(Etudiant e) {
    printf("Nom : %s\n", e.nom);
    printf("Âge : %d ans\n", e.age);
    printf("Moyenne : %.2f/20\n", e.moyenne);
}

int main() {
    Etudiant alice = {"Alice Dupont", 20, 15.5};
    Etudiant bob = {"Bob Martin", 22, 14.2};

    afficher_etudiant(alice);
    printf("\n");
    afficher_etudiant(bob);

    return 0;
}
```

---

## typedef avec enum

### Sans typedef

```c
enum Couleur {
    ROUGE,
    VERT,
    BLEU
};

enum Couleur ma_couleur = ROUGE;  // Répétitif
```

### Avec typedef

```c
typedef enum {
    ROUGE,
    VERT,
    BLEU
} Couleur;

Couleur ma_couleur = ROUGE;  // Beaucoup plus simple !
```

### Exemple pratique

```c
#include <stdio.h>

typedef enum {
    LUNDI,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE
} Jour;

const char* nom_jour(Jour j) {
    switch (j) {
        case LUNDI:    return "Lundi";
        case MARDI:    return "Mardi";
        case MERCREDI: return "Mercredi";
        case JEUDI:    return "Jeudi";
        case VENDREDI: return "Vendredi";
        case SAMEDI:   return "Samedi";
        case DIMANCHE: return "Dimanche";
        default:       return "Inconnu";
    }
}

int main() {
    Jour aujourd_hui = MERCREDI;
    printf("Aujourd'hui : %s\n", nom_jour(aujourd_hui));

    return 0;
}
```

---

## typedef avec union

### Syntaxe

```c
typedef union {
    int i;
    float f;
    char c;
} Valeur;

Valeur v;  
v.i = 42;  
```

### Exemple : Tagged union

```c
#include <stdio.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} TypeDonnee;

typedef union {
    int i;
    float f;
    char *s;
} DonneeUnion;

typedef struct {
    TypeDonnee type;
    DonneeUnion data;
} Donnee;

void afficher_donnee(Donnee d) {
    switch (d.type) {
        case TYPE_INT:
            printf("Entier : %d\n", d.data.i);
            break;
        case TYPE_FLOAT:
            printf("Float : %.2f\n", d.data.f);
            break;
        case TYPE_STRING:
            printf("String : %s\n", d.data.s);
            break;
    }
}

int main() {
    Donnee d1 = {TYPE_INT, {.i = 42}};
    Donnee d2 = {TYPE_FLOAT, {.f = 3.14f}};
    Donnee d3 = {TYPE_STRING, {.s = "Bonjour"}};

    afficher_donnee(d1);
    afficher_donnee(d2);
    afficher_donnee(d3);

    return 0;
}
```

---

## typedef avec les pointeurs

### Pointeurs simples

```c
typedef int* PointeurInt;

PointeurInt p;  
int x = 42;  
p = &x;  

printf("%d\n", *p);  // 42
```

**⚠️ Attention** : Comportement parfois déroutant

```c
typedef int* PointeurInt;

PointeurInt p1, p2;
// p1 est un int*
// p2 est aussi un int* (grâce au typedef)

// Mais avec int* directement :
int* q1, q2;
// q1 est un int*
// q2 est un int (pas un pointeur !)
```

### Pointeurs de pointeurs

```c
typedef char** TableauChaines;

TableauChaines argv;
```

### Pointeurs vers structures

```c
typedef struct Node {
    int valeur;
    struct Node *suivant;  // ⚠️ On doit utiliser "struct Node" ici
} Node;

typedef Node* PtrNode;

PtrNode tete = NULL;
```

---

## typedef avec les pointeurs de fonctions

### Le problème : syntaxe illisible

Sans typedef, les pointeurs de fonctions sont difficiles à lire :

```c
// Pointeur vers une fonction qui prend deux int et retourne un int
int (*operation)(int, int);

// Déclaration d'une fonction
int additionner(int a, int b) {
    return a + b;
}

// Utilisation
operation = &additionner;  
int resultat = operation(5, 3);  
```

### La solution : typedef

```c
// Créer un alias pour le type "pointeur de fonction"
typedef int (*Operation)(int, int);

// Utilisation simplifiée
Operation op;

int additionner(int a, int b) {
    return a + b;
}

op = additionner;  // Pas besoin de &  
int resultat = op(5, 3);  
```

### Exemple complet : Calculatrice

```c
#include <stdio.h>

// Définir le type pour les opérations
typedef int (*Operation)(int, int);

// Fonctions d'opération
int additionner(int a, int b) {
    return a + b;
}

int soustraire(int a, int b) {
    return a - b;
}

int multiplier(int a, int b) {
    return a * b;
}

int diviser(int a, int b) {
    if (b == 0) {
        printf("Erreur : division par zéro\n");
        return 0;
    }
    return a / b;
}

// Fonction qui utilise une opération
int calculer(int a, int b, Operation op) {
    return op(a, b);
}

int main() {
    int x = 10, y = 5;

    printf("%d + %d = %d\n", x, y, calculer(x, y, additionner));
    printf("%d - %d = %d\n", x, y, calculer(x, y, soustraire));
    printf("%d × %d = %d\n", x, y, calculer(x, y, multiplier));
    printf("%d ÷ %d = %d\n", x, y, calculer(x, y, diviser));

    return 0;
}
```

**Sortie** :
```
10 + 5 = 15
10 - 5 = 5
10 × 5 = 50
10 ÷ 5 = 2
```

### Tableau de pointeurs de fonctions

```c
#include <stdio.h>

typedef void (*Commande)(void);

void demarrer(void) {
    printf("Démarrage...\n");
}

void arreter(void) {
    printf("Arrêt...\n");
}

void redemarrer(void) {
    printf("Redémarrage...\n");
}

int main() {
    Commande commandes[] = {demarrer, arreter, redemarrer};

    for (int i = 0; i < 3; i++) {
        commandes[i]();
    }

    return 0;
}
```

---

## typedef avec les tableaux

### Tableaux de taille fixe

```c
typedef int Tableau10[10];

Tableau10 nombres = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

for (int i = 0; i < 10; i++) {
    printf("%d ", nombres[i]);
}
```

### Matrices

```c
typedef int Matrice3x3[3][3];

Matrice3x3 identite = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
};
```

### Tableaux de caractères (chaînes)

```c
typedef char Chaine50[50];

Chaine50 nom = "Alice";  
Chaine50 prenom = "Dupont";  

printf("%s %s\n", nom, prenom);
```

---

## Cas d'usage avancés

### 1. Types portables avec <stdint.h>

```c
#include <stdint.h>

// stdint.h utilise typedef pour définir des types de taille fixe
// typedef signed char        int8_t;
// typedef unsigned char      uint8_t;
// typedef short              int16_t;
// typedef unsigned short     uint16_t;
// typedef int                int32_t;
// typedef unsigned int       uint32_t;
// typedef long long          int64_t;
// typedef unsigned long long uint64_t;

// Utilisation
uint8_t octet = 255;  
int32_t entier = -2147483648;  
uint64_t grand = 18446744073709551615ULL;  
```

### 2. Types opaques (information hiding)

```c
// fichier: list.h
typedef struct List List;  // Déclaration forward

List* list_create(void);  
void list_destroy(List *list);  
void list_add(List *list, int value);  

// fichier: list.c
struct List {
    int *data;
    size_t size;
    size_t capacity;
};

// L'utilisateur manipule List* sans connaître les détails internes
```

### 3. Callbacks

```c
#include <stdio.h>

typedef void (*Callback)(int);

void traiter_donnees(int *data, size_t n, Callback cb) {
    for (size_t i = 0; i < n; i++) {
        cb(data[i]);
    }
}

void afficher(int x) {
    printf("%d ", x);
}

void doubler(int x) {
    printf("%d ", x * 2);
}

int main() {
    int nombres[] = {1, 2, 3, 4, 5};

    printf("Valeurs : ");
    traiter_donnees(nombres, 5, afficher);
    printf("\n");

    printf("Doublés : ");
    traiter_donnees(nombres, 5, doubler);
    printf("\n");

    return 0;
}
```

**Sortie** :
```
Valeurs : 1 2 3 4 5  
Doublés : 2 4 6 8 10  
```

### 4. Types génériques (avec void*)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*CompareFn)(const void*, const void*);

int comparer_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void tri_bulle(void *base, size_t n, size_t size, CompareFn cmp) {
    char *arr = (char*)base;
    char temp[size];

    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            void *elem1 = arr + j * size;
            void *elem2 = arr + (j + 1) * size;

            if (cmp(elem1, elem2) > 0) {
                // Échanger
                memcpy(temp, elem1, size);
                memcpy(elem1, elem2, size);
                memcpy(elem2, temp, size);
            }
        }
    }
}

int main() {
    int nombres[] = {5, 2, 8, 1, 9, 3};
    size_t n = sizeof(nombres) / sizeof(nombres[0]);

    printf("Avant tri : ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    tri_bulle(nombres, n, sizeof(int), comparer_int);

    printf("Après tri : ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    return 0;
}
```

---

## Organisation du code avec typedef

### Fichier d'en-tête (.h)

```c
// fichier: types.h
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// Types de base
typedef uint8_t Octet;  
typedef uint32_t U32;  

// Structures
typedef struct {
    char nom[50];
    int age;
} Personne;

// Énumérations
typedef enum {
    ETAT_ACTIF,
    ETAT_INACTIF,
    ETAT_ERREUR
} Etat;

// Pointeurs de fonctions
typedef int (*CompareFn)(const void*, const void*);  
typedef void (*CallbackFn)(void*);  

#endif
```

### Fichier source (.c)

```c
// fichier: main.c
#include "types.h"
#include <stdio.h>

int main() {
    Personne p = {"Alice", 25};
    Etat e = ETAT_ACTIF;
    U32 valeur = 12345;

    printf("Nom : %s\n", p.nom);
    printf("État : %d\n", e);
    printf("Valeur : %u\n", valeur);

    return 0;
}
```

---

## Bonnes pratiques

### 1. Noms descriptifs

```c
// ❌ Mauvais : peu clair
typedef int i;  
typedef struct { int x, y; } p;  

// ✅ Bon : descriptif
typedef int Compteur;  
typedef struct { int x, y; } Point;  
```

### 2. Convention de nommage

Plusieurs conventions existent :

```c
// Style 1 : PascalCase (recommandé)
typedef struct Point Point;  
typedef enum Couleur Couleur;  

// Style 2 : snake_case avec suffixe _t
typedef struct point point_t;  
typedef enum couleur couleur_t;  

// Style 3 : MAJUSCULES (pour constantes uniquement)
#define MAX_SIZE 100
```

**Recommandation** : Choisissez une convention et soyez cohérent !

### 3. Ne pas abuser de typedef

```c
// ❌ Mauvais : cache le type réel
typedef int* IntPtr;  
IntPtr p;  // On ne voit pas que c'est un pointeur  

// ✅ Bon : le * est explicite
int *p;
```

**Exception** : Pour les pointeurs de fonctions, typedef est recommandé car la syntaxe est complexe.

### 4. typedef vs #define

```c
// ❌ #define : pas de vérification de type
#define Int int

// ✅ typedef : vérification de type par le compilateur
typedef int Int;
```

### 5. Documenter les typedef

```c
/**
 * @brief Représente une coordonnée 2D en pixels
 */
typedef struct {
    int x;  ///< Coordonnée horizontale
    int y;  ///< Coordonnée verticale
} Point;

/**
 * @brief Fonction de comparaison pour le tri
 * @param a Premier élément
 * @param b Deuxième élément
 * @return <0 si a<b, 0 si a==b, >0 si a>b
 */
typedef int (*CompareFn)(const void *a, const void *b);
```

---

## typedef et portabilité

### Types de taille fixe

```c
#include <stdint.h>
#include <inttypes.h>

// Ces types ont la même taille sur toutes les plateformes
typedef uint8_t  U8;   // Toujours 8 bits  
typedef uint16_t U16;  // Toujours 16 bits  
typedef uint32_t U32;  // Toujours 32 bits  
typedef uint64_t U64;  // Toujours 64 bits  

// Utilisation
U32 valeur = 0x12345678;  
printf("Valeur : %" PRIx32 "\n", valeur);  
```

### Types dépendants de la plateforme

```c
// size_t : taille d'un objet (32 ou 64 bits selon la plateforme)
typedef size_t Taille;

// ptrdiff_t : différence entre deux pointeurs
typedef ptrdiff_t Difference;
```

---

## Exemples complexes

### 1. Graphe avec pointeurs de fonctions

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node Node;

struct Node {
    int valeur;
    Node *suivant;
};

typedef void (*VisitFn)(Node*);  
typedef int (*PredicatFn)(Node*);  

void parcourir(Node *tete, VisitFn visit) {
    while (tete != NULL) {
        visit(tete);
        tete = tete->suivant;
    }
}

Node* filtrer(Node *tete, PredicatFn pred) {
    Node *nouvelle_tete = NULL;
    Node *queue = NULL;

    while (tete != NULL) {
        if (pred(tete)) {
            Node *nouveau = malloc(sizeof(Node));
            nouveau->valeur = tete->valeur;
            nouveau->suivant = NULL;

            if (nouvelle_tete == NULL) {
                nouvelle_tete = nouveau;
                queue = nouveau;
            } else {
                queue->suivant = nouveau;
                queue = nouveau;
            }
        }
        tete = tete->suivant;
    }

    return nouvelle_tete;
}

void afficher_node(Node *n) {
    printf("%d ", n->valeur);
}

int est_pair(Node *n) {
    return n->valeur % 2 == 0;
}

int main() {
    // Créer une liste : 1 -> 2 -> 3 -> 4 -> 5
    Node n1 = {1, NULL};
    Node n2 = {2, NULL};
    Node n3 = {3, NULL};
    Node n4 = {4, NULL};
    Node n5 = {5, NULL};

    n1.suivant = &n2;
    n2.suivant = &n3;
    n3.suivant = &n4;
    n4.suivant = &n5;

    printf("Liste complète : ");
    parcourir(&n1, afficher_node);
    printf("\n");

    printf("Nombres pairs : ");
    Node *pairs = filtrer(&n1, est_pair);
    parcourir(pairs, afficher_node);
    printf("\n");

    // Nettoyer la mémoire
    while (pairs != NULL) {
        Node *temp = pairs;
        pairs = pairs->suivant;
        free(temp);
    }

    return 0;
}
```

### 2. Machine à états avec typedef

```c
#include <stdio.h>

typedef enum {
    ETAT_ETEINT,
    ETAT_DEMARRAGE,
    ETAT_ACTIF,
    ETAT_PAUSE,
    ETAT_ARRET
} Etat;

typedef enum {
    EVENT_DEMARRER,
    EVENT_PAUSE,
    EVENT_REPRENDRE,
    EVENT_ARRETER
} Evenement;

typedef Etat (*GestionnaireFn)(Evenement);

Etat gerer_eteint(Evenement e) {
    if (e == EVENT_DEMARRER) {
        printf("Démarrage...\n");
        return ETAT_DEMARRAGE;
    }
    return ETAT_ETEINT;
}

Etat gerer_actif(Evenement e) {
    switch (e) {
        case EVENT_PAUSE:
            printf("Mise en pause...\n");
            return ETAT_PAUSE;
        case EVENT_ARRETER:
            printf("Arrêt...\n");
            return ETAT_ARRET;
        default:
            return ETAT_ACTIF;
    }
}

Etat gerer_pause(Evenement e) {
    switch (e) {
        case EVENT_REPRENDRE:
            printf("Reprise...\n");
            return ETAT_ACTIF;
        case EVENT_ARRETER:
            printf("Arrêt...\n");
            return ETAT_ARRET;
        default:
            return ETAT_PAUSE;
    }
}

int main() {
    Etat etat_actuel = ETAT_ETEINT;
    GestionnaireFn gestionnaires[] = {
        [ETAT_ETEINT] = gerer_eteint,
        [ETAT_ACTIF] = gerer_actif,
        [ETAT_PAUSE] = gerer_pause
    };

    // Simulation
    etat_actuel = gestionnaires[etat_actuel](EVENT_DEMARRER);
    etat_actuel = ETAT_ACTIF;  // Après démarrage
    etat_actuel = gestionnaires[etat_actuel](EVENT_PAUSE);
    etat_actuel = gestionnaires[etat_actuel](EVENT_REPRENDRE);
    etat_actuel = gestionnaires[etat_actuel](EVENT_ARRETER);

    return 0;
}
```

---

## typedef vs déclaration directe : Comparaison

| Sans typedef | Avec typedef |
|--------------|--------------|
| `struct Point p;` | `Point p;` |
| `enum Couleur c;` | `Couleur c;` |
| `unsigned long long x;` | `U64 x;` |
| `int (*fn)(int, int);` | `Operation fn;` |
| `struct Node *liste[10];` | `PtrNode liste[10];` |

**Verdict** : typedef rend le code plus lisible, surtout pour les types complexes.

---

## Résumé

### Points clés

1. **typedef** : Crée des alias pour rendre les types plus lisibles
2. **Syntaxe** : `typedef type_existant nouveau_nom;`
3. **Usage principal** : Simplifier struct, enum, union et pointeurs de fonctions
4. **Avantages** : Code plus concis, plus lisible, meilleure maintenance
5. **Portabilité** : Facilite l'adaptation du code à différentes plateformes

### Quand utiliser typedef ?

✅ **Utiliser** pour :
- Simplifier struct, enum, union
- Rendre les pointeurs de fonctions lisibles
- Créer des types portables
- Masquer des détails d'implémentation (types opaques)
- Améliorer la cohérence du code

❌ **Éviter** pour :
- Types simples déjà clairs (`int`, `char`, etc.)
- Masquer des pointeurs simples (sauf pointeurs de fonctions)
- Créer de la confusion

### Syntaxes essentielles

```c
// Types de base
typedef unsigned long U64;

// Structures
typedef struct {
    int x, y;
} Point;

// Énumérations
typedef enum {
    ROUGE, VERT, BLEU
} Couleur;

// Unions
typedef union {
    int i;
    float f;
} Valeur;

// Pointeurs
typedef int* PtrInt;

// Pointeurs de fonctions
typedef int (*Operation)(int, int);

// Tableaux
typedef int Tableau10[10];
```

---

## À retenir

```c
// ❌ Sans typedef : répétitif et verbeux
struct Point {
    int x, y;
};
enum Couleur {
    ROUGE, VERT, BLEU
};

struct Point p1;  
enum Couleur c1;  
int (*operation)(int, int);  

// ✅ Avec typedef : concis et lisible
typedef struct {
    int x, y;
} Point;

typedef enum {
    ROUGE, VERT, BLEU
} Couleur;

typedef int (*Operation)(int, int);

Point p1;  
Couleur c1;  
Operation operation;  
```

**Règle d'or** : Utilisez typedef pour améliorer la lisibilité, mais ne masquez pas la nature des types (sauf pour les pointeurs de fonctions où c'est nécessaire).

**Prochaine étape** : Nous allons découvrir les **bit-fields** (section 10.7), qui permettent de stocker des données au niveau du bit pour économiser encore plus de mémoire.

⏭️ [Structures bit-fields](/10-types-utilisateur/07-bit-fields.md)
