🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.6 Double free et corruption de tas

## Introduction

Le **double free** et la **corruption de tas** (heap corruption) sont parmi les bugs les plus dangereux et les plus difficiles à déboguer en C. Contrairement aux fuites mémoire qui "seulement" gaspillent de la mémoire, ces erreurs peuvent :

- ⚠️ Corrompre les structures internes de l'allocateur
- ⚠️ Causer des crashs aléatoires et imprévisibles
- ⚠️ Créer des failles de sécurité exploitables
- ⚠️ Produire des comportements totalement imprévisibles

> **Danger critique :** Ces bugs sont souvent exploités par des attaquants pour prendre le contrôle d'un programme (exploitation de vulnérabilités).

Dans cette section, vous apprendrez :
- Ce qu'est un double free et pourquoi c'est grave
- Comment se produit la corruption de tas
- Les symptômes et la détection
- Comment prévenir ces erreurs
- Les outils pour les identifier

---

## Le Double Free

### Qu'est-ce qu'un double free ?

Un **double free** se produit lorsqu'on appelle `free()` **deux fois** (ou plus) sur le **même pointeur**.

```c
int* ptr = malloc(sizeof(int));
free(ptr);      // ✅ Première libération : OK
free(ptr);      // ❌ Deuxième libération : DOUBLE FREE !
```

### Pourquoi c'est grave ?

Lorsque vous appelez `free()`, l'allocateur :
1. Marque le bloc comme libre
2. Met à jour ses structures internes (listes chaînées de blocs libres)
3. Peut fusionner le bloc avec des blocs adjacents

**Le problème :** Si vous appelez `free()` une deuxième fois :
- L'allocateur essaie de libérer un bloc déjà dans sa liste libre
- Cela corrompt ses structures internes
- Les prochains `malloc()` ou `free()` peuvent crasher
- Un attaquant peut exploiter cette corruption

### Visualisation du double free

```
État initial :
┌────────────────────────────────────────┐
│  Heap : [Bloc A occupé] [Bloc B libre] │
│  Liste libre : B → NULL                │
└────────────────────────────────────────┘

Après malloc() :
ptr ───► [Bloc A occupé]

Après premier free(ptr) :
┌────────────────────────────────────────┐
│  Heap : [Bloc A libre] [Bloc B libre]  │
│  Liste libre : A → B → NULL            │
└────────────────────────────────────────┘

Après deuxième free(ptr) (DOUBLE FREE) :
┌────────────────────────────────────────┐
│  Heap : [Bloc A libre] [Bloc B libre]  │
│  Liste libre : A → B → A → ... ⚠️ CYCLE !
│                     ↑_____↓            │
│  CORRUPTION DE LA LISTE LIBRE !        │
└────────────────────────────────────────┘

Résultat : L'allocateur est dans un état incohérent
```

### Exemple simple de double free

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int* ptr = malloc(sizeof(int));

    if (ptr == NULL) {
        return 1;
    }

    *ptr = 42;
    printf("Valeur : %d\n", *ptr);

    free(ptr);   // ✅ Premier free : OK
    printf("Premier free effectué\n");

    free(ptr);   // ❌ DOUBLE FREE : DANGER !
    printf("Deuxième free effectué\n");  // Peut ne jamais s'afficher

    return 0;
}
```

**Comportements possibles :**
```
Scénario 1 : Crash immédiat
Valeur : 42
Premier free effectué
*** Error in `./programme': double free or corruption (fasttop): 0x0000000001234560 ***
Aborted (core dumped)

Scénario 2 : Semble fonctionner (danger !)
Valeur : 42
Premier free effectué
Deuxième free effectué
[Programme se termine "normalement" mais heap corrompu]

Scénario 3 : Crash plus tard
Valeur : 42
Premier free effectué
Deuxième free effectué
[Crash lors d'un prochain malloc ou free]
```

---

## Causes courantes de double free

### 1. Oubli de mettre à NULL après free()

**Le problème le plus fréquent.**

```c
// ❌ Double free accidentel
void fonction() {
    int* ptr = malloc(sizeof(int));

    // ... utilisation ...

    free(ptr);
    // ptr pointe toujours vers la mémoire libérée !

    // Plus tard, dans un autre chemin de code...
    if (ptr != NULL) {  // ⚠️ ptr n'est pas NULL !
        free(ptr);      // ❌ DOUBLE FREE
    }
}
```

**✅ Solution : Mettre à NULL après free()**

```c
void fonction() {
    int* ptr = malloc(sizeof(int));

    // ... utilisation ...

    free(ptr);
    ptr = NULL;  // ✅ Protection contre double free

    // Plus tard...
    if (ptr != NULL) {
        free(ptr);  // Ne sera pas exécuté
    }
}
```

**Note :** `free(NULL)` est sûr et ne fait rien (comportement standard).

### 2. Copies de pointeurs non gérées

```c
// ❌ Double free via copie de pointeur
void fonction() {
    int* ptr1 = malloc(sizeof(int));
    int* ptr2 = ptr1;  // ptr2 pointe vers la même zone

    *ptr1 = 42;

    free(ptr1);  // ✅ Libération du bloc
    free(ptr2);  // ❌ DOUBLE FREE : même adresse !
}
```

**Visualisation :**

```
Après malloc :
ptr1 ───┐
        ├──► [Bloc mémoire]
ptr2 ───┘

Après free(ptr1) :
ptr1 ───┐
        ├──► [Bloc LIBÉRÉ]
ptr2 ───┘

Après free(ptr2) :
❌ Tentative de libérer un bloc déjà libéré
```

**✅ Solution 1 : Compteur de références**

```c
typedef struct {
    void* data;
    int ref_count;
} RefCounted;

void release(RefCounted* rc) {
    if (rc == NULL) return;

    rc->ref_count--;

    if (rc->ref_count == 0) {
        free(rc->data);
        free(rc);
    }
}
```

**✅ Solution 2 : Propriété claire**

```c
void fonction() {
    int* owner = malloc(sizeof(int));
    int* alias = owner;  // Juste un alias, ne possède pas la mémoire

    // Seul owner peut libérer
    free(owner);
    owner = NULL;

    // Ne PAS libérer alias
}
```

### 3. Structures avec pointeurs partagés

```c
typedef struct {
    char* nom;
    char* alias;  // Pointe vers le même nom
} Personne;

// ❌ Double free dans la libération
void liberer_personne_incorrect(Personne* p) {
    if (p != NULL) {
        free(p->nom);
        free(p->alias);  // ❌ DOUBLE FREE si alias == nom
        free(p);
    }
}
```

**✅ Solution : Vérifier si les pointeurs sont identiques**

```c
void liberer_personne_correct(Personne* p) {
    if (p != NULL) {
        free(p->nom);

        // Ne libérer alias que si c'est une zone différente
        if (p->alias != NULL && p->alias != p->nom) {
            free(p->alias);
        }

        free(p);
    }
}
```

### 4. Double libération dans les chemins d'erreur

```c
// ❌ Double free dans la gestion d'erreurs
int traiter_donnees(const char* fichier) {
    char* buffer = malloc(1024);
    if (buffer == NULL) return -1;

    FILE* f = fopen(fichier, "r");
    if (f == NULL) {
        free(buffer);
        return -1;
    }

    if (/* erreur de lecture */) {
        free(buffer);
        fclose(f);
        return -1;
    }

    // Traitement...

    free(buffer);  // ❌ Peut être un double free si erreur lecture
    fclose(f);
    return 0;
}
```

**✅ Solution : goto cleanup ou flag**

```c
int traiter_donnees(const char* fichier) {
    char* buffer = malloc(1024);
    if (buffer == NULL) return -1;

    FILE* f = fopen(fichier, "r");
    if (f == NULL) {
        goto cleanup_buffer;
    }

    if (/* erreur de lecture */) {
        fclose(f);
        goto cleanup_buffer;
    }

    // Traitement...

    fclose(f);

cleanup_buffer:
    free(buffer);  // ✅ Un seul free, quel que soit le chemin
    return 0;
}
```

---

## Corruption de tas (Heap Corruption)

### Qu'est-ce que la corruption de tas ?

La **corruption de tas** se produit lorsque les structures internes de l'allocateur mémoire sont endommagées. Cela peut arriver de plusieurs façons :

1. **Double free** (vu ci-dessus)
2. **Buffer overflow** sur le Heap
3. **Use after free**
4. **Corruption des métadonnées** de l'allocateur

### Structure interne du Heap (simplifié)

```
Bloc alloué typique :

┌─────────────────────────────┐
│  Métadonnées (header)       │ ← Taille du bloc, flags, etc.
├─────────────────────────────┤
│  Données utilisateur        │ ← Ce que malloc() vous donne
│  (votre zone)               │
├─────────────────────────────┤
│  Métadonnées (footer)       │ ← Informations de contrôle
└─────────────────────────────┘

Si vous écrivez en dehors de "votre zone", vous
corrompez les métadonnées → heap corruption !
```

### Exemple 1 : Buffer Overflow sur le Heap

```c
// ❌ Corruption par buffer overflow
void fonction() {
    char* buffer = malloc(10);  // 10 octets

    if (buffer == NULL) return;

    // Écriture hors limites
    strcpy(buffer, "Ceci est une très longue chaîne");  // ❌ > 10 octets

    // La copie déborde et écrase :
    // - Les métadonnées du bloc suivant
    // - Potentiellement le contenu du bloc suivant

    free(buffer);  // Peut crasher ou corrompre le heap
}
```

**Visualisation :**

```
Avant strcpy :
┌──────────────┬──────────────┬──────────────┐
│ [Métadonnées]│ buffer (10B) │[Métadonnées] │
│    Bloc A    │              │   Bloc B     │
└──────────────┴──────────────┴──────────────┘

Après strcpy avec chaîne trop longue :
┌──────────────┬──────────────┬──────────────┐
│ [Métadonnées]│"Ceci est une"│ très longue  │
│    Bloc A    │              │ chaîne...    │
└──────────────┴──────────────┴──────────────┘
                               ↑
                         Métadonnées de B
                         sont ÉCRASÉES !
```

**Conséquences :**
- Le prochain `malloc()` ou `free()` peut crasher
- L'allocateur est dans un état incohérent
- Comportement imprévisible

**✅ Solution : Utiliser strncpy ou vérifier la taille**

```c
void fonction_securisee() {
    size_t taille = 10;
    char* buffer = malloc(taille);

    if (buffer == NULL) return;

    const char* source = "Ceci est une très longue chaîne";

    // ✅ Copie limitée à la taille du buffer
    strncpy(buffer, source, taille - 1);
    buffer[taille - 1] = '\0';  // Garantir le null-terminator

    free(buffer);
}
```

### Exemple 2 : Use After Free

**Use after free** : utiliser un pointeur après avoir libéré la mémoire.

```c
// ❌ Use after free
void fonction() {
    int* ptr = malloc(sizeof(int));
    *ptr = 42;

    free(ptr);  // Mémoire libérée

    *ptr = 100;  // ❌ USE AFTER FREE : écriture en mémoire libérée

    printf("%d\n", *ptr);  // ❌ Lecture en mémoire libérée
}
```

**Pourquoi c'est dangereux ?**

```
Après free(ptr) :
ptr ───► [Bloc libéré, ajouté à la free list]

L'allocateur peut maintenant réutiliser ce bloc :

Après un malloc() ailleurs :
ptr ───► [Bloc réutilisé par autre allocation]
         (contient maintenant d'autres données)

Si vous écrivez via ptr :
→ Vous corrompez les données de l'autre allocation !
→ Heap corruption + bugs impossibles à déboguer
```

**Exemple concret :**

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocation 1
    int* ptr1 = malloc(sizeof(int));
    *ptr1 = 42;
    printf("ptr1 pointe vers %p, valeur = %d\n", (void*)ptr1, *ptr1);

    // Libération
    free(ptr1);

    // Allocation 2 (peut réutiliser le même bloc)
    int* ptr2 = malloc(sizeof(int));
    *ptr2 = 100;
    printf("ptr2 pointe vers %p, valeur = %d\n", (void*)ptr2, *ptr2);

    // ❌ USE AFTER FREE : écriture via ptr1
    *ptr1 = 999;

    printf("ptr2 après corruption : %d\n", *ptr2);  // Affiche 999 !

    free(ptr2);
    return 0;
}
```

**Sortie possible :**
```
ptr1 pointe vers 0x5632a0, valeur = 42
ptr2 pointe vers 0x5632a0, valeur = 100
ptr2 après corruption : 999
```

**✅ Solution : NULL après free**

```c
void fonction_securisee() {
    int* ptr = malloc(sizeof(int));
    *ptr = 42;

    free(ptr);
    ptr = NULL;  // ✅ Protection

    if (ptr != NULL) {
        *ptr = 100;  // Ne sera pas exécuté
    }
}
```

---

## Détection des double free et corruptions

### Outil 1 : Valgrind

Valgrind détecte automatiquement les double free et corruptions.

```bash
# Compilation
gcc -g -o programme programme.c

# Exécution avec Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./programme
```

**Exemple de détection de double free :**

```c
// test_double_free.c
#include <stdlib.h>

int main() {
    int* ptr = malloc(sizeof(int));
    free(ptr);
    free(ptr);  // Double free
    return 0;
}
```

**Rapport Valgrind :**
```
==12345== Invalid free() / delete / delete[] / realloc()
==12345==    at 0x4C2EDEB: free (vg_replace_malloc.c:530)
==12345==    by 0x108677: main (test_double_free.c:6)
==12345==  Address 0x5200040 is 0 bytes inside a block of size 4 free'd
==12345==    at 0x4C2EDEB: free (vg_replace_malloc.c:530)
==12345==    by 0x10866D: main (test_double_free.c:5)
==12345==  Block was alloc'd at
==12345==    at 0x4C2DB8F: malloc (vg_replace_malloc.c:299)
==12345==    by 0x108651: main (test_double_free.c:4)
```

**Exemple de détection de use after free :**

```c
// test_use_after_free.c
#include <stdlib.h>

int main() {
    int* ptr = malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    *ptr = 100;  // Use after free
    return 0;
}
```

**Rapport Valgrind :**
```
==12345== Invalid write of size 4
==12345==    at 0x108677: main (test_use_after_free.c:7)
==12345==  Address 0x5200040 is 0 bytes inside a block of size 4 free'd
==12345==    at 0x4C2EDEB: free (vg_replace_malloc.c:530)
==12345==    by 0x10866D: main (test_use_after_free.c:6)
```

### Outil 2 : AddressSanitizer (ASan)

ASan est plus rapide que Valgrind et intégré aux compilateurs modernes.

```bash
# Compilation avec ASan
gcc -fsanitize=address -g -o programme programme.c

# Exécution
./programme
```

**Rapport ASan pour double free :**
```
=================================================================
==12345==ERROR: AddressSanitizer: attempting double-free on 0x602000000010
    #0 0x7f8b9c in __interceptor_free
    #1 0x400677 in main test_double_free.c:6
    #2 0x7f8b9d in __libc_start_main

0x602000000010 is located 0 bytes inside of 4-byte region
freed by thread T0 here:
    #0 0x7f8b9c in __interceptor_free
    #1 0x40066d in main test_double_free.c:5

previously allocated by thread T0 here:
    #0 0x7f8b9c in __interceptor_malloc
    #1 0x400651 in main test_double_free.c:4

SUMMARY: AddressSanitizer: attempting double-free
```

**Avantages d'ASan :**
- ✅ Détecte en temps réel
- ✅ Environ 2x plus lent (vs 10x pour Valgrind)
- ✅ Stack trace précise
- ✅ Détecte use-after-free, buffer overflow, etc.

### Outil 3 : Protections de la glibc

La glibc moderne a des protections intégrées.

```c
// Avec MALLOC_CHECK_
// export MALLOC_CHECK_=3
// ./programme
```

**Niveaux de MALLOC_CHECK_ :**
- `0` : Désactivé
- `1` : Affiche un message d'erreur
- `2` : Appelle abort() immédiatement
- `3` : Affiche un message ET appelle abort()

**Exemple :**
```bash
export MALLOC_CHECK_=3
./programme
```

**Sortie en cas de double free :**
```
*** Error in `./programme': double free or corruption (fasttop): 0x0000000001234560 ***
======= Backtrace: =========
/lib/x86_64-linux-gnu/libc.so.6(+0x777e5)[0x7f1234567e5]
...
Aborted (core dumped)
```

---

## Techniques de prévention

### 1. Pattern : NULL après free()

**La technique la plus simple et efficace.**

```c
#define SAFE_FREE(ptr) do { \
    free(ptr); \
    (ptr) = NULL; \
} while(0)

int main() {
    int* ptr = malloc(sizeof(int));

    SAFE_FREE(ptr);  // ✅ ptr est automatiquement mis à NULL
    SAFE_FREE(ptr);  // ✅ Sûr : free(NULL) ne fait rien

    return 0;
}
```

### 2. Wrapper de malloc/free

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    void* ptr;
    int freed;
} TrackedPointer;

TrackedPointer* tracked_malloc(size_t size) {
    TrackedPointer* tp = malloc(sizeof(TrackedPointer));
    if (tp == NULL) return NULL;

    tp->ptr = malloc(size);
    if (tp->ptr == NULL) {
        free(tp);
        return NULL;
    }

    tp->freed = 0;
    return tp;
}

void tracked_free(TrackedPointer* tp) {
    if (tp == NULL) return;

    if (tp->freed) {
        fprintf(stderr, "ERREUR : Double free détecté !\n");
        abort();
    }

    free(tp->ptr);
    tp->freed = 1;
    free(tp);
}

int main() {
    TrackedPointer* tp = tracked_malloc(sizeof(int));

    tracked_free(tp);
    // tracked_free(tp);  // Serait détecté et arrêté

    return 0;
}
```

### 3. Smart pointers en C (simulation)

```c
#include <stdlib.h>
#include <stdio.h>

typedef struct SmartPtr {
    void* data;
    void (*deleter)(void*);
} SmartPtr;

SmartPtr* make_smart(size_t size, void (*deleter)(void*)) {
    SmartPtr* sp = malloc(sizeof(SmartPtr));
    if (sp == NULL) return NULL;

    sp->data = malloc(size);
    if (sp->data == NULL) {
        free(sp);
        return NULL;
    }

    sp->deleter = deleter ? deleter : free;
    return sp;
}

void smart_free(SmartPtr** sp) {
    if (sp == NULL || *sp == NULL) return;

    if ((*sp)->data != NULL) {
        (*sp)->deleter((*sp)->data);
        (*sp)->data = NULL;  // Marquer comme libéré
    }

    free(*sp);
    *sp = NULL;  // Protéger contre double free
}

int main() {
    SmartPtr* sp = make_smart(sizeof(int), NULL);

    if (sp != NULL) {
        *(int*)(sp->data) = 42;
        printf("Valeur : %d\n", *(int*)(sp->data));

        smart_free(&sp);  // ✅ sp devient NULL
        smart_free(&sp);  // ✅ Sûr : ne fait rien
    }

    return 0;
}
```

### 4. Utilisation de __attribute__((cleanup))

**Extension GCC pour nettoyage automatique.**

```c
#include <stdlib.h>
#include <stdio.h>

void cleanup_free(void* ptr) {
    void** p = (void**)ptr;
    if (*p != NULL) {
        printf("Auto-free de %p\n", *p);
        free(*p);
        *p = NULL;
    }
}

#define auto_free __attribute__((cleanup(cleanup_free)))

void fonction() {
    auto_free char* buffer = malloc(100);

    if (buffer == NULL) return;

    strcpy(buffer, "Hello");
    printf("%s\n", buffer);

    // ✅ free() automatique à la fin du scope
    // ✅ Protection contre double free
}

int main() {
    fonction();
    printf("Pas de fuite, pas de double free\n");
    return 0;
}
```

### 5. Assertions et validations

```c
#include <assert.h>
#include <stdlib.h>

typedef struct {
    void* ptr;
    int magic;  // Nombre magique pour validation
} ValidatedPtr;

#define MAGIC_NUMBER 0xDEADBEEF

ValidatedPtr* validated_malloc(size_t size) {
    ValidatedPtr* vp = malloc(sizeof(ValidatedPtr));
    if (vp == NULL) return NULL;

    vp->ptr = malloc(size);
    if (vp->ptr == NULL) {
        free(vp);
        return NULL;
    }

    vp->magic = MAGIC_NUMBER;
    return vp;
}

void validated_free(ValidatedPtr* vp) {
    assert(vp != NULL);
    assert(vp->magic == MAGIC_NUMBER);  // Vérifier l'intégrité

    free(vp->ptr);
    vp->magic = 0;  // Invalider pour détecter double free
    free(vp);
}
```

---

## Exemples de corruptions complexes

### Cas 1 : Corruption par alias

```c
typedef struct {
    char* nom;
    char* surnom;  // Peut pointer vers nom
} Personne;

void scenario_dangereux() {
    Personne* p = malloc(sizeof(Personne));
    p->nom = malloc(50);
    strcpy(p->nom, "Alice");

    p->surnom = p->nom;  // Alias !

    // Modification via surnom
    p->surnom[0] = 'B';  // nom devient "Blice"

    // Libération
    free(p->nom);

    // ❌ DOUBLE FREE ou USE AFTER FREE
    // p->surnom pointe vers mémoire libérée
    printf("%s\n", p->surnom);  // Comportement indéfini

    free(p);
}
```

### Cas 2 : Corruption dans liste chaînée

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;

void corruption_liste() {
    Node* n1 = malloc(sizeof(Node));
    Node* n2 = malloc(sizeof(Node));

    n1->data = 1;
    n1->next = n2;

    n2->data = 2;
    n2->next = NULL;

    // Libération
    free(n1);

    // ❌ USE AFTER FREE : accès via n1 libéré
    Node* temp = n1->next;  // Comportement indéfini

    // ❌ Peut libérer n2 deux fois si on essaie de libérer la liste
    free(n2);
}
```

**✅ Solution : Libération propre**

```c
void liberer_liste_propre(Node* head) {
    Node* current = head;
    Node* next;

    while (current != NULL) {
        next = current->next;  // ✅ Sauvegarder AVANT free
        free(current);
        current = next;
    }
}
```

---

## Debugging de corruptions

### Stratégie de debugging

1. **Compiler avec symboles de debug**
   ```bash
   gcc -g -O0 -o programme programme.c
   ```

2. **Utiliser AddressSanitizer**
   ```bash
   gcc -fsanitize=address -g -o programme programme.c
   ./programme
   ```

3. **Activer MALLOC_CHECK_**
   ```bash
   export MALLOC_CHECK_=3
   ./programme
   ```

4. **Analyser avec Valgrind**
   ```bash
   valgrind --leak-check=full --track-origins=yes ./programme
   ```

5. **Examiner le core dump**
   ```bash
   ulimit -c unlimited
   ./programme  # Crash génère core
   gdb programme core
   (gdb) bt      # Backtrace
   (gdb) print ptr
   ```

### Exemple de session GDB

```bash
$ gdb ./programme
(gdb) run
Program received signal SIGABRT, Aborted.

(gdb) backtrace
#0  0x00007ffff7a42428 in __GI_raise
#1  0x00007ffff7a4402a in __GI_abort
#2  0x00007ffff7a857ea in __libc_message
#3  0x00007ffff7a8d37c in malloc_printerr
#4  0x0000000000400677 in main () at test.c:6

(gdb) frame 4
#4  0x0000000000400677 in main () at test.c:6
6           free(ptr);

(gdb) print ptr
$1 = (int *) 0x602010

(gdb) info locals
ptr = 0x602010
```

---

## Checklist de prévention

### ✅ Règles de code

- [ ] Toujours mettre les pointeurs à NULL après `free()`
- [ ] Utiliser SAFE_FREE macro ou équivalent
- [ ] Ne jamais libérer le même pointeur deux fois
- [ ] Vérifier les alias de pointeurs avant libération
- [ ] Ne jamais accéder à un pointeur après `free()`
- [ ] Utiliser `const` pour les pointeurs non propriétaires

### ✅ Pendant le développement

- [ ] Compiler avec `-Wall -Wextra -Werror`
- [ ] Tester avec AddressSanitizer
- [ ] Exécuter Valgrind régulièrement
- [ ] Activer MALLOC_CHECK_ en développement
- [ ] Code review focalisé sur malloc/free

### ✅ Outils automatiques

- [ ] CI/CD avec ASan activé
- [ ] Tests de régression avec Valgrind
- [ ] Analyse statique (cppcheck, clang-tidy)
- [ ] Fuzzing pour détecter corruptions

---

## Résumé des dangers

### Comparaison des erreurs mémoire

| Erreur | Symptôme | Gravité | Détection |
|--------|----------|---------|-----------|
| **Fuite mémoire** | Mémoire perdue | Moyenne | Valgrind, ASan |
| **Double free** | Crash ou corruption | Critique | ASan, Valgrind |
| **Use after free** | Données corrompues | Critique | ASan, Valgrind |
| **Buffer overflow** | Corruption heap | Critique | ASan, Valgrind |
| **Null dereference** | Crash immédiat | Faible | GDB |

### Impact sur la sécurité

**Double free et corruption de tas sont des vulnérabilités critiques :**

```
Exploitation typique :
1. Attaquant provoque un double free
2. Corrompt les métadonnées du heap
3. Contrôle les prochains malloc()
4. Injecte du code malveillant
5. Prend le contrôle du programme
```

**Exemples réels :**
- CVE-2006-4434 (Firefox) : Double free
- CVE-2014-0160 (Heartbleed) : Buffer over-read
- CVE-2017-5754 (Meltdown) : Use after free dans le kernel

---

## Récapitulatif

### Points clés à retenir

1. **Double free = libérer deux fois le même bloc**
2. **Corruptions = destruction des structures de l'allocateur**
3. **Use after free = utiliser un pointeur libéré**
4. **Ces bugs peuvent être exploités** par des attaquants
5. **NULL après free()** est la protection de base
6. **Outils = ASan, Valgrind, MALLOC_CHECK_**

### Règles d'or

- ✅ **free(ptr); ptr = NULL;** après chaque libération
- ✅ **Ne jamais accéder à un pointeur libéré**
- ✅ **Vérifier les alias avant libération**
- ✅ **Tester systématiquement avec ASan ou Valgrind**
- ✅ **Compiler avec warnings activés**
- ✅ **Code review focalisé sur malloc/free**

### Erreurs mortelles

- ❌ Oublier de mettre NULL après free()
- ❌ Libérer des copies de pointeurs
- ❌ Accéder à un pointeur après free()
- ❌ Buffer overflow sur le Heap
- ❌ Libérer deux fois via des alias

---

## Pour aller plus loin

### Sections recommandées

- **Section 9.5** : Fuites mémoire (risque moindre mais fréquent)
- **Module 5, Section 15.1** : AddressSanitizer en détail
- **Module 5, Section 15.4** : Valgrind Memcheck
- **Section 26** : Sécurité et code défensif

### Commandes essentielles

```bash
# Compilation sécurisée
gcc -Wall -Wextra -Werror -g -fsanitize=address programme.c

# Test Valgrind complet
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes --verbose ./programme

# Activation MALLOC_CHECK_
export MALLOC_CHECK_=3
./programme

# Core dump analysis
ulimit -c unlimited
gdb programme core
```

### Ressources

- "Secure Programming for Linux and Unix HOWTO"
- OWASP Memory Corruption Guidelines
- "The Art of Software Security Assessment"
- CVE Database (vulnerabilités réelles)

---


⏭️ [Stratégies d'allocation personnalisées](/09-allocation-dynamique/07-strategies-allocation.md)
