🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10. Types définis par l'utilisateur

## Introduction au chapitre

Jusqu'à présent, nous avons travaillé avec les **types de données primitifs** fournis par le langage C : `int`, `float`, `char`, `double`, etc. Ces types sont très utiles, mais ils ne suffisent pas toujours pour représenter des données complexes du monde réel.

Imaginez que vous devez gérer des informations sur des étudiants dans un programme. Chaque étudiant possède :
- Un nom (chaîne de caractères)
- Un âge (entier)
- Une moyenne (nombre à virgule)
- Un numéro d'étudiant (entier)

Comment organiser ces données ? Avec les types primitifs uniquement, vous devriez créer des variables séparées pour chaque information, ce qui devient vite ingérable :

```c
char nom1[50], nom2[50], nom3[50];
int age1, age2, age3;
float moyenne1, moyenne2, moyenne3;
int numero1, numero2, numero3;
// Cauchemar ! Et si vous avez 100 étudiants ?
```

C'est là qu'interviennent les **types définis par l'utilisateur** (user-defined types). Ces mécanismes permettent de créer vos propres types de données adaptés à vos besoins spécifiques.

---

## Pourquoi créer ses propres types ?

### 1. Regrouper des données liées

Au lieu de manipuler des dizaines de variables indépendantes, vous pouvez les regrouper logiquement :

```c
// Une seule structure représente un étudiant complet
struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
    int numero;
};

struct Etudiant etudiant1;  // Un étudiant, toutes ses infos ensemble
struct Etudiant etudiant2;
```

### 2. Améliorer la lisibilité du code

```c
// ❌ Peu clair avec des entiers
int statut = 2;  // Que signifie 2 ?

// ✅ Clair avec une énumération
enum Statut {
    EN_ATTENTE,
    EN_COURS,
    TERMINE
};
enum Statut statut = TERMINE;  // Explicite !
```

### 3. Faciliter la maintenance

Modifier un type personnalisé se fait à un seul endroit, et tous les usages sont automatiquement mis à jour :

```c
// Ajouter un champ "email" à tous les étudiants ?
// Il suffit de modifier la définition de struct Etudiant
struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
    int numero;
    char email[100];  // Nouveau champ ajouté ici
};
// Toutes les instances d'Etudiant ont maintenant un email
```

### 4. Optimiser l'utilisation de la mémoire

Les types personnalisés permettent d'optimiser la manière dont les données sont stockées :

```c
// Au lieu de 3 variables de 4 octets (12 octets)
int type, valeur1, valeur2;

// Une union peut stocker une seule valeur à la fois (4 octets)
union Donnee {
    int entier;
    float flottant;
    char caractere;
};
```

---

## Les cinq mécanismes de types personnalisés

Le langage C offre cinq outils principaux pour créer des types personnalisés. Voici un aperçu rapide de chacun :

### 1. **struct** : Structures

Les structures permettent de **regrouper plusieurs variables de types différents** sous un seul nom.

**Analogie** : Une structure est comme une fiche d'information contenant plusieurs champs.

```c
struct Point {
    int x;
    int y;
};

struct Point p = {10, 20};
```

**Utilité** : Représenter des entités complexes (personne, coordonnée, date, etc.)

→ **Section 10.1 à 10.3**

---

### 2. **union** : Unions

Les unions permettent de stocker **plusieurs types de données dans le même emplacement mémoire**, mais une seule valeur à la fois.

**Analogie** : Une boîte qui peut contenir soit un livre, soit des chaussures, mais pas les deux en même temps.

```c
union Valeur {
    int entier;
    float flottant;
    char caractere;
};

union Valeur v;
v.entier = 42;  // Stocke un entier
```

**Utilité** : Économiser de la mémoire quand plusieurs types sont mutuellement exclusifs.

→ **Section 10.4**

---

### 3. **enum** : Énumérations

Les énumérations définissent un **ensemble de constantes nommées**, rendant le code plus lisible.

**Analogie** : Une liste de choix fixes (comme les jours de la semaine ou les directions).

```c
enum Jour {
    LUNDI,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE
};

enum Jour aujourd_hui = MERCREDI;
```

**Utilité** : Remplacer les "nombres magiques" par des noms explicites.

→ **Section 10.5**

---

### 4. **typedef** : Alias de types

`typedef` permet de créer des **alias** (nouveaux noms) pour des types existants, simplifiant la syntaxe.

**Analogie** : Donner un surnom plus court à quelque chose de long ou complexe.

```c
typedef unsigned long long U64;
typedef struct Point Point;

U64 grand_nombre = 123456789;
Point p = {5, 10};  // Plus simple que "struct Point"
```

**Utilité** : Rendre le code plus concis et plus portable.

→ **Section 10.6**

---

### 5. **bit-fields** : Champs de bits

Les bit-fields permettent de spécifier combien de **bits** (et non d'octets) chaque membre d'une structure occupe.

**Analogie** : Diviser un coffre en compartiments de tailles variables selon les besoins.

```c
struct Options {
    unsigned int actif   : 1;  // 1 bit (0 ou 1)
    unsigned int niveau  : 4;  // 4 bits (0 à 15)
    unsigned int couleur : 3;  // 3 bits (0 à 7)
};
```

**Utilité** : Économiser de la mémoire au niveau du bit (hardware, protocoles).

→ **Section 10.7**

---

## Vue d'ensemble du chapitre

Ce chapitre est organisé en sept sections progressives :

| Section | Titre | Difficulté | Concepts clés |
|---------|-------|------------|---------------|
| **10.1** | struct : Création d'objets complexes | ⭐⭐ | Déclaration, initialisation, accès aux membres |
| **10.2** | Alignement mémoire et padding | ⭐⭐⭐ | Organisation en mémoire, optimisation |
| **10.3** | Packed structures | ⭐⭐⭐ | Suppression du padding, compromis |
| **10.4** | union : Optimisation mémoire | ⭐⭐ | Partage d'emplacement, tagged unions |
| **10.5** | enum : Code lisible et gestion d'états | ⭐⭐ | Constantes nommées, machines à états |
| **10.6** | typedef : Simplification de la syntaxe | ⭐⭐ | Alias de types, lisibilité |
| **10.7** | Structures bit-fields | ⭐⭐⭐ | Stockage au niveau du bit |

---

## Concepts fondamentaux à maîtriser

Avant de plonger dans les sections détaillées, voici les concepts de base que vous allez apprendre :

### 1. Abstraction de données

Les types personnalisés permettent de créer des **abstractions** : des représentations simplifiées d'entités complexes.

```c
// Au lieu de penser à des variables séparées...
char nom[50];
int x, y;

// ... on pense à une entité cohérente
struct Joueur {
    char nom[50];
    int x, y;
};
```

### 2. Encapsulation

Regrouper des données liées ensemble améliore l'organisation du code :

```c
// Fonction qui accepte une entité complète
void deplacer_joueur(struct Joueur *j, int dx, int dy) {
    j->x += dx;
    j->y += dy;
}
```

### 3. Organisation mémoire

Comprendre comment les données sont stockées en mémoire vous aidera à :
- Optimiser l'utilisation de la mémoire
- Éviter les bugs liés à l'alignement
- Écrire du code compatible avec le matériel

```c
struct Exemple {
    char c;    // 1 octet
    int i;     // 4 octets
};
// Taille réelle : 8 octets (pas 5) à cause du padding
```

### 4. Typage fort

Les types personnalisés renforcent la sécurité du code :

```c
enum Couleur { ROUGE, VERT, BLEU };
enum Animal { CHAT, CHIEN };

enum Couleur c = ROUGE;
// c = CHAT;  // ⚠️ Le compilateur peut avertir (types différents)
```

---

## Progression recommandée

### Pour les débutants

1. **Commencer par struct** (10.1) : C'est le type personnalisé le plus utilisé
2. **Puis enum** (10.5) : Très simple et immédiatement utile
3. **Ensuite typedef** (10.6) : Améliore la syntaxe des types précédents
4. **Continuer avec union** (10.4) : Concept plus avancé
5. **Approfondir** : padding (10.2), packed structures (10.3), bit-fields (10.7)

### Pour les développeurs intermédiaires

Vous pouvez suivre l'ordre du chapitre, en portant une attention particulière à :
- L'optimisation mémoire (10.2, 10.3)
- Les patterns avancés (tagged unions, machines à états)
- Les cas d'usage spécifiques (protocoles, hardware)

---

## Exemples introductifs

### Exemple 1 : Modélisation d'un point 2D

```c
// Avec types primitifs (peu pratique)
int point1_x = 10, point1_y = 20;
int point2_x = 30, point2_y = 40;

// Avec une structure (élégant)
struct Point {
    int x;
    int y;
};

struct Point p1 = {10, 20};
struct Point p2 = {30, 40};
```

### Exemple 2 : États d'une application

```c
// Avec des entiers (peu lisible)
#define ETAT_0 0
#define ETAT_1 1
#define ETAT_2 2

int etat = ETAT_1;  // Que fait l'application dans l'état 1 ?

// Avec une énumération (explicite)
enum Etat {
    INITIALISATION,
    EXECUTION,
    TERMINAISON
};

enum Etat etat = EXECUTION;  // Clair !
```

### Exemple 3 : Valeur de types multiples

```c
// Besoin de stocker soit un int, soit un float, soit un char
// mais jamais plusieurs en même temps

// Avec une union (économe)
union Valeur {
    int i;
    float f;
    char c;
};

union Valeur v;
v.i = 42;           // Stocke un entier
// v.f = 3.14;      // Écrase l'entier
```

---

## Vocabulaire important

Avant de continuer, familiarisez-vous avec ces termes :

| Terme | Définition |
|-------|------------|
| **Membre** | Variable à l'intérieur d'une structure ou union |
| **Champ** | Synonyme de membre |
| **Tag** | Nom donné à une structure, union ou enum |
| **Instanciation** | Création d'une variable d'un type personnalisé |
| **Padding** | Octets vides ajoutés pour l'alignement mémoire |
| **Alignement** | Contrainte sur l'adresse mémoire d'une variable |
| **Offset** | Position d'un membre par rapport au début de la structure |
| **Opaque** | Type dont les détails internes sont cachés |

---

## Conseils pour réussir ce chapitre

### 1. Pratiquez avec des exemples simples

Commencez par des structures simples avant de passer à des cas complexes :

```c
// Simple
struct Point {
    int x, y;
};

// Plus complexe
struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
    struct Date date_naissance;  // Structure imbriquée
};
```

### 2. Visualisez la mémoire

Essayez de dessiner comment les données sont disposées en mémoire :

```
struct Point p = {10, 20};

Mémoire :
+----+----+----+----+----+----+----+----+
| 10 | 10 | 10 | 10 | 20 | 20 | 20 | 20 |
+----+----+----+----+----+----+----+----+
  ^-- x (4 octets) --^-- y (4 octets) --^
```

### 3. Utilisez sizeof() et offsetof()

Ces outils vous aident à comprendre la disposition en mémoire :

```c
#include <stddef.h>

struct Exemple {
    char c;
    int i;
};

printf("Taille : %zu\n", sizeof(struct Exemple));
printf("Offset de i : %zu\n", offsetof(struct Exemple, i));
```

### 4. Compilez avec des warnings

Activez tous les avertissements pour détecter les problèmes :

```bash
gcc -Wall -Wextra -pedantic programme.c
```

### 5. Lisez du code réel

Étudiez comment les projets open-source utilisent les types personnalisés :
- Le noyau Linux (structures complexes)
- Redis (unions, types opaques)
- SQLite (énumérations d'états)

---

## Ce que vous saurez faire à la fin du chapitre

Après avoir complété ce chapitre, vous serez capable de :

- ✅ **Créer des structures** pour modéliser des entités complexes
- ✅ **Optimiser l'utilisation mémoire** avec unions et bit-fields
- ✅ **Rendre le code lisible** avec enums et typedef
- ✅ **Comprendre l'alignement mémoire** et le padding
- ✅ **Choisir le bon type** selon le contexte (struct vs union vs enum)
- ✅ **Écrire du code maintenable** avec des abstractions appropriées
- ✅ **Interagir avec le hardware** (registres, protocoles)
- ✅ **Déboguer des problèmes** liés aux types personnalisés

---

## Liens avec d'autres concepts C

Les types définis par l'utilisateur interagissent avec de nombreux concepts C :

### Avec les pointeurs (Module 3)

```c
struct Point {
    int x, y;
};

struct Point *ptr = &p;
ptr->x = 5;  // Accès via pointeur
```

### Avec l'allocation dynamique (Module 3)

```c
struct Etudiant *e = malloc(sizeof(struct Etudiant));
if (e != NULL) {
    strcpy(e->nom, "Alice");
}
free(e);
```

### Avec les fonctions (Module 2)

```c
void afficher_point(struct Point p) {
    printf("(%d, %d)\n", p.x, p.y);
}

struct Point creer_point(int x, int y) {
    struct Point p = {x, y};
    return p;
}
```

### Avec les tableaux (Module 3)

```c
struct Point points[10];
points[0] = (struct Point){5, 10};
```

---

## Erreurs courantes de débutants

Voici les pièges à éviter :

### 1. Oublier le mot-clé struct

```c
struct Point {
    int x, y;
};

Point p;  // ❌ Erreur : "Point" n'est pas un type
struct Point p;  // ✅ Correct
```

Solution : Utiliser `typedef` pour simplifier.

### 2. Confondre déclaration et instanciation

```c
struct Point {
    int x, y;
};  // ← C'est juste la définition du type

struct Point p;  // ← Ici on crée une variable
```

### 3. Accéder à un membre inexistant

```c
struct Point {
    int x, y;
};

struct Point p;
p.z = 5;  // ❌ Erreur : 'z' n'existe pas
```

### 4. Mélanger struct et union

```c
// Union : un seul membre valide à la fois
union Valeur {
    int i;
    float f;
};

union Valeur v;
v.i = 42;
v.f = 3.14;
printf("%d\n", v.i);  // ⚠️ Valeur invalide !
```

---

## Ressources complémentaires

Pour approfondir vos connaissances :

### Documentation

- **ISO C Standard** : Spécifications officielles
- **man pages** : `man 3 struct`, `man 3 union`
- **Norme C11** : Nouvelles fonctionnalités (unions anonymes, etc.)

### Outils

- **pahole** : Analyser le padding des structures
- **gdb** : Inspecter les structures en mémoire
- **sizeof()** et **offsetof()** : Vérifier la disposition

### Projets d'exemple

Au fil du chapitre, vous verrez des exemples de :
- Gestion d'étudiants (structures)
- Machine à états (enums)
- Protocoles réseau (bit-fields, packed structures)
- Système de fichiers (unions tagged)
- Configuration système (mélange de tous les types)

---

## Prêt à commencer ?

Maintenant que vous avez une vue d'ensemble du chapitre, vous êtes prêt à découvrir le premier et le plus important mécanisme : **les structures** (struct).

Les structures sont la fondation de l'organisation des données en C. Elles vous permettront de :
- Modéliser des entités du monde réel
- Passer des données complexes aux fonctions
- Créer des abstractions puissantes
- Organiser votre code de manière professionnelle

**Passons à la section 10.1 : struct - Création d'objets complexes** où vous apprendrez tout ce qu'il faut savoir pour maîtriser les structures en C.

---

> 💡 **Conseil** : N'hésitez pas à revenir à cette introduction si vous vous sentez perdu dans les sections suivantes. Elle vous rappellera la vue d'ensemble et comment chaque concept s'inscrit dans le tableau général.

**Objectif du chapitre 10** : À la fin de ce chapitre, vous serez capable de créer des types de données sophistiqués et d'organiser votre code comme un développeur C professionnel. Les types définis par l'utilisateur sont l'un des outils les plus puissants du langage C !

⏭️ [struct : Création d'objets complexes](/10-types-utilisateur/01-structures.md)
