🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 22.1 Pointeurs de fonctions (Callbacks)

## Introduction

Dans ce chapitre, nous allons découvrir l'un des concepts les plus puissants du langage C : **les pointeurs de fonctions**. Bien que cela puisse sembler intimidant au premier abord, les pointeurs de fonctions sont en réalité un outil très pratique qui vous permettra d'écrire du code plus flexible et réutilisable.

### Qu'est-ce qu'un pointeur de fonction ?

Vous savez déjà qu'un pointeur peut contenir l'adresse d'une variable en mémoire. Eh bien, les fonctions aussi sont stockées en mémoire ! Un **pointeur de fonction** est simplement une variable qui contient l'adresse d'une fonction.

**Analogie** : Imaginez que vous avez plusieurs outils dans votre garage (des fonctions), et que vous créez une liste de raccourcis sur votre téléphone pour savoir où se trouve chaque outil. Ces raccourcis sont comme des pointeurs de fonctions : ils ne sont pas les outils eux-mêmes, mais ils vous permettent d'y accéder rapidement.

### Pourquoi utiliser des pointeurs de fonctions ?

Les pointeurs de fonctions sont utiles dans plusieurs situations :

1. **Callbacks** : Passer une fonction en paramètre à une autre fonction
2. **Tables de dispatch** : Sélectionner dynamiquement quelle fonction exécuter
3. **Plugins et extensibilité** : Permettre à du code de s'étendre sans modification
4. **Programmation événementielle** : Associer des actions à des événements

---

## Syntaxe de base

### Déclaration d'un pointeur de fonction

La syntaxe peut sembler étrange au début, mais décomposons-la :

```c
int (*pointeur_fonction)(int, int);
```

Décortiquons cette déclaration :
- `int` (au début) : le type de retour de la fonction
- `(*pointeur_fonction)` : le nom de notre pointeur de fonction (les parenthèses sont **obligatoires**)
- `(int, int)` : les types des paramètres que la fonction accepte

**Important** : Les parenthèses autour de `*pointeur_fonction` sont cruciales. Sans elles, `int *pointeur_fonction(int, int)` déclarerait une fonction qui retourne un pointeur vers un int, et non un pointeur vers une fonction !

### Exemple simple complet

```c
#include <stdio.h>

// Une fonction simple qui additionne deux nombres
int addition(int a, int b) {
    return a + b;
}

// Une autre fonction qui soustrait deux nombres
int soustraction(int a, int b) {
    return a - b;
}

int main(void) {
    // Déclaration d'un pointeur de fonction
    int (*operation)(int, int);

    // On fait pointer notre pointeur vers la fonction addition
    operation = addition;

    // On appelle la fonction via le pointeur
    int resultat = operation(5, 3);
    printf("Résultat de l'addition : %d\n", resultat);  // Affiche : 8

    // On peut changer la fonction pointée
    operation = soustraction;
    resultat = operation(5, 3);
    printf("Résultat de la soustraction : %d\n", resultat);  // Affiche : 2

    return 0;
}
```

**Points clés** :
- On n'utilise **pas** l'opérateur `&` pour obtenir l'adresse d'une fonction (bien que ce soit possible)
- On peut appeler la fonction via le pointeur simplement avec `operation(5, 3)`
- On peut également utiliser la syntaxe explicite `(*operation)(5, 3)`, mais c'est moins courant

---

## Les Callbacks : Passer des fonctions en paramètres

Un **callback** est une fonction que vous passez en paramètre à une autre fonction. C'est l'une des utilisations les plus courantes des pointeurs de fonctions.

### Exemple : Calculatrice générique

Imaginons que nous voulons créer une fonction qui applique une opération mathématique sur deux nombres, mais sans savoir à l'avance quelle opération sera utilisée :

```c
#include <stdio.h>

// Définition de plusieurs opérations
int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}

int division(int a, int b) {
    if (b == 0) {
        printf("Erreur : division par zéro\n");
        return 0;
    }
    return a / b;
}

// Fonction générique qui prend un callback
int calculer(int x, int y, int (*operation)(int, int)) {
    printf("Calcul de %d et %d...\n", x, y);
    return operation(x, y);
}

int main(void) {
    int a = 10, b = 5;

    // Utilisation avec différentes opérations
    printf("Addition : %d\n", calculer(a, b, addition));
    printf("Multiplication : %d\n", calculer(a, b, multiplication));
    printf("Division : %d\n", calculer(a, b, division));

    return 0;
}
```

**Sortie** :
```
Calcul de 10 et 5...
Addition : 15
Calcul de 10 et 5...
Multiplication : 50
Calcul de 10 et 5...
Division : 2
```

### Pourquoi est-ce utile ?

Cette approche permet de :
- **Réutiliser le code** : La fonction `calculer()` peut être utilisée avec n'importe quelle opération
- **Séparer les responsabilités** : La logique d'exécution est séparée de l'opération elle-même
- **Étendre facilement** : On peut ajouter de nouvelles opérations sans modifier `calculer()`

---

## Exemple pratique : Tri personnalisé

Un cas d'usage très courant est le tri de tableaux avec une fonction de comparaison personnalisée. C'est exactement ce que fait la fonction standard `qsort()`.

```c
#include <stdio.h>
#include <stdlib.h>

// Fonction de comparaison pour tri croissant
int comparer_croissant(const void *a, const void *b) {
    int valeur_a = *(const int *)a;
    int valeur_b = *(const int *)b;
    return valeur_a - valeur_b;
}

// Fonction de comparaison pour tri décroissant
int comparer_decroissant(const void *a, const void *b) {
    int valeur_a = *(const int *)a;
    int valeur_b = *(const int *)b;
    return valeur_b - valeur_a;
}

void afficher_tableau(int tableau[], int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");
}

int main(void) {
    int nombres[] = {64, 34, 25, 12, 22, 11, 90};
    int taille = sizeof(nombres) / sizeof(nombres[0]);

    printf("Tableau original : ");
    afficher_tableau(nombres, taille);

    // Tri croissant en utilisant qsort avec notre callback
    qsort(nombres, taille, sizeof(int), comparer_croissant);
    printf("Tri croissant : ");
    afficher_tableau(nombres, taille);

    // Tri décroissant
    qsort(nombres, taille, sizeof(int), comparer_decroissant);
    printf("Tri décroissant : ");
    afficher_tableau(nombres, taille);

    return 0;
}
```

**Explication de `qsort()`** :
- `qsort()` est une fonction de la bibliothèque standard qui trie un tableau
- Elle prend en dernier paramètre un pointeur de fonction de comparaison
- Cette fonction de comparaison doit retourner :
  - Un nombre **négatif** si `a < b`
  - **Zéro** si `a == b`
  - Un nombre **positif** si `a > b`

---

## Typedef pour simplifier la syntaxe

La syntaxe des pointeurs de fonctions peut devenir lourde. On peut utiliser `typedef` pour la simplifier :

```c
#include <stdio.h>

// Définition d'un type pour les pointeurs de fonctions
typedef int (*OperationBinaire)(int, int);

int addition(int a, int b) {
    return a + b;
}

int soustraction(int a, int b) {
    return a - b;
}

// Fonction qui prend un callback (syntaxe plus lisible)
int appliquer_operation(int x, int y, OperationBinaire op) {
    return op(x, y);
}

int main(void) {
    OperationBinaire mon_operation;

    mon_operation = addition;
    printf("10 + 5 = %d\n", appliquer_operation(10, 5, mon_operation));

    mon_operation = soustraction;
    printf("10 - 5 = %d\n", appliquer_operation(10, 5, mon_operation));

    return 0;
}
```

**Avantages du typedef** :
- Code plus lisible
- Facilite la maintenance
- Réduit les erreurs de syntaxe

---

## Exemple avancé : Système d'événements

Voici un exemple plus réaliste d'utilisation de callbacks : un système simple de gestion d'événements.

```c
#include <stdio.h>
#include <string.h>

// Type pour les callbacks d'événements
typedef void (*EventCallback)(const char *message);

// Différents handlers d'événements
void on_error(const char *message) {
    printf("[ERREUR] %s\n", message);
}

void on_warning(const char *message) {
    printf("[ATTENTION] %s\n", message);
}

void on_info(const char *message) {
    printf("[INFO] %s\n", message);
}

// Structure pour gérer les événements
typedef struct {
    const char *nom_evenement;
    EventCallback callback;
} Evenement;

// Fonction qui déclenche un événement
void declencher_evenement(Evenement *evt, const char *message) {
    if (evt != NULL && evt->callback != NULL) {
        printf("Événement '%s' déclenché : ", evt->nom_evenement);
        evt->callback(message);
    }
}

int main(void) {
    // Création d'événements avec différents callbacks
    Evenement erreur = {"erreur", on_error};
    Evenement warning = {"warning", on_warning};
    Evenement info = {"info", on_info};

    // Déclenchement des événements
    declencher_evenement(&info, "Application démarrée");
    declencher_evenement(&warning, "Mémoire faible");
    declencher_evenement(&erreur, "Fichier introuvable");

    return 0;
}
```

**Sortie** :
```
Événement 'info' déclenché : [INFO] Application démarrée
Événement 'warning' déclenché : [ATTENTION] Mémoire faible
Événement 'erreur' déclenché : [ERREUR] Fichier introuvable
```

---

## Cas d'usage courants dans le monde réel

### 1. Bibliothèques de traitement de données

```c
// Exemple : appliquer une transformation sur chaque élément d'un tableau
typedef int (*TransformFunction)(int);

void map(int tableau[], int taille, TransformFunction transform) {
    for (int i = 0; i < taille; i++) {
        tableau[i] = transform(tableau[i]);
    }
}

int doubler(int x) {
    return x * 2;
}

int augmenter(int x) {
    return x + 1;
}
```

### 2. Interfaces de plugins

```c
// Exemple : système de plugins
typedef struct {
    const char *nom;
    void (*initialiser)(void);
    void (*executer)(void);
    void (*nettoyer)(void);
} Plugin;

void charger_plugin(Plugin *p) {
    if (p->initialiser) p->initialiser();
    if (p->executer) p->executer();
    if (p->nettoyer) p->nettoyer();
}
```

### 3. Gestionnaires de signaux (programmation système)

```c
#include <signal.h>

void gestionnaire_sigint(int signal) {
    printf("Signal SIGINT reçu, arrêt propre...\n");
    exit(0);
}

// Dans main() :
signal(SIGINT, gestionnaire_sigint);  // Callback pour Ctrl+C
```

---

## Pièges courants et comment les éviter

### 1. Oublier les parenthèses dans la déclaration

```c
// ❌ INCORRECT : déclare une fonction qui retourne un pointeur
int *mauvaise_declaration(int, int);

// ✅ CORRECT : déclare un pointeur de fonction
int (*bonne_declaration)(int, int);
```

### 2. Types incompatibles

```c
int addition(int a, int b) { return a + b; }
float addition_float(float a, float b) { return a + b; }

int main(void) {
    int (*operation)(int, int);

    operation = addition;            // ✅ OK : types compatibles
    // operation = addition_float;   // ❌ ERREUR : types incompatibles

    return 0;
}
```

### 3. Pointeur NULL

```c
int (*operation)(int, int) = NULL;

if (operation != NULL) {
    // Toujours vérifier avant d'appeler !
    int resultat = operation(5, 3);
}
```

### 4. Durée de vie des fonctions

Les fonctions ont une durée de vie qui correspond à toute l'exécution du programme. Contrairement aux variables locales, vous n'avez pas à vous soucier qu'une fonction "disparaisse". Un pointeur de fonction reste toujours valide tant que le programme s'exécute.

```c
int (*obtenir_fonction(void))(int, int) {
    // Cette fonction retourne un pointeur de fonction
    return addition;  // ✅ Totalement sûr
}
```

---

## Comparaison avec d'autres langages

Si vous connaissez d'autres langages de programmation, voici comment les callbacks en C se comparent :

| Concept en C | Équivalent Python | Équivalent JavaScript |
|--------------|-------------------|----------------------|
| Pointeur de fonction | Fonction de première classe | Fonction callback |
| `qsort(arr, n, sz, cmp)` | `sorted(list, key=func)` | `array.sort(compareFunc)` |
| Callback | Décorateur / Callable | Fonction callback / Promise |

**Exemple équivalent en Python** :
```python
def calculer(x, y, operation):
    return operation(x, y)

resultat = calculer(10, 5, lambda a, b: a + b)  # Callback anonyme
```

En C, nous ne pouvons pas créer de fonctions anonymes (lambdas) directement, mais les pointeurs de fonctions offrent une flexibilité similaire.

---

## Résumé et bonnes pratiques

### Points clés à retenir

1. **Syntaxe** : `type_retour (*nom_pointeur)(types_parametres)`
2. **Usage** : Les pointeurs de fonctions permettent de passer des fonctions en paramètres
3. **Callbacks** : Très utiles pour créer du code flexible et réutilisable
4. **Typedef** : Simplifie la syntaxe et améliore la lisibilité

### Bonnes pratiques

✅ **À FAIRE** :
- Utiliser `typedef` pour les types de pointeurs de fonctions complexes
- Toujours vérifier qu'un pointeur de fonction n'est pas NULL avant de l'appeler
- Documenter clairement le comportement attendu des callbacks
- Garder les signatures de fonctions simples et cohérentes

❌ **À ÉVITER** :
- Créer des chaînes de pointeurs de fonctions trop complexes
- Oublier les parenthèses dans la déclaration
- Ignorer la gestion des erreurs dans les callbacks
- Abuser des callbacks là où une solution plus simple existe

---

## Aller plus loin

Maintenant que vous maîtrisez les pointeurs de fonctions simples, vous êtes prêt pour des concepts plus avancés :

- **Tableaux de pointeurs de fonctions** (section 22.2) : Pour créer des tables de dispatch
- **Pointeurs de fonctions avec contexte** : Passage de données utilisateur
- **Fonctions variadiques** (section 22.6) : Callbacks avec nombre variable d'arguments
- **Programmation orientée objet en C** : Utilisation de pointeurs de fonctions dans des structures pour simuler des méthodes

### Ressources recommandées

- Man page de `qsort` : `man 3 qsort`
- K&R "The C Programming Language" : Section 5.11
- Documentation GLib : Exemples d'utilisation intensive de callbacks
- Code source de SQLite : Excellent exemple de callbacks pour les requêtes

---

## Conclusion

Les pointeurs de fonctions sont un outil puissant qui vous permet d'écrire du code C flexible et réutilisable. Bien que la syntaxe puisse sembler déroutante au début, avec de la pratique, vous trouverez de nombreuses situations où ils simplifient considérablement votre code.

La clé est de commencer simplement : créez des fonctions de callback simples, passez-les en paramètres, et observez comment cela rend votre code plus modulaire. Au fur et à mesure, vous découvrirez des patterns de plus en plus sophistiqués.

**N'oubliez pas** : Les pointeurs de fonctions ne sont qu'un moyen de stocker et passer des références à des fonctions. Une fois que vous avez compris ce principe fondamental, tout le reste découle naturellement !

---


⏭️ [Tableaux de pointeurs de fonctions](/22-pointeurs-avances/02-tableaux-pointeurs-fonctions.md)
