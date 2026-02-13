🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Chapitre 6 : Les Fonctions

## Introduction au chapitre

Bienvenue dans le chapitre 6, consacré aux **fonctions** en C. Si les chapitres précédents vous ont appris les briques de base du langage (types, opérateurs, structures de contrôle), ce chapitre vous apprend maintenant à **organiser votre code** de manière structurée et réutilisable.

Les fonctions sont au cœur de la programmation en C. Elles permettent de transformer un programme monolithique et difficile à maintenir en un ensemble de composants logiques, testables et réutilisables.

---

## Qu'est-ce qu'une fonction ?

### Définition intuitive

Une **fonction** est un bloc de code autonome qui :
- Porte un **nom**
- Effectue une **tâche spécifique**
- Peut recevoir des **données en entrée** (paramètres)
- Peut retourner un **résultat** (valeur de retour)

### Analogie : La recette de cuisine

Pensez à une fonction comme une **recette de cuisine** :

```
Recette : Faire un gâteau
┌────────────────────────────────┐
│ Ingrédients (paramètres) :     │
│   - 3 œufs                     │
│   - 200g de farine             │
│   - 100g de sucre              │
│                                │
│ Instructions (code) :          │
│   1. Mélanger les œufs         │
│   2. Ajouter la farine         │
│   3. Incorporer le sucre       │
│   4. Cuire 30 minutes          │
│                                │
│ Résultat (retour) :            │
│   → Un gâteau                  │
└────────────────────────────────┘
```

En C, cela ressemblerait à :

```c
Gateau faire_gateau(int oeufs, int farine, int sucre) {
    // Instructions pour faire le gâteau
    return gateau;
}
```

**Avantages** :
- Vous pouvez **réutiliser** la recette autant de fois que nécessaire
- Vous pouvez **adapter** les quantités (paramètres)
- Vous obtenez toujours le même type de résultat (un gâteau)
- La recette est **indépendante** : elle ne pollue pas votre cuisine avec ses ustensiles temporaires

---

## Pourquoi utiliser des fonctions ?

### 1. Réutilisabilité du code

Sans fonction, si vous devez calculer le carré d'un nombre plusieurs fois :

```c
// ❌ Code répétitif
int main(void) {
    int a = 5;
    int carre_a = a * a;
    printf("Carré de %d : %d\n", a, carre_a);

    int b = 7;
    int carre_b = b * b;
    printf("Carré de %d : %d\n", b, carre_b);

    int c = 12;
    int carre_c = c * c;
    printf("Carré de %d : %d\n", c, carre_c);

    return 0;
}
```

Avec une fonction :

```c
// ✅ Code réutilisable
int carre(int n) {
    return n * n;
}

int main(void) {
    printf("Carré de 5 : %d\n", carre(5));
    printf("Carré de 7 : %d\n", carre(7));
    printf("Carré de 12 : %d\n", carre(12));
    return 0;
}
```

**Principe DRY** : *Don't Repeat Yourself* (Ne vous répétez pas)

---

### 2. Lisibilité et maintenabilité

Un programme complexe devient **illisible** sans fonctions :

```c
// ❌ Tout dans main() : difficile à lire
int main(void) {
    // 500 lignes de code enchevêtrées...
    // Calculs
    // Affichages
    // Traitements de fichiers
    // Etc.
}
```

Avec des fonctions bien nommées, le code devient **auto-documenté** :

```c
// ✅ Structure claire et lisible
int main(void) {
    initialiser_systeme();
    charger_configuration();
    traiter_donnees();
    sauvegarder_resultats();
    nettoyer_ressources();
    return 0;
}
```

Rien qu'en lisant `main()`, on comprend **ce que fait** le programme.

---

### 3. Modularité et séparation des responsabilités

Chaque fonction a une **responsabilité unique** :

```c
// Chaque fonction fait UNE chose
double calculer_moyenne(int *tableau, int taille);  
void afficher_resultat(double moyenne);  
int lire_donnees_fichier(const char *fichier, int *tableau);  
```

**Principe de responsabilité unique** : Une fonction = Une tâche

---

### 4. Testabilité

Vous pouvez **tester** chaque fonction individuellement :

```c
int additionner(int a, int b) {
    return a + b;
}

// Test de la fonction
int main(void) {
    assert(additionner(2, 3) == 5);
    assert(additionner(-1, 1) == 0);
    assert(additionner(0, 0) == 0);
    printf("Tous les tests passent !\n");
    return 0;
}
```

---

### 5. Abstraction

Les fonctions **cachent la complexité** :

```c
// L'utilisateur n'a pas besoin de savoir COMMENT on trie
trier_tableau(nombres, taille);

// Il sait juste que son tableau sera trié
```

**Principe d'abstraction** : Exposer "quoi" faire, cacher "comment" le faire.

---

## Anatomie d'une fonction

Une fonction C se compose de plusieurs éléments :

```c
type_retour nom_fonction(type_param1 param1, type_param2 param2) {
    // Corps de la fonction
    // Instructions
    return valeur;
}
```

### Décomposition

```c
int calculer_somme(int a, int b) {
    int resultat = a + b;
    return resultat;
}
```

| Élément | Description | Exemple |
|---------|-------------|---------|
| **Type de retour** | Type de la valeur retournée | `int` |
| **Nom** | Identifiant de la fonction | `calculer_somme` |
| **Paramètres** | Données d'entrée (entre parenthèses) | `int a, int b` |
| **Corps** | Instructions à exécuter (entre accolades) | `{ ... }` |
| **Valeur de retour** | Résultat renvoyé à l'appelant | `return resultat;` |

---

## Les fonctions que vous connaissez déjà

Vous utilisez des fonctions depuis le début sans le savoir explicitement !

### printf() : Afficher du texte

```c
printf("Bonjour monde\n");
```

- **Nom** : `printf`
- **Paramètres** : Une chaîne de format (et éventuellement des valeurs)
- **Retour** : Nombre de caractères affichés (souvent ignoré)
- **Tâche** : Afficher du texte formaté

---

### scanf() : Lire des données

```c
int age;  
scanf("%d", &age);  
```

- **Nom** : `scanf`
- **Paramètres** : Format et adresses des variables
- **Retour** : Nombre de valeurs lues avec succès
- **Tâche** : Lire des données depuis l'entrée standard

---

### main() : Le point d'entrée

```c
int main(void) {
    // Votre code
    return 0;
}
```

**Surprise** : `main` est une fonction ! Elle a :
- Un type de retour (`int`)
- Un nom (`main`)
- Des paramètres (optionnels : `argc`, `argv`)
- Un corps avec des instructions
- Une valeur de retour (`return 0`)

La particularité de `main` : c'est le **point d'entrée** de votre programme, appelée automatiquement par le système d'exploitation.

---

## Premier exemple complet

Créons notre première fonction personnalisée :

```c
#include <stdio.h>

// Déclaration de la fonction (prototype)
int doubler(int nombre);

int main(void) {
    int x = 5;
    int resultat = doubler(x);

    printf("%d doublé = %d\n", x, resultat);

    return 0;
}

// Définition de la fonction (implémentation)
int doubler(int nombre) {
    return nombre * 2;
}
```

**Sortie :**
```
5 doublé = 10
```

**Ce qui se passe :**

1. Le programme démarre dans `main()`
2. `main()` appelle `doubler(5)`
3. L'exécution saute vers `doubler`
4. `doubler` calcule `5 * 2 = 10`
5. `doubler` retourne `10` à `main`
6. `main` stocke le résultat dans `resultat`
7. `main` affiche le résultat

---

## Vocabulaire clé

### Appel de fonction (Function Call)

Lorsque vous **utilisez** une fonction :

```c
int resultat = additionner(3, 5);
//              ^
//              └── Appel de fonction
```

---

### Définition de fonction (Function Definition)

Lorsque vous **écrivez le code** de la fonction :

```c
int additionner(int a, int b) {
    return a + b;
}
// ^
// └── Définition de la fonction
```

---

### Prototype/Déclaration (Function Prototype)

Lorsque vous **annoncez l'existence** de la fonction :

```c
int additionner(int a, int b);
//                             ^
//                             └── Point-virgule : c'est un prototype
```

---

### Paramètres (Parameters)

Les variables dans la **définition** de la fonction :

```c
int additionner(int a, int b) {  // a et b sont les paramètres
    return a + b;
}
```

---

### Arguments (Arguments)

Les valeurs passées lors de l'**appel** :

```c
int resultat = additionner(3, 5);  // 3 et 5 sont les arguments
```

---

### Valeur de retour (Return Value)

La donnée **renvoyée** par la fonction :

```c
int additionner(int a, int b) {
    return a + b;  // a + b est la valeur de retour
}
```

---

## Types de fonctions

### 1. Fonctions avec retour

Retournent une valeur :

```c
int maximum(int a, int b) {
    return (a > b) ? a : b;
}
```

---

### 2. Fonctions void (sans retour)

Ne retournent rien, effectuent simplement une action :

```c
void afficher_message(void) {
    printf("Bonjour !\n");
    // Pas de return (ou return; sans valeur)
}
```

---

### 3. Fonctions sans paramètres

```c
int obtenir_nombre_aleatoire(void) {
    return 42;  // Toujours 42 ! (pas très aléatoire)
}
```

**Note** : `void` entre les parenthèses signifie explicitement "aucun paramètre".

---

### 4. Fonctions avec plusieurs paramètres

```c
double calculer_moyenne(double a, double b, double c) {
    return (a + b + c) / 3.0;
}
```

---

## Le flux d'exécution

Comprendre comment le programme **saute** d'une fonction à l'autre :

```c
#include <stdio.h>

void fonction_c(void) {
    printf("3. Dans fonction_c\n");
}

void fonction_b(void) {
    printf("2. Dans fonction_b\n");
    fonction_c();
    printf("4. Retour dans fonction_b\n");
}

void fonction_a(void) {
    printf("1. Dans fonction_a\n");
    fonction_b();
    printf("5. Retour dans fonction_a\n");
}

int main(void) {
    printf("0. Dans main\n");
    fonction_a();
    printf("6. Retour dans main\n");
    return 0;
}
```

**Sortie :**
```
0. Dans main
1. Dans fonction_a
2. Dans fonction_b
3. Dans fonction_c
4. Retour dans fonction_b
5. Retour dans fonction_a
6. Retour dans main
```

**Visualisation du flux :**

```
main()
  └→ fonction_a()
       └→ fonction_b()
            └→ fonction_c()
            ← retour
       ← retour
  ← retour
```

---

## La bibliothèque standard C

Le C fournit une **bibliothèque standard** riche en fonctions prêtes à l'emploi :

### Entrées/Sorties (<stdio.h>)

```c
printf()   // Affichage formaté  
scanf()    // Lecture formatée  
fopen()    // Ouvrir un fichier  
fclose()   // Fermer un fichier  
fgets()    // Lire une ligne  
```

### Chaînes de caractères (<string.h>)

```c
strlen()   // Longueur d'une chaîne  
strcpy()   // Copier une chaîne  
strcmp()   // Comparer deux chaînes  
strcat()   // Concaténer des chaînes  
```

### Mathématiques (<math.h>)

```c
sqrt()     // Racine carrée  
pow()      // Puissance  
sin()      // Sinus  
cos()      // Cosinus  
fabs()     // Valeur absolue (flottants)  
```

### Utilitaires (<stdlib.h>)

```c
malloc()   // Allocation mémoire dynamique  
free()     // Libération mémoire  
atoi()     // Convertir chaîne → entier  
rand()     // Nombre aléatoire  
abs()      // Valeur absolue (entiers)  
exit()     // Quitter le programme  
```

**Ces fonctions sont disponibles sans que vous ayez besoin de les écrire vous-même !**

---

## Exemple : Programme structuré avec fonctions

Comparons un programme **sans** et **avec** fonctions :

### Sans fonctions (monolithique)

```c
#include <stdio.h>

int main(void) {
    // Tout dans main : 100 lignes de code désorganisé
    int a = 5, b = 10, c = 15;
    int somme = a + b + c;
    double moyenne = somme / 3.0;
    printf("Somme : %d\n", somme);
    printf("Moyenne : %.2f\n", moyenne);

    // ... 90 autres lignes ...

    return 0;
}
```

---

### Avec fonctions (structuré)

```c
#include <stdio.h>

// Prototypes
int calculer_somme(int a, int b, int c);  
double calculer_moyenne(int somme, int nombre);  
void afficher_resultats(int somme, double moyenne);  

int main(void) {
    int a = 5, b = 10, c = 15;

    int somme = calculer_somme(a, b, c);
    double moyenne = calculer_moyenne(somme, 3);
    afficher_resultats(somme, moyenne);

    return 0;
}

int calculer_somme(int a, int b, int c) {
    return a + b + c;
}

double calculer_moyenne(int somme, int nombre) {
    return (double)somme / nombre;
}

void afficher_resultats(int somme, double moyenne) {
    printf("Somme : %d\n", somme);
    printf("Moyenne : %.2f\n", moyenne);
}
```

**Avantages de la version structurée :**
- ✅ `main()` est **lisible** : on voit immédiatement les étapes
- ✅ Chaque fonction est **testable** individuellement
- ✅ Le code est **réutilisable** dans d'autres programmes
- ✅ Les modifications sont **localisées** (changer le calcul de moyenne ne touche qu'une fonction)

---

## Ce que vous allez apprendre dans ce chapitre

Le chapitre 6 est divisé en **5 sections** qui couvrent tous les aspects des fonctions en C :

### 6.1 - Déclaration, définition et prototypes
- Différence entre déclarer et définir une fonction
- Pourquoi et comment utiliser les prototypes
- Organisation du code avec les fichiers d'en-tête

### 6.2 - La Pile : Comprendre les Stack Frames
- Comment la mémoire est organisée lors des appels de fonction
- Le concept de stack frame (cadre de pile)
- Durée de vie des variables locales
- Visualisation de l'exécution en mémoire

### 6.3 - Passage par valeur
- Comment les paramètres sont transmis aux fonctions
- Pourquoi modifier un paramètre ne modifie pas l'original
- Introduction au concept de passage par référence (via pointeurs)

### 6.4 - Valeurs de retour et codes d'erreur
- Comment retourner des résultats
- Les conventions de gestion d'erreur en C
- La variable `errno` et les codes de retour
- Patterns de gestion d'erreur robuste

### 6.5 - La fonction main et ses paramètres
- Les arguments de ligne de commande (`argc`, `argv`)
- Comment créer des programmes CLI interactifs
- Les codes de retour vers le système d'exploitation
- Parsing d'options avec `getopt()`

---

## Règles d'or des fonctions

### 1. Une fonction = Une responsabilité

```c
// ❌ Mauvais : fait trop de choses
void tout_faire(void) {
    lire_donnees();
    calculer();
    afficher();
    sauvegarder();
}

// ✅ Bon : chaque fonction a un rôle clair
void lire_donnees(void);  
void calculer(void);  
void afficher_resultats(void);  
void sauvegarder_resultats(void);  
```

---

### 2. Noms explicites

```c
// ❌ Mauvais : noms vagues
int f(int x);  
void proc(void);  

// ✅ Bon : noms descriptifs
int calculer_factorielle(int n);  
void initialiser_systeme(void);  
```

---

### 3. Fonctions courtes

**Règle empirique** : Une fonction devrait tenir sur un écran (environ 20-50 lignes max).

Si une fonction est trop longue, décomposez-la en plusieurs sous-fonctions.

---

### 4. Minimiser les effets de bord

Une fonction devrait idéalement :
- Ne modifier que ce qu'on attend d'elle
- Ne pas avoir d'effets cachés sur des variables globales

```c
// ✅ Bon : fonction pure (pas d'effet de bord)
int additionner(int a, int b) {
    return a + b;  // Ne modifie rien d'autre
}

// ⚠️ Attention : effet de bord (modifie une variable globale)
int compteur = 0;  
void incrementer_global(void) {  
    compteur++;  // Effet de bord
}
```

---

## Conventions de nommage

En C, les conventions les plus courantes sont :

```c
// snake_case (recommandé en C)
int calculer_moyenne(int a, int b);  
void afficher_resultat(double valeur);  

// Éviter : camelCase (plus courant en Java/C++)
int calculerMoyenne(int a, int b);

// Éviter : PascalCase (pour les types/structures)
int CalculerMoyenne(int a, int b);
```

**Conseil** : Choisissez un style et restez cohérent dans tout votre projet.

---

## Prérequis pour ce chapitre

Avant d'aborder les fonctions en détail, assurez-vous de maîtriser :

- ✅ **Types de données** (int, float, char, etc.)
- ✅ **Variables** et leur portée
- ✅ **Opérateurs** (arithmétiques, logiques, comparaison)
- ✅ **Structures de contrôle** (if, while, for)
- ✅ **Compilation et exécution** d'un programme C

Si ces concepts ne sont pas clairs, revoyez les chapitres précédents avant de continuer.

---

## À retenir avant de commencer

### Points clés

1. ✅ Les fonctions sont des **blocs de code réutilisables**
2. ✅ Elles rendent le code plus **lisible** et **maintenable**
3. ✅ Une fonction a un **nom**, des **paramètres** (optionnels), et une **valeur de retour** (optionnelle)
4. ✅ `main()` est une fonction spéciale : le point d'entrée du programme
5. ✅ Vous utilisez déjà des fonctions : `printf()`, `scanf()`, etc.
6. ✅ Le C fournit une riche **bibliothèque standard** de fonctions

### Ce que vous saurez faire à la fin

À la fin de ce chapitre, vous serez capable de :

- 📝 Écrire vos propres fonctions réutilisables
- 🔍 Organiser un programme complexe en fonctions logiques
- 🧠 Comprendre comment la mémoire est gérée lors des appels
- 🛡️ Gérer les erreurs de manière robuste
- 🖥️ Créer des programmes en ligne de commande interactifs
- 🚀 Structurer vos projets comme un développeur professionnel

---

## Prêt à commencer ?

Maintenant que vous comprenez **pourquoi** les fonctions sont essentielles, plongeons dans les détails techniques !

Dans la **section 6.1**, nous commencerons par comprendre la différence entre **déclaration** et **définition**, et comment utiliser les **prototypes** pour organiser votre code proprement.

**Rendez-vous en section 6.1 !** 🚀

⏭️ [Déclaration, définition et prototypes](/06-fonctions/01-declaration-definition.md)
