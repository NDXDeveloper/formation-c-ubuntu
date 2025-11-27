🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25.4 Assertions

## Introduction

Les **assertions** sont un mécanisme de **débogage** qui permet de vérifier qu'une condition est vraie à un point donné du programme. Si la condition est fausse, le programme s'arrête immédiatement avec un message d'erreur. Les assertions sont un outil puissant pour détecter les bugs **pendant le développement**, mais elles peuvent être désactivées en production pour ne pas impacter les performances.

---

## Qu'est-ce qu'une assertion ?

### Définition

Une assertion est une **affirmation** que vous faites sur l'état de votre programme. C'est comme dire : "À ce point du code, cette condition DOIT être vraie. Si elle ne l'est pas, c'est un bug."

### La macro assert()

En C, les assertions sont fournies par la macro `assert()`, définie dans `<assert.h>` :

```c
#include <assert.h>

void assert(scalar expression);
```

**Comportement :**
- Si l'expression est **vraie** (non-nulle) : rien ne se passe, le programme continue
- Si l'expression est **fausse** (nulle) : le programme affiche un message d'erreur et termine avec `abort()`

### Premier exemple

```c
#include <stdio.h>
#include <assert.h>

int main(void) {
    int age = 25;

    // Cette assertion passe (25 >= 0 est vrai)
    assert(age >= 0);
    printf("Age valide : %d\n", age);

    age = -5;

    // Cette assertion échoue (−5 >= 0 est faux)
    assert(age >= 0);  // ⚠️ Le programme s'arrête ICI

    printf("Cette ligne ne sera jamais exécutée\n");

    return 0;
}
```

**Sortie :**
```
Age valide : 25
main.c:12: main: Assertion `age >= 0' failed.
Aborted (core dumped)
```

Le programme s'arrête brutalement avec un message indiquant :
- Le **fichier** (main.c)
- La **ligne** (12)
- La **fonction** (main)
- L'**expression** qui a échoué (age >= 0)

---

## Quand utiliser les assertions ?

### Assertions vs Gestion d'erreurs

Il est **crucial** de comprendre la différence entre assertions et gestion d'erreurs :

| Aspect | Assertions | Gestion d'erreurs |
|--------|------------|-------------------|
| **But** | Détecter les **bugs** | Gérer les **conditions normales** |
| **Déclenchement** | Conditions qui ne devraient **jamais** arriver | Conditions attendues (fichier absent, réseau down) |
| **Production** | Peuvent être **désactivées** | Toujours **actives** |
| **Réponse** | **Arrêt immédiat** du programme | **Récupération** gracieuse |
| **Responsabilité** | Erreur du **programmeur** | Erreur de l'**environnement/utilisateur** |

### ✅ Bon usage des assertions

Les assertions sont appropriées pour vérifier :

#### 1. Préconditions (invariants d'entrée)

```c
#include <assert.h>
#include <string.h>

// Cette fonction suppose que le pointeur n'est jamais NULL
size_t calculer_longueur(const char *chaine) {
    // Précondition : chaine ne doit JAMAIS être NULL
    assert(chaine != NULL);

    return strlen(chaine);
}
```

#### 2. Postconditions (invariants de sortie)

```c
#include <assert.h>
#include <stdlib.h>

int* creer_tableau(size_t taille) {
    int *tableau = malloc(taille * sizeof(int));

    // Postcondition : l'allocation doit avoir réussi
    assert(tableau != NULL);

    // Initialisation
    for (size_t i = 0; i < taille; i++) {
        tableau[i] = 0;
    }

    return tableau;
}
```

#### 3. Invariants de boucle

```c
#include <assert.h>

void traiter_tableau(int *tab, size_t taille) {
    assert(tab != NULL);
    assert(taille > 0);

    for (size_t i = 0; i < taille; i++) {
        // Invariant : i doit toujours être dans les limites
        assert(i < taille);

        tab[i] *= 2;
    }
}
```

#### 4. États impossibles

```c
#include <assert.h>

typedef enum {
    ETAT_INIT,
    ETAT_EN_COURS,
    ETAT_TERMINE
} Etat;

void traiter_etat(Etat etat) {
    switch (etat) {
        case ETAT_INIT:
            printf("Initialisation\n");
            break;
        case ETAT_EN_COURS:
            printf("En cours\n");
            break;
        case ETAT_TERMINE:
            printf("Terminé\n");
            break;
        default:
            // Cet état ne devrait JAMAIS se produire
            assert(0 && "État invalide");
    }
}
```

#### 5. Vérifications mathématiques

```c
#include <assert.h>

int division(int a, int b) {
    // Le diviseur ne doit JAMAIS être zéro
    assert(b != 0);

    return a / b;
}

unsigned int factorielle(unsigned int n) {
    // n doit être raisonnable pour éviter l'overflow
    assert(n <= 12);  // 13! dépasse uint max

    if (n <= 1) return 1;
    return n * factorielle(n - 1);
}
```

### ❌ Mauvais usage des assertions

Les assertions ne doivent **PAS** être utilisées pour :

#### 1. Valider les entrées utilisateur

```c
// ❌ FAUX : L'utilisateur peut entrer n'importe quoi
void traiter_age(int age) {
    assert(age >= 0 && age <= 150);  // Mauvais !
    // ...
}

// ✅ CORRECT : Validation avec gestion d'erreur
int traiter_age(int age) {
    if (age < 0 || age > 150) {
        fprintf(stderr, "Erreur : âge invalide\n");
        return -1;
    }
    // ...
    return 0;
}
```

#### 2. Gérer les erreurs système

```c
// ❌ FAUX : L'ouverture de fichier peut légitimement échouer
FILE *f = fopen("data.txt", "r");
assert(f != NULL);  // Mauvais !

// ✅ CORRECT : Gestion d'erreur classique
FILE *f = fopen("data.txt", "r");
if (f == NULL) {
    perror("fopen");
    return -1;
}
```

#### 3. Vérifier les conditions réseau

```c
// ❌ FAUX : Le réseau peut être indisponible
int sock = socket(AF_INET, SOCK_STREAM, 0);
assert(sock >= 0);  // Mauvais !

// ✅ CORRECT
int sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) {
    perror("socket");
    return -1;
}
```

#### 4. Détecter les débordements mémoire

```c
// ❌ FAUX : malloc peut échouer légitimement (mémoire insuffisante)
void *ptr = malloc(1000000000);
assert(ptr != NULL);  // Mauvais !

// ✅ CORRECT
void *ptr = malloc(1000000000);
if (ptr == NULL) {
    fprintf(stderr, "Erreur : allocation échouée\n");
    return -1;
}
```

### Règle d'or

> **Utilisez les assertions pour les bugs du programmeur, pas pour les erreurs d'exécution.**

```c
// Assertion : pour les bugs (erreur de logique)
assert(pointeur != NULL);  // Le programmeur a mal codé

// Gestion d'erreur : pour les conditions attendues
if (fichier_introuvable) {  // L'utilisateur a mal saisi le nom
    return -1;
}
```

---

## Désactivation des assertions : NDEBUG

### Principe

Les assertions ont un **coût en performance** (même minime). En production, on souhaite souvent les **désactiver** pour gagner en vitesse.

### La macro NDEBUG

Si la macro `NDEBUG` est définie **avant** l'inclusion de `<assert.h>`, toutes les assertions sont **désactivées** (remplacées par du code vide).

#### Méthode 1 : Définir NDEBUG dans le code

```c
#define NDEBUG  // Désactive les assertions
#include <assert.h>
#include <stdio.h>

int main(void) {
    int valeur = -10;

    // Cette assertion est ignorée (code remplacé par rien)
    assert(valeur >= 0);

    printf("valeur = %d\n", valeur);  // Affiche -10

    return 0;
}
```

**Sortie :**
```
valeur = -10
```

Le programme ne s'arrête pas car l'assertion est désactivée.

#### Méthode 2 : Définir NDEBUG à la compilation

C'est l'approche la plus courante :

```bash
# Compilation en mode debug (assertions activées)
gcc -Wall -g main.c -o main

# Compilation en mode release (assertions désactivées)
gcc -Wall -O2 -DNDEBUG main.c -o main
```

Le flag `-DNDEBUG` définit la macro `NDEBUG` globalement.

### Exemple avec CMake

```cmake
# Mode Debug : assertions activées
set(CMAKE_C_FLAGS_DEBUG "-g -Wall")

# Mode Release : assertions désactivées
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -Wall")

# Sélection du mode
cmake -DCMAKE_BUILD_TYPE=Debug ..   # Avec assertions
cmake -DCMAKE_BUILD_TYPE=Release .. # Sans assertions
```

### Impact sur le code

```c
#include <assert.h>

void fonction(int *ptr) {
    assert(ptr != NULL);  // Coût : une comparaison + branchement

    *ptr = 42;
}
```

**Avec assertions (Debug) :**
```asm
; Pseudocode assembleur
cmp ptr, 0
jne continue
call abort
continue:
  mov [ptr], 42
```

**Sans assertions (Release, avec NDEBUG) :**
```asm
; Pseudocode assembleur
mov [ptr], 42  ; L'assertion a complètement disparu
```

### Précautions avec NDEBUG

**⚠️ ATTENTION** : Ne mettez jamais d'**effets de bord** dans une assertion, car ils disparaîtront en mode release !

```c
// ❌ DANGER : Comportement différent debug vs release
int compteur = 0;
assert(++compteur > 0);  // Mauvais ! compteur n'est pas incrémenté en release

// ✅ CORRECT
int compteur = 0;
compteur++;
assert(compteur > 0);  // Bon : pas d'effet de bord dans l'assertion
```

**Autre exemple dangereux :**

```c
// ❌ FAUX : Cette allocation disparaît en release !
assert((ptr = malloc(100)) != NULL);

// ✅ CORRECT
ptr = malloc(100);
assert(ptr != NULL);
```

---

## Assertions avec messages personnalisés

### Technique 1 : Utiliser l'opérateur &&

L'astuce consiste à utiliser `&& "message"` car une chaîne non-vide est toujours vraie :

```c
#include <assert.h>

int main(void) {
    int valeur = -5;

    // Message personnalisé
    assert(valeur >= 0 && "La valeur doit être positive");

    return 0;
}
```

**Sortie :**
```
main.c:6: main: Assertion `valeur >= 0 && "La valeur doit être positive"' failed.
Aborted
```

Le message apparaît dans l'assertion !

### Technique 2 : Macro personnalisée

Pour plus de flexibilité, créez votre propre macro :

```c
#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
    #define ASSERT_MSG(condition, message) ((void)0)
#else
    #define ASSERT_MSG(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "Assertion échouée : %s\n", message); \
                fprintf(stderr, "  Fichier : %s\n", __FILE__); \
                fprintf(stderr, "  Ligne : %d\n", __LINE__); \
                fprintf(stderr, "  Fonction : %s\n", __func__); \
                abort(); \
            } \
        } while (0)
#endif

int main(void) {
    int age = 200;

    ASSERT_MSG(age >= 0 && age < 150,
               "L'âge doit être entre 0 et 150");

    return 0;
}
```

**Sortie :**
```
Assertion échouée : L'âge doit être entre 0 et 150
  Fichier : main.c
  Ligne : 19
  Fonction : main
Aborted
```

---

## Assertions statiques (C11)

### Principe

Les **assertions statiques** sont évaluées **à la compilation** et non à l'exécution. Elles permettent de vérifier des conditions sur les types, tailles, etc.

### La macro static_assert (C11)

Depuis C11, la macro `static_assert()` est disponible :

```c
#include <assert.h>  // ou <static_assert.h> en C23

static_assert(expression_constante, "message");
```

### Exemples d'utilisation

#### 1. Vérifier la taille d'un type

```c
#include <stdint.h>
#include <assert.h>

// Vérifier que int fait bien 4 octets sur ce système
static_assert(sizeof(int) == 4, "int doit faire 4 octets");

// Vérifier l'alignement d'un pointeur
static_assert(sizeof(void*) == 8, "Ce code nécessite un système 64 bits");

int main(void) {
    return 0;
}
```

**Si la condition est fausse à la compilation :**
```
error: static assertion failed: "int doit faire 4 octets"
```

#### 2. Vérifier la taille d'une structure

```c
#include <assert.h>
#include <stddef.h>

typedef struct {
    uint8_t type;
    uint32_t valeur;
} __attribute__((packed)) Paquet;

// S'assurer que la structure a la taille attendue
static_assert(sizeof(Paquet) == 5, "Paquet doit faire 5 octets");

int main(void) {
    return 0;
}
```

#### 3. Vérifier des constantes

```c
#include <assert.h>

#define TAILLE_BUFFER 1024
#define TAILLE_MIN 512

// Vérifier que le buffer est assez grand
static_assert(TAILLE_BUFFER >= TAILLE_MIN,
              "Le buffer doit faire au moins 512 octets");

int main(void) {
    char buffer[TAILLE_BUFFER];
    return 0;
}
```

#### 4. Vérifier l'ordre des énumérations

```c
#include <assert.h>

typedef enum {
    PRIORITE_BASSE = 0,
    PRIORITE_NORMALE = 1,
    PRIORITE_HAUTE = 2
} Priorite;

// Vérifier que l'ordre est correct
static_assert(PRIORITE_BASSE < PRIORITE_NORMALE,
              "Ordre des priorités invalide");
static_assert(PRIORITE_NORMALE < PRIORITE_HAUTE,
              "Ordre des priorités invalide");

int main(void) {
    return 0;
}
```

### Avantages de static_assert

- ✅ **Détection précoce** : Erreurs détectées à la compilation
- ✅ **Zéro overhead** : Aucun coût à l'exécution
- ✅ **Documentation** : Explicite les hypothèses du code
- ✅ **Portabilité** : Vérifie les hypothèses sur différentes plateformes

---

## Assertions et structures de données

### Exemple : Liste chaînée

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Noeud {
    int valeur;
    struct Noeud *suivant;
} Noeud;

// Créer un nouveau noeud
Noeud* creer_noeud(int valeur) {
    Noeud *nouveau = malloc(sizeof(Noeud));

    // Postcondition : l'allocation doit réussir
    assert(nouveau != NULL);

    nouveau->valeur = valeur;
    nouveau->suivant = NULL;

    return nouveau;
}

// Insérer en tête
void inserer_tete(Noeud **tete, int valeur) {
    // Précondition : pointeur valide
    assert(tete != NULL);

    Noeud *nouveau = creer_noeud(valeur);
    nouveau->suivant = *tete;
    *tete = nouveau;

    // Postcondition : la tête pointe sur le nouveau noeud
    assert(*tete == nouveau);
}

// Compter les éléments
int compter(Noeud *tete) {
    int compte = 0;

    for (Noeud *courant = tete; courant != NULL; courant = courant->suivant) {
        compte++;

        // Invariant : pas de boucle infinie
        assert(compte < 10000 && "Possible boucle infinie détectée");
    }

    return compte;
}

// Libérer la liste
void liberer(Noeud *tete) {
    while (tete != NULL) {
        Noeud *suivant = tete->suivant;
        free(tete);
        tete = suivant;
    }
}

int main(void) {
    Noeud *liste = NULL;

    inserer_tete(&liste, 10);
    inserer_tete(&liste, 20);
    inserer_tete(&liste, 30);

    int nb = compter(liste);
    assert(nb == 3);  // Vérification

    printf("Nombre d'éléments : %d\n", nb);

    liberer(liste);
    return 0;
}
```

---

## Assertions avancées : Patterns utiles

### Pattern 1 : Vérifier les invariants de structure

```c
typedef struct {
    int *donnees;
    size_t taille;
    size_t capacite;
} VecteurDynamique;

// Fonction pour vérifier l'invariant
static inline void verifier_invariant(const VecteurDynamique *v) {
    assert(v != NULL);
    assert(v->taille <= v->capacite);
    assert((v->capacite == 0) == (v->donnees == NULL));
    assert(v->capacite == 0 || v->donnees != NULL);
}

void ajouter_element(VecteurDynamique *v, int element) {
    verifier_invariant(v);  // Précondition

    // ... ajout de l'élément ...

    verifier_invariant(v);  // Postcondition
}
```

### Pattern 2 : Assertions de debugging temporaires

```c
#ifdef DEBUG_VERBOSE
    #define DEBUG_ASSERT(cond) assert(cond)
#else
    #define DEBUG_ASSERT(cond) ((void)0)
#endif

void traiter_donnees(int *tab, size_t n) {
    // Cette assertion n'est active qu'en mode DEBUG_VERBOSE
    DEBUG_ASSERT(tab != NULL && n > 0);

    for (size_t i = 0; i < n; i++) {
        DEBUG_ASSERT(tab[i] >= 0);  // Vérification intensive
        tab[i] *= 2;
    }
}
```

### Pattern 3 : Assertions dans les switch

```c
typedef enum {
    TYPE_A,
    TYPE_B,
    TYPE_C
} Type;

void traiter_type(Type type) {
    switch (type) {
        case TYPE_A:
            printf("Type A\n");
            break;
        case TYPE_B:
            printf("Type B\n");
            break;
        case TYPE_C:
            printf("Type C\n");
            break;
        default:
            assert(0 && "Type inconnu");
            // ou : __builtin_unreachable(); sur GCC/Clang
    }
}
```

---

## Assertions et Tests Unitaires

Les assertions sont **complémentaires** aux tests unitaires :

```c
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Fonction à tester
int ajouter(int a, int b) {
    // Assertion pour détecter les bugs de débordement
    assert(a >= 0 && b >= 0);  // Suppose qu'on ne travaille qu'avec des positifs

    int resultat = a + b;

    // Postcondition
    assert(resultat >= a && resultat >= b);

    return resultat;
}

// Tests unitaires
void test_ajouter(void) {
    // Test 1
    assert(ajouter(2, 3) == 5);
    printf("✓ Test 1 passé\n");

    // Test 2
    assert(ajouter(0, 0) == 0);
    printf("✓ Test 2 passé\n");

    // Test 3
    assert(ajouter(100, 200) == 300);
    printf("✓ Test 3 passé\n");
}

int main(void) {
    printf("=== Exécution des tests ===\n");
    test_ajouter();
    printf("=== Tous les tests sont passés ===\n");

    return 0;
}
```

**Différences :**

| Assertions | Tests Unitaires |
|------------|-----------------|
| Dans le code de production | Dans un fichier de test séparé |
| Vérifier les invariants | Vérifier le comportement |
| Désactivables en release | Toujours actifs |
| Détection de bugs internes | Validation fonctionnelle |

---

## Bonnes pratiques

### ✅ À FAIRE

1. **Documenter vos hypothèses** avec des assertions
   ```c
   // Cette fonction suppose que le tableau n'est jamais vide
   assert(taille > 0);
   ```

2. **Vérifier les préconditions** au début des fonctions
   ```c
   void traiter(int *ptr, size_t n) {
       assert(ptr != NULL);
       assert(n > 0);
       // ...
   }
   ```

3. **Vérifier les postconditions** avant de retourner
   ```c
   int* allouer_et_initialiser(size_t n) {
       int *tab = malloc(n * sizeof(int));
       assert(tab != NULL);

       for (size_t i = 0; i < n; i++) {
           tab[i] = 0;
       }

       assert(tab[0] == 0);  // Postcondition
       return tab;
   }
   ```

4. **Utiliser static_assert** pour les vérifications de types
   ```c
   static_assert(sizeof(long) == 8, "Ce code nécessite des longs de 64 bits");
   ```

5. **Désactiver en production** avec `-DNDEBUG`

### ❌ À ÉVITER

1. **Mettre des effets de bord** dans les assertions
   ```c
   // ❌ FAUX
   assert(++compteur > 0);

   // ✅ CORRECT
   compteur++;
   assert(compteur > 0);
   ```

2. **Utiliser pour valider les entrées externes**
   ```c
   // ❌ FAUX
   void traiter_age_utilisateur(int age) {
       assert(age >= 0);  // L'utilisateur peut entrer -5 !
   }

   // ✅ CORRECT
   int traiter_age_utilisateur(int age) {
       if (age < 0) {
           return -1;
       }
       // ...
   }
   ```

3. **Trop d'assertions** peut ralentir le debug
   ```c
   // ❌ Excessif
   for (size_t i = 0; i < 1000000; i++) {
       assert(i < 1000000);  // Évident et coûteux
       // ...
   }
   ```

4. **Oublier que les assertions disparaissent en release**

---

## Checklist pour les assertions

Avant d'écrire une assertion, demandez-vous :

- [ ] Cette condition peut-elle légitimement échouer ?
  - Si **OUI** → Utilisez la gestion d'erreurs (`if`)
  - Si **NON** → Utilisez une assertion

- [ ] Est-ce une erreur du programmeur ou de l'utilisateur ?
  - **Programmeur** → Assertion
  - **Utilisateur** → Gestion d'erreurs

- [ ] L'assertion contient-elle des effets de bord ?
  - Si **OUI** → Refactorez pour séparer l'effet de bord

- [ ] Voulez-vous que cette vérification reste en production ?
  - Si **OUI** → Utilisez `if`, pas `assert`
  - Si **NON** → `assert` est OK

---

## Exemple complet : Gestionnaire de mémoire

Voici un exemple combinant tous les concepts :

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

// Vérifications statiques
static_assert(sizeof(void*) == 8, "Ce code nécessite un système 64 bits");
static_assert(sizeof(size_t) >= 4, "size_t doit faire au moins 4 octets");

typedef struct {
    void *donnees;
    size_t taille_utilisee;
    size_t taille_allouee;
} Buffer;

// Fonction pour vérifier l'invariant
static inline int invariant_buffer_valide(const Buffer *b) {
    if (b == NULL) return 0;
    if (b->taille_utilisee > b->taille_allouee) return 0;
    if (b->taille_allouee > 0 && b->donnees == NULL) return 0;
    if (b->taille_allouee == 0 && b->donnees != NULL) return 0;
    return 1;
}

// Initialiser un buffer
Buffer* buffer_creer(size_t taille_initiale) {
    // Précondition
    assert(taille_initiale > 0 && taille_initiale < SIZE_MAX / 2);

    Buffer *b = malloc(sizeof(Buffer));
    assert(b != NULL);  // Pour la démo, en vrai il faut gérer l'erreur

    b->donnees = malloc(taille_initiale);
    assert(b->donnees != NULL);

    b->taille_utilisee = 0;
    b->taille_allouee = taille_initiale;

    // Postcondition
    assert(invariant_buffer_valide(b));

    return b;
}

// Ajouter des données
void buffer_ajouter(Buffer *b, const void *donnees, size_t taille) {
    // Préconditions
    assert(invariant_buffer_valide(b));
    assert(donnees != NULL || taille == 0);
    assert(taille < SIZE_MAX - b->taille_utilisee);  // Pas d'overflow

    // Réallocation si nécessaire
    if (b->taille_utilisee + taille > b->taille_allouee) {
        size_t nouvelle_taille = (b->taille_allouee + taille) * 2;
        assert(nouvelle_taille > b->taille_allouee);  // Pas d'overflow

        void *nouveau = realloc(b->donnees, nouvelle_taille);
        assert(nouveau != NULL);

        b->donnees = nouveau;
        b->taille_allouee = nouvelle_taille;
    }

    // Copie des données
    if (taille > 0) {
        memcpy((char*)b->donnees + b->taille_utilisee, donnees, taille);
        b->taille_utilisee += taille;
    }

    // Postcondition
    assert(invariant_buffer_valide(b));
}

// Libérer le buffer
void buffer_detruire(Buffer *b) {
    if (b != NULL) {
        assert(invariant_buffer_valide(b));

        free(b->donnees);
        free(b);
    }
}

int main(void) {
    printf("=== Test du buffer ===\n");

    Buffer *b = buffer_creer(16);
    printf("Buffer créé : %zu octets alloués\n", b->taille_allouee);

    const char *msg1 = "Hello, ";
    buffer_ajouter(b, msg1, strlen(msg1));
    printf("Après ajout 1 : %zu/%zu octets\n",
           b->taille_utilisee, b->taille_allouee);

    const char *msg2 = "World!";
    buffer_ajouter(b, msg2, strlen(msg2));
    printf("Après ajout 2 : %zu/%zu octets\n",
           b->taille_utilisee, b->taille_allouee);

    // Afficher le contenu
    printf("Contenu : %.*s\n", (int)b->taille_utilisee, (char*)b->donnees);

    buffer_detruire(b);
    printf("Buffer détruit\n");

    return 0;
}
```

---

## Résumé

### 🎯 Points clés

1. **Assertions = Bugs du programmeur**
   - Utilisez pour les conditions qui ne devraient JAMAIS arriver
   - Pas pour les erreurs d'exécution normales

2. **assert() pour le runtime**
   - Vérifications pendant l'exécution
   - Désactivables avec `-DNDEBUG`

3. **static_assert() pour la compilation**
   - Vérifications à la compilation
   - Zéro overhead

4. **Pas d'effets de bord**
   - Jamais `assert(++x > 0)`
   - Toujours séparer l'effet de l'assertion

5. **Debug vs Release**
   - Debug : assertions actives
   - Release : assertions désactivées (`-DNDEBUG`)

### 💡 Règle d'or

> **Les assertions documentent et vérifient vos hypothèses. Elles sont vos alliées pour détecter les bugs tôt.**

### Quand utiliser quoi ?

```
Condition impossible (bug) → assert()
Condition rare mais possible → if + gestion erreur
Vérification de type/taille → static_assert()
Entrée utilisateur → if + validation
Erreur système → if + errno
```

Les assertions sont un outil **essentiel** pour écrire du code C robuste et maintenable. Utilisées correctement, elles vous permettent de détecter les bugs **avant** qu'ils ne causent des problèmes en production.

---

**Prochaine section** : 25.5 Design by contract

⏭️ [Design by contract](/25-gestion-erreurs/05-design-by-contract.md)
