🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.5 Fuites mémoire : causes et prévention

## Introduction

Une **fuite mémoire** (memory leak) se produit lorsqu'un programme alloue de la mémoire sur le Heap mais ne la libère jamais, rendant cette mémoire inaccessible et inutilisable jusqu'à la fin du programme.

Les fuites mémoire sont l'un des bugs les plus insidieux en C car :
- ⚠️ Elles ne causent pas de crash immédiat
- ⚠️ Elles s'accumulent progressivement
- ⚠️ Elles peuvent passer inaperçues pendant longtemps
- ⚠️ Elles finissent par épuiser la mémoire disponible

> **Impact réel :** Un serveur web avec une petite fuite de 1 KB par requête peut perdre 1 GB de mémoire après seulement 1 million de requêtes !

Dans cette section, vous apprendrez :
- À identifier les différents types de fuites
- Les causes les plus courantes
- Comment les détecter avec des outils
- Les techniques de prévention
- Les bonnes pratiques de gestion mémoire

---

## Qu'est-ce qu'une fuite mémoire ?

### Définition

Une fuite mémoire survient quand vous perdez la **référence** (le pointeur) vers un bloc de mémoire alloué sans l'avoir libéré.

```
Situation normale :
┌─────────────┐
│   Pointeur  │ ──────► [Mémoire allouée]
└─────────────┘
        ↓
   free() appelé
        ↓
[Mémoire libérée]


Fuite mémoire :
┌─────────────┐
│   Pointeur  │ ──────► [Mémoire allouée]
└─────────────┘
        ↓
  Pointeur détruit/écrasé
        ↓
        ✗  [Mémoire allouée PERDUE]
           Plus aucune référence !
           Impossible de libérer !
```

### Exemple simple de fuite

```c
#include <stdlib.h>

void fonction_avec_fuite(void) {
    int* ptr = malloc(100 * sizeof(int));

    // Utilisation de ptr...
    ptr[0] = 42;

    // ❌ OUBLI DE free(ptr) !

}  // ptr est détruit, mais la mémoire reste allouée → FUITE

int main(void) {
    // Chaque appel perd 400 octets
    for (int i = 0; i < 1000000; i++) {
        fonction_avec_fuite();
    }
    // Total perdu : 400 MB !

    return 0;
}
```

### Visualisation temporelle

```
Ligne de temps du programme :

t=0  : Mémoire disponible : 1000 MB
       │
t=1  : fonction_avec_fuite() #1
       Allocation : 400 octets
       Fuite : 400 octets
       Mémoire disponible : 999.6 MB
       │
t=2  : fonction_avec_fuite() #2
       Allocation : 400 octets
       Fuite : 400 octets
       Mémoire disponible : 999.2 MB
       │
...  : (répété 1 million de fois)
       │
t=N  : fonction_avec_fuite() #1000000
       Mémoire disponible : 600 MB
       │
       Total perdu : 400 MB

Programme terminé → OS récupère TOUTE la mémoire
```

---

## Les causes courantes de fuites

### 1. Oubli simple de free()

**Le cas le plus fréquent** : on alloue mais on oublie de libérer.

```c
// ❌ FUITE : Oubli de free()
void traiter_donnees(void) {
    char* buffer = malloc(1024);

    if (buffer == NULL) return;

    // Traitement...
    strcpy(buffer, "Hello");
    printf("%s\n", buffer);

    // ❌ Oubli de free(buffer) !
}
```

**✅ Correction :**
```c
void traiter_donnees(void) {
    char* buffer = malloc(1024);

    if (buffer == NULL) return;

    strcpy(buffer, "Hello");
    printf("%s\n", buffer);

    free(buffer);  // ✅ Libération
}
```

### 2. Sortie prématurée de fonction

Les `return` multiples peuvent causer des oublis.

```c
// ❌ FUITE : Return sans free()
int traiter_fichier(const char* nom) {
    char* buffer = malloc(1024);
    if (buffer == NULL) return -1;

    FILE* f = fopen(nom, "r");
    if (f == NULL) {
        return -1;  // ❌ FUITE : buffer n'est pas libéré !
    }

    // Traitement...

    fclose(f);
    free(buffer);
    return 0;
}
```

**✅ Correction avec goto :**
```c
int traiter_fichier(const char* nom) {
    char* buffer = malloc(1024);
    if (buffer == NULL) return -1;

    FILE* f = fopen(nom, "r");
    if (f == NULL) {
        goto cleanup_buffer;  // ✅ Nettoyage avant sortie
    }

    // Traitement...

    fclose(f);

cleanup_buffer:
    free(buffer);
    return f ? 0 : -1;
}
```

**✅ Alternative : Ordre inverse d'allocation :**
```c
int traiter_fichier(const char* nom) {
    FILE* f = fopen(nom, "r");
    if (f == NULL) {
        return -1;  // Pas encore d'allocation
    }

    char* buffer = malloc(1024);
    if (buffer == NULL) {
        fclose(f);
        return -1;
    }

    // Traitement...

    free(buffer);
    fclose(f);
    return 0;
}
```

### 3. Perte du pointeur (écrasement)

**Écraser un pointeur sans libérer la mémoire.**

```c
// ❌ FUITE : Pointeur écrasé
void fonction(void) {
    int* ptr = malloc(100 * sizeof(int));

    // ... utilisation ...

    ptr = malloc(50 * sizeof(int));  // ❌ FUITE : le premier bloc est perdu !

    free(ptr);  // Libère seulement le 2e bloc
}
```

**Visualisation :**
```
Après 1er malloc :  
ptr ───► [Bloc 100 int]  

Après 2e malloc :  
ptr ───► [Bloc 50 int]  

         [Bloc 100 int]  ← PERDU ! Plus de référence !
```

**✅ Correction :**
```c
void fonction(void) {
    int* ptr = malloc(100 * sizeof(int));

    // ... utilisation ...

    free(ptr);  // ✅ Libérer d'abord
    ptr = malloc(50 * sizeof(int));

    free(ptr);
}
```

### 4. Réassignation dans une boucle

```c
// ❌ FUITE : Allocation répétée dans une boucle
void fonction(void) {
    char* buffer = NULL;

    for (int i = 0; i < 100; i++) {
        buffer = malloc(1024);  // ❌ FUITE à chaque itération !
        // Traitement...
    }

    free(buffer);  // Libère seulement le dernier
    // Les 99 premiers blocs sont perdus !
}
```

**✅ Correction 1 : Allouer une seule fois**
```c
void fonction(void) {
    char* buffer = malloc(1024);
    if (buffer == NULL) return;

    for (int i = 0; i < 100; i++) {
        // Réutiliser le même buffer
        // Traitement...
    }

    free(buffer);
}
```

**✅ Correction 2 : Libérer à chaque itération**
```c
void fonction(void) {
    for (int i = 0; i < 100; i++) {
        char* buffer = malloc(1024);
        if (buffer == NULL) continue;

        // Traitement...

        free(buffer);  // ✅ Libération dans la boucle
    }
}
```

### 5. Structures avec pointeurs internes

**Oublier de libérer les membres alloués dynamiquement.**

```c
typedef struct {
    char* nom;
    char* adresse;
    int age;
} Personne;

// ❌ FUITE : Libération incomplète
void exemple_fuite(void) {
    Personne* p = malloc(sizeof(Personne));
    p->nom = malloc(50);
    p->adresse = malloc(100);

    strcpy(p->nom, "Alice");
    strcpy(p->adresse, "123 Rue Example");
    p->age = 30;

    free(p);  // ❌ FUITE : nom et adresse ne sont pas libérés !
}
```

**Visualisation :**
```
Avant free(p) :  
p ───► [Structure Personne]  
         ├─ nom ───────► [Chaîne "Alice"]
         ├─ adresse ───► [Chaîne "123 Rue..."]
         └─ age = 30

Après free(p) seulement :
       [Structure libérée]

         [Chaîne "Alice"]     ← FUITE !
         [Chaîne "123 Rue..."] ← FUITE !
```

**✅ Correction :**
```c
void exemple_correct(void) {
    Personne* p = malloc(sizeof(Personne));
    if (p == NULL) return;

    p->nom = malloc(50);
    p->adresse = malloc(100);

    if (p->nom == NULL || p->adresse == NULL) {
        free(p->adresse);
        free(p->nom);
        free(p);
        return;
    }

    strcpy(p->nom, "Alice");
    strcpy(p->adresse, "123 Rue Example");
    p->age = 30;

    // ✅ Libération dans l'ordre inverse
    free(p->adresse);
    free(p->nom);
    free(p);
}
```

**✅ Meilleure pratique : Fonction de libération dédiée**
```c
void liberer_personne(Personne* p) {
    if (p != NULL) {
        free(p->adresse);
        free(p->nom);
        free(p);
    }
}

void exemple_avec_fonction(void) {
    Personne* p = malloc(sizeof(Personne));
    // ... initialisation ...

    liberer_personne(p);  // ✅ Libération complète
}
```

### 6. Listes chaînées et structures récursives

**Fuite en cascade dans les structures dynamiques.**

```c
typedef struct Node {
    int valeur;
    struct Node* suivant;
} Node;

// ❌ FUITE : Libération incomplète de la liste
void liberer_liste_incorrecte(Node* tete) {
    free(tete);  // ❌ Libère seulement le premier nœud !
    // Tous les autres nœuds sont perdus
}
```

**Visualisation :**
```
Liste avant libération :  
tete ───► [Node 1] ───► [Node 2] ───► [Node 3] ───► NULL  

Après free(tete) seulement :
          [Node 1 libéré]

                  [Node 2] ───► [Node 3] ───► NULL
                  ↑ FUITES : Plus de référence !
```

**✅ Correction : Libération récursive**
```c
void liberer_liste_recursive(Node* tete) {
    if (tete == NULL) return;

    liberer_liste_recursive(tete->suivant);  // Récursion
    free(tete);
}
```

**✅ Alternative : Libération itérative**
```c
void liberer_liste_iterative(Node* tete) {
    Node* courant = tete;
    Node* suivant;

    while (courant != NULL) {
        suivant = courant->suivant;  // ✅ Sauvegarder le suivant
        free(courant);
        courant = suivant;
    }
}
```

### 7. Exceptions simulées (setjmp/longjmp)

**Les sauts non-locaux peuvent contourner les free().**

```c
#include <setjmp.h>
#include <stdlib.h>

jmp_buf env;

void fonction_critique(void) {
    char* buffer = malloc(1024);

    if (/* condition d'erreur */) {
        longjmp(env, 1);  // ❌ FUITE : buffer n'est pas libéré !
    }

    // ... traitement ...

    free(buffer);
}
```

**✅ Correction : Ne pas utiliser longjmp avec malloc/free, ou utiliser cleanup handlers**

### 8. realloc() mal géré

```c
// ❌ FUITE avec realloc()
void fonction(void) {
    int* ptr = malloc(10 * sizeof(int));

    // Tentative de redimensionnement
    ptr = realloc(ptr, 20 * sizeof(int));  // ❌ Si realloc échoue, fuite !

    free(ptr);
}
```

**Problème :** Si `realloc()` échoue, il retourne NULL, mais l'ancien bloc reste alloué. En écrasant `ptr` avec NULL, on perd la référence.

**✅ Correction :**
```c
void fonction(void) {
    int* ptr = malloc(10 * sizeof(int));
    if (ptr == NULL) return;

    int* temp = realloc(ptr, 20 * sizeof(int));
    if (temp == NULL) {
        free(ptr);  // ✅ Libérer l'ancien bloc
        return;
    }

    ptr = temp;  // ✅ Mise à jour seulement si succès

    // ... utilisation ...

    free(ptr);
}
```

---

## Détection des fuites mémoire

### Outil 1 : Valgrind

**Valgrind** est l'outil de référence pour détecter les fuites mémoire sous Linux.

#### Installation

```bash
sudo apt-get install valgrind
```

#### Utilisation de base

```bash
# Compiler avec symboles de debug
gcc -g -o programme programme.c

# Exécuter avec Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./programme
```

#### Exemple de programme avec fuite

```c
// fuite.c
#include <stdlib.h>

void fonction_avec_fuite(void) {
    int* ptr = malloc(100 * sizeof(int));
    // Oubli de free()
}

int main(void) {
    fonction_avec_fuite();
    return 0;
}
```

#### Compilation et test

```bash
gcc -g -o fuite fuite.c  
valgrind --leak-check=full --show-leak-kinds=all ./fuite  
```

#### Rapport Valgrind

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 400 bytes in 1 blocks
==12345==   total heap usage: 1 allocs, 0 frees, 400 bytes allocated
==12345==
==12345== 400 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4C2FB0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==12345==    by 0x10869B: fonction_avec_fuite (fuite.c:4)
==12345==    by 0x1086B7: main (fuite.c:9)
==12345==
==12345== LEAK SUMMARY:
==12345==    definitely lost: 400 bytes in 1 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
==12345==    still reachable: 0 bytes in 0 blocks
==12345==         suppressed: 0 bytes in 0 blocks
```

**Interprétation :**
- **definitely lost** : Fuites certaines (aucune référence)
- **indirectly lost** : Fuites indirectes (via structures)
- **possibly lost** : Fuites possibles (pointeurs internes)
- **still reachable** : Mémoire accessible mais non libérée

### Outil 2 : AddressSanitizer (ASan)

**ASan** est intégré à GCC et Clang, plus rapide que Valgrind.

#### Compilation avec ASan

```bash
gcc -fsanitize=address -g -o programme programme.c
```

#### Exécution

```bash
./programme
```

#### Rapport ASan

```
=================================================================
==12345==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 400 byte(s) in 1 object(s) allocated from:
    #0 0x7f8b9c malloc (/usr/lib/x86_64-linux-gnu/libasan.so.5+0x10d3f8)
    #1 0x40069b in fonction_avec_fuite fuite.c:4
    #2 0x4006b7 in main fuite.c:9

SUMMARY: AddressSanitizer: 400 byte(s) leaked in 1 allocation(s).
```

### Outil 3 : mtrace (GNU libc)

**mtrace** est un outil léger intégré à la glibc.

#### Code avec mtrace

```c
#include <mcheck.h>
#include <stdlib.h>

int main(void) {
    mtrace();  // Activer le traçage

    int* ptr = malloc(100 * sizeof(int));
    // Oubli de free()

    muntrace();  // Désactiver le traçage
    return 0;
}
```

#### Compilation et exécution

```bash
gcc -g -o programme programme.c  
export MALLOC_TRACE=malloc.log  
./programme
mtrace programme malloc.log
```

### Comparaison des outils

| Outil | Avantages | Inconvénients | Usage |
|-------|-----------|---------------|-------|
| **Valgrind** | Très précis, détaillé | Lent (~10x) | Développement, tests |
| **ASan** | Rapide (~2x) | Nécessite recompilation | CI/CD, développement |
| **mtrace** | Léger | Moins détaillé | Debugging rapide |

---

## Techniques de prévention

### 1. RAII-like en C avec __attribute__((cleanup))

**Extension GCC** permettant un nettoyage automatique (similaire au RAII en C++).

```c
#include <stdlib.h>
#include <stdio.h>

// Fonction de nettoyage appelée automatiquement
void cleanup_ptr(void* ptr) {
    void** p = (void**)ptr;
    if (*p != NULL) {
        printf("Auto-free de %p\n", *p);
        free(*p);
        *p = NULL;
    }
}

// Macro pour simplifier
#define auto_free __attribute__((cleanup(cleanup_ptr)))

void exemple_auto_free(void) {
    auto_free int* ptr = malloc(100 * sizeof(int));

    if (ptr == NULL) return;

    ptr[0] = 42;
    printf("Valeur : %d\n", ptr[0]);

    // ✅ free() appelé AUTOMATIQUEMENT à la fin du scope
}

int main(void) {
    exemple_auto_free();
    printf("Pas de fuite !\n");
    return 0;
}
```

**⚠️ Limitation :** Extension GNU, non portable vers tous les compilateurs.

### 2. Macros de sécurité

```c
#include <stdlib.h>
#include <stdio.h>

// Macro pour malloc avec vérification
#define SAFE_MALLOC(ptr, size) do { \
    (ptr) = malloc(size); \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Erreur malloc à la ligne %d\n", __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

// Macro pour free sécurisé
#define SAFE_FREE(ptr) do { \
    free(ptr); \
    (ptr) = NULL; \
} while(0)

int main(void) {
    int* tableau;
    SAFE_MALLOC(tableau, 100 * sizeof(int));

    // Utilisation...
    tableau[0] = 42;

    SAFE_FREE(tableau);
    // tableau est maintenant NULL

    return 0;
}
```

### 3. Compteur de références (Reference Counting)

```c
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    void* data;
    size_t ref_count;
} RefCounted;

RefCounted* create_refcounted(size_t size) {
    RefCounted* rc = malloc(sizeof(RefCounted));
    if (rc == NULL) return NULL;

    rc->data = malloc(size);
    if (rc->data == NULL) {
        free(rc);
        return NULL;
    }

    rc->ref_count = 1;
    printf("Créé avec ref_count = 1\n");
    return rc;
}

void retain(RefCounted* rc) {
    if (rc != NULL) {
        rc->ref_count++;
        printf("Retain : ref_count = %zu\n", rc->ref_count);
    }
}

void release(RefCounted* rc) {
    if (rc == NULL) return;

    rc->ref_count--;
    printf("Release : ref_count = %zu\n", rc->ref_count);

    if (rc->ref_count == 0) {
        printf("ref_count = 0, libération automatique\n");
        free(rc->data);
        free(rc);
    }
}

int main(void) {
    RefCounted* obj = create_refcounted(100);

    retain(obj);   // ref_count = 2
    retain(obj);   // ref_count = 3

    release(obj);  // ref_count = 2
    release(obj);  // ref_count = 1
    release(obj);  // ref_count = 0 → libération automatique

    return 0;
}
```

### 4. Memory Pools

```c
#include <stdlib.h>
#include <stdio.h>

typedef struct MemoryPool {
    void* memory;
    size_t size;
    size_t used;
} MemoryPool;

MemoryPool* create_pool(size_t size) {
    MemoryPool* pool = malloc(sizeof(MemoryPool));
    if (pool == NULL) return NULL;

    pool->memory = malloc(size);
    if (pool->memory == NULL) {
        free(pool);
        return NULL;
    }

    pool->size = size;
    pool->used = 0;

    return pool;
}

void* pool_alloc(MemoryPool* pool, size_t size) {
    if (pool == NULL || pool->used + size > pool->size) {
        return NULL;
    }

    void* ptr = (char*)pool->memory + pool->used;
    pool->used += size;

    return ptr;
}

void destroy_pool(MemoryPool* pool) {
    if (pool != NULL) {
        free(pool->memory);  // ✅ Une seule libération pour tout
        free(pool);
    }
}

int main(void) {
    MemoryPool* pool = create_pool(1024 * 1024);  // 1 MB

    // Plusieurs allocations
    int* a = pool_alloc(pool, 10 * sizeof(int));
    char* b = pool_alloc(pool, 256);
    double* c = pool_alloc(pool, 100 * sizeof(double));

    // Utilisation...
    if (a) a[0] = 42;
    if (b) b[0] = 'X';
    if (c) c[0] = 3.14;

    // ✅ Une seule libération pour tout le pool
    destroy_pool(pool);

    return 0;
}
```

### 5. Conventions de nommage

Utiliser des conventions pour clarifier la propriété de la mémoire :

```c
// Convention : les fonctions "create" ou "new" allouent
//             les fonctions "destroy" ou "free" libèrent

// L'appelant est responsable de libérer
char* create_string(const char* source);

// La fonction libère elle-même
void process_and_free(char* string);

// L'appelant garde la responsabilité
void process_only(const char* string);
```

---

## Bonnes pratiques pour éviter les fuites

### Règle 1 : Une allocation = Un free()

```c
// ✅ Pattern correct
void fonction(void) {
    char* buffer = malloc(1024);  // 1 malloc
    if (buffer == NULL) return;

    // ... utilisation ...

    free(buffer);  // 1 free
}
```

### Règle 2 : Libérer dans l'ordre inverse d'allocation

```c
void fonction(void) {
    char* a = malloc(100);
    char* b = malloc(200);
    char* c = malloc(300);

    // ... utilisation ...

    // ✅ Libération dans l'ordre inverse
    free(c);
    free(b);
    free(a);
}
```

### Règle 3 : Mettre les pointeurs à NULL après free()

```c
void fonction(void) {
    int* ptr = malloc(sizeof(int));

    // ... utilisation ...

    free(ptr);
    ptr = NULL;  // ✅ Évite use-after-free et double-free
}
```

### Règle 4 : Utiliser des fonctions de libération

```c
typedef struct {
    char* nom;
    int* donnees;
    size_t taille;
} Structure;

// ✅ Fonction dédiée à la libération
void liberer_structure(Structure* s) {
    if (s != NULL) {
        free(s->donnees);
        free(s->nom);
        free(s);
    }
}
```

### Règle 5 : Documenter la propriété de la mémoire

```c
/**
 * Crée une copie d'une chaîne
 * @return Nouvelle chaîne allouée (l'appelant doit libérer avec free())
 */
char* dupliquer_chaine(const char* source);

/**
 * Traite une chaîne
 * @param chaine Chaîne à traiter (ne sera PAS libérée par cette fonction)
 */
void traiter_chaine(const char* chaine);
```

### Règle 6 : Utiliser des outils d'analyse statique

```bash
# cppcheck : analyse statique
cppcheck --enable=all programme.c

# clang-tidy : analyse Clang
clang-tidy programme.c -- -std=c11
```

---

## Checklist anti-fuites

### ✅ Avant de commiter le code

- [ ] Chaque `malloc` a un `free` correspondant
- [ ] Tous les chemins de sortie libèrent la mémoire
- [ ] Les structures avec pointeurs ont une fonction de libération
- [ ] Les listes/arbres ont une libération récursive ou itérative
- [ ] `realloc` est géré avec un pointeur temporaire
- [ ] Les pointeurs sont mis à NULL après `free`
- [ ] Le code a été testé avec Valgrind
- [ ] Les warnings du compilateur sont corrigés (`-Wall -Wextra`)

### ✅ Lors du code review

- [ ] Pas de `return` sans libération
- [ ] Pas de boucle avec allocation sans libération
- [ ] Pas de réassignation de pointeur sans `free`
- [ ] Les fonctions d'erreur libèrent les ressources
- [ ] La documentation indique qui libère quoi

---

## Exemple complet : Programme sans fuites

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    size_t count;
} Liste;

// Créer une liste vide
Liste* creer_liste(void) {
    Liste* liste = malloc(sizeof(Liste));
    if (liste == NULL) return NULL;

    liste->head = NULL;
    liste->count = 0;

    return liste;
}

// Ajouter un élément
int ajouter(Liste* liste, const char* data) {
    if (liste == NULL || data == NULL) return 0;

    Node* nouveau = malloc(sizeof(Node));
    if (nouveau == NULL) return 0;

    nouveau->data = malloc(strlen(data) + 1);
    if (nouveau->data == NULL) {
        free(nouveau);
        return 0;
    }

    strcpy(nouveau->data, data);
    nouveau->next = liste->head;
    liste->head = nouveau;
    liste->count++;

    return 1;
}

// Afficher la liste
void afficher(const Liste* liste) {
    if (liste == NULL) return;

    printf("Liste (%zu éléments) : ", liste->count);

    Node* courant = liste->head;
    while (courant != NULL) {
        printf("%s ", courant->data);
        courant = courant->next;
    }
    printf("\n");
}

// ✅ Libération complète de la liste
void liberer_liste(Liste* liste) {
    if (liste == NULL) return;

    Node* courant = liste->head;
    Node* suivant;

    while (courant != NULL) {
        suivant = courant->next;
        free(courant->data);  // Libérer la chaîne
        free(courant);        // Libérer le nœud
        courant = suivant;
    }

    free(liste);  // Libérer la structure
}

int main(void) {
    // Créer la liste
    Liste* ma_liste = creer_liste();
    if (ma_liste == NULL) {
        fprintf(stderr, "Erreur de création\n");
        return 1;
    }

    // Ajouter des éléments
    if (!ajouter(ma_liste, "Premier")) goto cleanup;
    if (!ajouter(ma_liste, "Deuxième")) goto cleanup;
    if (!ajouter(ma_liste, "Troisième")) goto cleanup;

    // Afficher
    afficher(ma_liste);

cleanup:
    // ✅ Nettoyage garanti
    liberer_liste(ma_liste);

    printf("Programme terminé sans fuite\n");
    return 0;
}
```

**Test avec Valgrind :**
```bash
gcc -g -o liste liste.c  
valgrind --leak-check=full ./liste  
```

**Résultat attendu :**
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: X allocs, X frees, Y bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
```

---

## Fuites acceptables

### Cas particuliers

Dans certains cas, des "fuites" peuvent être acceptables :

**1. Caches globaux**
```c
// Cache initialisé une seule fois
static char* cache_global = NULL;

void initialiser_cache(void) {
    if (cache_global == NULL) {
        cache_global = malloc(1024);
        // ⚠️ Jamais libéré, mais c'est voulu (cache permanent)
    }
}
```

**2. Configuration de démarrage**
```c
// Configuration chargée au démarrage
Config* config_globale = NULL;

void charger_config(void) {
    config_globale = malloc(sizeof(Config));
    // Reste en mémoire toute la durée du programme
    // L'OS récupère à la fin
}
```

**3. Programmes très courts**
```c
int main(void) {
    char* buffer = malloc(100);
    printf("%s\n", "Hello");
    // Pas de free() : programme se termine immédiatement
    // L'OS nettoie automatiquement
    return 0;
}
```

> **Note :** Ces cas sont des exceptions. Dans le doute, libérez toujours.

---

## Récapitulatif

### Points clés à retenir

1. **Une fuite = mémoire allouée + référence perdue**
2. **Les fuites s'accumulent** et finissent par épuiser la mémoire
3. **Chaque malloc doit avoir son free** correspondant
4. **Attention aux sorties multiples** de fonction
5. **Structures complexes = fonctions de libération dédiées**
6. **Tester systématiquement avec Valgrind**

### Causes principales

| Cause | Fréquence | Solution |
|-------|-----------|----------|
| Oubli de free() | Très courante | Code review, outils |
| Return prématuré | Courante | goto cleanup |
| Pointeur écrasé | Courante | free() avant réassignation |
| Structures complexes | Moyenne | Fonction de libération |
| Boucles | Moyenne | free() dans la boucle |

### Outils essentiels

- **Valgrind** : Détection précise (développement)
- **AddressSanitizer** : Détection rapide (CI/CD)
- **Analyse statique** : Prévention (cppcheck, clang-tidy)

### Règles d'or

- ✅ **1 allocation = 1 libération**
- ✅ **Libérer dans l'ordre inverse**
- ✅ **NULL après free()**
- ✅ **goto pour le nettoyage**
- ✅ **Tester avec Valgrind**
- ✅ **Documenter la propriété**

---

## Pour aller plus loin

### Sections recommandées

- **Section 9.6** : Double free et corruption de tas
- **Module 5, Section 15.4** : Valgrind en détail
- **Module 5, Section 15.1** : AddressSanitizer
- **Section 24.1** : RAII-like avec __attribute__((cleanup))

### Commandes utiles

```bash
# Valgrind complet
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes --verbose ./programme

# ASan avec options
ASAN_OPTIONS=detect_leaks=1:leak_check_at_exit=1 ./programme

# Analyse statique
cppcheck --enable=all --inconclusive programme.c
```

### Ressources

- Valgrind User Manual
- CERT C Coding Standard : MEM rules
- "Expert C Programming" (Peter van der Linden)

---


⏭️ [Double free et corruption de tas](/09-allocation-dynamique/06-double-free.md)
