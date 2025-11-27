🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25.5 Design by contract

## Introduction

Le **Design by Contract** (DbC), ou "Conception par contrat", est une approche de programmation formalisée par Bertrand Meyer dans le langage Eiffel. L'idée centrale est simple mais puissante : chaque fonction est vue comme un **contrat** entre le code appelant et la fonction appelée. Ce contrat spécifie :

- Ce que l'appelant **doit garantir** (préconditions)
- Ce que la fonction **doit garantir** en retour (postconditions)
- Ce qui doit **toujours être vrai** pour un objet (invariants)

Bien que C ne supporte pas nativement le DbC, nous pouvons l'implémenter efficacement avec les assertions et des conventions de code.

---

## Les trois piliers du Design by Contract

### 1. Préconditions (Preconditions)

**Définition :** Ce que l'appelant **doit garantir** avant d'appeler la fonction. C'est la **responsabilité de l'appelant**.

```c
// Contrat : l'appelant doit fournir un pointeur non-NULL et une taille > 0
void traiter_tableau(int *tableau, size_t taille) {
    // Préconditions
    assert(tableau != NULL);
    assert(taille > 0);

    // ... traitement ...
}
```

**Métaphore :** Comme un contrat de location, le locataire doit respecter certaines conditions (ne pas avoir d'animaux, être solvable) **avant** de signer.

### 2. Postconditions (Postconditions)

**Définition :** Ce que la fonction **garantit** après son exécution, si les préconditions sont respectées. C'est la **responsabilité de la fonction**.

```c
// Contrat : si x et y sont positifs, le résultat sera >= x et >= y
int ajouter(int x, int y) {
    // Préconditions
    assert(x >= 0);
    assert(y >= 0);

    int resultat = x + y;

    // Postconditions
    assert(resultat >= x);
    assert(resultat >= y);

    return resultat;
}
```

**Métaphore :** Le propriétaire garantit que le logement sera propre, chauffé et conforme à la description.

### 3. Invariants de classe/structure

**Définition :** Propriétés qui doivent **toujours être vraies** pour un objet, avant et après chaque opération publique.

```c
typedef struct {
    int *donnees;
    size_t taille;
    size_t capacite;
} Vecteur;

// Invariant :
// - taille <= capacite
// - Si capacite > 0, alors donnees != NULL
// - Si capacite == 0, alors donnees == NULL

void verifier_invariant_vecteur(const Vecteur *v) {
    assert(v != NULL);
    assert(v->taille <= v->capacite);
    assert((v->capacite == 0) == (v->donnees == NULL));
}
```

**Métaphore :** Les règles de copropriété qui s'appliquent en permanence (pas de bruit après 22h, entretien des parties communes).

---

## Exemple introductif : Fonction de division

Appliquons le DbC à une simple fonction de division :

```c
#include <stdio.h>
#include <assert.h>

/**
 * Division entière.
 *
 * CONTRAT :
 * Préconditions :
 *   - diviseur != 0
 *
 * Postconditions :
 *   - résultat * diviseur + reste == dividende
 *   - 0 <= reste < diviseur (pour diviseur positif)
 */
int diviser(int dividende, int diviseur, int *reste) {
    // Préconditions
    assert(diviseur != 0 && "Le diviseur ne peut pas être zéro");
    assert(reste != NULL && "Le pointeur reste doit être valide");

    // Calcul
    int quotient = dividende / diviseur;
    *reste = dividende % diviseur;

    // Postconditions
    assert(quotient * diviseur + *reste == dividende &&
           "Le théorème de la division doit être respecté");

    return quotient;
}

int main(void) {
    int reste;

    // Utilisation correcte
    int q = diviser(17, 5, &reste);
    printf("17 / 5 = %d reste %d\n", q, reste);

    // ⚠️ Violation de précondition : diviseur = 0
    // Cette ligne déclencherait une assertion
    // int q2 = diviser(10, 0, &reste);

    return 0;
}
```

**Sortie :**
```
17 / 5 = 3 reste 2
```

---

## Implémentation du DbC en C

### Macros de contrat

Pour rendre le code plus lisible, créons des macros spécifiques :

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Macros pour le Design by Contract
#ifdef NDEBUG
    #define PRECONDITION(condition)  ((void)0)
    #define POSTCONDITION(condition) ((void)0)
    #define INVARIANT(condition)     ((void)0)
#else
    #define PRECONDITION(condition) \
        assert((condition) && "Précondition violée")

    #define POSTCONDITION(condition) \
        assert((condition) && "Postcondition violée")

    #define INVARIANT(condition) \
        assert((condition) && "Invariant violé")
#endif

// Exemple d'utilisation
int factorielle(int n) {
    PRECONDITION(n >= 0);
    PRECONDITION(n <= 12);  // Éviter l'overflow pour int

    int resultat = 1;
    for (int i = 2; i <= n; i++) {
        resultat *= i;
    }

    POSTCONDITION(resultat > 0);

    return resultat;
}

int main(void) {
    printf("5! = %d\n", factorielle(5));

    // ⚠️ Violation de précondition
    // printf("20! = %d\n", factorielle(20));

    return 0;
}
```

### Macros avec messages personnalisés

Version plus élaborée avec des messages d'erreur informatifs :

```c
#ifndef NDEBUG
    #define REQUIRE(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "\n=== VIOLATION DE CONTRAT ===\n"); \
                fprintf(stderr, "Type       : Précondition\n"); \
                fprintf(stderr, "Condition  : %s\n", #condition); \
                fprintf(stderr, "Message    : %s\n", message); \
                fprintf(stderr, "Fichier    : %s\n", __FILE__); \
                fprintf(stderr, "Ligne      : %d\n", __LINE__); \
                fprintf(stderr, "Fonction   : %s\n", __func__); \
                fprintf(stderr, "=============================\n\n"); \
                abort(); \
            } \
        } while (0)

    #define ENSURE(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "\n=== VIOLATION DE CONTRAT ===\n"); \
                fprintf(stderr, "Type       : Postcondition\n"); \
                fprintf(stderr, "Condition  : %s\n", #condition); \
                fprintf(stderr, "Message    : %s\n", message); \
                fprintf(stderr, "Fichier    : %s\n", __FILE__); \
                fprintf(stderr, "Ligne      : %d\n", __LINE__); \
                fprintf(stderr, "Fonction   : %s\n", __func__); \
                fprintf(stderr, "=============================\n\n"); \
                abort(); \
            } \
        } while (0)

    #define CHECK_INVARIANT(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "\n=== VIOLATION DE CONTRAT ===\n"); \
                fprintf(stderr, "Type       : Invariant\n"); \
                fprintf(stderr, "Condition  : %s\n", #condition); \
                fprintf(stderr, "Message    : %s\n", message); \
                fprintf(stderr, "Fichier    : %s\n", __FILE__); \
                fprintf(stderr, "Ligne      : %d\n", __LINE__); \
                fprintf(stderr, "Fonction   : %s\n", __func__); \
                fprintf(stderr, "=============================\n\n"); \
                abort(); \
            } \
        } while (0)
#else
    #define REQUIRE(condition, message)      ((void)0)
    #define ENSURE(condition, message)       ((void)0)
    #define CHECK_INVARIANT(condition, message) ((void)0)
#endif
```

---

## Exemple complet : Pile (Stack)

Implémentons une pile avec un contrat rigoureux :

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define PRECONDITION(cond)  assert((cond) && "Précondition")
#define POSTCONDITION(cond) assert((cond) && "Postcondition")
#define INVARIANT(cond)     assert((cond) && "Invariant")

// Structure Pile
typedef struct {
    int *elements;
    size_t taille;      // Nombre d'éléments actuels
    size_t capacite;    // Capacité maximale
} Pile;

/**
 * Invariant de la pile :
 * - elements != NULL si capacite > 0
 * - elements == NULL si capacite == 0
 * - taille <= capacite
 * - taille >= 0
 */
static void pile_verifier_invariant(const Pile *p) {
    INVARIANT(p != NULL);
    INVARIANT(p->taille <= p->capacite);
    INVARIANT((p->capacite == 0) == (p->elements == NULL));
    INVARIANT((p->capacite > 0) == (p->elements != NULL));
}

/**
 * Créer une pile.
 *
 * Préconditions :
 *   - capacite > 0
 *
 * Postconditions :
 *   - Retourne une pile vide
 *   - pile->taille == 0
 *   - pile->capacite == capacite
 *   - Invariant respecté
 */
Pile* pile_creer(size_t capacite) {
    PRECONDITION(capacite > 0);

    Pile *p = malloc(sizeof(Pile));
    POSTCONDITION(p != NULL);  // Pour la démo

    p->elements = malloc(capacite * sizeof(int));
    POSTCONDITION(p->elements != NULL);

    p->taille = 0;
    p->capacite = capacite;

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == 0);
    POSTCONDITION(p->capacite == capacite);

    return p;
}

/**
 * Vérifier si la pile est vide.
 *
 * Préconditions :
 *   - p != NULL
 *   - Invariant respecté
 *
 * Postconditions :
 *   - Retourne true si taille == 0
 *   - Invariant toujours respecté
 */
bool pile_est_vide(const Pile *p) {
    PRECONDITION(p != NULL);
    pile_verifier_invariant(p);

    bool resultat = (p->taille == 0);

    pile_verifier_invariant(p);
    POSTCONDITION((resultat == true) == (p->taille == 0));

    return resultat;
}

/**
 * Vérifier si la pile est pleine.
 *
 * Préconditions :
 *   - p != NULL
 *   - Invariant respecté
 *
 * Postconditions :
 *   - Retourne true si taille == capacite
 *   - Invariant toujours respecté
 */
bool pile_est_pleine(const Pile *p) {
    PRECONDITION(p != NULL);
    pile_verifier_invariant(p);

    bool resultat = (p->taille == p->capacite);

    pile_verifier_invariant(p);
    POSTCONDITION((resultat == true) == (p->taille == p->capacite));

    return resultat;
}

/**
 * Empiler un élément.
 *
 * Préconditions :
 *   - p != NULL
 *   - !pile_est_pleine(p)
 *   - Invariant respecté
 *
 * Postconditions :
 *   - p->taille a augmenté de 1
 *   - Le sommet contient la valeur empilée
 *   - Invariant respecté
 */
void pile_empiler(Pile *p, int valeur) {
    PRECONDITION(p != NULL);
    PRECONDITION(!pile_est_pleine(p));
    pile_verifier_invariant(p);

    size_t ancienne_taille = p->taille;

    p->elements[p->taille] = valeur;
    p->taille++;

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == ancienne_taille + 1);
    POSTCONDITION(p->elements[p->taille - 1] == valeur);
}

/**
 * Dépiler un élément.
 *
 * Préconditions :
 *   - p != NULL
 *   - !pile_est_vide(p)
 *   - Invariant respecté
 *
 * Postconditions :
 *   - p->taille a diminué de 1
 *   - Retourne l'ancien sommet
 *   - Invariant respecté
 */
int pile_depiler(Pile *p) {
    PRECONDITION(p != NULL);
    PRECONDITION(!pile_est_vide(p));
    pile_verifier_invariant(p);

    size_t ancienne_taille = p->taille;

    p->taille--;
    int valeur = p->elements[p->taille];

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == ancienne_taille - 1);

    return valeur;
}

/**
 * Consulter le sommet sans dépiler.
 *
 * Préconditions :
 *   - p != NULL
 *   - !pile_est_vide(p)
 *   - Invariant respecté
 *
 * Postconditions :
 *   - p->taille n'a pas changé
 *   - Invariant respecté
 */
int pile_sommet(const Pile *p) {
    PRECONDITION(p != NULL);
    PRECONDITION(!pile_est_vide(p));
    pile_verifier_invariant(p);

    size_t ancienne_taille = p->taille;
    int valeur = p->elements[p->taille - 1];

    pile_verifier_invariant(p);
    POSTCONDITION(p->taille == ancienne_taille);

    return valeur;
}

/**
 * Détruire la pile.
 *
 * Préconditions :
 *   - Invariant respecté (si p != NULL)
 *
 * Postconditions :
 *   - La mémoire est libérée
 */
void pile_detruire(Pile *p) {
    if (p != NULL) {
        pile_verifier_invariant(p);
        free(p->elements);
        free(p);
    }
}

int main(void) {
    printf("=== Test de la pile avec DbC ===\n\n");

    // Créer une pile
    Pile *p = pile_creer(5);
    printf("Pile créée (capacité: 5)\n");

    // Empiler des éléments
    printf("\nEmpilage de 10, 20, 30\n");
    pile_empiler(p, 10);
    pile_empiler(p, 20);
    pile_empiler(p, 30);

    // Consulter le sommet
    printf("Sommet : %d\n", pile_sommet(p));

    // Dépiler
    printf("\nDépilage :\n");
    while (!pile_est_vide(p)) {
        printf("  Dépilé : %d\n", pile_depiler(p));
    }

    printf("\nPile vide : %s\n", pile_est_vide(p) ? "oui" : "non");

    // ⚠️ Violation de précondition : dépiler une pile vide
    // Décommentez pour voir l'assertion
    // printf("Tentative de dépiler une pile vide...\n");
    // pile_depiler(p);

    pile_detruire(p);
    printf("\nPile détruite\n");

    return 0;
}
```

**Sortie :**
```
=== Test de la pile avec DbC ===

Pile créée (capacité: 5)

Empilage de 10, 20, 30
Sommet : 30

Dépilage :
  Dépilé : 30
  Dépilé : 20
  Dépilé : 10

Pile vide : oui

Pile détruite
```

---

## Avantages du Design by Contract

### 1. Documentation vivante

Le contrat **documente** le comportement de la fonction de manière **exécutable** :

```c
/**
 * Recherche binaire dans un tableau trié.
 *
 * Préconditions :
 *   - tableau != NULL
 *   - taille > 0
 *   - Le tableau DOIT être trié en ordre croissant
 */
int recherche_binaire(const int *tableau, size_t taille, int valeur) {
    PRECONDITION(tableau != NULL);
    PRECONDITION(taille > 0);

    // Vérifier que le tableau est trié (coûteux, uniquement en debug)
    #ifndef NDEBUG
    for (size_t i = 1; i < taille; i++) {
        PRECONDITION(tableau[i-1] <= tableau[i]);
    }
    #endif

    // ... recherche binaire ...
}
```

### 2. Détection précoce des bugs

Les violations de contrat sont détectées **immédiatement** au point de l'erreur :

```c
void traiter_utilisateur(const char *nom, int age) {
    PRECONDITION(nom != NULL);
    PRECONDITION(age >= 0 && age <= 150);

    // Si on arrive ici, les données sont valides
    printf("Utilisateur : %s, %d ans\n", nom, age);
}

int main(void) {
    // ⚠️ Violation immédiate à l'appel
    traiter_utilisateur(NULL, -5);  // Assertion déclenche ICI

    return 0;
}
```

Sans DbC, le bug pourrait se manifester **bien plus tard** (segfault, corruption de données).

### 3. Clarification des responsabilités

Le contrat définit **qui** est responsable de **quoi** :

```c
// Responsabilité de l'APPELANT : fournir un tableau non-NULL
// Responsabilité de la FONCTION : retourner la somme correcte
int somme_tableau(const int *tableau, size_t taille) {
    PRECONDITION(tableau != NULL);  // Responsabilité appelant
    PRECONDITION(taille > 0);       // Responsabilité appelant

    int somme = 0;
    for (size_t i = 0; i < taille; i++) {
        somme += tableau[i];
    }

    // Si taille > 0, la somme a été calculée
    POSTCONDITION(taille == 0 || somme != 0 || tableau[0] == 0);

    return somme;
}
```

### 4. Facilite le refactoring

Avec un contrat clair, on peut **changer l'implémentation** tant qu'on respecte le contrat :

```c
// Version 1 : Itérative
int factorielle_v1(int n) {
    PRECONDITION(n >= 0 && n <= 12);

    int resultat = 1;
    for (int i = 2; i <= n; i++) {
        resultat *= i;
    }

    POSTCONDITION(resultat > 0);
    return resultat;
}

// Version 2 : Récursive (même contrat)
int factorielle_v2(int n) {
    PRECONDITION(n >= 0 && n <= 12);

    int resultat = (n <= 1) ? 1 : n * factorielle_v2(n - 1);

    POSTCONDITION(resultat > 0);
    return resultat;
}

// Les deux versions respectent le même contrat
// → Interchangeables sans affecter le code appelant
```

### 5. Tests intégrés

Les assertions servent de **tests automatiques** :

```c
void test_pile(void) {
    Pile *p = pile_creer(3);

    // Les préconditions/postconditions vérifient automatiquement
    pile_empiler(p, 10);
    pile_empiler(p, 20);

    assert(pile_sommet(p) == 20);
    assert(pile_depiler(p) == 20);
    assert(pile_sommet(p) == 10);

    pile_detruire(p);
}
```

---

## Limites et compromis

### 1. Performance en mode debug

Les vérifications ont un **coût** :

```c
void traiter_grand_tableau(int *tab, size_t n) {
    PRECONDITION(tab != NULL);
    PRECONDITION(n > 0);

    // Vérification coûteuse : O(n)
    #ifndef NDEBUG
    for (size_t i = 1; i < n; i++) {
        PRECONDITION(tab[i] >= tab[i-1]);  // Vérifier le tri
    }
    #endif

    // ... traitement ...
}
```

**Solution :** Compilez en mode release (`-DNDEBUG`) pour la production.

### 2. Préconditions vs Validation

Ne confondez pas les préconditions avec la validation d'entrées utilisateur :

```c
// ❌ FAUX : Les entrées utilisateur doivent être validées, pas assertées
void traiter_age_utilisateur(int age) {
    PRECONDITION(age >= 0);  // Mauvais !
    // ...
}

// ✅ CORRECT : Validation avec gestion d'erreur
int traiter_age_utilisateur(int age) {
    if (age < 0 || age > 150) {
        return -1;  // Erreur
    }
    // ...
    return 0;
}

// ✅ CORRECT : Précondition pour une fonction interne
static void calculer_avec_age_valide(int age) {
    PRECONDITION(age >= 0 && age <= 150);  // Fonction privée
    // ...
}
```

### 3. Postconditions complexes

Certaines postconditions sont difficiles ou coûteuses à vérifier :

```c
// Trier un tableau
void tri_rapide(int *tab, size_t n) {
    PRECONDITION(tab != NULL);

    // ... algorithme de tri ...

    // Postcondition : le tableau doit être trié
    // Vérification coûteuse : O(n)
    #ifndef NDEBUG
    for (size_t i = 1; i < n; i++) {
        POSTCONDITION(tab[i-1] <= tab[i]);
    }
    #endif
}
```

**Compromis :** Désactivez les vérifications coûteuses en release.

---

## Design by Contract et Tests Unitaires

Le DbC et les tests sont **complémentaires** :

| Aspect | DbC | Tests Unitaires |
|--------|-----|-----------------|
| **Quand** | Pendant l'exécution normale | Dans un environnement de test |
| **Où** | Dans le code de production | Dans des fichiers de test séparés |
| **Quoi** | Vérifie le **contrat** | Vérifie le **comportement** |
| **Niveau** | Micro (chaque fonction) | Macro (scénarios complets) |
| **Désactivable** | Oui (NDEBUG) | Non |

### Exemple combiné

```c
// File: pile.c
Pile* pile_creer(size_t capacite) {
    PRECONDITION(capacite > 0);
    // ... implémentation ...
    POSTCONDITION(pile->taille == 0);
    return pile;
}

// File: test_pile.c
void test_pile_creer(void) {
    // Test unitaire : vérifier le comportement
    Pile *p = pile_creer(5);

    assert(p != NULL);
    assert(p->taille == 0);
    assert(p->capacite == 5);

    pile_detruire(p);
    printf("✓ test_pile_creer passé\n");
}
```

**DbC** vérifie les **invariants**, les tests vérifient les **scénarios d'usage**.

---

## Patterns avancés

### Pattern 1 : Old value (Valeur ancienne)

Pour vérifier qu'une valeur a changé, sauvegardez l'ancienne valeur :

```c
void incrementer(int *valeur) {
    PRECONDITION(valeur != NULL);

    // Sauvegarder l'ancienne valeur
    int ancienne_valeur = *valeur;

    (*valeur)++;

    // Postcondition : la valeur a augmenté de 1
    POSTCONDITION(*valeur == ancienne_valeur + 1);
}
```

### Pattern 2 : Frame condition

Spécifier ce qui **ne change pas** :

```c
typedef struct {
    char nom[50];
    int age;
    float salaire;
} Employe;

void augmenter_salaire(Employe *e, float pourcentage) {
    PRECONDITION(e != NULL);
    PRECONDITION(pourcentage > 0);

    // Sauvegarder les valeurs qui ne doivent pas changer
    char nom_avant[50];
    strcpy(nom_avant, e->nom);
    int age_avant = e->age;
    float salaire_avant = e->salaire;

    e->salaire += e->salaire * (pourcentage / 100.0f);

    // Postconditions
    POSTCONDITION(strcmp(e->nom, nom_avant) == 0);  // Nom inchangé
    POSTCONDITION(e->age == age_avant);              // Age inchangé
    POSTCONDITION(e->salaire > salaire_avant);       // Salaire augmenté
}
```

### Pattern 3 : Invariant checking wrapper

Encapsuler la vérification d'invariant :

```c
typedef struct {
    int *donnees;
    size_t taille;
    size_t capacite;
} Vecteur;

static void vecteur_check_invariant(const Vecteur *v) {
    INVARIANT(v != NULL);
    INVARIANT(v->taille <= v->capacite);
    INVARIANT((v->capacite == 0) == (v->donnees == NULL));
}

// Macro pour simplifier
#define CHECK_VECTEUR(v) vecteur_check_invariant(v)

void vecteur_ajouter(Vecteur *v, int valeur) {
    CHECK_VECTEUR(v);  // Vérification avant

    // ... code ...

    CHECK_VECTEUR(v);  // Vérification après
}
```

### Pattern 4 : Contract inheritance

En C, simuler l'héritage de contrat avec des fonctions de validation :

```c
// Contrat de base pour toutes les formes
typedef struct {
    float (*aire)(const void *forme);
    float (*perimetre)(const void *forme);
} Forme;

// Précondition commune : forme != NULL
static void forme_precondition(const Forme *f) {
    PRECONDITION(f != NULL);
    PRECONDITION(f->aire != NULL);
    PRECONDITION(f->perimetre != NULL);
}

// Postcondition commune : résultat >= 0
static void forme_postcondition_positive(float resultat) {
    POSTCONDITION(resultat >= 0);
}

// Utilisation
float forme_calculer_aire(const Forme *f) {
    forme_precondition(f);

    float aire = f->aire(f);

    forme_postcondition_positive(aire);
    return aire;
}
```

---

## Exemple pratique : File d'attente (Queue)

Implémentation complète d'une file avec DbC :

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define PRECONDITION(c)  assert((c) && "Précondition")
#define POSTCONDITION(c) assert((c) && "Postcondition")
#define INVARIANT(c)     assert((c) && "Invariant")

typedef struct {
    int *elements;
    size_t debut;       // Indice du premier élément
    size_t fin;         // Indice après le dernier élément
    size_t capacite;
} File;

/**
 * Invariants :
 * 1. elements != NULL si capacite > 0
 * 2. debut < capacite
 * 3. fin < capacite
 * 4. Nombre d'éléments calculable : (fin - debut + capacite) % capacite
 */
static void file_verifier_invariant(const File *f) {
    INVARIANT(f != NULL);
    INVARIANT((f->capacite == 0) == (f->elements == NULL));
    INVARIANT(f->capacite == 0 || f->debut < f->capacite);
    INVARIANT(f->capacite == 0 || f->fin < f->capacite);
}

static size_t file_taille_interne(const File *f) {
    if (f->fin >= f->debut) {
        return f->fin - f->debut;
    } else {
        return f->capacite - f->debut + f->fin;
    }
}

File* file_creer(size_t capacite) {
    PRECONDITION(capacite > 0);

    File *f = malloc(sizeof(File));
    POSTCONDITION(f != NULL);

    f->elements = malloc(capacite * sizeof(int));
    POSTCONDITION(f->elements != NULL);

    f->debut = 0;
    f->fin = 0;
    f->capacite = capacite;

    file_verifier_invariant(f);
    POSTCONDITION(file_taille_interne(f) == 0);

    return f;
}

bool file_est_vide(const File *f) {
    PRECONDITION(f != NULL);
    file_verifier_invariant(f);

    bool vide = (f->debut == f->fin);

    file_verifier_invariant(f);
    POSTCONDITION(vide == (file_taille_interne(f) == 0));

    return vide;
}

bool file_est_pleine(const File *f) {
    PRECONDITION(f != NULL);
    file_verifier_invariant(f);

    bool pleine = ((f->fin + 1) % f->capacite == f->debut);

    file_verifier_invariant(f);

    return pleine;
}

void file_enfiler(File *f, int valeur) {
    PRECONDITION(f != NULL);
    PRECONDITION(!file_est_pleine(f));
    file_verifier_invariant(f);

    size_t ancienne_taille = file_taille_interne(f);

    f->elements[f->fin] = valeur;
    f->fin = (f->fin + 1) % f->capacite;

    file_verifier_invariant(f);
    POSTCONDITION(file_taille_interne(f) == ancienne_taille + 1);
}

int file_defiler(File *f) {
    PRECONDITION(f != NULL);
    PRECONDITION(!file_est_vide(f));
    file_verifier_invariant(f);

    size_t ancienne_taille = file_taille_interne(f);

    int valeur = f->elements[f->debut];
    f->debut = (f->debut + 1) % f->capacite;

    file_verifier_invariant(f);
    POSTCONDITION(file_taille_interne(f) == ancienne_taille - 1);

    return valeur;
}

void file_detruire(File *f) {
    if (f != NULL) {
        file_verifier_invariant(f);
        free(f->elements);
        free(f);
    }
}

int main(void) {
    printf("=== Test File avec DbC ===\n\n");

    File *f = file_creer(5);
    printf("File créée (capacité: 5)\n");

    printf("\nEnfilage : 10, 20, 30\n");
    file_enfiler(f, 10);
    file_enfiler(f, 20);
    file_enfiler(f, 30);

    printf("\nDéfilage :\n");
    printf("  %d\n", file_defiler(f));
    printf("  %d\n", file_defiler(f));

    printf("\nEnfilage : 40, 50\n");
    file_enfiler(f, 40);
    file_enfiler(f, 50);

    printf("\nDéfilage complet :\n");
    while (!file_est_vide(f)) {
        printf("  %d\n", file_defiler(f));
    }

    file_detruire(f);
    printf("\nFile détruite\n");

    return 0;
}
```

---

## Checklist pour appliquer le DbC

Lors de l'écriture d'une fonction, demandez-vous :

### Préconditions
- [ ] Quels paramètres ne doivent jamais être NULL ?
- [ ] Quelles valeurs sont interdites (division par zéro, indices négatifs) ?
- [ ] Quelles relations entre paramètres doivent être respectées ?
- [ ] Quel état doit avoir l'objet avant l'appel ?

### Postconditions
- [ ] Quel est le résultat garanti si les préconditions sont respectées ?
- [ ] Quelles propriétés du résultat puis-je affirmer ?
- [ ] Quels éléments de l'état ont changé ?
- [ ] Quels éléments sont restés inchangés ?

### Invariants
- [ ] Quelles propriétés doivent TOUJOURS être vraies pour cet objet ?
- [ ] Ces propriétés sont-elles vraies avant ET après chaque opération ?
- [ ] Ai-je des contraintes sur les relations entre champs ?

---

## Résumé

### 🎯 Points clés

1. **Le contrat = Préconditions + Postconditions + Invariants**
   - Préconditions : ce que l'appelant doit garantir
   - Postconditions : ce que la fonction garantit
   - Invariants : ce qui reste toujours vrai

2. **Implémentation en C**
   - Utilisez des macros `PRECONDITION`, `POSTCONDITION`, `INVARIANT`
   - Basées sur `assert()` pour être désactivables avec `-DNDEBUG`

3. **Séparation des responsabilités**
   - Préconditions = responsabilité de l'appelant
   - Postconditions = responsabilité de la fonction

4. **Documentation exécutable**
   - Le contrat documente ET vérifie
   - Détection immédiate des violations

5. **Production vs Debug**
   - Debug : vérifications actives
   - Production : désactiver avec `-DNDEBUG`

### 💡 Règles d'or

> **Précondition = Obligation de l'appelant**
> **Postcondition = Obligation de la fonction**
> **Invariant = Vérité permanente**

> **Le Design by Contract rend les bugs impossibles à ignorer.**

### Quand appliquer le DbC ?

```
Bibliothèques et APIs → Contrat strict
Code système critique → Contrat rigoureux
Structures de données → Invariants systématiques
Prototypes rapides → Contrat léger
```

Le Design by Contract transforme votre code en un système où chaque composant respecte un **contrat clair et vérifiable**. C'est un outil puissant pour écrire du code C **robuste**, **maintenable** et **autodocumenté**.

---

**Prochaine section** : 25.6 Logging

⏭️ [Logging](/25-gestion-erreurs/06-logging.md)
