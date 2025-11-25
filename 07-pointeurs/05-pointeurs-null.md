🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.5 Pointeurs NULL et dangling pointers

## Introduction

Les pointeurs sont puissants, mais avec cette puissance vient la responsabilité. Deux des problèmes les plus courants et dangereux en C sont les **pointeurs NULL** non vérifiés et les **dangling pointers** (pointeurs pendants). Cette section vous apprendra à reconnaître, éviter et corriger ces problèmes.

## Le pointeur NULL

### Qu'est-ce que NULL ?

**NULL** est une valeur spéciale qui indique qu'un pointeur **ne pointe vers rien**.

```c
#include <stdio.h>

int main(void) {
    int *ptr = NULL;    // Pointeur initialisé à NULL

    printf("Valeur de ptr : %p\n", (void*)ptr);

    return 0;
}
```

**Sortie typique :**
```
Valeur de ptr : (nil)
```
ou
```
Valeur de ptr : 0x0
```

### Définition technique

```c
#define NULL ((void*)0)    // Définition courante dans <stddef.h>
```

NULL est essentiellement l'adresse `0`, qui est une adresse invalide sur tous les systèmes modernes.

### Analogie

Pensez à NULL comme à :
- Une **adresse invalide** dans une rue (numéro 0 qui n'existe pas)
- Un **numéro de téléphone inexistant**
- Une **case vide** dans un formulaire

## Pourquoi utiliser NULL ?

NULL est utilisé pour indiquer plusieurs situations :

1. **Pointeur non initialisé** (de manière sûre)
2. **Absence de valeur** (optionnel)
3. **Erreur** ou **échec** d'une fonction
4. **Fin de structure** (liste chaînée, etc.)

### Exemple : malloc peut retourner NULL

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *tableau = malloc(5 * sizeof(int));

    if (tableau == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée\n");
        return 1;
    }

    // Utilisation sécurisée...
    tableau[0] = 42;

    free(tableau);
    return 0;
}
```

**Point clé :** `malloc` retourne NULL si l'allocation échoue (mémoire insuffisante).

## Vérification de NULL : TOUJOURS nécessaire

### ❌ Code dangereux (sans vérification)

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(sizeof(int));

    *ptr = 42;    // ⚠️ DANGER : si malloc a échoué, ptr est NULL !

    printf("Valeur : %d\n", *ptr);

    free(ptr);
    return 0;
}
```

**Problème :** Si `malloc` échoue, `ptr` est NULL, et déréférencer NULL cause un **segmentation fault** (crash).

### ✅ Code sûr (avec vérification)

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(sizeof(int));

    if (ptr == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return 1;
    }

    *ptr = 42;    // ✓ Sûr : on sait que ptr n'est pas NULL
    printf("Valeur : %d\n", *ptr);

    free(ptr);
    return 0;
}
```

## Idiomes de vérification NULL

### Méthode 1 : Comparaison explicite

```c
if (ptr == NULL) {
    // Gérer l'erreur
}
```

### Méthode 2 : Test implicite

```c
if (!ptr) {    // Équivalent à : if (ptr == NULL)
    // Gérer l'erreur
}
```

### Méthode 3 : Test positif

```c
if (ptr) {    // Équivalent à : if (ptr != NULL)
    // Utiliser le pointeur
}
```

### Exemple avec les trois méthodes

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(sizeof(int));

    // Méthode 1
    if (ptr == NULL) {
        fprintf(stderr, "Allocation échouée\n");
        return 1;
    }

    *ptr = 42;

    // Méthode 3 (vérification avant utilisation)
    if (ptr) {
        printf("Valeur : %d\n", *ptr);
    }

    free(ptr);
    ptr = NULL;    // Bonne pratique

    // Méthode 2 (vérification après free)
    if (!ptr) {
        printf("ptr est NULL, ne peut pas être utilisé\n");
    }

    return 0;
}
```

## Initialisation des pointeurs

### ❌ Pointeur non initialisé (TRÈS DANGEREUX)

```c
int *ptr;    // ⚠️ Contient une valeur aléatoire (garbage)

if (ptr == NULL) {    // ✗ Test inutile : ptr n'est pas NULL, il est aléatoire !
    // ...
}

*ptr = 42;    // ⚠️ CRASH potentiel : écrit à une adresse aléatoire
```

**Problème :** Un pointeur non initialisé ne vaut **pas** NULL, il contient une adresse **aléatoire** (garbage value).

### ✅ Initialisation systématique

```c
int *ptr = NULL;    // ✓ Bonne pratique : toujours initialiser
```

### Visualisation

```
Pointeur non initialisé :
    ptr
┌──────────┐
│ 0x??????? │  ← Adresse aléatoire (dangereuse)
└──────────┘

Pointeur initialisé à NULL :
    ptr
┌──────────┐
│   NULL   │  ← Valeur sûre et testable
└──────────┘
```

## Réinitialiser après free

### ❌ Code dangereux

```c
int *ptr = malloc(sizeof(int));
*ptr = 42;

free(ptr);    // Libère la mémoire

*ptr = 50;    // ⚠️ DANGER : ptr pointe toujours vers l'ancienne adresse !
```

### ✅ Bonne pratique

```c
int *ptr = malloc(sizeof(int));
*ptr = 42;

free(ptr);
ptr = NULL;    // ✓ Réinitialiser à NULL après free

if (ptr) {
    *ptr = 50;    // Ne s'exécute pas car ptr est NULL
}
```

**Règle d'or :** Toujours mettre un pointeur à NULL après `free()`.

## Les dangling pointers (pointeurs pendants)

Un **dangling pointer** est un pointeur qui pointe vers une zone mémoire qui n'est **plus valide**.

### Analogie

Un dangling pointer, c'est comme :
- Avoir l'**adresse d'une maison qui a été démolie**
- Utiliser un **ticket de cinéma périmé**
- Appeler un **numéro de téléphone désactivé**

Le pointeur existe encore, mais ce qu'il pointe a disparu.

## Causes des dangling pointers

### Cause 1 : Pointeur vers une variable locale

```c
#include <stdio.h>

int* fonction_dangereuse(void) {
    int variable_locale = 42;
    return &variable_locale;    // ⚠️ DANGEREUX !
}

int main(void) {
    int *ptr = fonction_dangereuse();

    printf("%d\n", *ptr);    // ⚠️ Comportement indéfini !

    return 0;
}
```

**Problème :** `variable_locale` est détruite quand la fonction se termine. Le pointeur pointe vers une zone mémoire qui n'existe plus.

**Visualisation :**

```
Pendant l'exécution de fonction_dangereuse() :

    Stack de fonction_dangereuse :
    ┌─────────────────┐
    │ variable_locale │  @ 0x1000
    │      42         │
    └─────────────────┘
           ↑
           │
    ptr retourne 0x1000

Après le retour de fonction_dangereuse() :

    Stack (la zone est maintenant invalide) :
    ┌─────────────────┐
    │   ????????      │  @ 0x1000  ← Mémoire réutilisée/corrompue
    └─────────────────┘
           ↑
           │
    ptr pointe toujours ici (DANGEREUX !)
```

### Cause 2 : Mémoire libérée avec free

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    free(ptr);    // Libère la mémoire

    // ptr est maintenant un dangling pointer !

    printf("%d\n", *ptr);    // ⚠️ Comportement indéfini !
    *ptr = 50;               // ⚠️ Corruption possible !

    return 0;
}
```

**Problème :** Après `free()`, la mémoire est retournée au système, mais `ptr` contient toujours l'ancienne adresse.

### Cause 3 : Plusieurs pointeurs vers la même zone

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr1 = malloc(sizeof(int));
    int *ptr2 = ptr1;    // ptr2 pointe vers la même zone que ptr1

    *ptr1 = 42;

    free(ptr1);
    ptr1 = NULL;    // ptr1 est sécurisé

    // Mais ptr2 est maintenant un dangling pointer !
    printf("%d\n", *ptr2);    // ⚠️ DANGER !

    return 0;
}
```

**Problème :** Quand on libère via `ptr1`, `ptr2` devient invalide mais on l'a oublié.

**Visualisation :**

```
Avant free :
    Mémoire allouée
    ┌───────┐
    │  42   │  @ 0x2000
    └───────┘
       ↑  ↑
       │  │
     ptr1 ptr2

Après free(ptr1) et ptr1 = NULL :
    Mémoire libérée (invalide)
    ┌───────┐
    │  ???  │  @ 0x2000
    └───────┘
           ↑
           │
          ptr2  (dangling pointer !)

    ptr1 = NULL ✓
```

### Cause 4 : Réallocation avec realloc

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr1 = malloc(10 * sizeof(int));
    int *ptr2 = ptr1;    // Copie de l'adresse

    ptr1[0] = 42;

    // Réallocation
    ptr1 = realloc(ptr1, 20 * sizeof(int));    // Peut changer l'adresse !

    if (ptr1 != NULL) {
        // Si realloc a déplacé la mémoire, ptr2 est invalide !
        printf("%d\n", *ptr2);    // ⚠️ Potentiellement dangereux !
    }

    free(ptr1);
    return 0;
}
```

**Problème :** `realloc` peut déplacer le bloc mémoire, rendant l'ancienne adresse invalide.

## Conséquences des dangling pointers

### Comportements observables

1. **Segmentation fault** (crash du programme)
2. **Valeurs corrompues** (données incorrectes)
3. **Comportement erratique** (bugs intermittents)
4. **Failles de sécurité** (exploitation possible)
5. **Fonctionnement "normal"** (le pire : le bug est caché !)

### Exemple de corruption silencieuse

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr1 = malloc(sizeof(int));
    *ptr1 = 42;

    free(ptr1);

    // Entre-temps, d'autres allocations peuvent réutiliser la mémoire
    int *ptr2 = malloc(sizeof(int));
    *ptr2 = 100;

    // ptr1 est un dangling pointer qui peut maintenant pointer vers ptr2 !
    printf("*ptr1 = %d\n", *ptr1);    // Peut afficher 100 au lieu de 42 !

    free(ptr2);
    return 0;
}
```

**Sortie possible :**
```
*ptr1 = 100
```

## Comment éviter les dangling pointers

### Règle 1 : Toujours initialiser à NULL

```c
int *ptr = NULL;    // ✓ Initialisation systématique
```

### Règle 2 : Mettre à NULL après free

```c
free(ptr);
ptr = NULL;    // ✓ Empêche la réutilisation accidentelle
```

### Règle 3 : Pattern "free et NULL" en une macro

```c
#define SAFE_FREE(p) do { free(p); (p) = NULL; } while(0)

int main(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    SAFE_FREE(ptr);    // Libère et met à NULL

    // Utilisation accidentelle détectée
    if (ptr) {
        *ptr = 50;    // Ne s'exécute pas
    }

    return 0;
}
```

### Règle 4 : Ne jamais retourner l'adresse d'une variable locale

```c
// ✗ MAUVAIS
int* mauvaise_fonction(void) {
    int local = 42;
    return &local;    // DANGEREUX !
}

// ✓ BON (option 1 : allocation dynamique)
int* bonne_fonction_1(void) {
    int *ptr = malloc(sizeof(int));
    if (ptr) *ptr = 42;
    return ptr;    // L'appelant doit free()
}

// ✓ BON (option 2 : paramètre de sortie)
void bonne_fonction_2(int *resultat) {
    *resultat = 42;
}

int main(void) {
    // Option 1
    int *ptr1 = bonne_fonction_1();
    if (ptr1) {
        printf("%d\n", *ptr1);
        free(ptr1);
    }

    // Option 2
    int valeur;
    bonne_fonction_2(&valeur);
    printf("%d\n", valeur);

    return 0;
}
```

### Règle 5 : Vérifier NULL avant tout déréférencement

```c
if (ptr != NULL) {
    *ptr = 42;    // ✓ Sûr
}
```

### Règle 6 : Documenter la propriété de la mémoire

```c
// Cette fonction alloue de la mémoire.
// L'appelant est responsable de free().
int* creer_entier(int valeur) {
    int *p = malloc(sizeof(int));
    if (p) *p = valeur;
    return p;
}

// Cette fonction ne prend PAS possession du pointeur.
// L'appelant reste responsable.
void afficher_entier(const int *p) {
    if (p) printf("%d\n", *p);
}
```

## Outils de détection

### Valgrind

Valgrind détecte les accès à la mémoire invalide :

```bash
$ gcc -g programme.c -o programme
$ valgrind ./programme
```

**Exemple de sortie Valgrind :**
```
==12345== Invalid read of size 4
==12345==    at 0x40052A: main (programme.c:12)
==12345==  Address 0x5204040 is 0 bytes inside a block of size 4 free'd
```

### AddressSanitizer

Compilez avec `-fsanitize=address` :

```bash
$ gcc -fsanitize=address -g programme.c -o programme
$ ./programme
```

**Exemple de sortie ASan :**
```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free
```

## Double free : un danger connexe

### Qu'est-ce qu'un double free ?

Appeler `free()` deux fois sur le même pointeur :

```c
int *ptr = malloc(sizeof(int));
*ptr = 42;

free(ptr);
free(ptr);    // ⚠️ DOUBLE FREE : corruption du tas !
```

**Conséquence :** Corruption de la structure interne du gestionnaire de mémoire, peut causer :
- Crash immédiat ou différé
- Corruption de données
- Failles de sécurité exploitables

### Solution : NULL après free

```c
int *ptr = malloc(sizeof(int));
*ptr = 42;

free(ptr);
ptr = NULL;

free(ptr);    // ✓ OK : free(NULL) ne fait rien
```

**Point important :** `free(NULL)` est **toujours sûr** et ne fait rien.

## Patterns de code sûr

### Pattern 1 : Propriété claire

```c
// Qui possède ce pointeur ? Qui doit le libérer ?

typedef struct {
    char *nom;    // Cette structure possède le nom
} Personne;

Personne* creer_personne(const char *nom) {
    Personne *p = malloc(sizeof(Personne));
    if (!p) return NULL;

    p->nom = malloc(strlen(nom) + 1);
    if (!p->nom) {
        free(p);
        return NULL;
    }

    strcpy(p->nom, nom);
    return p;
}

void detruire_personne(Personne *p) {
    if (p) {
        free(p->nom);    // Libère ce que la structure possède
        free(p);         // Puis libère la structure
    }
}
```

### Pattern 2 : Vérification défensive

```c
void fonction_sure(int *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Erreur : pointeur NULL\n");
        return;
    }

    // Utilisation sûre
    *ptr = 42;
}
```

### Pattern 3 : RAII-like avec goto cleanup

```c
#include <stdio.h>
#include <stdlib.h>

int fonction_complexe(void) {
    int *buffer1 = NULL;
    int *buffer2 = NULL;
    int resultat = -1;

    buffer1 = malloc(100 * sizeof(int));
    if (!buffer1) goto cleanup;

    buffer2 = malloc(200 * sizeof(int));
    if (!buffer2) goto cleanup;

    // Traitement...
    resultat = 0;    // Succès

cleanup:
    free(buffer2);    // free(NULL) est sûr
    free(buffer1);
    return resultat;
}
```

## Exemple complet : liste chaînée sûre

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Crée un nouveau nœud
Node* creer_node(int valeur) {
    Node *node = malloc(sizeof(Node));
    if (node) {
        node->data = valeur;
        node->next = NULL;
    }
    return node;
}

// Ajoute un nœud à la fin
void ajouter(Node **head, int valeur) {
    if (head == NULL) return;    // Vérification défensive

    Node *nouveau = creer_node(valeur);
    if (nouveau == NULL) return;    // Échec d'allocation

    if (*head == NULL) {
        *head = nouveau;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = nouveau;
    }
}

// Affiche la liste
void afficher(Node *head) {
    Node *current = head;
    printf("Liste : ");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

// Libère toute la liste
void liberer_liste(Node **head) {
    if (head == NULL) return;

    Node *current = *head;
    while (current != NULL) {
        Node *next = current->next;    // Sauvegarder next avant free
        free(current);
        current = next;
    }

    *head = NULL;    // Réinitialiser le pointeur de tête
}

int main(void) {
    Node *liste = NULL;

    ajouter(&liste, 10);
    ajouter(&liste, 20);
    ajouter(&liste, 30);

    afficher(liste);

    liberer_liste(&liste);

    // Vérification après libération
    if (liste == NULL) {
        printf("Liste correctement libérée\n");
    }

    return 0;
}
```

**Sortie :**
```
Liste : 10 20 30
Liste correctement libérée
```

### Points clés de l'exemple

1. **Vérifications NULL** systématiques
2. **Sauvegarde de `next`** avant `free`
3. **Réinitialisation à NULL** après libération
4. **Pointeur vers pointeur** (`Node **head`) pour modifier la tête

## Résumé des erreurs courantes

| Erreur | Description | Solution |
|--------|-------------|----------|
| **Pointeur non initialisé** | `int *ptr;` sans valeur | `int *ptr = NULL;` |
| **Déréférencement de NULL** | `*ptr` quand `ptr == NULL` | Vérifier avant : `if (ptr)` |
| **Use after free** | Utiliser après `free(ptr)` | `ptr = NULL` après `free` |
| **Double free** | `free(ptr); free(ptr);` | `ptr = NULL` après `free` |
| **Retour de variable locale** | `return &local;` | Utiliser malloc ou paramètres |
| **Fuites avec dangling** | Perdre la seule référence | Bien gérer la propriété |

## Bonnes pratiques récapitulatives

✅ **Toujours initialiser** les pointeurs à NULL

✅ **Vérifier NULL** avant tout déréférencement

✅ **Mettre à NULL** après free

✅ **Ne jamais retourner** l'adresse d'une variable locale

✅ **Documenter** qui possède et qui libère la mémoire

✅ **Utiliser** Valgrind et AddressSanitizer pendant le développement

✅ **Pattern SAFE_FREE** pour combiner free et réinitialisation

✅ **Vérifications défensives** dans les fonctions publiques

✅ **free(NULL)** est toujours sûr

## Ce qu'il faut retenir

✅ **NULL** = adresse invalide (0), utilisée pour marquer un pointeur vide

✅ **Dangling pointer** = pointeur vers une zone mémoire invalide

✅ Les causes principales :
   - Mémoire libérée avec `free()`
   - Variable locale détruite
   - Réallocation avec `realloc()`
   - Plusieurs pointeurs vers la même zone

✅ **Toujours** : initialiser à NULL, vérifier avant utilisation, réinitialiser après free

✅ **Double free** est dangereux, évité par réinitialisation à NULL

✅ **free(NULL)** ne fait rien et est parfaitement sûr

✅ Utiliser les outils de détection (Valgrind, ASan) pour trouver les bugs

## Prochaine étape

Maintenant que vous connaissez les pièges des pointeurs NULL et dangling, la prochaine section explorera les **pointeurs constants et const**, qui vous permettront d'écrire du code plus sûr en utilisant les garanties du compilateur.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je sais ce qu'est NULL et comment l'utiliser
- [ ] Je vérifie toujours NULL avant de déréférencer
- [ ] Je comprends ce qu'est un dangling pointer
- [ ] Je connais les causes des dangling pointers
- [ ] Je mets systématiquement NULL après free
- [ ] Je n'utilise jamais l'adresse d'une variable locale
- [ ] Je sais qu'un pointeur non initialisé n'est PAS NULL
- [ ] Je comprends le danger du double free

---

**Prochaine étape :** 7.6 Pointeurs constants et const

⏭️ [Pointeurs constants et const](/07-pointeurs/06-pointeurs-const.md)
