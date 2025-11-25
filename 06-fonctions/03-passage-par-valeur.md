🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 6.3 Passage par valeur

## Introduction

Lorsque vous appelez une fonction en C en lui passant des arguments, une question fondamentale se pose : **que reçoit exactement la fonction ?** Reçoit-elle les variables elles-mêmes, ou bien des copies ?

La réponse en C est claire et sans ambiguïté : les fonctions reçoivent toujours des **copies** des valeurs qu'on leur passe. C'est ce qu'on appelle le **passage par valeur** (pass by value).

Comprendre ce mécanisme est essentiel pour :
- ✅ Éviter les bugs liés à des modifications inattendues (ou absentes) de variables
- ✅ Comprendre les performances de votre programme
- ✅ Préparer l'apprentissage des pointeurs (Module 3)
- ✅ Écrire du code prévisible et sans effets de bord non désirés

---

## Le concept : Une copie, pas l'original

### Analogie : La photocopie

Imaginez que vous donnez un document à un collègue :

**Scénario 1 : Vous donnez l'original**
- Votre collègue peut modifier le document
- Les modifications affectent **votre** document
- Vous n'avez plus l'original

**Scénario 2 : Vous donnez une photocopie**
- Votre collègue peut modifier la copie
- Les modifications n'affectent **pas** votre document original
- Vous conservez votre version intacte

**En C, c'est toujours le scénario 2** : la fonction reçoit une **photocopie** de vos données.

---

## Exemple simple

```c
#include <stdio.h>

void modifier(int x) {
    printf("  Dans modifier, avant : x = %d\n", x);
    x = 100;  // On modifie le paramètre
    printf("  Dans modifier, après : x = %d\n", x);
}

int main(void) {
    int nombre = 42;

    printf("Avant l'appel : nombre = %d\n", nombre);
    modifier(nombre);
    printf("Après l'appel : nombre = %d\n", nombre);

    return 0;
}
```

**Sortie :**
```
Avant l'appel : nombre = 42
  Dans modifier, avant : x = 42
  Dans modifier, après : x = 100
Après l'appel : nombre = 42
```

**Observation cruciale** : `nombre` dans `main` est toujours `42` ! La modification de `x` dans `modifier` n'a **aucun effet** sur `nombre`.

---

## Que se passe-t-il en mémoire ?

Revisitons l'exemple avec la pile (stack frames) :

### Étape 1 : Avant l'appel

```
    Stack
    ┌─────────────────────┐
    │  main frame         │
    │  ┌───────────────┐  │
    │  │ nombre = 42   │  │ ← Variable dans main
    │  └───────────────┘  │
    └─────────────────────┘
```

---

### Étape 2 : Appel de modifier(nombre)

Quand `modifier(nombre)` est appelé, la **valeur** de `nombre` (c'est-à-dire `42`) est **copiée** dans le paramètre `x` du nouveau stack frame :

```
    Stack
    ┌─────────────────────┐
    │  modifier frame     │ ← Nouveau frame
    │  ┌───────────────┐  │
    │  │ x = 42        │  │ ← COPIE de nombre
    │  └───────────────┘  │
    ├─────────────────────┤
    │  main frame         │
    │  ┌───────────────┐  │
    │  │ nombre = 42   │  │ ← Original inchangé
    │  └───────────────┘  │
    └─────────────────────┘
```

**Points clés** :
- `x` et `nombre` sont **deux variables différentes** en mémoire
- `x` contient une **copie** de la valeur de `nombre`
- Elles occupent des **emplacements mémoire distincts**

---

### Étape 3 : Modification de x

Lorsqu'on exécute `x = 100;` dans `modifier` :

```
    Stack
    ┌─────────────────────┐
    │  modifier frame     │
    │  ┌───────────────┐  │
    │  │ x = 100       │  │ ← x modifié
    │  └───────────────┘  │
    ├─────────────────────┤
    │  main frame         │
    │  ┌───────────────┐  │
    │  │ nombre = 42   │  │ ← nombre TOUJOURS 42 !
    │  └───────────────┘  │
    └─────────────────────┘
```

`nombre` reste à `42` car `x` est une **variable indépendante**.

---

### Étape 4 : Retour de la fonction

Quand `modifier` se termine, son stack frame est détruit :

```
    Stack
    ┌─────────────────────┐
    │  main frame         │
    │  ┌───────────────┐  │
    │  │ nombre = 42   │  │ ← Toujours 42
    │  └───────────────┘  │
    └─────────────────────┘

    x n'existe plus !
```

`x` disparaît complètement. La modification de `x` est donc perdue.

---

## Exemple avec plusieurs paramètres

```c
#include <stdio.h>

void echanger_copie(int a, int b) {
    printf("  Avant échange : a=%d, b=%d\n", a, b);
    int temp = a;
    a = b;
    b = temp;
    printf("  Après échange : a=%d, b=%d\n", a, b);
}

int main(void) {
    int x = 10;
    int y = 20;

    printf("Avant l'appel : x=%d, y=%d\n", x, y);
    echanger_copie(x, y);
    printf("Après l'appel : x=%d, y=%d\n", x, y);

    return 0;
}
```

**Sortie :**
```
Avant l'appel : x=10, y=20
  Avant échange : a=10, b=20
  Après échange : a=20, b=10
Après l'appel : x=10, y=20
```

**Constat** : L'échange fonctionne **à l'intérieur** de la fonction (sur les copies `a` et `b`), mais `x` et `y` dans `main` restent inchangés.

**Visualisation mémoire :**

```
Lors de l'appel echanger_copie(x, y) :

    ┌────────────────────┐
    │  echanger_copie    │
    │  a = 10 (copie)    │ ← Ces valeurs sont échangées
    │  b = 20 (copie)    │
    │  temp = ...        │
    ├────────────────────┤
    │  main              │
    │  x = 10            │ ← Ces valeurs restent identiques
    │  y = 20            │
    └────────────────────┘
```

---

## Conséquences du passage par valeur

### 1. Protection des données

Le passage par valeur **protège** vos variables originales contre les modifications accidentelles :

```c
void fonction_complexe(int donnee) {
    // Peu importe ce qu'on fait ici avec donnee...
    donnee = donnee * 2;
    donnee = -donnee;
    donnee = 0;
    // ... cela n'affecte PAS la variable dans l'appelant
}

int main(void) {
    int valeur_importante = 42;
    fonction_complexe(valeur_importante);
    // valeur_importante est toujours 42 !
}
```

**Avantage** : Sécurité et prévisibilité du code.

---

### 2. Coût en mémoire et performance

Chaque passage par valeur implique une **copie**. Pour les types simples (int, char, float), c'est négligeable :

```c
void calculer(int a, int b, int c) {
    // 3 entiers copiés : ~12 octets, très rapide
}
```

Mais pour les grandes structures, cela peut devenir coûteux :

```c
struct GrandeStructure {
    int tableau[10000];  // 40 000 octets !
    double matrice[100][100];  // 80 000 octets !
};

void traiter(struct GrandeStructure s) {
    // ⚠️ Copie de ~120 KB à chaque appel !
}

int main(void) {
    struct GrandeStructure data;
    traiter(data);  // Copie coûteuse
}
```

**Solution** : Passer un **pointeur** vers la structure (nous verrons cela dans le Module 3).

---

### 3. Impossibilité de modifier les arguments

Si vous voulez qu'une fonction **modifie** une variable de l'appelant, le passage par valeur ne fonctionne pas :

```c
// ❌ Cette fonction ne peut PAS incrémenter le compteur de l'appelant
void incrementer_echec(int compteur) {
    compteur++;  // Modifie la copie locale, pas l'original
}

int main(void) {
    int compte = 0;
    incrementer_echec(compte);
    printf("%d\n", compte);  // Affiche 0, pas 1 !
    return 0;
}
```

**Solution** : Passer un **pointeur** (adresse mémoire) de la variable.

---

## Passage par valeur avec différents types

### Types primitifs

Tous les types primitifs sont passés par valeur :

```c
void exemples(char c, short s, int i, long l, float f, double d) {
    // c, s, i, l, f, d sont des copies
    c = 'Z';
    i = 999;
    f = 3.14f;
    // Aucune de ces modifications n'affecte l'appelant
}
```

---

### Tableaux : Exception importante !

⚠️ **Attention** : Les tableaux sont une **exception** en C. Lorsque vous passez un tableau à une fonction, vous passez en réalité un **pointeur** vers le premier élément (passage par référence implicite) :

```c
void modifier_tableau(int tab[], int taille) {
    tab[0] = 999;  // ✅ Ceci MODIFIE le tableau original !
}

int main(void) {
    int nombres[5] = {1, 2, 3, 4, 5};

    printf("Avant : nombres[0] = %d\n", nombres[0]);
    modifier_tableau(nombres, 5);
    printf("Après : nombres[0] = %d\n", nombres[0]);

    return 0;
}
```

**Sortie :**
```
Avant : nombres[0] = 1
Après : nombres[0] = 999
```

**Explication** :
```c
int tab[]  // Est équivalent à : int *tab
```

Le nom d'un tableau **se dégrade en pointeur** vers son premier élément. Ce n'est pas une copie du tableau entier, mais une copie de l'adresse.

**Nous détaillerons les tableaux et pointeurs dans le Module 3.**

---

### Structures

Les structures sont passées par valeur (copie complète) :

```c
#include <stdio.h>

struct Point {
    int x;
    int y;
};

void deplacer(struct Point p) {
    p.x = 100;
    p.y = 200;
    printf("  Dans deplacer : (%d, %d)\n", p.x, p.y);
}

int main(void) {
    struct Point origine = {0, 0};

    printf("Avant : (%d, %d)\n", origine.x, origine.y);
    deplacer(origine);
    printf("Après : (%d, %d)\n", origine.x, origine.y);

    return 0;
}
```

**Sortie :**
```
Avant : (0, 0)
  Dans deplacer : (100, 200)
Après : (0, 0)
```

La structure entière est **copiée** dans le stack frame de `deplacer`. Les modifications n'affectent pas `origine`.

---

## Comment modifier une variable de l'appelant ?

Si vous voulez vraiment modifier une variable dans l'appelant, vous devez utiliser des **pointeurs** :

```c
#include <stdio.h>

// Version avec pointeur (passage par référence simulé)
void incrementer_ok(int *compteur) {
    (*compteur)++;  // Déréférence le pointeur et incrémente
}

int main(void) {
    int compte = 0;

    printf("Avant : %d\n", compte);
    incrementer_ok(&compte);  // On passe l'ADRESSE de compte
    printf("Après : %d\n", compte);

    return 0;
}
```

**Sortie :**
```
Avant : 0
Après : 1
```

**Explication** :
- `&compte` : Adresse mémoire de `compte` (pas sa valeur)
- `int *compteur` : Paramètre qui reçoit une **copie de l'adresse**
- `(*compteur)++` : Déréférence l'adresse et modifie la valeur à cet emplacement

**Visualisation mémoire :**

```
    ┌────────────────────────────┐
    │  incrementer_ok frame      │
    │  ┌──────────────────────┐  │
    │  │ compteur = 0x7fff12  │  │ ← Copie de l'ADRESSE
    │  └──────────────────────┘  │
    ├────────────────────────────┤
    │  main frame                │
    │  ┌──────────────────────┐  │
    │  │ compte = 1           │  │ ← Variable modifiée via le pointeur
    │  │ @ = 0x7fff12         │  │
    │  └──────────────────────┘  │
    └────────────────────────────┘
```

Même si `compteur` est une copie (passage par valeur du pointeur), cette copie **contient l'adresse** de `compte`. Donc déréférencer le pointeur permet d'accéder à la variable originale.

**Nous approfondirons les pointeurs dans le Module 3.**

---

## Exemple pratique : Échanger deux variables

### Tentative 1 : Passage par valeur (échec)

```c
void echanger_echec(int a, int b) {
    int temp = a;
    a = b;
    b = temp;
    // a et b sont échangés localement, mais ce sont des copies !
}

int main(void) {
    int x = 5, y = 10;
    echanger_echec(x, y);
    printf("x=%d, y=%d\n", x, y);  // ❌ Affiche : x=5, y=10 (pas d'échange)
    return 0;
}
```

---

### Tentative 2 : Passage par pointeur (succès)

```c
void echanger_ok(int *a, int *b) {
    int temp = *a;  // Lit la valeur pointée par a
    *a = *b;        // Écrit la valeur de b dans l'emplacement de a
    *b = temp;      // Écrit temp dans l'emplacement de b
}

int main(void) {
    int x = 5, y = 10;
    echanger_ok(&x, &y);  // On passe les adresses
    printf("x=%d, y=%d\n", x, y);  // ✅ Affiche : x=10, y=5 (échange réussi)
    return 0;
}
```

**Visualisation de l'échange :**

```
Avant l'appel :
    main : x = 5 (à l'adresse 0xAAA)
           y = 10 (à l'adresse 0xBBB)

Pendant echanger_ok(&x, &y) :
    echanger_ok frame :
        a = 0xAAA (pointeur vers x)
        b = 0xBBB (pointeur vers y)
        temp = 5

    Étape 1 : temp = *a        → temp = 5
    Étape 2 : *a = *b          → x devient 10
    Étape 3 : *b = temp        → y devient 5

Après l'appel :
    main : x = 10
           y = 5
```

---

## Comparaison : Valeur vs Pointeur

| Aspect | Passage par valeur | Passage par pointeur |
|--------|-------------------|---------------------|
| **Syntaxe appel** | `fonction(x)` | `fonction(&x)` |
| **Paramètre** | `type param` | `type *param` |
| **Copie** | Copie de la valeur | Copie de l'adresse |
| **Modification** | ❌ Impossible | ✅ Possible (via déréférencement) |
| **Performance** | ⚠️ Coûteux pour grandes structures | ✅ Rapide (8 octets sur 64 bits) |
| **Sécurité** | ✅ Protège l'original | ⚠️ Peut modifier accidentellement |
| **Usage** | Types simples, lecture seule | Modification nécessaire, grandes données |

---

## Erreurs courantes à éviter

### Erreur 1 : Attendre une modification impossible

```c
void doubler(int n) {
    n = n * 2;  // Modifie la copie locale
}

int main(void) {
    int valeur = 5;
    doubler(valeur);
    printf("%d\n", valeur);  // ❌ Affiche 5, pas 10 !
    return 0;
}
```

**Correction :**
```c
// Option 1 : Retourner la nouvelle valeur
int doubler(int n) {
    return n * 2;
}

int main(void) {
    int valeur = 5;
    valeur = doubler(valeur);  // ✅ Assigner le résultat
    printf("%d\n", valeur);    // ✅ Affiche 10
    return 0;
}

// Option 2 : Utiliser un pointeur
void doubler_ptr(int *n) {
    *n = *n * 2;
}

int main(void) {
    int valeur = 5;
    doubler_ptr(&valeur);  // ✅ Passe l'adresse
    printf("%d\n", valeur); // ✅ Affiche 10
    return 0;
}
```

---

### Erreur 2 : Confondre tableaux et autres types

```c
void modifier_entier(int x) {
    x = 100;  // ❌ Ne modifie PAS l'original
}

void modifier_tableau(int tab[]) {
    tab[0] = 100;  // ✅ Modifie l'original (tableau = pointeur)
}

int main(void) {
    int n = 42;
    int arr[3] = {1, 2, 3};

    modifier_entier(n);
    printf("n = %d\n", n);  // Affiche 42

    modifier_tableau(arr);
    printf("arr[0] = %d\n", arr[0]);  // Affiche 100

    return 0;
}
```

---

### Erreur 3 : Passer de grandes structures par valeur

```c
struct Grosse {
    int data[1000];  // 4000 octets
};

// ❌ Copie 4000 octets à chaque appel !
void traiter_mauvais(struct Grosse s) {
    // ...
}

// ✅ Copie seulement 8 octets (pointeur sur 64 bits)
void traiter_bon(struct Grosse *s) {
    // Accès aux membres : s->data[0]
}

int main(void) {
    struct Grosse g;
    traiter_bon(&g);  // Passe un pointeur
    return 0;
}
```

---

## Passage par valeur et const

Vous pouvez utiliser `const` pour indiquer qu'un paramètre ne sera pas modifié, même si c'est une copie :

```c
void afficher(const int valeur) {
    printf("%d\n", valeur);
    // valeur = 42;  // ❌ ERREUR : valeur est const
}
```

C'est surtout utile avec les pointeurs :

```c
void afficher_tableau(const int *tab, int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tab[i]);
    }
    // tab[0] = 999;  // ❌ ERREUR : tab pointe vers des données const
}
```

`const` sert de **documentation** et de **protection** contre les modifications accidentelles.

---

## Bonnes pratiques

### 1. Préférez le passage par valeur pour les types simples

```c
// ✅ Bon : simple et clair
int calculer(int a, int b, int c) {
    return a + b + c;
}

// ❌ Inutilement complexe
int calculer(int *a, int *b, int *c) {
    return *a + *b + *c;
}
```

---

### 2. Utilisez des pointeurs pour les grandes structures

```c
struct Config {
    char nom[256];
    int parametres[100];
    double valeurs[50];
};

// ❌ Copie ~1500 octets
void charger_mauvais(struct Config cfg) { }

// ✅ Copie seulement 8 octets
void charger_bon(const struct Config *cfg) { }
```

---

### 3. Utilisez const avec les pointeurs en lecture seule

```c
// Indique clairement que la chaîne ne sera pas modifiée
void afficher_message(const char *message) {
    printf("%s\n", message);
}

// Peut modifier les données
void modifier_buffer(char *buffer, int taille) {
    // ...
}
```

---

### 4. Documentez l'intention avec les noms

```c
// Le nom indique qu'on retourne une valeur
int calculer_somme(int a, int b) {
    return a + b;
}

// Le nom indique qu'on modifie le paramètre
void incrementer_compteur(int *compteur) {
    (*compteur)++;
}
```

---

## Récapitulatif

### Points clés à retenir

1. ✅ En C, **tout est passé par valeur** (sauf les tableaux qui se dégradent en pointeurs)
2. ✅ Passer par valeur signifie **copier** la donnée dans un nouveau stack frame
3. ✅ Les modifications des paramètres **n'affectent pas** les variables de l'appelant
4. ✅ Le passage par valeur **protège** les données originales
5. ✅ Pour modifier une variable de l'appelant, il faut passer son **adresse** (pointeur)
6. ✅ Le passage par valeur est **rapide** pour les types simples, mais **coûteux** pour les grandes structures
7. ✅ Les tableaux sont une **exception** : le nom se dégrade en pointeur (passage par référence implicite)

---

### Tableau de décision

| Besoin | Solution |
|--------|----------|
| Lire une valeur simple | Passage par valeur |
| Modifier une valeur simple | Passage par pointeur |
| Lire une grande structure | Passage par pointeur const |
| Modifier une grande structure | Passage par pointeur |
| Lire un tableau | Passage du nom (= pointeur) |
| Modifier un tableau | Passage du nom (= pointeur) |

---

## Exemple final : Programme complet

```c
#include <stdio.h>

// Passage par valeur : lecture seule
int carre(int n) {
    return n * n;
}

// Passage par pointeur : modification
void tripler(int *n) {
    *n = *n * 3;
}

// Passage par valeur de structure (petite)
struct Point {
    int x;
    int y;
};

int distance_manhattan(struct Point p1, struct Point p2) {
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}

// Passage par pointeur de structure (modification)
void deplacer_point(struct Point *p, int dx, int dy) {
    p->x += dx;
    p->y += dy;
}

int main(void) {
    // Test 1 : carre (passage par valeur)
    int nombre = 5;
    int resultat = carre(nombre);
    printf("Le carré de %d est %d\n", nombre, resultat);
    printf("nombre vaut toujours %d\n\n", nombre);

    // Test 2 : tripler (passage par pointeur)
    int valeur = 7;
    printf("Avant tripler : %d\n", valeur);
    tripler(&valeur);
    printf("Après tripler : %d\n\n", valeur);

    // Test 3 : distance (passage par valeur de structures)
    struct Point p1 = {0, 0};
    struct Point p2 = {3, 4};
    int dist = distance_manhattan(p1, p2);
    printf("Distance Manhattan : %d\n\n", dist);

    // Test 4 : deplacer (passage par pointeur de structure)
    printf("Point p1 avant : (%d, %d)\n", p1.x, p1.y);
    deplacer_point(&p1, 10, 20);
    printf("Point p1 après : (%d, %d)\n", p1.x, p1.y);

    return 0;
}
```

**Sortie :**
```
Le carré de 5 est 25
nombre vaut toujours 5

Avant tripler : 7
Après tripler : 21

Distance Manhattan : 7

Point p1 avant : (0, 0)
Point p1 après : (10, 20)
```

---

## Pour aller plus loin

Le passage par valeur est un concept fondamental qui prépare l'apprentissage de :

- **Module 3 (Section 7)** : Les pointeurs en détail
- **Section 6.4** : Les valeurs de retour et codes d'erreur
- **Module 3 (Section 9)** : L'allocation dynamique (heap)

Dans la prochaine section, nous verrons comment les fonctions retournent des valeurs et comment gérer les erreurs de manière robuste ! 🚀

⏭️ [Valeurs de retour et codes d'erreur](/06-fonctions/04-valeurs-retour.md)
