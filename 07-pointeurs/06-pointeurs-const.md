🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.6 Pointeurs constants et const

## Introduction

Le mot-clé **`const`** est un outil puissant pour rendre votre code plus sûr et plus clair. Avec les pointeurs, `const` peut être placé à différents endroits, ce qui change complètement sa signification. Cette section démystifie l'utilisation de `const` avec les pointeurs.

## Rappel : const avec les variables normales

Avant de parler des pointeurs, rappelons l'usage basique de `const` :

```c
const int constante = 42;
constante = 50;    // ✗ ERREUR de compilation : tentative de modification
```

Une variable déclarée `const` ne peut **pas être modifiée** après son initialisation.

## Les quatre cas avec les pointeurs

Avec les pointeurs, il existe **quatre combinaisons** principales de `const` :

| Déclaration | Lecture | Signification |
|-------------|---------|---------------|
| `const int *ptr` | Pointeur vers un **int constant** | La valeur ne peut pas être modifiée via ce pointeur |
| `int const *ptr` | Pointeur vers un **int constant** | Identique au précédent (alternative syntaxique) |
| `int *const ptr` | **Pointeur constant** vers un int | Le pointeur ne peut pas être modifié (mais la valeur oui) |
| `const int *const ptr` | **Pointeur constant** vers un **int constant** | Ni le pointeur ni la valeur ne peuvent être modifiés |

## La règle de lecture

Pour comprendre où s'applique `const`, lisez **de droite à gauche** :

```
const int *ptr     →  "ptr est un pointeur vers un int constant"
int *const ptr     →  "ptr est un pointeur constant vers un int"
const int *const ptr → "ptr est un pointeur constant vers un int constant"
```

**Astuce mnémotechnique :** `const` s'applique à ce qui est **immédiatement à sa gauche**. S'il n'y a rien à gauche (début de ligne), il s'applique à ce qui est à sa droite.

## Cas 1 : Pointeur vers une valeur constante

### Syntaxe

```c
const int *ptr;        // Méthode 1 (la plus courante)
int const *ptr;        // Méthode 2 (équivalente)
```

Ces deux déclarations sont **strictement identiques**.

### Signification

Le pointeur peut changer d'adresse, mais on ne peut **pas modifier la valeur pointée** via ce pointeur.

### Exemple

```c
#include <stdio.h>

int main(void) {
    int x = 10;
    int y = 20;

    const int *ptr = &x;    // ptr pointe vers x

    printf("Valeur pointée : %d\n", *ptr);    // ✓ Lecture OK

    // *ptr = 15;    // ✗ ERREUR : tentative de modification de la valeur

    ptr = &y;    // ✓ OK : changer l'adresse est autorisé
    printf("Nouvelle valeur pointée : %d\n", *ptr);

    return 0;
}
```

**Sortie :**
```
Valeur pointée : 10
Nouvelle valeur pointée : 20
```

### Visualisation

```
État initial :
    x = 10        y = 20
    [  10  ]      [  20  ]
      ↑
      │
    ptr (const int*)

Opérations :
✓ *ptr           → Lecture : OK
✗ *ptr = 15      → Écriture : INTERDIT
✓ ptr = &y       → Changer le pointeur : OK

État après ptr = &y :
    x = 10        y = 20
    [  10  ]      [  20  ]
                    ↑
                    │
                  ptr (const int*)
```

### Message d'erreur typique

```c
const int *ptr = &x;
*ptr = 15;
```

**Erreur de compilation :**
```
error: assignment of read-only location '*ptr'
```

### Cas d'usage courant

Passer un paramètre qu'on ne veut **pas modifier** :

```c
void afficher(const int *valeur) {
    if (valeur != NULL) {
        printf("Valeur : %d\n", *valeur);    // ✓ Lecture OK
        // *valeur = 0;    // ✗ ERREUR : modification interdite
    }
}

int main(void) {
    int nombre = 42;
    afficher(&nombre);    // Garantit que afficher() ne modifie pas nombre
    return 0;
}
```

**Avantage :** Le compilateur garantit que la fonction ne modifiera pas la valeur.

## Cas 2 : Pointeur constant vers une valeur modifiable

### Syntaxe

```c
int *const ptr = &variable;    // Initialisation obligatoire !
```

**Important :** Un pointeur constant **doit être initialisé** à la déclaration, car on ne pourra plus le changer après.

### Signification

Le pointeur **ne peut pas changer d'adresse**, mais on peut modifier la valeur pointée.

### Exemple

```c
#include <stdio.h>

int main(void) {
    int x = 10;
    int y = 20;

    int *const ptr = &x;    // ptr pointe vers x (fixe)

    printf("Valeur initiale : %d\n", *ptr);

    *ptr = 15;    // ✓ OK : modification de la valeur
    printf("Valeur modifiée : %d\n", *ptr);

    // ptr = &y;    // ✗ ERREUR : tentative de changer le pointeur

    return 0;
}
```

**Sortie :**
```
Valeur initiale : 10
Valeur modifiée : 15
```

### Visualisation

```
    x = 10        y = 20
    [  15  ]      [  20  ]
      ↑
      │ (fixe, ne peut pas bouger)
      │
    ptr (int *const)

Opérations :
✓ *ptr = 15      → Modifier la valeur : OK
✗ ptr = &y       → Changer le pointeur : INTERDIT
```

### Message d'erreur typique

```c
int *const ptr = &x;
ptr = &y;
```

**Erreur de compilation :**
```
error: assignment of read-only variable 'ptr'
```

### Cas d'usage courant

Lier un pointeur à une ressource unique :

```c
int tableau[100];
int *const ptr_tableau = tableau;    // Le pointeur ne changera jamais

// Dans toute la suite du code, ptr_tableau pointe toujours vers tableau
ptr_tableau[0] = 10;    // ✓ OK
ptr_tableau[1] = 20;    // ✓ OK
```

## Cas 3 : Pointeur constant vers une valeur constante

### Syntaxe

```c
const int *const ptr = &variable;
```

Ou de manière équivalente :

```c
int const *const ptr = &variable;
```

### Signification

**Ni le pointeur ni la valeur** ne peuvent être modifiés. C'est la combinaison la plus restrictive.

### Exemple

```c
#include <stdio.h>

int main(void) {
    int x = 10;
    int y = 20;

    const int *const ptr = &x;    // Tout est constant

    printf("Valeur : %d\n", *ptr);    // ✓ Lecture OK

    // *ptr = 15;    // ✗ ERREUR : modification de la valeur interdite
    // ptr = &y;     // ✗ ERREUR : modification du pointeur interdite

    return 0;
}
```

### Visualisation

```
    x = 10        y = 20
    [  10  ]      [  20  ]
      ↑
      │ (fixe)
      │
    ptr (const int *const)

Opérations :
✓ *ptr           → Lecture : OK
✗ *ptr = 15      → Modifier la valeur : INTERDIT
✗ ptr = &y       → Changer le pointeur : INTERDIT
```

### Cas d'usage courant

Paramètres de fonction avec garantie totale :

```c
void traiter_tableau(const int *const tableau, size_t taille) {
    // Le pointeur 'tableau' ne peut pas être modifié
    // Les valeurs dans le tableau ne peuvent pas être modifiées

    for (size_t i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);    // ✓ Lecture OK
        // tableau[i] = 0;    // ✗ ERREUR
    }
    printf("\n");

    // tableau = NULL;    // ✗ ERREUR
}
```

## Tableau récapitulatif complet

| Déclaration | Pointeur modifiable ? | Valeur modifiable ? | Usage typique |
|-------------|:---------------------:|:-------------------:|---------------|
| `int *ptr` | ✓ Oui | ✓ Oui | Usage normal |
| `const int *ptr` | ✓ Oui | ✗ Non | Lire sans modifier |
| `int *const ptr` | ✗ Non | ✓ Oui | Pointeur fixe vers données modifiables |
| `const int *const ptr` | ✗ Non | ✗ Non | Tout est immuable |

## Astuce de lecture visuelle

Utilisez cette technique pour lire une déclaration :

```
const int *const ptr
  ↓       ↓    ↓
  │       │    └─ "ptr est un"
  │       └────── "pointeur constant vers un"
  └────────────── "int constant"
```

**Résultat :** "ptr est un pointeur constant vers un int constant"

Autre exemple :

```
int *const ptr
    ↓    ↓
    │    └─ "ptr est un"
    └────── "pointeur constant vers un int"
```

## Conversions implicites

### ✅ Conversions valides (ajout de const)

```c
int x = 10;

int *ptr1 = &x;
const int *ptr2 = ptr1;    // ✓ OK : ajoute const sur la valeur

// On peut passer un int* à une fonction qui attend const int*
void afficher(const int *p) {
    printf("%d\n", *p);
}

afficher(ptr1);    // ✓ OK
```

**Règle :** On peut toujours **ajouter** `const` (augmenter les restrictions).

### ❌ Conversions invalides (retrait de const)

```c
const int y = 20;

const int *ptr_const = &y;
int *ptr_modifiable = ptr_const;    // ✗ ERREUR : retire const
```

**Erreur :**
```
warning: initialization discards 'const' qualifier from pointer target type
```

**Règle :** On ne peut pas **retirer** `const` sans cast explicite (dangereux).

## Cast et const

### Cast explicite (à utiliser avec prudence)

```c
const int constante = 42;
const int *ptr_const = &constante;

// Cast pour retirer const (DANGEREUX !)
int *ptr = (int*)ptr_const;
*ptr = 50;    // Comportement indéfini !
```

**⚠️ Danger :** Modifier une variable déclarée `const` via un cast est un **comportement indéfini**. Le compilateur peut optimiser en supposant que la valeur ne change jamais.

### Exemple de comportement indéfini

```c
#include <stdio.h>

int main(void) {
    const int constante = 42;

    printf("Avant : %d\n", constante);

    // Cast dangereux
    int *ptr = (int*)&constante;
    *ptr = 100;

    printf("Après : %d\n", constante);    // Peut afficher 42 OU 100 !
    printf("Via ptr : %d\n", *ptr);       // Peut afficher 100

    return 0;
}
```

**Sortie possible (comportement imprévisible) :**
```
Avant : 42
Après : 42      ← Le compilateur a optimisé !
Via ptr : 100
```

Le compilateur peut mettre `constante` dans un registre ou en lecture seule, ignorant la modification.

## const avec les tableaux

### Tableau de constantes

```c
const int tableau[] = {10, 20, 30, 40, 50};

printf("%d\n", tableau[0]);    // ✓ Lecture OK
// tableau[0] = 100;    // ✗ ERREUR : modification interdite
```

### Pointeur vers un tableau de constantes

```c
const int *ptr = tableau;

printf("%d\n", *ptr);      // ✓ OK
printf("%d\n", ptr[2]);    // ✓ OK
// ptr[2] = 100;    // ✗ ERREUR
ptr++;    // ✓ OK : le pointeur peut bouger
```

## const avec les chaînes de caractères

### Chaînes littérales (constantes)

```c
const char *message = "Hello";    // ✓ Correct

// message[0] = 'h';    // ✗ ERREUR (comportement indéfini sans const)
```

**Important :** Les chaînes littérales (`"Hello"`) sont stockées en mémoire lecture seule. Utiliser `const char*` rend cela explicite.

### ⚠️ Piège classique

```c
char *ptr = "Hello";    // ⚠️ Devrait être const char*
ptr[0] = 'h';           // Comportement indéfini ! Peut crasher
```

**Bonne pratique :**
```c
const char *ptr = "Hello";    // ✓ Correct et sûr
```

## const dans les paramètres de fonctions

### Protection en lecture seule

```c
void afficher_tableau(const int *tableau, size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);    // ✓ Lecture OK
    }
    printf("\n");
}

int main(void) {
    int nombres[] = {1, 2, 3, 4, 5};
    afficher_tableau(nombres, 5);    // La fonction ne peut pas modifier nombres
    return 0;
}
```

### Modification autorisée

```c
void doubler_valeurs(int *tableau, size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        tableau[i] *= 2;    // ✓ Modification autorisée
    }
}
```

### Documentation par le type

```c
// La signature indique clairement les intentions :

void lire_donnees(const int *data);      // Ne modifie pas les données
void ecrire_donnees(int *data);          // Peut modifier les données
void traiter(const int *input, int *output);    // input en lecture, output en écriture
```

## Pointeurs de pointeurs et const

### Différentes combinaisons

```c
int x = 10;
int *ptr = &x;

// Pointeur vers un (pointeur vers int)
int **pp1 = &ptr;                    // Tout modifiable

// Pointeur vers un (pointeur constant vers int)
int *const *pp2 = &ptr;              // Le pointeur intermédiaire est constant

// Pointeur vers un (pointeur vers int constant)
const int **pp3 = &ptr;              // La valeur finale est constante

// Pointeur constant vers un (pointeur vers int)
int **const pp4 = &ptr;              // pp4 est constant

// Pointeur constant vers un (pointeur constant vers int constant)
const int *const *const pp5 = &ptr;  // Tout est constant !
```

### Exemple pratique

```c
void modifier_pointeur(int **pp) {
    static int y = 20;
    *pp = &y;    // Change où pointe le pointeur original
}

int main(void) {
    int x = 10;
    int *ptr = &x;

    printf("Avant : *ptr = %d\n", *ptr);

    modifier_pointeur(&ptr);    // Passe l'adresse du pointeur

    printf("Après : *ptr = %d\n", *ptr);

    return 0;
}
```

**Sortie :**
```
Avant : *ptr = 10
Après : *ptr = 20
```

## const avec les structures

### Structure constante

```c
typedef struct {
    int x;
    int y;
} Point;

void afficher_point(const Point *p) {
    if (p) {
        printf("Point (%d, %d)\n", p->x, p->y);    // ✓ Lecture OK
        // p->x = 0;    // ✗ ERREUR : modification interdite
    }
}

int main(void) {
    Point pt = {10, 20};
    afficher_point(&pt);
    return 0;
}
```

### Membre constant dans une structure

```c
typedef struct {
    const int id;    // id ne peut pas être modifié après initialisation
    char nom[50];
} Personne;

int main(void) {
    Personne p = {1, "Alice"};

    // p.id = 2;    // ✗ ERREUR : id est const
    strcpy(p.nom, "Bob");    // ✓ OK : nom n'est pas const

    return 0;
}
```

## Avantages de const

### 1. Sécurité

Le compilateur empêche les modifications accidentelles :

```c
void traiter(const int *data) {
    // data[0] = 0;    // Le compilateur détecte l'erreur
}
```

### 2. Documentation

La signature de fonction documente les intentions :

```c
// On sait immédiatement que cette fonction ne modifie pas str
size_t calculer_longueur(const char *str);
```

### 3. Optimisations

Le compilateur peut optimiser en sachant que certaines valeurs ne changent pas.

### 4. Contrats clairs

```c
// Contrat : la fonction ne modifiera pas le tableau source
void copier_tableau(int *dest, const int *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
```

## Exemple complet : fonction de recherche

```c
#include <stdio.h>
#include <string.h>

// Retourne un pointeur vers le premier élément trouvé, ou NULL
const int* trouver_valeur(const int *tableau, size_t taille, int valeur) {
    for (size_t i = 0; i < taille; i++) {
        if (tableau[i] == valeur) {
            return &tableau[i];    // Retourne un pointeur const
        }
    }
    return NULL;
}

int main(void) {
    int nombres[] = {10, 20, 30, 40, 50};

    const int *resultat = trouver_valeur(nombres, 5, 30);

    if (resultat != NULL) {
        printf("Valeur trouvée : %d\n", *resultat);
        printf("Index : %ld\n", resultat - nombres);

        // *resultat = 0;    // ✗ ERREUR : retour const
    } else {
        printf("Valeur non trouvée\n");
    }

    return 0;
}
```

**Sortie :**
```
Valeur trouvée : 30
Index : 2
```

### Variante sans const (modifiable)

```c
// Si on veut permettre la modification du résultat
int* trouver_valeur_modifiable(int *tableau, size_t taille, int valeur) {
    for (size_t i = 0; i < taille; i++) {
        if (tableau[i] == valeur) {
            return &tableau[i];
        }
    }
    return NULL;
}

int main(void) {
    int nombres[] = {10, 20, 30, 40, 50};

    int *resultat = trouver_valeur_modifiable(nombres, 5, 30);

    if (resultat != NULL) {
        *resultat = 99;    // ✓ OK : modification autorisée
    }

    printf("Tableau modifié : ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    return 0;
}
```

**Sortie :**
```
Tableau modifié : 10 20 99 40 50
```

## Erreurs courantes

### ❌ Erreur 1 : Oublier const dans les paramètres

```c
// Mauvais : laisse penser que la fonction peut modifier str
size_t longueur(char *str);

// Bon : indique clairement que str n'est pas modifiée
size_t longueur(const char *str);
```

### ❌ Erreur 2 : const au mauvais endroit

```c
// Voulait : pointeur vers int constant
const int* ptr;    // ✓ Correct

// Confusion fréquente :
int const* ptr;    // ✓ Correct aussi (identique)
int *const ptr;    // ✗ Différent : pointeur constant !
```

### ❌ Erreur 3 : Retirer const par cast sans raison

```c
const int *ptr_const = ...;
int *ptr = (int*)ptr_const;    // ⚠️ Dangereux et inutile la plupart du temps
```

## Bonnes pratiques

### ✅ 1. Utilisez const par défaut

```c
// Par défaut, tout est const sauf ce qui doit être modifié
void traiter(const Data *input, Data *output);
```

### ✅ 2. Const avec les chaînes littérales

```c
const char *message = "Hello";    // Toujours const
```

### ✅ 3. Paramètres const dans les fonctions

```c
void afficher(const int *valeurs, size_t n);
```

### ✅ 4. Retour const si modification interdite

```c
const char* obtenir_nom(const Personne *p);
```

### ✅ 5. Documentez les intentions

```c
// Le pointeur config ne changera jamais
static const Config *const config = &default_config;
```

## Résumé visuel

```
Déclaration               Position de const       Ce qui est constant
-----------------         -----------------       -------------------
const int *ptr            Avant le type           La valeur pointée
int const *ptr            Après le type           La valeur pointée (identique)
int *const ptr            Après l'astérisque      Le pointeur lui-même
const int *const ptr      Aux deux endroits       Les deux !

Règle de lecture (droite à gauche) :
const int *const ptr
        ↑       ↑
        |       |
        |       +-- "ptr est un pointeur constant"
        +---------- "vers un int constant"
```

## Ce qu'il faut retenir

✅ **`const`** empêche la modification (protection par le compilateur)

✅ **Quatre combinaisons principales** avec les pointeurs :
   - `const int *ptr` : valeur constante
   - `int *const ptr` : pointeur constant
   - `const int *const ptr` : les deux constants

✅ **Lire de droite à gauche** pour comprendre la déclaration

✅ **Toujours utiliser const** avec les chaînes littérales

✅ **Dans les paramètres de fonction** : const documente les intentions

✅ **On peut ajouter const** (conversion implicite OK)

✅ **On ne peut pas retirer const** sans cast explicite (dangereux)

✅ **const améliore** : sécurité, documentation, optimisations

✅ **Retirer const par cast** = comportement indéfini

## Prochaine étape

Maintenant que vous maîtrisez les pointeurs constants, la prochaine section explorera les **pointeurs restreints (restrict)**, une fonctionnalité avancée du C99 qui aide le compilateur à optimiser le code.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je comprends la différence entre `const int*` et `int *const`
- [ ] Je sais lire une déclaration de droite à gauche
- [ ] J'utilise const pour les paramètres non modifiés
- [ ] Je sais qu'on peut ajouter const mais pas le retirer
- [ ] J'utilise toujours `const char*` pour les chaînes littérales
- [ ] Je comprends que `const int*` et `int const*` sont identiques
- [ ] Je connais les dangers de retirer const par cast

---

**Prochaine étape :** 7.7 Pointeurs restreints (restrict)

⏭️ [Pointeurs restreints (restrict)](/07-pointeurs/07-pointeurs-restrict.md)
