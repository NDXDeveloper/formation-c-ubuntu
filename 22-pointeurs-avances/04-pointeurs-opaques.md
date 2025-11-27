🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 22.4 Pointeurs opaques

## Introduction

Les **pointeurs opaques** (ou "opaque pointers" en anglais) sont une technique de programmation qui permet de masquer les détails d'implémentation d'une structure de données. C'est un concept fondamental pour créer des **API propres** et **maintenables** en C.

### Qu'est-ce qu'un pointeur opaque ?

Un pointeur opaque est un pointeur vers une structure dont la définition complète n'est **pas visible** par le code client. Le client sait que la structure existe, mais ne connaît pas son contenu interne.

**Analogie** : Imaginez une télécommande de télévision. Vous savez comment l'utiliser (appuyer sur les boutons), mais vous ne voyez pas les circuits électroniques à l'intérieur. La télécommande est "opaque" : son interface est visible, mais son implémentation interne est cachée. Si le fabricant décide de changer les composants internes, cela n'affecte pas votre façon de l'utiliser.

**Autre analogie** : C'est comme une boîte noire. Vous pouvez interagir avec la boîte via des interfaces définies (fonctions), mais vous ne pouvez pas voir ou manipuler directement ce qu'il y a à l'intérieur.

### Pourquoi utiliser des pointeurs opaques ?

Les pointeurs opaques offrent plusieurs avantages majeurs :

1. **Encapsulation** : Cache les détails d'implémentation
2. **Stabilité d'API** : Les modifications internes n'affectent pas le code client
3. **Sécurité** : Le client ne peut pas accéder directement aux champs internes
4. **Flexibilité** : Permet de changer l'implémentation sans recompiler les clients
5. **Abstraction** : Sépare l'interface de l'implémentation

---

## Comprendre le problème

### Approche classique (sans opacité)

Imaginons une structure classique exposée dans un header :

**fichier : personne.h**
```c
#ifndef PERSONNE_H
#define PERSONNE_H

// ❌ Structure entièrement visible
typedef struct {
    char nom[50];
    char prenom[50];
    int age;
    float salaire;  // Information sensible !
} Personne;

// Fonctions publiques
Personne* creer_personne(const char *nom, const char *prenom, int age);
void afficher_personne(Personne *p);
void liberer_personne(Personne *p);

#endif
```

**fichier : main.c**
```c
#include "personne.h"
#include <stdio.h>

int main(void) {
    Personne *p = creer_personne("Dupont", "Jean", 30);

    // ❌ PROBLÈME : Le client peut accéder directement aux champs !
    printf("Salaire : %.2f\n", p->salaire);
    p->salaire = 999999.99;  // Modification non contrôlée !
    p->age = -5;  // Valeur invalide !

    liberer_personne(p);
    return 0;
}
```

**Problèmes de cette approche** :
- ⚠️ Pas d'encapsulation : tout est accessible
- ⚠️ Validation impossible : le client peut mettre des valeurs invalides
- ⚠️ Couplage fort : changer la structure nécessite de recompiler tous les clients
- ⚠️ Pas de contrôle : impossible de logger les accès, vérifier les permissions, etc.

---

## Solution : Pointeurs opaques

### Déclaration forward (forward declaration)

La technique des pointeurs opaques repose sur la **déclaration forward** :

**fichier : personne.h** (Interface publique)
```c
#ifndef PERSONNE_H
#define PERSONNE_H

// ✅ Déclaration forward : on dit que la structure existe,
// mais on ne révèle pas son contenu
typedef struct Personne Personne;

// Fonctions publiques (API)
Personne* creer_personne(const char *nom, const char *prenom, int age);
void definir_salaire(Personne *p, float salaire);
float obtenir_salaire(const Personne *p);
int obtenir_age(const Personne *p);
void afficher_personne(const Personne *p);
void liberer_personne(Personne *p);

#endif
```

**Points clés** :
- Le client voit seulement `typedef struct Personne Personne;`
- Il sait qu'un type `Personne` existe
- Il ne connaît **pas** les champs de la structure
- Il ne peut **manipuler** que via des pointeurs

### Définition complète (dans le fichier .c)

**fichier : personne.c** (Implémentation privée)
```c
#include "personne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ✅ Définition complète : visible UNIQUEMENT dans ce fichier
struct Personne {
    char nom[50];
    char prenom[50];
    int age;
    float salaire;
    int id_employe;  // Peut changer sans affecter les clients !
};

Personne* creer_personne(const char *nom, const char *prenom, int age) {
    Personne *p = malloc(sizeof(Personne));
    if (p == NULL) {
        return NULL;
    }

    strncpy(p->nom, nom, sizeof(p->nom) - 1);
    p->nom[sizeof(p->nom) - 1] = '\0';

    strncpy(p->prenom, prenom, sizeof(p->prenom) - 1);
    p->prenom[sizeof(p->prenom) - 1] = '\0';

    p->age = age;
    p->salaire = 0.0f;
    p->id_employe = 0;

    return p;
}

void definir_salaire(Personne *p, float salaire) {
    if (p != NULL && salaire >= 0) {
        p->salaire = salaire;
        printf("[LOG] Salaire modifié pour %s %s\n", p->prenom, p->nom);
    }
}

float obtenir_salaire(const Personne *p) {
    if (p == NULL) {
        return 0.0f;
    }
    return p->salaire;
}

int obtenir_age(const Personne *p) {
    return (p != NULL) ? p->age : 0;
}

void afficher_personne(const Personne *p) {
    if (p != NULL) {
        printf("Personne : %s %s, %d ans\n", p->prenom, p->nom, p->age);
    }
}

void liberer_personne(Personne *p) {
    free(p);
}
```

### Utilisation par le client

**fichier : main.c**
```c
#include "personne.h"
#include <stdio.h>

int main(void) {
    Personne *p = creer_personne("Dupont", "Jean", 30);
    if (p == NULL) {
        return 1;
    }

    // ✅ Accès contrôlé via des fonctions
    definir_salaire(p, 45000.0f);

    printf("Âge : %d\n", obtenir_age(p));
    printf("Salaire : %.2f\n", obtenir_salaire(p));

    afficher_personne(p);

    // ❌ IMPOSSIBLE : p->age n'est pas accessible !
    // printf("%d\n", p->age);  // Erreur de compilation

    // ❌ IMPOSSIBLE : sizeof(*p) ne compile pas
    // int taille = sizeof(*p);  // Erreur : type incomplet

    liberer_personne(p);
    return 0;
}
```

**Compilation** :
```bash
gcc -c personne.c -o personne.o
gcc -c main.c -o main.o
gcc personne.o main.o -o programme
```

**Avantages observés** :
- ✅ Le client **ne peut pas** accéder directement aux champs
- ✅ Toute modification passe par des fonctions qui peuvent **valider**
- ✅ On peut **logger** ou **auditer** les accès
- ✅ Changer `struct Personne` ne nécessite **pas** de recompiler `main.c`

---

## Exemples réels dans la bibliothèque standard

### FILE* : Le pointeur opaque le plus connu

Le type `FILE` est un pointeur opaque très utilisé en C :

```c
#include <stdio.h>

int main(void) {
    // FILE est un type opaque défini dans <stdio.h>
    FILE *fichier = fopen("test.txt", "w");
    if (fichier == NULL) {
        return 1;
    }

    // ✅ On utilise des fonctions pour interagir avec FILE
    fprintf(fichier, "Hello World\n");
    fclose(fichier);

    // ❌ On ne peut PAS faire :
    // fichier->buffer = ...;  // Les champs ne sont pas visibles !

    return 0;
}
```

**Pourquoi `FILE` est opaque ?**
- L'implémentation de `FILE` varie selon le système (Linux, Windows, macOS)
- Les détails internes (buffers, descripteurs de fichiers) sont complexes
- L'opacité permet de changer l'implémentation sans casser le code existant

### DIR* : Manipulation de répertoires

```c
#include <dirent.h>
#include <stdio.h>

int main(void) {
    // DIR est aussi un type opaque
    DIR *repertoire = opendir(".");
    if (repertoire == NULL) {
        return 1;
    }

    struct dirent *entree;
    while ((entree = readdir(repertoire)) != NULL) {
        printf("%s\n", entree->d_name);
    }

    closedir(repertoire);
    return 0;
}
```

---

## Pattern complet : Bibliothèque avec types opaques

Créons une bibliothèque complète pour gérer une pile (stack) avec un pointeur opaque.

### Interface publique (stack.h)

```c
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

// Déclaration forward : type opaque
typedef struct Stack Stack;

// API publique
Stack* stack_create(void);
void stack_destroy(Stack *s);
bool stack_push(Stack *s, int value);
bool stack_pop(Stack *s, int *value);
bool stack_peek(const Stack *s, int *value);
bool stack_is_empty(const Stack *s);
int stack_size(const Stack *s);

#endif
```

### Implémentation (stack.c)

```c
#include "stack.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 10

// Définition complète : privée
struct Stack {
    int *data;
    int size;
    int capacity;
};

Stack* stack_create(void) {
    Stack *s = malloc(sizeof(Stack));
    if (s == NULL) {
        return NULL;
    }

    s->data = malloc(INITIAL_CAPACITY * sizeof(int));
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    s->size = 0;
    s->capacity = INITIAL_CAPACITY;

    return s;
}

void stack_destroy(Stack *s) {
    if (s != NULL) {
        free(s->data);
        free(s);
    }
}

static bool stack_resize(Stack *s) {
    int new_capacity = s->capacity * 2;
    int *new_data = realloc(s->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return false;
    }

    s->data = new_data;
    s->capacity = new_capacity;
    return true;
}

bool stack_push(Stack *s, int value) {
    if (s == NULL) {
        return false;
    }

    if (s->size >= s->capacity) {
        if (!stack_resize(s)) {
            return false;
        }
    }

    s->data[s->size++] = value;
    return true;
}

bool stack_pop(Stack *s, int *value) {
    if (s == NULL || s->size == 0) {
        return false;
    }

    if (value != NULL) {
        *value = s->data[--s->size];
    } else {
        s->size--;
    }

    return true;
}

bool stack_peek(const Stack *s, int *value) {
    if (s == NULL || s->size == 0 || value == NULL) {
        return false;
    }

    *value = s->data[s->size - 1];
    return true;
}

bool stack_is_empty(const Stack *s) {
    return (s == NULL || s->size == 0);
}

int stack_size(const Stack *s) {
    return (s != NULL) ? s->size : 0;
}
```

### Utilisation

```c
#include "stack.h"
#include <stdio.h>

int main(void) {
    Stack *ma_pile = stack_create();
    if (ma_pile == NULL) {
        fprintf(stderr, "Erreur de création\n");
        return 1;
    }

    // Ajout d'éléments
    stack_push(ma_pile, 10);
    stack_push(ma_pile, 20);
    stack_push(ma_pile, 30);

    printf("Taille : %d\n", stack_size(ma_pile));

    // Consultation du sommet
    int valeur;
    if (stack_peek(ma_pile, &valeur)) {
        printf("Sommet : %d\n", valeur);
    }

    // Retrait d'éléments
    while (!stack_is_empty(ma_pile)) {
        if (stack_pop(ma_pile, &valeur)) {
            printf("Dépilé : %d\n", valeur);
        }
    }

    stack_destroy(ma_pile);
    return 0;
}
```

**Sortie** :
```
Taille : 3
Sommet : 30
Dépilé : 30
Dépilé : 20
Dépilé : 10
```

---

## Avantages des pointeurs opaques

### 1. Encapsulation et sécurité

```c
// ✅ AVANT (avec pointeur opaque) :
// Pour modifier le salaire, on DOIT passer par la fonction
definir_salaire(p, 50000.0f);
// La fonction peut valider, logger, etc.

// ❌ SANS pointeur opaque :
// Accès direct, pas de contrôle
p->salaire = -1000.0f;  // Valeur invalide acceptée !
```

### 2. Stabilité de l'ABI (Application Binary Interface)

Si vous modifiez la structure interne :

```c
// Version 1
struct Personne {
    char nom[50];
    int age;
};

// Version 2 : ajout d'un champ
struct Personne {
    char nom[50];
    int age;
    char email[100];  // Nouveau champ
    int id;           // Nouveau champ
};
```

**Avec pointeurs opaques** :
- ✅ Les clients n'ont **pas besoin** d'être recompilés
- ✅ L'ABI reste stable
- ✅ Compatibilité binaire préservée

**Sans pointeurs opaques** :
- ❌ Tous les clients doivent être recompilés
- ❌ `sizeof(Personne)` change
- ❌ Risque de plantages si mélange d'anciennes/nouvelles versions

### 3. Flexibilité d'implémentation

Vous pouvez changer radicalement l'implémentation :

```c
// Version 1 : tableau statique
struct Stack {
    int data[100];
    int size;
};

// Version 2 : tableau dynamique (meilleur !)
struct Stack {
    int *data;
    int size;
    int capacity;
};

// Version 3 : liste chaînée (encore différent !)
struct Stack {
    struct Node *top;
    int size;
};
```

Les clients ne voient aucune différence, l'API reste identique !

### 4. Possibilité de remplacer l'implémentation au runtime

```c
// Différentes implémentations de "Base de données"
struct DatabaseSQLite { /* ... */ };
struct DatabasePostgres { /* ... */ };

// Le client voit juste :
typedef struct Database Database;

Database* db_create(const char *type) {
    if (strcmp(type, "sqlite") == 0) {
        return (Database*)create_sqlite_db();
    } else {
        return (Database*)create_postgres_db();
    }
}
```

---

## Inconvénients et limitations

### 1. Allocation dynamique obligatoire

```c
// ❌ IMPOSSIBLE : on ne connaît pas sizeof(Personne)
Personne p;  // Erreur : type incomplet

// ✅ On DOIT utiliser un pointeur et malloc
Personne *p = creer_personne(...);
```

**Impact** :
- ⚠️ Performance : allocation heap plus lente que stack
- ⚠️ Gestion mémoire : risque de fuites
- ⚠️ Indirection : un niveau de pointeur supplémentaire

### 2. Pas d'accès direct (overhead fonctionnel)

```c
// Sans opacité (rapide)
int age = p.age;  // Accès direct

// Avec opacité (plus lent)
int age = obtenir_age(p);  // Appel de fonction
```

Pour du code **très** critique en performance, cela peut être un problème.

### 3. API plus verbeuse

```c
// Sans opacité
p.x = 10;
p.y = 20;

// Avec opacité
point_set_x(p, 10);
point_set_y(p, 20);
```

### 4. Debugging plus difficile

Dans GDB, vous ne pouvez pas inspecter directement la structure :

```bash
(gdb) print *p
# Erreur : type incomplet

# Solution : charger le fichier source d'implémentation
(gdb) file personne.c
(gdb) print *p
```

---

## Variantes et techniques avancées

### Pointeur opaque partiel

On peut révéler certains champs et en cacher d'autres :

```c
// Header public
typedef struct {
    int id;          // ✅ Public : visible
    char nom[50];    // ✅ Public : visible
    void *privee;    // ❌ Privé : pointeur vers données cachées
} Personne;
```

### Handle générique (void*)

Certaines API anciennes utilisent `void*` comme handle opaque :

```c
void* contexte_creer(void);
void contexte_utiliser(void *contexte);
void contexte_detruire(void *contexte);
```

**Inconvénient** : Perte de type-safety (aucune vérification de type).

### Pointeur opaque avec identification

```c
#define MAGIC_STACK 0x53544B00  // "STK" en hexa

struct Stack {
    unsigned int magic;  // Pour vérifier la validité
    int *data;
    int size;
    int capacity;
};

bool stack_push(Stack *s, int value) {
    if (s == NULL || s->magic != MAGIC_STACK) {
        // Handle invalide détecté !
        return false;
    }
    // ...
}
```

---

## Pattern : PIMPL (Pointer to Implementation)

Le pattern PIMPL (ou "compilation firewall") est très proche des pointeurs opaques :

```c
// public.h
typedef struct Widget Widget;
Widget* widget_create(void);
void widget_do_something(Widget *w);

// private_impl.h (optionnel, pour l'implémentation)
struct WidgetImpl {
    int data;
    void *more_data;
};

// widget.c
struct Widget {
    struct WidgetImpl *impl;  // Pointeur vers implémentation
};

Widget* widget_create(void) {
    Widget *w = malloc(sizeof(Widget));
    w->impl = malloc(sizeof(struct WidgetImpl));
    // ...
    return w;
}
```

**Avantage** : Encore plus de flexibilité, car même la taille de `Widget` est fixe.

---

## Bonnes pratiques

### ✅ À FAIRE

1. **Conventions de nommage cohérentes**
```c
// Préfixe pour toutes les fonctions liées au type
Stack* stack_create(void);
void stack_destroy(Stack *s);
bool stack_push(Stack *s, int value);
```

2. **Toujours valider les paramètres**
```c
int stack_size(const Stack *s) {
    if (s == NULL) {
        return 0;  // Valeur par défaut sûre
    }
    return s->size;
}
```

3. **Utiliser const pour les fonctions de lecture**
```c
// ✅ const : la fonction ne modifie pas la structure
int stack_size(const Stack *s);
bool stack_peek(const Stack *s, int *value);

// ✅ Sans const : la fonction peut modifier
bool stack_push(Stack *s, int value);
```

4. **Documenter l'API**
```c
/**
 * Crée une nouvelle pile vide.
 *
 * @return Pointeur vers la pile créée, ou NULL en cas d'erreur.
 *         L'appelant est responsable de libérer la mémoire avec stack_destroy().
 */
Stack* stack_create(void);
```

5. **Fonctions de création/destruction symétriques**
```c
Type* type_create(void);   // Allocation
void type_destroy(Type *t); // Libération
```

### ❌ À ÉVITER

1. **Exposer sizeof dans le header**
```c
// ❌ BAD : révèle la taille interne
#define PERSONNE_SIZE 128

// ✅ GOOD : garder cela privé
```

2. **Retourner la structure par valeur**
```c
// ❌ Impossible avec type opaque
Personne obtenir_personne(void);  // Ne compile pas !

// ✅ Retourner un pointeur
Personne* obtenir_personne(void);
```

3. **Mélanger opaque et non-opaque**
```c
// ⚠️ Confusion : choisis un style
typedef struct {
    int x;  // Public
    void *opaque;  // Semi-opaque
} Mixte;
```

---

## Comparaison avec la POO

Les pointeurs opaques sont la façon du C de faire de l'**encapsulation** :

| Concept | C (pointeurs opaques) | C++ | Java |
|---------|----------------------|-----|------|
| Encapsulation | Types opaques | `private` | `private` |
| Interface publique | Fonctions dans .h | Méthodes `public` | Méthodes `public` |
| Implémentation | Définition dans .c | Classe .cpp | Classe .java |
| Polymorphisme | Pointeurs de fonctions | Héritage virtuel | Interfaces |

**Exemple équivalent en C++** :
```cpp
// personne.h
class Personne {
public:
    Personne(const char *nom, int age);
    ~Personne();
    int getAge() const;
    void setAge(int age);

private:  // ← Équivalent à l'opacité en C
    char nom[50];
    int age;
};
```

---

## Cas d'usage recommandés

### ✅ Utiliser des pointeurs opaques pour :

1. **Bibliothèques publiques**
   - Vous créez une lib que d'autres vont utiliser
   - Vous voulez stabilité et rétrocompatibilité

2. **APIs système ou bas niveau**
   - Abstraction de ressources système (fichiers, sockets, threads)
   - Encapsulation de détails spécifiques à la plateforme

3. **Structures de données complexes**
   - Implémentation peut changer (liste → arbre → hash table)
   - Détails internes complexes à cacher

4. **Sécurité et validation**
   - Données sensibles (cryptographie, authentification)
   - Besoin de contrôler tous les accès

### ❌ Ne pas utiliser pour :

1. **Structures de données simples et stables**
   - Point 2D avec x, y → pas besoin d'opacité
   - Rectangle avec width, height → inutilement complexe

2. **Code interne à un projet**
   - Si vous contrôlez tous les clients
   - Overhead sans bénéfice

3. **Performance critique**
   - Calculs mathématiques intensifs
   - Traitement d'images pixel par pixel
   - L'indirection peut coûter cher

4. **Petits projets personnels**
   - Complexité supplémentaire sans avantage réel

---

## Exemples de bibliothèques utilisant des pointeurs opaques

### GTK+ (Interface graphique)

```c
#include <gtk/gtk.h>

// GtkWidget est un type opaque
GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_title(GTK_WINDOW(window), "Ma fenêtre");
gtk_widget_show(window);
```

### OpenSSL (Cryptographie)

```c
#include <openssl/ssl.h>

// SSL_CTX est opaque
SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
// ...
SSL_CTX_free(ctx);
```

### SQLite (Base de données)

```c
#include <sqlite3.h>

// sqlite3 est opaque
sqlite3 *db;
sqlite3_open("database.db", &db);
// ...
sqlite3_close(db);
```

### Pourquoi ces bibliothèques utilisent des pointeurs opaques ?

- ✅ Évolution de l'API sans casser la compatibilité
- ✅ Portabilité (implémentation différente selon OS)
- ✅ Sécurité (pas d'accès direct aux données sensibles)
- ✅ Abstraction de la complexité

---

## Résumé

Les **pointeurs opaques** sont une technique fondamentale pour créer des API robustes en C.

**Principe** :
- Déclaration forward dans le header : `typedef struct Type Type;`
- Définition complète dans le .c : `struct Type { /* champs */ };`
- Client manipule uniquement via des pointeurs et des fonctions

**Avantages** :
- ✅ **Encapsulation** : masque les détails d'implémentation
- ✅ **Stabilité** : modifications internes sans recompilation clients
- ✅ **Sécurité** : contrôle des accès aux données
- ✅ **Flexibilité** : changement d'implémentation transparent

**Inconvénients** :
- ⚠️ Allocation dynamique obligatoire
- ⚠️ Overhead d'appels de fonctions
- ⚠️ API plus verbeuse
- ⚠️ Debugging plus difficile

**Quand utiliser** :
- Bibliothèques publiques
- APIs système
- Structures complexes évolutives
- Besoin de sécurité/validation

**Quand éviter** :
- Structures simples et stables
- Code haute performance
- Petits projets internes

**Pattern clé** :
```c
// header.h
typedef struct Type Type;
Type* type_create(void);
void type_destroy(Type *t);

// source.c
struct Type {
    // Implémentation privée
};
```

Les pointeurs opaques sont la base de la programmation orientée objet en C, permettant d'appliquer les principes SOLID même sans langage orienté objet !

---


⏭️ [Fonctions variadiques](/22-pointeurs-avances/05-fonctions-variadiques.md)
