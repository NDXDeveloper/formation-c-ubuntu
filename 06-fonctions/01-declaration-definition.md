🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 6.1 Déclaration, définition et prototypes

## Introduction

Les fonctions sont les blocs de construction fondamentaux de tout programme C. Elles permettent de découper votre code en morceaux réutilisables, plus faciles à comprendre et à maintenir. Avant de plonger dans les détails techniques, comprenons pourquoi trois termes différents existent : **déclaration**, **définition** et **prototype**.

---

## Qu'est-ce qu'une fonction ?

Une fonction est un bloc de code qui effectue une tâche spécifique. Elle peut :
- Recevoir des données en entrée (paramètres)
- Effectuer des opérations
- Retourner un résultat (ou non)

**Analogie** : Pensez à une fonction comme une machine dans une usine. Vous lui donnez des matières premières (paramètres), elle effectue un traitement, et elle produit un résultat (valeur de retour).

```c
// Exemple simple d'une fonction qui additionne deux nombres
int additionner(int a, int b) {
    return a + b;
}
```

---

## Les trois concepts clés

### 1. La définition de fonction

La **définition** est l'implémentation complète de la fonction. C'est là que vous écrivez le code qui s'exécutera quand la fonction sera appelée.

**Structure d'une définition :**

```c
type_retour nom_fonction(type_param1 param1, type_param2 param2, ...) {
    // Corps de la fonction
    // Instructions à exécuter
    return valeur; // Si type_retour n'est pas void
}
```

**Exemple concret :**

```c
int multiplier(int x, int y) {
    int resultat = x * y;
    return resultat;
}
```

**Décomposition :**
- `int` : Type de retour (la fonction renvoie un entier)
- `multiplier` : Nom de la fonction
- `(int x, int y)` : Liste des paramètres (deux entiers nommés x et y)
- `{ ... }` : Corps de la fonction contenant les instructions
- `return resultat;` : Valeur retournée à l'appelant

---

### 2. La déclaration de fonction (ou prototype)

La **déclaration** (aussi appelée **prototype**) informe le compilateur de l'existence d'une fonction sans fournir son implémentation. C'est comme une promesse : "Cette fonction existe, voici son interface, je te donnerai le code plus tard".

**Structure d'un prototype :**

```c
type_retour nom_fonction(type_param1, type_param2, ...);
```

**Exemple :**

```c
int multiplier(int, int);
```

Ou avec les noms de paramètres (recommandé pour la lisibilité) :

```c
int multiplier(int x, int y);
```

**Point important :** Notez le **point-virgule** à la fin. C'est ce qui distingue visuellement un prototype d'une définition.

---

### 3. Différence entre déclaration et définition

| Aspect | Déclaration (Prototype) | Définition |
|--------|-------------------------|------------|
| Contenu | Signature uniquement | Signature + implémentation |
| Terminaison | Point-virgule `;` | Accolades `{ }` |
| Mémoire | Aucune allocation | Code machine généré |
| Obligatoire | Oui (avant utilisation) | Oui (une seule fois) |
| Répétition | Peut être répétée | Une seule fois dans le programme |

---

## Pourquoi utiliser des prototypes ?

### Raison 1 : Ordre de déclaration

En C, le compilateur lit votre code de haut en bas. Si vous appelez une fonction avant de la définir, le compilateur ne la connaît pas encore.

**Problème sans prototype :**

```c
#include <stdio.h>

int main() {
    int resultat = additionner(5, 3);  // ❌ ERREUR : additionner n'est pas connu
    printf("Résultat : %d\n", resultat);
    return 0;
}

int additionner(int a, int b) {
    return a + b;
}
```

**Solution avec prototype :**

```c
#include <stdio.h>

int additionner(int a, int b);  // ✅ Prototype : le compilateur sait que cette fonction existe

int main() {
    int resultat = additionner(5, 3);  // ✅ OK maintenant
    printf("Résultat : %d\n", resultat);
    return 0;
}

int additionner(int a, int b) {  // Définition après main()
    return a + b;
}
```

---

### Raison 2 : Compilation séparée

Dans les vrais projets, votre code est divisé en plusieurs fichiers. Les prototypes permettent de déclarer des fonctions définies dans d'autres fichiers.

**Fichier : operations.h** (fichier d'en-tête)
```c
#ifndef OPERATIONS_H
#define OPERATIONS_H

// Prototypes des fonctions
int additionner(int a, int b);
int soustraire(int a, int b);
int multiplier(int a, int b);

#endif
```

**Fichier : operations.c** (implémentation)
```c
#include "operations.h"

// Définitions des fonctions
int additionner(int a, int b) {
    return a + b;
}

int soustraire(int a, int b) {
    return a - b;
}

int multiplier(int a, int b) {
    return a * b;
}
```

**Fichier : main.c** (utilisation)
```c
#include <stdio.h>
#include "operations.h"  // On inclut le fichier d'en-tête avec les prototypes

int main() {
    int x = 10, y = 5;

    printf("%d + %d = %d\n", x, y, additionner(x, y));
    printf("%d - %d = %d\n", x, y, soustraire(x, y));
    printf("%d * %d = %d\n", x, y, multiplier(x, y));

    return 0;
}
```

---

## Syntaxe des prototypes en détail

### Noms de paramètres optionnels

Dans un prototype, les noms des paramètres sont **optionnels** :

```c
// Trois prototypes équivalents
int calculer(int a, int b);     // ✅ Recommandé (lisible)
int calculer(int, int);          // ✅ Valide mais moins lisible
int calculer(int x, int y);      // ✅ Les noms peuvent différer de la définition
```

**Bonne pratique :** Utilisez toujours des noms de paramètres explicites dans vos prototypes pour la documentation et la lisibilité.

---

### Prototypes avec void

Si une fonction ne prend **aucun paramètre** :

```c
void afficher_bienvenue(void);  // ✅ Correct
void afficher_bienvenue();       // ⚠️ Ancienne syntaxe (à éviter en C moderne)
```

**Différence importante :**
- `(void)` : La fonction ne prend explicitement aucun paramètre
- `()` : En C (contrairement au C++), cela signifie "nombre de paramètres non spécifié" (dangereux !)

Si une fonction ne retourne **rien** :

```c
void afficher_message(const char *message);  // Retourne void (rien)
```

---

### Prototypes avec plusieurs paramètres

```c
double calculer_moyenne(int tableau[], int taille);
void echanger(int *a, int *b);
char* concatener(const char *str1, const char *str2, char *resultat);
```

---

## Exemples complets

### Exemple 1 : Programme simple

```c
#include <stdio.h>

// Prototypes
int carre(int n);
void afficher_carre(int n);

int main() {
    int nombre = 7;
    afficher_carre(nombre);
    return 0;
}

// Définition de carre
int carre(int n) {
    return n * n;
}

// Définition de afficher_carre
void afficher_carre(int n) {
    printf("Le carré de %d est %d\n", n, carre(n));
}
```

**Sortie :**
```
Le carré de 7 est 49
```

---

### Exemple 2 : Fonctions avec différents types de retour

```c
#include <stdio.h>
#include <stdbool.h>

// Prototypes
int maximum(int a, int b);
bool est_pair(int n);
void afficher_separateur(void);

int main() {
    int x = 15, y = 23;

    printf("Maximum entre %d et %d : %d\n", x, y, maximum(x, y));

    if (est_pair(x)) {
        printf("%d est pair\n", x);
    } else {
        printf("%d est impair\n", x);
    }

    afficher_separateur();

    return 0;
}

// Retourne le plus grand des deux nombres
int maximum(int a, int b) {
    return (a > b) ? a : b;
}

// Retourne true si n est pair, false sinon
bool est_pair(int n) {
    return (n % 2 == 0);
}

// Ne retourne rien (void)
void afficher_separateur(void) {
    printf("====================\n");
}
```

---

## Erreurs courantes à éviter

### Erreur 1 : Oublier le prototype

```c
#include <stdio.h>

int main() {
    afficher(42);  // ❌ ERREUR : fonction non déclarée
    return 0;
}

void afficher(int n) {
    printf("%d\n", n);
}
```

**Correction :**
```c
#include <stdio.h>

void afficher(int n);  // ✅ Ajout du prototype

int main() {
    afficher(42);  // ✅ OK
    return 0;
}

void afficher(int n) {
    printf("%d\n", n);
}
```

---

### Erreur 2 : Incohérence entre prototype et définition

```c
int additionner(int a, int b);  // Prototype dit : retourne int

// ❌ ERREUR : la définition dit void
void additionner(int a, int b) {
    printf("%d\n", a + b);
}
```

**Le compilateur générera une erreur car la signature ne correspond pas.**

---

### Erreur 3 : Oublier le point-virgule dans le prototype

```c
int multiplier(int a, int b)  // ❌ ERREUR : manque le ;

int main() {
    // ...
}
```

---

### Erreur 4 : Confusion entre déclaration et définition

```c
// Ceci est une définition (pas un prototype) car il y a des accolades
int diviser(int a, int b) {
    return a / b;
}

// Ceci est un prototype (déclaration)
int diviser(int a, int b);
```

---

## Conventions et bonnes pratiques

### 1. Organisation du code

**Pour un fichier unique :**
```c
#include <stdio.h>

// Tous les prototypes en haut
int fonction1(int x);
void fonction2(void);
double fonction3(double a, double b);

// La fonction main
int main() {
    // Code principal
    return 0;
}

// Toutes les définitions après main
int fonction1(int x) {
    // ...
}

void fonction2(void) {
    // ...
}

double fonction3(double a, double b) {
    // ...
}
```

---

### 2. Utilisation des fichiers d'en-tête

**Pour les projets multi-fichiers :**

- **Prototypes** → fichiers `.h` (headers)
- **Définitions** → fichiers `.c` (source)

```c
// Dans operations.h
#ifndef OPERATIONS_H
#define OPERATIONS_H

int additionner(int a, int b);
int soustraire(int a, int b);

#endif

// Dans operations.c
#include "operations.h"

int additionner(int a, int b) {
    return a + b;
}

int soustraire(int a, int b) {
    return a - b;
}
```

---

### 3. Nommage des fonctions

- Utilisez des **verbes** pour les actions : `calculer()`, `afficher()`, `lire()`
- Soyez **explicites** : `calculer_moyenne()` plutôt que `calc()`
- Convention **snake_case** en C : `ma_fonction()` plutôt que `MaFonction()`

---

### 4. Documentation dans les prototypes

Les prototypes sont l'endroit idéal pour documenter vos fonctions :

```c
/**
 * Calcule la factorielle d'un nombre positif
 *
 * @param n Le nombre dont on veut la factorielle (doit être >= 0)
 * @return La factorielle de n, ou -1 en cas d'erreur
 */
long factorielle(int n);
```

---

## Cas particulier : la fonction main

La fonction `main` est spéciale : c'est le point d'entrée de votre programme. Elle peut prendre deux formes :

```c
// Forme 1 : sans paramètres
int main(void) {
    return 0;
}

// Forme 2 : avec paramètres (ligne de commande)
int main(int argc, char *argv[]) {
    return 0;
}
```

**Note :** La fonction `main` n'a **jamais** de prototype car elle est appelée automatiquement par le système, pas par votre code.

---

## Récapitulatif

| Concept | C'est quoi ? | Où ? | Obligatoire ? |
|---------|--------------|------|---------------|
| **Définition** | Implémentation complète de la fonction | Fichier `.c` | Oui (une seule fois) |
| **Prototype** | Signature de la fonction (déclaration) | Avant utilisation ou fichier `.h` | Oui (si fonction utilisée avant sa définition) |
| **Déclaration** | Synonyme de prototype | Fichier `.h` généralement | Selon contexte |

---

## Points clés à retenir

1. ✅ Un **prototype** informe le compilateur qu'une fonction existe
2. ✅ Une **définition** fournit le code de la fonction
3. ✅ Le prototype se termine par un `;`, la définition utilise `{ }`
4. ✅ Les prototypes permettent d'appeler des fonctions définies plus tard
5. ✅ Dans les projets multi-fichiers, mettez les prototypes dans les `.h` et les définitions dans les `.c`
6. ✅ Le prototype et la définition doivent avoir la **même signature** (type de retour, nom, paramètres)

---

## Exemple final : Programme complet

```c
#include <stdio.h>

// ========== PROTOTYPES ==========
int puissance(int base, int exposant);
void afficher_resultat(int base, int exposant, int resultat);
void afficher_titre(void);

// ========== MAIN ==========
int main(void) {
    afficher_titre();

    int base = 2;
    int exposant = 10;
    int resultat = puissance(base, exposant);

    afficher_resultat(base, exposant, resultat);

    return 0;
}

// ========== DÉFINITIONS ==========
int puissance(int base, int exposant) {
    int resultat = 1;
    for (int i = 0; i < exposant; i++) {
        resultat *= base;
    }
    return resultat;
}

void afficher_resultat(int base, int exposant, int resultat) {
    printf("%d^%d = %d\n", base, exposant, resultat);
}

void afficher_titre(void) {
    printf("=== Calculateur de puissance ===\n");
}
```

**Sortie :**
```
=== Calculateur de puissance ===
2^10 = 1024
```

---

## Pour aller plus loin

Dans les sections suivantes, nous explorerons :
- **6.2** : La pile d'exécution (Stack Frames) et comment les fonctions utilisent la mémoire
- **6.3** : Le passage de paramètres par valeur
- **6.4** : Les valeurs de retour et la gestion des erreurs
- **6.5** : Les paramètres de la fonction main

Ces concepts s'appuient sur la compréhension des prototypes et définitions que vous venez d'acquérir ! 🚀

⏭️ [La Pile : Comprendre les Stack Frames](/06-fonctions/02-stack-frames.md)
