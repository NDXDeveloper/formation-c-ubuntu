🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 7.4 Pointeurs void* et casting

## Introduction

Dans les sections précédentes, vous avez travaillé avec des pointeurs typés (`int*`, `char*`, etc.). Maintenant, nous allons découvrir un type de pointeur spécial : **`void*`** (pointeur générique), ainsi que les techniques de **casting** (conversion de types).

## Qu'est-ce qu'un pointeur void* ?

Un pointeur **`void*`** est un **pointeur générique** qui peut pointer vers n'importe quel type de données.

### Analogie

Imaginez :
- Un **carton étiqueté "Fragile"** : vous savez que c'est fragile, mais vous ne savez pas exactement ce qu'il contient (void*)
- Un **carton étiqueté "Livres"** : vous savez exactement ce qu'il y a dedans (int*, char*, etc.)

Le pointeur `void*` est comme un carton sans étiquette spécifique : il contient une adresse, mais le compilateur ne sait pas quel type de données s'y trouve.

### Déclaration

```c
void *ptr_generique;
```

## Pourquoi utiliser void* ?

Les pointeurs `void*` sont utiles dans plusieurs situations :

1. **Fonctions génériques** : Créer des fonctions qui fonctionnent avec différents types
2. **API de bas niveau** : Allocation mémoire (`malloc`, `calloc`)
3. **Structures de données génériques** : Listes chaînées, arbres, etc.
4. **Callbacks** : Passer des données de type arbitraire

## Caractéristiques de void*

### ✅ Ce qu'on peut faire

```c
int age = 25;  
float prix = 19.99f;  
void *ptr;  

// ✓ Stocker l'adresse de n'importe quel type
ptr = &age;     // OK  
ptr = &prix;    // OK aussi  

// ✓ Comparer avec NULL
if (ptr == NULL) {
    // ...
}

// ✓ Affecter à un autre void*
void *ptr2 = ptr;
```

### ❌ Ce qu'on ne peut PAS faire

```c
void *ptr = &age;

// ✗ Pas de déréférencement direct
// printf("%d\n", *ptr);    // ERREUR de compilation !

// ✗ Pas d'arithmétique de pointeurs
// ptr++;                   // ERREUR (en C strict, OK en GCC comme extension)
// ptr + 1;                 // ERREUR (en C strict)
```

**Raison :** Le compilateur ne connaît pas la taille de l'élément pointé, donc il ne peut pas déréférencer ou faire de l'arithmétique.

## Le casting de pointeurs

Le **casting** (conversion de type) permet de transformer un pointeur d'un type vers un autre.

### Syntaxe générale

```c
(nouveau_type)expression
```

### Exemple de base

```c
#include <stdio.h>

int main(void) {
    int age = 25;
    void *ptr_generique = &age;    // Conversion implicite vers void*

    // Pour utiliser la valeur, il faut caster
    int *ptr_int = (int*)ptr_generique;    // Cast explicite

    printf("Valeur : %d\n", *ptr_int);

    return 0;
}
```

**Sortie :**
```
Valeur : 25
```

### Conversion implicite vs explicite

#### Conversion implicite (vers void*)

```c
int age = 25;  
void *ptr = &age;    // ✓ OK : conversion automatique vers void*  
```

Tout pointeur peut être **automatiquement** converti en `void*`.

#### Conversion explicite (depuis void*)

```c
void *ptr = &age;  
int *ptr_int = (int*)ptr;    // Cast explicite (recommandé pour la lisibilité)  
int *ptr_int2 = ptr;         // Conversion implicite (valide en C, pas en C++)
```

En C, la conversion de `void*` vers un type spécifique est **implicite** (automatique). Le cast explicite n'est pas obligatoire, mais il est **recommandé** pour la lisibilité et la documentation du code. En **C++**, le cast est obligatoire.

## Exemple complet avec void*

```c
#include <stdio.h>

void afficher_octets(void *ptr, size_t taille) {
    unsigned char *bytes = (unsigned char*)ptr;    // Cast vers char*

    printf("Contenu en octets : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

int main(void) {
    int nombre = 0x12345678;
    float decimal = 3.14f;

    printf("Représentation d'un int :\n");
    afficher_octets(&nombre, sizeof(nombre));

    printf("\nReprésentation d'un float :\n");
    afficher_octets(&decimal, sizeof(decimal));

    return 0;
}
```

**Sortie possible (architecture little-endian) :**
```
Représentation d'un int :  
Contenu en octets : 78 56 34 12  

Représentation d'un float :  
Contenu en octets : C3 F5 48 40  
```

### Explication

1. `void *ptr` : Paramètre générique qui accepte n'importe quel type
2. `(unsigned char*)ptr` : Cast vers `char*` pour accéder octet par octet
3. La fonction fonctionne avec **n'importe quel type** de données

## malloc et void*

La fonction `malloc` retourne un `void*` car elle alloue de la mémoire brute, sans savoir quel type y sera stocké.

### En C moderne

```c
#include <stdlib.h>

int main(void) {
    // malloc retourne void*, pas besoin de cast en C
    int *tableau = malloc(5 * sizeof(int));

    if (tableau == NULL) {
        return 1;    // Échec d'allocation
    }

    // Utilisation...
    tableau[0] = 10;

    free(tableau);
    return 0;
}
```

### Note sur le casting de malloc

```c
// En C (recommandé) :
int *tableau = malloc(5 * sizeof(int));

// En C++ (obligatoire) :
int *tableau = (int*)malloc(5 * sizeof(int));
```

**Bonne pratique en C :** Ne castez **pas** le retour de `malloc`. Le C convertit automatiquement `void*` vers n'importe quel pointeur.

**Pourquoi ?**
- Plus lisible
- Évite des erreurs si vous changez le type
- Le cast peut masquer l'oubli du `#include <stdlib.h>`

## Casting entre différents types de pointeurs

### Casting compatible

```c
#include <stdio.h>

int main(void) {
    int nombre = 65;    // Code ASCII de 'A'

    int *ptr_int = &nombre;
    char *ptr_char = (char*)ptr_int;    // Cast explicite

    printf("Via int* : %d\n", *ptr_int);
    printf("Via char* : %c\n", *ptr_char);    // Lit le premier octet

    return 0;
}
```

**Sortie :**
```
Via int* : 65  
Via char* : A  
```

### Visualisation mémoire

```
Mémoire (little-endian) :  
Adresse    Contenu (int = 65)  
0x1000     [41] [00] [00] [00]
            ↑
            |
        ptr_char lit 1 octet (0x41 = 'A')

        ←------ ptr_int lit 4 octets ------→
```

### ⚠️ Attention aux problèmes d'alignement

Certains types ont des **exigences d'alignement** :

```c
char tableau[8] = {0, 0, 0, 0, 1, 2, 3, 4};

// ⚠️ DANGER : mauvais alignement potentiel
int *ptr_int = (int*)(&tableau[1]);    // Adresse peut ne pas être alignée sur 4
```

**Problème :** Sur certaines architectures, accéder à un `int` à une adresse non alignée peut :
- Être plus lent
- Causer une erreur (bus error)
- Fonctionner mais donner des résultats incorrects

**Solution :** Vérifiez toujours l'alignement lors de casts vers des types plus larges.

## Exemple pratique : fonction générique de swap

```c
#include <stdio.h>
#include <string.h>

void swap_generique(void *a, void *b, size_t taille) {
    unsigned char temp[taille];    // Tableau temporaire (VLA)

    memcpy(temp, a, taille);       // temp = a
    memcpy(a, b, taille);          // a = b
    memcpy(b, temp, taille);       // b = temp
}

int main(void) {
    int x = 10, y = 20;
    float f1 = 3.14f, f2 = 2.71f;

    printf("Avant swap (int) : x = %d, y = %d\n", x, y);
    swap_generique(&x, &y, sizeof(int));
    printf("Après swap (int) : x = %d, y = %d\n\n", x, y);

    printf("Avant swap (float) : f1 = %.2f, f2 = %.2f\n", f1, f2);
    swap_generique(&f1, &f2, sizeof(float));
    printf("Après swap (float) : f1 = %.2f, f2 = %.2f\n", f1, f2);

    return 0;
}
```

**Sortie :**
```
Avant swap (int) : x = 10, y = 20  
Après swap (int) : x = 20, y = 10  

Avant swap (float) : f1 = 3.14, f2 = 2.71  
Après swap (float) : f1 = 2.71, f2 = 3.14  
```

### Explication

1. `void *a, void *b` : Acceptent des pointeurs de n'importe quel type
2. `size_t taille` : Indique combien d'octets copier
3. `memcpy` : Copie les octets bruts
4. La fonction fonctionne avec **tous les types** !

## Casting et perte d'information

### Cas 1 : Cast vers un type plus petit

```c
#include <stdio.h>

int main(void) {
    int grand_nombre = 1000;

    char *ptr_char = (char*)&grand_nombre;

    printf("int complet : %d\n", grand_nombre);
    printf("Premier octet (via char*) : %d\n", *ptr_char);

    return 0;
}
```

**Sortie possible (little-endian) :**
```
int complet : 1000  
Premier octet (via char*) : -24  
```

**Explication :**
- 1000 en décimal = 0x000003E8 en hexadécimal
- Little-endian : stocké comme [E8] [03] [00] [00]
- Le premier octet (0xE8 = 232, interprété comme `signed char` = -24)

### Cas 2 : Cast vers un type plus grand

```c
#include <stdio.h>

int main(void) {
    char petit = 65;    // 'A'

    // ⚠️ Casting vers int* : lit 4 octets à partir de l'adresse de 'petit'
    // Ceci peut lire au-delà de la variable !
    int *ptr_int = (int*)&petit;    // DANGEREUX

    // Ne faites JAMAIS ceci en pratique !

    return 0;
}
```

**Problème :** Vous lisez potentiellement **au-delà** de la variable `petit` (qui ne fait qu'1 octet).

## Pointeurs de fonction et void*

On ne peut **pas** convertir directement entre pointeurs de fonctions et `void*` selon le standard C.

```c
void ma_fonction(void) {
    printf("Hello!\n");
}

// ⚠️ Non portable (peut fonctionner, mais pas garanti)
void *ptr = (void*)ma_fonction;
```

**Raison :** Sur certaines architectures, les pointeurs de fonctions et les pointeurs de données ont des tailles différentes.

**Solution portable :**
```c
void (*ptr_fonction)(void) = ma_fonction;    // Pointeur de fonction typé
```

## Exemple d'API générique : qsort

La fonction standard `qsort` utilise `void*` pour trier n'importe quel type :

```c
#include <stdio.h>
#include <stdlib.h>

int comparer_entiers(const void *a, const void *b) {
    int x = *(const int*)a;    // Cast explicite
    int y = *(const int*)b;
    return (x > y) - (x < y);  // Retourne -1, 0, ou 1
}

int main(void) {
    int nombres[5] = {50, 10, 40, 20, 30};

    printf("Avant tri : ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    qsort(nombres, 5, sizeof(int), comparer_entiers);

    printf("Après tri : ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", nombres[i]);
    }
    printf("\n");

    return 0;
}
```

**Sortie :**
```
Avant tri : 50 10 40 20 30  
Après tri : 10 20 30 40 50  
```

### Explication de qsort

```c
void qsort(void *base,           // Tableau (void* pour être générique)
           size_t nmemb,         // Nombre d'éléments
           size_t size,          // Taille d'un élément
           int (*compar)(const void*, const void*));  // Fonction de comparaison
```

La fonction de comparaison reçoit des `void*` qu'elle doit caster vers le bon type.

## Bonnes pratiques avec void*

### ✅ À faire

1. **Documentez le type attendu** dans les commentaires
```c
// ptr doit pointer vers un int
void traiter(void *ptr);
```

2. **Vérifiez NULL avant de caster**
```c
if (ptr != NULL) {
    int *p = (int*)ptr;
    // ...
}
```

3. **Utilisez const pour la sécurité**
```c
void afficher(const void *data);    // Ne modifie pas les données
```

4. **Passez la taille avec void***
```c
void copier(void *dest, const void *src, size_t n);
```

### ❌ À éviter

1. **Caster sans vérifier le type réel**
```c
void *ptr = &mon_float;  
int *p = (int*)ptr;    // ⚠️ Erreur de type !  
```

2. **Faire de l'arithmétique sur void*** (non portable)
```c
void *ptr = ...;  
ptr++;    // ⚠️ Extension GCC, pas du C standard  
```

3. **Oublier le cast lors du déréférencement**
```c
void *ptr = &age;  
printf("%d\n", *ptr);    // ✗ ERREUR  
printf("%d\n", *(int*)ptr);    // ✓ OK  
```

## Casting de const

### Retirer const (DANGEREUX)

```c
const int constante = 42;  
const int *ptr_const = &constante;  

// ⚠️ Cast qui retire const
int *ptr_modifiable = (int*)ptr_const;
*ptr_modifiable = 100;    // Comportement indéfini !
```

**Problème :** Modifier une variable déclarée `const` est un **comportement indéfini**. Le compilateur peut optimiser en supposant que la valeur ne change jamais.

### Ajouter const (OK)

```c
int variable = 42;  
int *ptr = &variable;  

const int *ptr_const = ptr;    // ✓ OK : conversion implicite
```

Ajouter `const` est toujours sûr.

## Résumé visuel

```
Types de pointeurs :

int*     char*    float*    ...
  ↓       ↓         ↓        ↓
  └───────┴─────────┴────────┘
             ↓
          void*  (pointeur générique)

Conversions :
- Vers void* : automatique ✓
- Depuis void* : cast explicite nécessaire (type*)
```

## Tableau récapitulatif

| Conversion | Syntaxe | Note |
|------------|---------|------|
| `int*` → `void*` | Automatique | Aucun cast nécessaire |
| `void*` → `int*` | `(int*)ptr` | Cast explicite obligatoire |
| `int*` → `char*` | `(char*)ptr` | Cast explicite, attention à l'alignement |
| `float*` → `int*` | `(int*)ptr` | ⚠️ Réinterprétation des octets |
| `const int*` → `int*` | `(int*)ptr` | ⚠️ DANGEREUX : retire const |
| `int*` → `const int*` | Automatique | ✓ Sûr : ajoute const |

## Exemple complet : conteneur générique

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void *data;
    size_t taille_element;
    size_t capacite;
    size_t nombre;
} Tableau_Generique;

Tableau_Generique* creer_tableau(size_t taille_element, size_t capacite) {
    Tableau_Generique *t = malloc(sizeof(Tableau_Generique));
    if (t == NULL) return NULL;

    t->data = malloc(taille_element * capacite);
    if (t->data == NULL) {
        free(t);
        return NULL;
    }

    t->taille_element = taille_element;
    t->capacite = capacite;
    t->nombre = 0;
    return t;
}

void ajouter_element(Tableau_Generique *t, const void *element) {
    if (t->nombre >= t->capacite) return;    // Tableau plein

    // Calcul de l'adresse où ajouter
    char *destination = (char*)t->data + (t->nombre * t->taille_element);
    memcpy(destination, element, t->taille_element);
    t->nombre++;
}

void* obtenir_element(Tableau_Generique *t, size_t index) {
    if (index >= t->nombre) return NULL;

    char *adresse = (char*)t->data + (index * t->taille_element);
    return adresse;
}

void detruire_tableau(Tableau_Generique *t) {
    if (t != NULL) {
        free(t->data);
        free(t);
    }
}

int main(void) {
    // Tableau d'entiers
    Tableau_Generique *entiers = creer_tableau(sizeof(int), 5);

    int valeurs[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        ajouter_element(entiers, &valeurs[i]);
    }

    printf("Entiers stockés : ");
    for (size_t i = 0; i < entiers->nombre; i++) {
        int *val = (int*)obtenir_element(entiers, i);
        printf("%d ", *val);
    }
    printf("\n");

    detruire_tableau(entiers);

    // Tableau de floats
    Tableau_Generique *decimaux = creer_tableau(sizeof(float), 3);

    float flottants[] = {3.14f, 2.71f, 1.41f};
    for (int i = 0; i < 3; i++) {
        ajouter_element(decimaux, &flottants[i]);
    }

    printf("Floats stockés : ");
    for (size_t i = 0; i < decimaux->nombre; i++) {
        float *val = (float*)obtenir_element(decimaux, i);
        printf("%.2f ", *val);
    }
    printf("\n");

    detruire_tableau(decimaux);

    return 0;
}
```

**Sortie :**
```
Entiers stockés : 10 20 30 40 50  
Floats stockés : 3.14 2.71 1.41  
```

### Points clés de l'exemple

1. **`void *data`** : Stocke n'importe quel type
2. **`size_t taille_element`** : Mémorise la taille du type
3. **Cast vers `char*`** : Pour l'arithmétique de pointeurs octet par octet
4. **`memcpy`** : Copie les octets bruts
5. **Cast lors de la lecture** : L'utilisateur doit caster vers le bon type

## Ce qu'il faut retenir

✅ **`void*`** est un pointeur générique qui peut pointer vers n'importe quel type

✅ Conversion **automatique** de n'importe quel pointeur vers `void*`

✅ Conversion **explicite** (cast) nécessaire de `void*` vers un type spécifique

✅ On **ne peut pas déréférencer** directement un `void*`

✅ Pas d'arithmétique de pointeurs sur `void*` (en C standard)

✅ `malloc` retourne `void*` pour être générique

✅ Les fonctions génériques (`qsort`, `memcpy`, etc.) utilisent `void*`

✅ Toujours **documenter** le type attendu derrière un `void*`

✅ Faire attention à l'**alignement** lors de casts entre types

✅ Ne **jamais retirer** `const` via un cast (comportement indéfini)

## Prochaine étape

Maintenant que vous comprenez les pointeurs génériques et le casting, la prochaine section vous montrera comment gérer les **pointeurs NULL et les dangling pointers** pour écrire du code plus sûr et éviter les bugs courants.

---

**🎯 Points de vérification de compréhension :**
- [ ] Je comprends ce qu'est un `void*`
- [ ] Je sais quand utiliser un cast explicite
- [ ] Je peux caster un `void*` vers le bon type
- [ ] Je comprends pourquoi on ne peut pas déréférencer un `void*` directement
- [ ] Je connais l'utilité de `void*` avec `malloc` et `qsort`
- [ ] Je sais éviter les casts dangereux (const, alignement)
- [ ] Je peux créer des fonctions génériques avec `void*`

---

**Prochaine étape :** 7.5 Pointeurs NULL et dangling pointers

⏭️ [Pointeurs NULL et dangling pointers](/07-pointeurs/05-pointeurs-null.md)
