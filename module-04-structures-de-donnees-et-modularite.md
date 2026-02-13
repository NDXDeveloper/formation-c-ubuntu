🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Module 4 : Structures de Données et Modularité

> **Formation : Maîtriser C sur Ubuntu - De la Syntaxe au Système**
> Niveau : **Intermédiaire** • Durée estimée : **25-35 heures de lecture et expérimentation**

---

## 🎯 Objectifs du module

Vous maîtrisez désormais les pointeurs et la gestion mémoire (Module 3). Il est temps de **construire des abstractions** et d'**organiser votre code** de manière professionnelle.

Ce module vous apprend à créer des programmes complexes, bien structurés et maintenables. Vous passerez de scripts monolithiques à des projets modulaires multi-fichiers.

À l'issue de ce module, vous serez capable de :

1. ✅ **Créer vos propres types** avec `struct`, `union`, `enum`, `typedef`
2. ✅ **Optimiser la mémoire** avec l'alignement et le padding
3. ✅ **Implémenter les structures de données classiques** (listes, piles, files, arbres, tables de hachage)
4. ✅ **Organiser un projet multi-fichiers** avec headers et compilation séparée
5. ✅ **Gérer les dépendances** et éviter les includes circulaires
6. ✅ **Écrire du code maintenable** suivant les conventions professionnelles

---

## 🧩 Place dans la formation globale

### D'où venez-vous ?

**Module 3** vous a donné les outils pour manipuler la mémoire. Vous savez :
- Créer des pointeurs et naviguer entre adresses
- Allouer dynamiquement de la mémoire
- Éviter les bugs mémoire classiques

**Mais vous êtes limité à :**
- Des types primitifs (`int`, `char`, `float`...)
- Des tableaux de types simples
- Des fichiers sources uniques qui deviennent vite ingérables

### Où allez-vous ?

Ce module transforme ces limitations en possibilités :

**Types personnalisés** → Représenter des concepts métiers complexes
```c
struct User {
    char name[50];
    int age;
    char email[100];
};
```

**Structures de données dynamiques** → Gérer des collections de taille variable
```c
struct Node {
    int data;
    struct Node *next;  // Liste chaînée
};
```

**Modularité** → Diviser votre code en modules réutilisables
```
project/
├── src/
│   ├── user.c      ← Implémentation
│   └── database.c
├── include/
│   ├── user.h      ← Interface publique
│   └── database.h
└── main.c
```

### Pourquoi ce module est essentiel

**Sans les compétences de ce module, vous ne pouvez pas :**
- Écrire de vrais programmes (> 500 lignes)
- Travailler en équipe sur un projet C
- Créer des bibliothèques réutilisables
- Maintenir du code sur le long terme
- Implémenter des algorithmes complexes

**Ce module vous fait passer de "codeur" à "ingénieur logiciel".**

---

## 📚 Structure du module

Ce module est organisé en **trois chapitres complémentaires** qui construisent progressivement votre capacité à structurer du code :

### 🏗️ [Chapitre 10 : Types définis par l'utilisateur](10-types-utilisateur/README.md)
**Contenu** : 7 sections • **Niveau** : Intermédiaire • **Durée** : 8-10 heures

**Vous découvrirez :**
- `struct` : Créer des objets complexes avec plusieurs champs
- Alignement mémoire et padding (comprendre `sizeof(struct)`)
- Packed structures pour optimiser l'espace
- `union` : Plusieurs représentations pour la même mémoire
- `enum` : Rendre le code lisible avec des constantes nommées
- `typedef` : Simplifier la syntaxe des types complexes
- Bit-fields : Contrôler finement la mémoire au niveau du bit

**Concepts clés :**
```c
// Structure : grouper des données liées
struct Point {
    int x;
    int y;
};

// Union : économiser de la mémoire
union Data {
    int i;
    float f;
    char str[20];
};  // Taille = max(sizeof(int), sizeof(float), 20)

// Enum : lisibilité
enum Status {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_PENDING = 1
};

// Typedef : simplifier
typedef struct {
    char name[50];
    int age;
} Person;  // Plus besoin de "struct Person"

// Bit-fields : économiser de l'espace
struct Flags {
    unsigned int is_active : 1;  // 1 bit seulement
    unsigned int priority : 3;    // 3 bits (0-7)
    unsigned int reserved : 28;   // Le reste
};
```

**Pourquoi c'est important :** Les types personnalisés sont la base de tout programme structuré. Comprendre l'alignement mémoire est crucial pour :
- Optimiser les performances (cache-friendly structures)
- Interagir avec le matériel (embedded, drivers)
- Comprendre les protocoles réseau (paquets)
- Déboguer les problèmes de padding

**Focus sur le padding :**
```c
struct Example {
    char c;    // 1 byte
    // 3 bytes de padding !
    int i;     // 4 bytes
    char d;    // 1 byte
    // 3 bytes de padding !
};  // sizeof = 12 bytes, pas 6 !

// Optimisé :
struct Optimized {
    int i;     // 4 bytes
    char c;    // 1 byte
    char d;    // 1 byte
    // 2 bytes de padding
};  // sizeof = 8 bytes
```

---

### 🔗 [Chapitre 11 : Structures de données dynamiques](11-structures-dynamiques/README.md)
**Contenu** : 7 sections • **Niveau** : Intermédiaire-Avancé • **Durée** : 12-15 heures

**Vous maîtriserez :**
- Listes chaînées simples (insertion, suppression, traversée)
- Listes doublement chaînées
- Piles et Files (LIFO, FIFO)
- Tables de hachage (fonction de hachage, gestion des collisions)
- Arbres binaires (BST, parcours)
- Critères de choix d'une structure de données
- Gestion mémoire des structures dynamiques (éviter les fuites)

**Concepts clés :**
```c
// Liste chaînée
struct Node {
    int data;
    struct Node *next;
};

typedef struct {
    struct Node *head;
    size_t size;
} LinkedList;

void list_append(LinkedList *list, int value) {
    struct Node *new_node = malloc(sizeof(struct Node));
    new_node->data = value;
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = new_node;
    } else {
        struct Node *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    list->size++;
}

// Table de hachage (simplifiée)
#define TABLE_SIZE 100

typedef struct Entry {
    char *key;
    void *value;
    struct Entry *next;  // Chaînage pour collisions
} Entry;

typedef struct {
    Entry *buckets[TABLE_SIZE];
} HashTable;

// Arbre binaire
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;
```

**Complexité algorithmique :**
| Structure | Accès | Recherche | Insertion | Suppression | Mémoire |
|-----------|-------|-----------|-----------|-------------|---------|
| Tableau | O(1) | O(n) | O(n) | O(n) | Contigu |
| Liste chaînée | O(n) | O(n) | O(1)* | O(1)* | Fragmenté |
| Pile/File | O(1) | - | O(1) | O(1) | Variable |
| Table hachage | - | O(1)** | O(1)** | O(1)** | Extra |
| Arbre binaire | O(log n)*** | O(log n)*** | O(log n)*** | O(log n)*** | Pointeurs |

\* À la tête de liste
\*\* Amortisé, dépend de la fonction de hachage
\*\*\* Si équilibré

**Pourquoi c'est important :** Choisir la bonne structure de données peut faire la différence entre un algorithme O(n) et O(log n). C'est souvent plus impactant que l'optimisation du code.

**Gestion mémoire critique :**
Chaque `malloc` doit avoir son `free` correspondant. Pour une liste chaînée :
```c
void list_destroy(LinkedList *list) {
    struct Node *current = list->head;
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp);  // Libérer chaque nœud
    }
    list->head = NULL;
    list->size = 0;
}
```

---

### 📦 [Chapitre 12 : Organisation du code et Compilation modulaire](12-organisation-code/README.md)
**Contenu** : 6 sections • **Niveau** : Intermédiaire • **Durée** : 8-10 heures

**Vous apprendrez :**
- Fichiers d'en-tête (.h) et gardes d'inclusion
- La portée des variables (static, extern)
- Compilation séparée (fichiers .c compilés indépendamment)
- Organisation standard d'un projet C
- Conventions de nommage professionnelles
- Forward declarations et dépendances circulaires

**Concepts clés :**
```c
// user.h - Interface publique
#ifndef USER_H
#define USER_H

typedef struct {
    char name[50];
    int age;
} User;

// Prototypes des fonctions publiques
User* user_create(const char *name, int age);  
void user_destroy(User *user);  
void user_print(const User *user);

#endif  // USER_H

// user.c - Implémentation
#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction privée (static = visible uniquement dans ce fichier)
static void validate_age(int age) {
    if (age < 0 || age > 150) {
        fprintf(stderr, "Invalid age\n");
        exit(1);
    }
}

User* user_create(const char *name, int age) {
    validate_age(age);
    User *user = malloc(sizeof(User));
    strncpy(user->name, name, sizeof(user->name) - 1);
    user->age = age;
    return user;
}
```

**Organisation standard d'un projet :**
```
my_project/
├── include/          ← Headers publics
│   ├── mylib.h
│   └── utils.h
├── src/              ← Implémentations
│   ├── mylib.c
│   ├── utils.c
│   └── internal.h    ← Headers privés
├── tests/            ← Tests unitaires
│   └── test_mylib.c
├── build/            ← Fichiers compilés (généré)
├── CMakeLists.txt    ← Configuration CMake
├── Makefile          ← Ou Makefile
└── README.md
```

**Compilation séparée :**
```bash
# Compiler chaque fichier séparément en .o
gcc -c -I./include src/mylib.c -o build/mylib.o  
gcc -c -I./include src/utils.c -o build/utils.o  
gcc -c -I./include src/main.c -o build/main.o

# Lier tous les .o ensemble
gcc build/mylib.o build/utils.o build/main.o -o my_program

# Avantage : recompiler seulement les fichiers modifiés
```

**Pourquoi c'est important :** Sans modularité, votre code devient :
- ❌ Impossible à maintenir au-delà de 1000 lignes
- ❌ Difficile à tester
- ❌ Lent à compiler (tout recompiler à chaque changement)
- ❌ Impossible à réutiliser dans d'autres projets
- ❌ Source de conflits en équipe

Avec une bonne organisation :
- ✅ Code séparé par responsabilité
- ✅ Compilation incrémentale rapide
- ✅ Tests unitaires par module
- ✅ Réutilisation facile
- ✅ Collaboration fluide

---

## 🎓 Prérequis

### Absolument nécessaire
- ✅ **Module 3 complété** : Maîtrise des pointeurs et allocation dynamique
- ✅ **Gestion mémoire** : `malloc`/`free` sans fuites
- ✅ **Pointeurs de pointeurs** : Comprendre `int **ptr`
- ✅ **Structures de contrôle** : Boucles et conditions maîtrisées

### Fortement recommandé
- ✅ Expérience avec **Valgrind** pour détecter les fuites
- ✅ Maîtrise de **GDB** pour déboguer
- ✅ Compréhension de la **compilation séparée** (Module 1)

### Pas nécessaire à ce stade
- ❌ Programmation système (Module 6)
- ❌ Threads et concurrence (Module 6)
- ❌ Optimisation avancée (Module 7)

---

## 🗺️ Stratégie d'apprentissage recommandée

### Approche progressive pour ce module

#### 1. **Maîtrisez les types avant les structures de données**
Ne passez pas au Chapitre 11 avant de bien comprendre le Chapitre 10. Les structures de données utilisent intensivement `struct` et pointeurs de structures.

#### 2. **Dessinez les structures de données**
Visualisez chaque structure avant de coder :
```
Liste chaînée :
[data|next] -> [data|next] -> [data|next] -> NULL

Arbre binaire :
        5
       / \
      3   7
     / \
    1   4
```

#### 3. **Testez avec Valgrind systématiquement**
Les structures de données dynamiques sont propices aux fuites :
```bash
# Workflow obligatoire
gcc -g structures.c -o structures  
valgrind --leak-check=full ./structures
```

#### 4. **Implémentez les structures vous-même**
Ne vous contentez pas de lire. Implémentez au moins :
- Une liste chaînée complète (insert, delete, search, destroy)
- Une table de hachage simple
- Un arbre binaire de recherche

#### 5. **Organisez dès le début**
Même pour de petits programmes, pratiquez la modularité :
```
test_project/
├── include/
│   └── list.h
├── src/
│   └── list.c
└── main.c
```

---

## 🛠️ Outils et workflow pour ce module

### Configuration de projet modulaire

```bash
# Créer la structure
mkdir -p ~/c-learning/module-04/{include,src,tests,build}  
cd ~/c-learning/module-04

# Créer un Makefile simple
cat > Makefile << 'EOF'  
CC = gcc  
CFLAGS = -Wall -Wextra -Werror -I./include -g  
SRC = $(wildcard src/*.c)  
OBJ = $(SRC:src/%.c=build/%.o)

all: program

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

program: $(OBJ)
	$(CC) $(OBJ) -o $@

clean:
	rm -rf build/*.o program

.PHONY: all clean
EOF
```

### Vérification de l'alignement mémoire
```c
#include <stdio.h>
#include <stddef.h>

struct Example {
    char c;
    int i;
    char d;
};

int main(void) {
    printf("Size: %zu\n", sizeof(struct Example));
    printf("Offset c: %zu\n", offsetof(struct Example, c));
    printf("Offset i: %zu\n", offsetof(struct Example, i));
    printf("Offset d: %zu\n", offsetof(struct Example, d));
    return 0;
}
```

### Visualisation des structures de données
```bash
# Utiliser GDB pour inspecter
gdb ./program
(gdb) break main
(gdb) run
(gdb) print *list
(gdb) print list->head->next->data
```

---

## ⚠️ Pièges courants et solutions

### 🚨 Piège 1 : Oublier de libérer les structures imbriquées
```c
struct Person {
    char *name;  // Alloué dynamiquement
    int age;
};

// ❌ Fuite mémoire
void person_destroy(struct Person *p) {
    free(p);  // name n'est pas libéré !
}

// ✅ Correct
void person_destroy(struct Person *p) {
    free(p->name);  // Libérer d'abord les membres
    free(p);        // Puis la structure
}
```

### 🚨 Piège 2 : Modifier une structure passée par valeur
```c
void modify_user(struct User user) {
    strcpy(user.name, "New Name");  // ❌ Modifie une copie !
}

// ✅ Passer un pointeur
void modify_user(struct User *user) {
    strcpy(user->name, "New Name");
}
```

### 🚨 Piège 3 : Comparer des structures avec ==
```c
struct Point p1 = {1, 2};  
struct Point p2 = {1, 2};

if (p1 == p2) { }  // ❌ Erreur de compilation !

// ✅ Comparer membre par membre
if (p1.x == p2.x && p1.y == p2.y) { }

// Ou écrire une fonction
int point_equal(struct Point *a, struct Point *b) {
    return a->x == b->x && a->y == b->y;
}
```

### 🚨 Piège 4 : Includes circulaires
```c
// a.h
#include "b.h"
struct A { struct B *b; };

// b.h
#include "a.h"  // ❌ Dépendance circulaire !
struct B { struct A *a; };

// ✅ Solution : Forward declaration
// a.h
struct B;  // Forward declaration  
struct A { struct B *b; };

// b.h
struct A;  // Forward declaration  
struct B { struct A *a; };
```

### 🚨 Piège 5 : Oublier les gardes d'inclusion
```c
// header.h sans gardes
struct Point { int x, y; };

// Si header.h est inclus 2 fois → erreur de redéfinition !

// ✅ Toujours utiliser les gardes
#ifndef HEADER_H
#define HEADER_H

struct Point { int x, y; };

#endif
```

### 🚨 Piège 6 : Perdre des nœuds lors de la suppression
```c
// ❌ Fuite mémoire
void list_delete_node(Node **head, int value) {
    Node *current = *head;
    Node *prev = NULL;

    while (current && current->data != value) {
        prev = current;
        current = current->next;
    }

    if (current) {
        if (prev) {
            prev->next = current->next;
        } else {
            *head = current->next;
        }
        // Oubli de free(current) !
    }
}

// ✅ Correct
if (current) {
    if (prev) {
        prev->next = current->next;
    } else {
        *head = current->next;
    }
    free(current);  // Libérer le nœud
}
```

---

## 🏆 Critères de maîtrise

### ✅ Niveau 1 : Types personnalisés
- [ ] Créer des structures complexes avec pointeurs
- [ ] Expliquer le padding et optimiser les structures
- [ ] Utiliser les unions pour économiser de la mémoire
- [ ] Créer des enums pour améliorer la lisibilité
- [ ] Maîtriser typedef et simplifier la syntaxe

### ✅ Niveau 2 : Structures de données
- [ ] Implémenter une liste chaînée complète (CRUD)
- [ ] Créer une pile ou une file avec allocation dynamique
- [ ] Comprendre et implémenter une table de hachage
- [ ] Implémenter un arbre binaire de recherche
- [ ] Choisir la structure appropriée selon le cas d'usage
- [ ] Gérer la mémoire sans fuites (Valgrind clean)

### ✅ Niveau 3 : Modularité
- [ ] Organiser un projet multi-fichiers
- [ ] Écrire des headers avec gardes d'inclusion
- [ ] Utiliser static pour l'encapsulation
- [ ] Gérer les forward declarations
- [ ] Compiler de manière séparée
- [ ] Suivre les conventions de nommage

### ✅ Niveau 4 : Pratique professionnelle
- [ ] Créer une bibliothèque réutilisable
- [ ] Documenter les interfaces publiques
- [ ] Séparer interface et implémentation
- [ ] Gérer les dépendances entre modules
- [ ] Écrire du code testable
- [ ] Mesurer la complexité algorithmique

---

## 📊 Progression attendue

### Semaine 1-2 (Types personnalisés)
- ✅ Structures simples et complexes
- ✅ Compréhension du padding
- ✅ Unions et enums maîtrisés
- ✅ Typedef utilisé naturellement

### Semaine 3-4 (Structures de données)
- ✅ Liste chaînée implémentée de A à Z
- ✅ Compréhension des complexités O(n), O(1), O(log n)
- ✅ Premier arbre binaire fonctionnel
- ✅ Aucune fuite mémoire détectée

### Semaine 5 (Modularité)
- ✅ Projet organisé en modules
- ✅ Compilation séparée maîtrisée
- ✅ Headers propres avec documentation
- ✅ Code réutilisable et maintenable

---

## 📚 Ressources complémentaires

### Livres de référence
- **Data Structures Using C** (Reema Thareja) - Chapitre 6-10
- **Mastering Algorithms with C** (Kyle Loudon) - Partie II
- **The C Programming Language** (K&R) - Chapitre 6
- **C Interfaces and Implementations** (David R. Hanson) - Tout le livre

### Documentation en ligne
- [GeeksforGeeks - Data Structures in C](https://www.geeksforgeeks.org/data-structures/)
- [Visualgo](https://visualgo.net/) - Visualisation des structures de données
- [Big-O Cheat Sheet](https://www.bigocheatsheet.com/) - Complexités

### Code open-source à étudier
- [glib](https://github.com/GNOME/glib) - Collections en C (GList, GHashTable...)
- [uthash](https://github.com/troydhanson/uthash) - Table de hachage en C
- [Redis](https://github.com/redis/redis) - Structures de données en production

---

## 💡 Conseils pour réussir ce module

### 1. **Implémentez, ne copiez pas**
Taper du code trouvé sur internet n'apprend rien. Implémentez vous-même chaque structure.

### 2. **Testez la complexité**
Mesurez le temps d'exécution avec des données de tailles croissantes :
```c
#include <time.h>

clock_t start = clock();
// ... opération ...
clock_t end = clock();  
double time = (double)(end - start) / CLOCKS_PER_SEC;  
printf("Temps: %f secondes\n", time);
```

### 3. **Utilisez des assertions**
```c
#include <assert.h>

void list_insert(List *list, int value) {
    assert(list != NULL);  // Détecte les bugs tôt
    // ...
}
```

### 4. **Documentez vos choix**
Pourquoi une liste chaînée plutôt qu'un tableau ? Écrivez-le en commentaire.

### 5. **Refactorisez régulièrement**
Code = première version fonctionne, deuxième version est propre.

---

## 🗓️ Planning de lecture suggéré

### Approche intensive (temps plein)
| Semaine | Contenu | Focus |
|---------|---------|-------|
| **Semaine 1** | Chapitre 10 complet | struct, union, enum, padding |
| **Semaine 2** | Chapitre 11 (11.1-11.3) | Listes chaînées, piles, files |
| **Semaine 3** | Chapitre 11 (11.4-11.7) | Tables hachage, arbres |
| **Semaine 4** | Chapitre 12 complet | Modularité et organisation |

### Approche à temps partiel (10h/semaine)
| Semaine | Contenu |
|---------|---------|
| **Semaines 1-2** | Chapitre 10 |
| **Semaines 3-5** | Chapitre 11 |
| **Semaines 6-7** | Chapitre 12 |

---

## 🔗 Liens avec les autres modules

### Ce module prépare
- **Module 5** : Build systems (CMake pour projets multi-fichiers)
- **Module 6** : Programmation système (structures pour IPC, protocoles)
- **Module 8** : CI/CD (tests unitaires par module)
- **Module 9** : Projets réels (architecture modulaire)

### Ce module utilise
- **Module 3** : Pointeurs et allocation (base des structures dynamiques)
- **Module 2** : Fonctions (organisation du code)
- **Module 1** : Compilation (linking de plusieurs fichiers)

---

## 📑 Prochaines sections

**Construisez vos premières abstractions :**

- **[Chapitre 10 : Types définis par l'utilisateur →](10-types-utilisateur/README.md)**
- [Chapitre 11 : Structures de données dynamiques](11-structures-dynamiques/README.md)
- [Chapitre 12 : Organisation du code et Compilation modulaire](12-organisation-code/README.md)

---

## 🎯 Mot de conclusion

Ce module marque un tournant dans votre apprentissage. Vous passez de **programmes simples** à **systèmes logiciels structurés**.

Les compétences de ce module sont directement applicables en entreprise :
- Organisation de code professionnel
- Structures de données performantes
- Architecture modulaire maintenable

Prenez le temps de bien faire. Un code bien organisé dès le début vous fait gagner des semaines lors de la maintenance.

---

**Bonne conception, et que votre code soit élégant et efficace ! 🚀**

---

## 📝 Note sur l'utilisation

Cette formation est un **guide de référence complet**. Pour ce module :

- **Lisez** les explications sur les types et structures
- **Dessinez** les structures de données avant de coder
- **Implémentez** au moins une fois chaque structure de données
- **Testez** systématiquement avec Valgrind
- **Organisez** vos projets dès le début avec une structure multi-fichiers
- **Consultez** le code open-source pour voir les bonnes pratiques

Le **Module 9** vous permettra d'appliquer tout cela dans des projets complets et réalistes.

⏭️ [Chapitre 10 : Types définis par l'utilisateur →](10-types-utilisateur/README.md)
