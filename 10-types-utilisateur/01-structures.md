🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.1 struct : Création d'objets complexes

## Introduction

Jusqu'à présent, nous avons travaillé avec des types de données simples comme `int`, `float`, `char`, etc. Mais dans la vraie vie, nous manipulons souvent des entités plus complexes qui combinent plusieurs informations. Par exemple, un **étudiant** possède un nom, un âge, une moyenne, etc. Comment représenter cela en C ?

C'est là qu'interviennent les **structures** (ou `struct`). Une structure est un type de données personnalisé qui regroupe plusieurs variables (appelées **membres** ou **champs**) sous un seul nom.

---

## Pourquoi utiliser des structures ?

### Problème sans structure

Imaginons que nous voulions stocker les informations de 3 étudiants. Sans structure, nous devrions faire :

```c
char nom1[50] = "Alice";  
int age1 = 20;  
float moyenne1 = 15.5;  

char nom2[50] = "Bob";  
int age2 = 22;  
float moyenne2 = 14.2;  

char nom3[50] = "Charlie";  
int age3 = 21;  
float moyenne3 = 16.8;  
```

**Problèmes** :
- Code répétitif et difficile à maintenir
- Impossible de passer toutes ces informations à une fonction facilement
- Pas de lien logique entre les variables d'un même étudiant

### Solution avec une structure

```c
struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

struct Etudiant alice = {"Alice", 20, 15.5};  
struct Etudiant bob = {"Bob", 22, 14.2};  
struct Etudiant charlie = {"Charlie", 21, 16.8};  
```

**Avantages** :
- Code plus clair et organisé
- Facilite le passage de données aux fonctions
- Représentation logique d'une entité du monde réel

---

## Déclaration d'une structure

### Syntaxe de base

```c
struct NomDeLaStructure {
    type1 membre1;
    type2 membre2;
    type3 membre3;
    // ...
};
```

**Attention** : N'oubliez pas le point-virgule `;` après l'accolade fermante !

### Exemple concret

```c
struct Point {
    int x;
    int y;
};
```

Cette déclaration crée un nouveau type `struct Point` qui contient deux entiers : `x` et `y`.

### Où déclarer une structure ?

Une structure peut être déclarée :

1. **Globalement** (avant `main()`) : accessible dans tout le fichier
2. **Localement** (dans une fonction) : accessible uniquement dans cette fonction

```c
#include <stdio.h>

// Déclaration globale
struct Rectangle {
    int largeur;
    int hauteur;
};

int main() {
    // Déclaration locale
    struct Point {
        int x;
        int y;
    };

    struct Rectangle r;  // OK
    struct Point p;      // OK uniquement dans main()

    return 0;
}
```

---

## Création de variables de structure

### Méthode 1 : Déclaration puis initialisation

```c
struct Point p1;  // Déclaration  
p1.x = 10;        // Initialisation membre par membre  
p1.y = 20;  
```

### Méthode 2 : Initialisation directe

```c
struct Point p2 = {10, 20};  // x=10, y=20
```

**Ordre important** : Les valeurs sont assignées dans l'ordre de déclaration des membres.

### Méthode 3 : Initialisation partielle

```c
struct Point p3 = {10};  // x=10, y=0 (initialisé à zéro)  
struct Point p4 = {0};   // x=0, y=0 (tous les membres à zéro)  
```

Les membres non spécifiés sont automatiquement initialisés à zéro.

### Méthode 4 : Initialisation désignée (C99 et plus récent)

```c
struct Point p5 = {.y = 30, .x = 15};  // Ordre libre !
```

Cette syntaxe permet de spécifier explicitement quel membre initialiser.

---

## Accès aux membres d'une structure

On utilise l'**opérateur point** (`.`) pour accéder aux membres :

```c
struct Point p = {5, 10};

printf("x = %d\n", p.x);  // Lecture  
printf("y = %d\n", p.y);  

p.x = 25;  // Modification  
p.y = 30;  

printf("Nouveau x = %d\n", p.x);
```

**Sortie** :
```
x = 5  
y = 10  
Nouveau x = 25  
```

---

## Exemple complet : Gestion d'un étudiant

```c
#include <stdio.h>
#include <string.h>

struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

int main() {
    // Création d'un étudiant
    struct Etudiant etudiant1;

    // Initialisation
    strcpy(etudiant1.nom, "Alice Dupont");
    etudiant1.age = 20;
    etudiant1.moyenne = 15.5;

    // Affichage
    printf("Informations de l'étudiant :\n");
    printf("Nom : %s\n", etudiant1.nom);
    printf("Âge : %d ans\n", etudiant1.age);
    printf("Moyenne : %.2f/20\n", etudiant1.moyenne);

    return 0;
}
```

**Sortie** :
```
Informations de l'étudiant :  
Nom : Alice Dupont  
Âge : 20 ans
Moyenne : 15.50/20
```

---

## Structures et fonctions

### Passage par valeur

```c
#include <stdio.h>

struct Point {
    int x;
    int y;
};

void afficherPoint(struct Point p) {
    printf("Point(%d, %d)\n", p.x, p.y);
}

int main() {
    struct Point p1 = {10, 20};
    afficherPoint(p1);
    return 0;
}
```

**Important** : La structure est **copiée** lors du passage à la fonction. Les modifications dans la fonction n'affectent pas la structure originale.

### Passage par pointeur (plus efficace)

```c
void afficherPoint(struct Point *p) {
    printf("Point(%d, %d)\n", p->x, p->y);
}

int main() {
    struct Point p1 = {10, 20};
    afficherPoint(&p1);  // On passe l'adresse
    return 0;
}
```

**Note** : Avec un pointeur, on utilise l'opérateur `->` au lieu de `.` pour accéder aux membres.

### Retourner une structure

```c
struct Point creerPoint(int x, int y) {
    struct Point p = {x, y};
    return p;
}

int main() {
    struct Point p1 = creerPoint(5, 15);
    printf("x=%d, y=%d\n", p1.x, p1.y);
    return 0;
}
```

---

## Tableaux de structures

On peut créer des tableaux de structures pour stocker plusieurs entités :

```c
#include <stdio.h>
#include <string.h>

struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

int main() {
    // Tableau de 3 étudiants
    struct Etudiant classe[3] = {
        {"Alice", 20, 15.5},
        {"Bob", 22, 14.2},
        {"Charlie", 21, 16.8}
    };

    // Parcours du tableau
    for (int i = 0; i < 3; i++) {
        printf("Étudiant %d : %s, %d ans, moyenne %.2f\n",
               i + 1, classe[i].nom, classe[i].age, classe[i].moyenne);
    }

    return 0;
}
```

**Sortie** :
```
Étudiant 1 : Alice, 20 ans, moyenne 15.50
Étudiant 2 : Bob, 22 ans, moyenne 14.20
Étudiant 3 : Charlie, 21 ans, moyenne 16.80
```

---

## Structures imbriquées

Une structure peut contenir d'autres structures :

```c
struct Date {
    int jour;
    int mois;
    int annee;
};

struct Personne {
    char nom[50];
    struct Date dateNaissance;  // Structure imbriquée
};

int main() {
    struct Personne p = {
        "Alice",
        {15, 3, 2003}  // jour=15, mois=3, annee=2003
    };

    printf("%s est née le %d/%d/%d\n",
           p.nom,
           p.dateNaissance.jour,
           p.dateNaissance.mois,
           p.dateNaissance.annee);

    return 0;
}
```

---

## Comparaison de structures

**Attention** : On ne peut pas comparer directement deux structures avec `==` !

```c
struct Point p1 = {10, 20};  
struct Point p2 = {10, 20};  

// ❌ ERREUR : if (p1 == p2) { ... }
```

Il faut comparer membre par membre :

```c
int comparerPoints(struct Point p1, struct Point p2) {
    return (p1.x == p2.x) && (p1.y == p2.y);
}

int main() {
    struct Point p1 = {10, 20};
    struct Point p2 = {10, 20};

    if (comparerPoints(p1, p2)) {
        printf("Les points sont identiques\n");
    }

    return 0;
}
```

---

## Copie de structures

Contrairement à la comparaison, l'**affectation** entre structures fonctionne directement :

```c
struct Point p1 = {10, 20};  
struct Point p2;  

p2 = p1;  // ✅ Copie tous les membres automatiquement

printf("p2: x=%d, y=%d\n", p2.x, p2.y);  // Affiche: p2: x=10, y=20
```

---

## Bonnes pratiques

### 1. Nommage cohérent

```c
// Convention : Majuscule pour les noms de structures
struct Etudiant { ... };  
struct Personne { ... };  
struct Voiture { ... };  
```

### 2. Utiliser typedef pour simplifier

Au lieu de :
```c
struct Point p1;  
struct Point p2;  
```

On peut utiliser `typedef` :
```c
typedef struct {
    int x;
    int y;
} Point;

Point p1;  // Plus concis !  
Point p2;  
```

Nous verrons `typedef` en détail dans une section ultérieure (10.6).

### 3. Initialisation systématique

Toujours initialiser les structures pour éviter des valeurs aléatoires :

```c
struct Point p = {0};  // Tous les membres à zéro
```

### 4. Passage par pointeur pour les grandes structures

Pour les structures volumineuses, passer un pointeur à une fonction plutôt que de copier toute la structure :

```c
// ❌ Moins efficace (copie 1024 octets)
void traiter(struct GrosseStructure s);

// ✅ Plus efficace (copie 8 octets sur 64 bits)
void traiter(struct GrosseStructure *s);
```

---

## Exemple pratique : Carnet d'adresses simple

```c
#include <stdio.h>
#include <string.h>

#define MAX_CONTACTS 100

struct Contact {
    char nom[50];
    char telephone[15];
    char email[50];
};

void afficherContact(struct Contact c) {
    printf("Nom       : %s\n", c.nom);
    printf("Téléphone : %s\n", c.telephone);
    printf("Email     : %s\n", c.email);
    printf("-------------------\n");
}

int main() {
    struct Contact carnet[MAX_CONTACTS];
    int nbContacts = 0;

    // Ajout de quelques contacts
    strcpy(carnet[nbContacts].nom, "Alice Dupont");
    strcpy(carnet[nbContacts].telephone, "06.12.34.56.78");
    strcpy(carnet[nbContacts].email, "alice@example.com");
    nbContacts++;

    strcpy(carnet[nbContacts].nom, "Bob Martin");
    strcpy(carnet[nbContacts].telephone, "06.98.76.54.32");
    strcpy(carnet[nbContacts].email, "bob@example.com");
    nbContacts++;

    // Affichage de tous les contacts
    printf("=== CARNET D'ADRESSES ===\n\n");
    for (int i = 0; i < nbContacts; i++) {
        afficherContact(carnet[i]);
    }

    return 0;
}
```

---

## Résumé

- Une **structure** (`struct`) permet de regrouper plusieurs variables de types différents sous un seul nom
- Elle représente une entité logique du monde réel (personne, point, date, etc.)
- On accède aux membres avec l'opérateur **point** (`.`)
- Les structures peuvent être passées aux fonctions, retournées, stockées dans des tableaux
- L'affectation entre structures fonctionne, mais pas la comparaison directe
- Les structures peuvent être imbriquées

Les structures sont un outil fondamental en C pour organiser et manipuler des données complexes de manière claire et efficace.

---

## À retenir

```c
// Déclaration
struct Point {
    int x;
    int y;
};

// Création et initialisation
struct Point p1 = {10, 20};

// Accès aux membres
p1.x = 5;  
printf("%d\n", p1.y);  

// Passage à une fonction
void afficher(struct Point p);  
afficher(p1);  

// Tableau de structures
struct Point points[10];
```

**Prochaine étape** : Nous allons découvrir comment optimiser l'utilisation de la mémoire avec l'alignement et le padding (section 10.2).

⏭️ [Alignement mémoire et padding](/10-types-utilisateur/02-alignement-padding.md)
