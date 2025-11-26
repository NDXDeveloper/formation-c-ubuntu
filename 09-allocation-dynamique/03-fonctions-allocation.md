🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.3 Les fonctions malloc, calloc, realloc et free

## Introduction

L'allocation dynamique de mémoire est l'une des fonctionnalités les plus puissantes du langage C. Elle vous permet de demander de la mémoire **au moment de l'exécution** (runtime) plutôt qu'à la compilation.

Le langage C fournit quatre fonctions principales pour gérer l'allocation dynamique :
- **`malloc()`** : Allouer de la mémoire brute
- **`calloc()`** : Allouer de la mémoire initialisée à zéro
- **`realloc()`** : Redimensionner une zone mémoire existante
- **`free()`** : Libérer la mémoire allouée

Ces fonctions sont déclarées dans l'en-tête `<stdlib.h>`.

---

## malloc() - Allocation de base

### Prototype

```c
void* malloc(size_t size);
```

### Description

`malloc()` (Memory ALLOCation) alloue un bloc de mémoire de **`size` octets** sur le Heap et retourne un pointeur vers le début de ce bloc.

### Caractéristiques importantes

| Aspect | Détail |
|--------|--------|
| **Retour** | Pointeur `void*` vers la mémoire allouée |
| **En cas d'échec** | Retourne `NULL` |
| **Initialisation** | ⚠️ La mémoire n'est **PAS initialisée** (contient des "déchets") |
| **Alignement** | Garanti pour tous les types standards |

### Syntaxe de base

```c
#include <stdlib.h>

int main() {
    // Allouer de la mémoire pour un entier
    int* ptr = (int*)malloc(sizeof(int));

    if (ptr == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return 1;
    }

    // Utilisation
    *ptr = 42;
    printf("Valeur : %d\n", *ptr);

    // Libération
    free(ptr);

    return 0;
}
```

### Pourquoi utiliser sizeof() ?

**❌ Mauvaise pratique :**
```c
int* ptr = malloc(4);  // Taille codée en dur - non portable !
```

**✅ Bonne pratique :**
```c
int* ptr = malloc(sizeof(int));  // Portable, s'adapte à la plateforme
```

**Raison :** La taille d'un `int` peut varier selon les systèmes (16, 32 ou 64 bits).

### Visualisation de malloc()

```
Avant malloc(sizeof(int)) :

HEAP :
┌────────────────────────────────┐
│      (mémoire libre)           │
└────────────────────────────────┘


Après malloc(sizeof(int)) :

HEAP :
┌────────────────────────────────┐
│ [Métadonnées allocateur]       │
├────────────────────────────────┤
│ [Zone allouée: 4 octets]       │ ← ptr pointe ici
│ (contenu indéfini: ????????)   │
├────────────────────────────────┤
│      (mémoire libre)           │
└────────────────────────────────┘
```

### Allocation d'un tableau

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int taille = 10;

    // Allouer un tableau de 10 entiers
    int* tableau = (int*)malloc(taille * sizeof(int));

    if (tableau == NULL) {
        printf("Erreur d'allocation\n");
        return 1;
    }

    // Initialisation manuelle nécessaire
    for (int i = 0; i < taille; i++) {
        tableau[i] = i * 10;
    }

    // Affichage
    for (int i = 0; i < taille; i++) {
        printf("tableau[%d] = %d\n", i, tableau[i]);
    }

    // Libération
    free(tableau);

    return 0;
}
```

### Allocation de structures

```c
typedef struct {
    char nom[50];
    int age;
    double salaire;
} Personne;

int main() {
    // Allouer une structure
    Personne* p = (Personne*)malloc(sizeof(Personne));

    if (p == NULL) {
        return 1;
    }

    // Utilisation
    strcpy(p->nom, "Alice");
    p->age = 30;
    p->salaire = 45000.0;

    printf("%s a %d ans\n", p->nom, p->age);

    free(p);
    return 0;
}
```

### ⚠️ Attention : Mémoire non initialisée

```c
int* ptr = malloc(sizeof(int));

// ❌ DANGER : *ptr contient une valeur aléatoire !
printf("%d\n", *ptr);  // Affiche n'importe quoi

// ✅ CORRECT : Initialiser avant d'utiliser
*ptr = 0;
printf("%d\n", *ptr);  // Affiche 0
```

---

## calloc() - Allocation avec initialisation

### Prototype

```c
void* calloc(size_t nmemb, size_t size);
```

### Description

`calloc()` (Contiguous ALLOCation) alloue de la mémoire pour un **tableau de `nmemb` éléments** de `size` octets chacun, et **initialise tous les octets à zéro**.

### Différences avec malloc()

| Aspect | malloc() | calloc() |
|--------|----------|----------|
| **Paramètres** | 1 (taille totale) | 2 (nombre + taille élément) |
| **Initialisation** | ❌ Non initialisé | ✅ Tout à zéro |
| **Performance** | Légèrement plus rapide | Légèrement plus lent |
| **Usage typique** | Données simples | Tableaux, structures |

### Syntaxe

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int taille = 5;

    // Allouer un tableau de 5 entiers, tous initialisés à 0
    int* tableau = (int*)calloc(taille, sizeof(int));

    if (tableau == NULL) {
        printf("Erreur d'allocation\n");
        return 1;
    }

    // Affichage : tous les éléments valent 0
    for (int i = 0; i < taille; i++) {
        printf("tableau[%d] = %d\n", i, tableau[i]);
    }

    free(tableau);
    return 0;
}
```

**Sortie :**
```
tableau[0] = 0
tableau[1] = 0
tableau[2] = 0
tableau[3] = 0
tableau[4] = 0
```

### Équivalence malloc() vs calloc()

Ces deux codes sont **équivalents** :

**Avec malloc() :**
```c
int* tableau = (int*)malloc(10 * sizeof(int));
if (tableau != NULL) {
    memset(tableau, 0, 10 * sizeof(int));  // Initialisation manuelle
}
```

**Avec calloc() :**
```c
int* tableau = (int*)calloc(10, sizeof(int));  // Initialisation automatique
```

### Visualisation de calloc()

```
Après calloc(5, sizeof(int)) :

HEAP :
┌────────────────────────────────┐
│ [Métadonnées allocateur]       │
├────────────────────────────────┤
│ [0] [0] [0] [0] [0]            │ ← 5 int initialisés à 0
│  ↑                             │
│  ptr                           │
├────────────────────────────────┤
│      (mémoire libre)           │
└────────────────────────────────┘
```

### Quand utiliser calloc() ?

✅ **Utilisez calloc() quand :**
- Vous avez besoin d'un tableau initialisé à zéro
- Vous travaillez avec des structures contenant des pointeurs (NULL = 0)
- La sécurité est critique (éviter les valeurs aléatoires)

✅ **Utilisez malloc() quand :**
- Vous allez immédiatement remplir la mémoire
- La performance est critique (gain marginal)
- Vous n'avez besoin que d'un seul élément

### Exemple pratique : Matrice

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int lignes = 3, colonnes = 4;

    // Allouer une matrice 3x4 initialisée à 0
    int** matrice = (int**)calloc(lignes, sizeof(int*));
    if (matrice == NULL) return 1;

    for (int i = 0; i < lignes; i++) {
        matrice[i] = (int*)calloc(colonnes, sizeof(int));
        if (matrice[i] == NULL) {
            // Libérer ce qui a été alloué
            for (int j = 0; j < i; j++) {
                free(matrice[j]);
            }
            free(matrice);
            return 1;
        }
    }

    // Affichage : toute la matrice est à 0
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%d ", matrice[i][j]);
        }
        printf("\n");
    }

    // Libération
    for (int i = 0; i < lignes; i++) {
        free(matrice[i]);
    }
    free(matrice);

    return 0;
}
```

---

## realloc() - Redimensionnement

### Prototype

```c
void* realloc(void* ptr, size_t size);
```

### Description

`realloc()` (REALLOCation) modifie la taille d'un bloc mémoire précédemment alloué avec `malloc()`, `calloc()` ou `realloc()`.

### Comportements possibles

| Cas | Comportement |
|-----|--------------|
| **`ptr == NULL`** | Équivalent à `malloc(size)` |
| **`size == 0`** | Équivalent à `free(ptr)` (comportement dépendant de l'implémentation) |
| **Agrandissement** | Étend le bloc (peut déplacer en mémoire) |
| **Réduction** | Réduit le bloc (généralement sur place) |

### Syntaxe de base

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocation initiale : 5 entiers
    int* tableau = (int*)malloc(5 * sizeof(int));
    if (tableau == NULL) return 1;

    // Initialisation
    for (int i = 0; i < 5; i++) {
        tableau[i] = i;
    }

    // Redimensionnement : passer à 10 entiers
    int* nouveau_tableau = (int*)realloc(tableau, 10 * sizeof(int));

    if (nouveau_tableau == NULL) {
        // ⚠️ En cas d'échec, l'ancien tableau est toujours valide !
        free(tableau);
        return 1;
    }

    // Mettre à jour le pointeur
    tableau = nouveau_tableau;

    // Initialiser les nouveaux éléments
    for (int i = 5; i < 10; i++) {
        tableau[i] = i;
    }

    // Affichage
    for (int i = 0; i < 10; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");

    free(tableau);
    return 0;
}
```

### ⚠️ ATTENTION : Ne jamais faire ceci !

```c
// ❌ CODE DANGEREUX - NE PAS FAIRE !
int* tableau = malloc(5 * sizeof(int));
tableau = realloc(tableau, 10 * sizeof(int));  // ERREUR !

// Si realloc échoue et retourne NULL, vous perdez
// la référence au bloc original → FUITE MÉMOIRE !
```

**✅ Toujours utiliser un pointeur temporaire :**

```c
int* tableau = malloc(5 * sizeof(int));
int* temp = realloc(tableau, 10 * sizeof(int));

if (temp == NULL) {
    // L'ancien tableau existe toujours
    free(tableau);
    return 1;
}

tableau = temp;  // Mise à jour seulement si succès
```

### Visualisation de realloc()

**Cas 1 : Extension sur place (rare)**

```
Avant realloc() :
┌────────────────────────────────┐
│ [Bloc A: 5 int]                │
├────────────────────────────────┤
│ (espace libre)                 │
└────────────────────────────────┘

Après realloc() pour 10 int :
┌────────────────────────────────┐
│ [Bloc A étendu: 10 int]        │ ← Même adresse
│ [5 int ancien] [5 int nouveau] │
├────────────────────────────────┤
│ (espace libre)                 │
└────────────────────────────────┘
```

**Cas 2 : Déplacement en mémoire (fréquent)**

```
Avant realloc() :
┌────────────────────────────────┐
│ [Bloc A: 5 int]                │
├────────────────────────────────┤
│ [Bloc B occupé]                │ ← Pas de place pour étendre
├────────────────────────────────┤
│ (espace libre)                 │
└────────────────────────────────┘

Après realloc() pour 10 int :
┌────────────────────────────────┐
│ [Ancien bloc libéré]           │
├────────────────────────────────┤
│ [Bloc B occupé]                │
├────────────────────────────────┤
│ [Nouveau bloc: 10 int]         │ ← NOUVELLE adresse
│ (données copiées + extension)  │
├────────────────────────────────┤
│ (espace libre)                 │
└────────────────────────────────┘
```

### Exemple : Liste dynamique

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int capacite = 2;
    int taille = 0;
    int* liste = (int*)malloc(capacite * sizeof(int));

    if (liste == NULL) return 1;

    // Ajout d'éléments avec redimensionnement automatique
    for (int i = 0; i < 10; i++) {
        // Vérifier si on doit agrandir
        if (taille == capacite) {
            capacite *= 2;  // Doubler la capacité

            int* temp = (int*)realloc(liste, capacite * sizeof(int));
            if (temp == NULL) {
                free(liste);
                return 1;
            }
            liste = temp;

            printf("Redimensionné à capacité %d\n", capacite);
        }

        liste[taille++] = i;
    }

    // Affichage
    printf("Liste finale : ");
    for (int i = 0; i < taille; i++) {
        printf("%d ", liste[i]);
    }
    printf("\n");

    free(liste);
    return 0;
}
```

**Sortie :**
```
Redimensionné à capacité 4
Redimensionné à capacité 8
Redimensionné à capacité 16
Liste finale : 0 1 2 3 4 5 6 7 8 9
```

### Réduction de taille

```c
int* tableau = malloc(100 * sizeof(int));

// Utilisation...

// Réduire à 50 éléments pour économiser la mémoire
int* temp = realloc(tableau, 50 * sizeof(int));
if (temp != NULL) {
    tableau = temp;
}
// Si realloc échoue pour une réduction, ce n'est généralement pas grave
// On garde simplement le bloc plus grand
```

---

## free() - Libération de mémoire

### Prototype

```c
void free(void* ptr);
```

### Description

`free()` libère un bloc de mémoire précédemment alloué par `malloc()`, `calloc()` ou `realloc()`.

### Règles cruciales

| Règle | Description |
|-------|-------------|
| **Obligation** | Toute mémoire allouée DOIT être libérée |
| **Une seule fois** | Ne jamais appeler `free()` deux fois sur le même pointeur |
| **Pointeur valide** | Ne libérer QUE des pointeurs retournés par malloc/calloc/realloc |
| **Après free()** | Le pointeur devient invalide (dangling pointer) |

### Syntaxe de base

```c
int* ptr = malloc(sizeof(int));

if (ptr != NULL) {
    *ptr = 42;
    // ... utilisation ...

    free(ptr);  // Libération
    ptr = NULL; // Bonne pratique : éviter les dangling pointers
}
```

### Visualisation de free()

```
Avant free() :
┌────────────────────────────────┐
│ [Métadonnées]                  │
├────────────────────────────────┤
│ [Zone allouée: utilisée]       │ ← ptr pointe ici
├────────────────────────────────┤
│ (mémoire libre)                │
└────────────────────────────────┘

Après free(ptr) :
┌────────────────────────────────┐
│ (zone rendue disponible)       │ ← ptr pointe toujours ici !
│ (peut être réutilisée)         │    MAIS c'est invalide
├────────────────────────────────┤
│ (mémoire libre)                │
└────────────────────────────────┘

Après ptr = NULL :
┌────────────────────────────────┐
│ (zone rendue disponible)       │
├────────────────────────────────┤
│ (mémoire libre)                │
└────────────────────────────────┘
                ptr → NULL (sécurisé)
```

### free() avec NULL

```c
int* ptr = NULL;
free(ptr);  // ✅ SÉCURITAIRE : free(NULL) ne fait rien
```

**Comportement normalisé :** Appeler `free(NULL)` est parfaitement valide et ne fait rien.

### Exemple : Libération d'un tableau

```c
int* tableau = malloc(10 * sizeof(int));

if (tableau != NULL) {
    // Utilisation...

    free(tableau);
    tableau = NULL;
}
```

### Exemple : Libération d'une structure avec pointeurs

```c
typedef struct {
    char* nom;
    char* prenom;
    int age;
} Personne;

Personne* creer_personne(const char* nom, const char* prenom, int age) {
    Personne* p = malloc(sizeof(Personne));
    if (p == NULL) return NULL;

    p->nom = malloc(strlen(nom) + 1);
    p->prenom = malloc(strlen(prenom) + 1);

    if (p->nom == NULL || p->prenom == NULL) {
        free(p->nom);
        free(p->prenom);
        free(p);
        return NULL;
    }

    strcpy(p->nom, nom);
    strcpy(p->prenom, prenom);
    p->age = age;

    return p;
}

void liberer_personne(Personne* p) {
    if (p != NULL) {
        free(p->nom);      // Libérer les membres d'abord
        free(p->prenom);
        free(p);           // Puis la structure elle-même
    }
}

int main() {
    Personne* p = creer_personne("Dupont", "Marie", 25);

    if (p != NULL) {
        printf("%s %s a %d ans\n", p->prenom, p->nom, p->age);
        liberer_personne(p);
    }

    return 0;
}
```

---

## Gestion des erreurs

### Toujours vérifier le retour

**❌ Code dangereux :**
```c
int* ptr = malloc(sizeof(int));
*ptr = 42;  // ⚠️ CRASH si malloc a échoué !
```

**✅ Code correct :**
```c
int* ptr = malloc(sizeof(int));
if (ptr == NULL) {
    fprintf(stderr, "Erreur : mémoire insuffisante\n");
    return 1;
}
*ptr = 42;
```

### Vérifier errno

```c
#include <errno.h>
#include <string.h>

int* ptr = malloc(1000000000 * sizeof(int));  // Demande énorme
if (ptr == NULL) {
    fprintf(stderr, "Erreur malloc : %s\n", strerror(errno));
    return 1;
}
```

### Pattern de gestion robuste

```c
#include <stdio.h>
#include <stdlib.h>

void* malloc_securise(size_t taille, const char* message) {
    void* ptr = malloc(taille);
    if (ptr == NULL) {
        fprintf(stderr, "Erreur d'allocation : %s\n", message);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

int main() {
    int* tableau = malloc_securise(10 * sizeof(int),
                                    "allocation du tableau");

    // Utilisation garantie : pas de vérification nécessaire
    for (int i = 0; i < 10; i++) {
        tableau[i] = i;
    }

    free(tableau);
    return 0;
}
```

---

## Erreurs courantes et comment les éviter

### ❌ Erreur 1 : Double Free

**Problème :**
```c
int* ptr = malloc(sizeof(int));
free(ptr);
free(ptr);  // ❌ ERREUR : Double free → Corruption de mémoire
```

**Solution :**
```c
int* ptr = malloc(sizeof(int));
free(ptr);
ptr = NULL;  // Bonne pratique

free(ptr);   // ✅ OK : free(NULL) ne fait rien
```

### ❌ Erreur 2 : Use After Free

**Problème :**
```c
int* ptr = malloc(sizeof(int));
*ptr = 42;
free(ptr);

printf("%d\n", *ptr);  // ❌ ERREUR : Lecture après libération
*ptr = 100;            // ❌ ERREUR : Écriture après libération
```

**Solution :**
```c
int* ptr = malloc(sizeof(int));
*ptr = 42;
printf("%d\n", *ptr);  // Utiliser AVANT free()
free(ptr);
ptr = NULL;            // Empêcher l'utilisation accidentelle
```

### ❌ Erreur 3 : Memory Leak (Fuite mémoire)

**Problème :**
```c
void fonction() {
    int* ptr = malloc(100 * sizeof(int));
    // ... code ...
    // ❌ Oubli de free(ptr) !
}  // La mémoire est perdue définitivement
```

**Solution :**
```c
void fonction() {
    int* ptr = malloc(100 * sizeof(int));
    if (ptr == NULL) return;

    // ... code ...

    free(ptr);  // ✅ Toujours libérer avant de sortir
}
```

### ❌ Erreur 4 : Perte du pointeur

**Problème :**
```c
int* ptr = malloc(100 * sizeof(int));
ptr = malloc(50 * sizeof(int));  // ❌ Le premier bloc est perdu !
```

**Solution :**
```c
int* ptr = malloc(100 * sizeof(int));
free(ptr);  // Libérer l'ancien bloc d'abord
ptr = malloc(50 * sizeof(int));  // Puis allouer le nouveau
```

### ❌ Erreur 5 : Libérer un pointeur non alloué

**Problème :**
```c
int tableau[10];
free(tableau);  // ❌ ERREUR : tableau est sur la Stack !

int* ptr;
free(ptr);      // ❌ ERREUR : ptr non initialisé !
```

**Solution :**
```c
// Ne libérer QUE les pointeurs retournés par malloc/calloc/realloc
int* ptr = malloc(sizeof(int));
if (ptr != NULL) {
    free(ptr);  // ✅ OK
}
```

---

## Bonnes pratiques

### 1. Toujours vérifier NULL

```c
int* ptr = malloc(sizeof(int));
if (ptr == NULL) {
    // Gérer l'erreur
    return 1;
}
```

### 2. Utiliser sizeof() sur le type

```c
// ✅ Bon
int* ptr = malloc(sizeof(int));

// ✅ Encore mieux (évite la duplication du type)
int* ptr = malloc(sizeof(*ptr));
```

### 3. Initialiser à NULL après free()

```c
free(ptr);
ptr = NULL;  // Évite les dangling pointers
```

### 4. Pattern de libération en cas d'erreur

```c
char* buffer1 = malloc(100);
if (buffer1 == NULL) goto cleanup;

char* buffer2 = malloc(200);
if (buffer2 == NULL) goto cleanup;

char* buffer3 = malloc(300);
if (buffer3 == NULL) goto cleanup;

// ... code principal ...

cleanup:
    free(buffer3);
    free(buffer2);
    free(buffer1);
    return status;
```

### 5. Macro pour allocation sécurisée

```c
#define SAFE_MALLOC(ptr, size) do { \
    (ptr) = malloc(size); \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Erreur malloc ligne %d\n", __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

// Utilisation
int* tableau;
SAFE_MALLOC(tableau, 10 * sizeof(int));
```

### 6. Valgrind pour détecter les erreurs

```bash
# Compiler avec symboles de debug
gcc -g -o programme programme.c

# Détecter les fuites et erreurs
valgrind --leak-check=full --show-leak-kinds=all ./programme
```

---

## Tableau récapitulatif

| Fonction | Usage | Initialisation | Nombre de paramètres |
|----------|-------|----------------|----------------------|
| **malloc** | Allocation basique | ❌ Non | 1 (taille totale) |
| **calloc** | Allocation + init à zéro | ✅ Oui (zéros) | 2 (nombre, taille) |
| **realloc** | Redimensionnement | ⚠️ Partielle (extension non initialisée) | 2 (pointeur, nouvelle taille) |
| **free** | Libération | N/A | 1 (pointeur) |

---

## Exemple complet : Gestionnaire de tableau dynamique

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int* data;
    size_t taille;
    size_t capacite;
} TableauDynamique;

// Créer un tableau dynamique
TableauDynamique* creer_tableau(size_t capacite_initiale) {
    TableauDynamique* tab = malloc(sizeof(TableauDynamique));
    if (tab == NULL) return NULL;

    tab->data = malloc(capacite_initiale * sizeof(int));
    if (tab->data == NULL) {
        free(tab);
        return NULL;
    }

    tab->taille = 0;
    tab->capacite = capacite_initiale;

    return tab;
}

// Ajouter un élément
int ajouter_element(TableauDynamique* tab, int valeur) {
    if (tab == NULL) return 0;

    // Redimensionner si nécessaire
    if (tab->taille == tab->capacite) {
        size_t nouvelle_capacite = tab->capacite * 2;
        int* nouveau_data = realloc(tab->data, nouvelle_capacite * sizeof(int));

        if (nouveau_data == NULL) {
            return 0;  // Échec
        }

        tab->data = nouveau_data;
        tab->capacite = nouvelle_capacite;

        printf("Tableau redimensionné : capacité %zu\n", tab->capacite);
    }

    tab->data[tab->taille++] = valeur;
    return 1;
}

// Afficher le tableau
void afficher_tableau(const TableauDynamique* tab) {
    if (tab == NULL) return;

    printf("Tableau [taille=%zu, capacité=%zu] : ", tab->taille, tab->capacite);
    for (size_t i = 0; i < tab->taille; i++) {
        printf("%d ", tab->data[i]);
    }
    printf("\n");
}

// Libérer le tableau
void liberer_tableau(TableauDynamique* tab) {
    if (tab != NULL) {
        free(tab->data);
        free(tab);
    }
}

int main() {
    TableauDynamique* tab = creer_tableau(2);

    if (tab == NULL) {
        fprintf(stderr, "Erreur de création\n");
        return 1;
    }

    // Ajouter des éléments
    for (int i = 0; i < 10; i++) {
        if (!ajouter_element(tab, i * 10)) {
            fprintf(stderr, "Erreur d'ajout\n");
            liberer_tableau(tab);
            return 1;
        }
        afficher_tableau(tab);
    }

    // Nettoyage
    liberer_tableau(tab);

    return 0;
}
```

**Sortie :**
```
Tableau [taille=1, capacité=2] : 0
Tableau [taille=2, capacité=2] : 0 10
Tableau redimensionné : capacité 4
Tableau [taille=3, capacité=4] : 0 10 20
Tableau [taille=4, capacité=4] : 0 10 20 30
Tableau redimensionné : capacité 8
Tableau [taille=5, capacité=8] : 0 10 20 30 40
...
```

---

## Différences importantes entre les fonctions

### malloc vs calloc : Quel choisir ?

**Utilisez malloc :**
```c
// Vous allez immédiatement remplir les données
int* buffer = malloc(1000 * sizeof(int));
for (int i = 0; i < 1000; i++) {
    buffer[i] = i;  // Initialisation immédiate
}
```

**Utilisez calloc :**
```c
// Vous voulez des zéros ou une initialisation différée
int* flags = calloc(1000, sizeof(int));
// Tous les flags sont à 0 (false)

// Plus tard...
flags[42] = 1;  // Activer un flag spécifique
```

### realloc : Cas particuliers

**Comportement avec NULL :**
```c
int* ptr = NULL;
ptr = realloc(ptr, 100);  // ✅ Équivalent à malloc(100)
```

**Comportement avec taille 0 :**
```c
int* ptr = malloc(100);
ptr = realloc(ptr, 0);  // ⚠️ Comportement dépendant de l'implémentation
// Sur certains systèmes : équivalent à free(ptr)
// Préférez explicitement free() pour la clarté
```

---

## Détection et débogage

### Outils essentiels

**1. Valgrind :**
```bash
valgrind --leak-check=full --track-origins=yes ./programme
```

**2. AddressSanitizer (ASan) :**
```bash
gcc -fsanitize=address -g programme.c -o programme
./programme
```

**3. Exemple de rapport Valgrind :**
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 40 bytes in 1 blocks
==12345==   total heap usage: 2 allocs, 1 frees, 140 bytes allocated
==12345==
==12345== 40 bytes in 1 blocks are definitely lost
==12345==    at malloc (vg_replace_malloc.c:380)
==12345==    by main (programme.c:15)
```

---

## Récapitulatif

### Points clés à retenir

1. **malloc()** : Allocation brute, non initialisée
2. **calloc()** : Allocation initialisée à zéro
3. **realloc()** : Redimensionnement (peut déplacer en mémoire)
4. **free()** : Libération obligatoire de toute mémoire allouée

### Règles d'or

- ✅ **Toujours vérifier si le retour est NULL**
- ✅ **Toujours appeler free() pour chaque malloc/calloc**
- ✅ **Ne jamais utiliser un pointeur après free()**
- ✅ **Mettre les pointeurs à NULL après free()**
- ✅ **Utiliser Valgrind pour détecter les fuites**
- ✅ **Préférer sizeof(*ptr) à sizeof(type)**

### Erreurs à éviter

- ❌ Double free
- ❌ Use after free
- ❌ Memory leak (oubli de free)
- ❌ Perte du pointeur
- ❌ Buffer overflow
- ❌ Utiliser un pointeur sans vérifier NULL

---

## Pour aller plus loin

### Prochaines sections recommandées

- **Section 9.4** : Gestion des erreurs d'allocation
- **Section 9.5** : Fuites mémoire : causes et prévention
- **Section 9.6** : Double free et corruption de tas
- **Module 5, Section 15.4** : Détection de fuites avec Valgrind

### Man pages à consulter

```bash
man 3 malloc
man 3 calloc
man 3 realloc
man 3 free
```

### Ressources externes

- Standard C11 : Section 7.22.3 (Memory management functions)
- CERT C Coding Standard : MEM rules
- Valgrind User Manual

---


⏭️ [Gestion des erreurs d'allocation](/09-allocation-dynamique/04-gestion-erreurs.md)
