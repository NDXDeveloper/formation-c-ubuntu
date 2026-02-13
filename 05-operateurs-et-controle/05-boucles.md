🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5.5 Boucles et contrôle de flux

## Introduction

Les boucles sont des structures de contrôle essentielles qui permettent de **répéter** un bloc de code plusieurs fois. Au lieu d'écrire le même code encore et encore, vous utilisez une boucle pour automatiser la répétition. C'est l'un des concepts les plus puissants de la programmation.

En C, il existe trois types principaux de boucles :
- **`while`** : Répète tant qu'une condition est vraie
- **`do-while`** : Répète au moins une fois, puis tant qu'une condition est vraie
- **`for`** : Répète un nombre déterminé de fois (généralement)

Nous explorerons également les **instructions de contrôle de flux** (`break`, `continue`, `goto`) qui permettent de modifier le comportement des boucles.

---

## La boucle `while`

### Concept

La boucle `while` répète un bloc de code **tant que** la condition est vraie. La condition est évaluée **avant** chaque itération.

**Syntaxe :**
```c
while (condition) {
    // Code à répéter
}
```

**Diagramme de flux :**
```
     ┌─────────────┐
     │  Condition  │
     │   vraie ?   │
     └──────┬──────┘
            │
       OUI  │  NON
            │   └───────> Sortie de la boucle
            ▼
     ┌─────────────┐
     │  Exécution  │
     │  du corps   │
     └──────┬──────┘
            │
            │ (retour à la condition)
            └──────┐
```

### Exemple simple

```c
#include <stdio.h>

int main(void) {
    int compteur = 1;

    // Affiche les nombres de 1 à 5
    while (compteur <= 5) {
        printf("%d ", compteur);
        compteur++;  // Incrémenter le compteur
    }

    printf("\n");
    return 0;
}
```

**Résultat :**
```
1 2 3 4 5
```

### ⚠️ Attention : Boucle infinie

Si la condition reste toujours vraie, la boucle ne s'arrêtera jamais !

```c
#include <stdio.h>

int main(void) {
    int compteur = 1;

    // ❌ ERREUR : Boucle infinie (compteur n'est jamais incrémenté)
    while (compteur <= 5) {
        printf("%d ", compteur);
        // Oubli de compteur++ !
    }

    return 0;
}
```

**💡 Conseil :** Assurez-vous toujours que la condition de la boucle finira par devenir fausse.

### Exemples pratiques

**1. Demander une saisie valide**
```c
#include <stdio.h>

int main(void) {
    int age;

    printf("Entrez votre âge (entre 0 et 120) : ");
    scanf("%d", &age);

    // Redemander tant que l'entrée est invalide
    while (age < 0 || age > 120) {
        printf("Âge invalide. Réessayez : ");
        scanf("%d", &age);
    }

    printf("Votre âge : %d ans\n", age);

    return 0;
}
```

**2. Calculer la somme des nombres de 1 à N**
```c
#include <stdio.h>

int main(void) {
    int n = 10;
    int somme = 0;
    int i = 1;

    while (i <= n) {
        somme += i;  // somme = somme + i
        i++;
    }

    printf("La somme des nombres de 1 à %d est : %d\n", n, somme);

    return 0;
}
```

**3. Afficher les puissances de 2**
```c
#include <stdio.h>

int main(void) {
    int puissance = 1;
    int limite = 1000;

    printf("Puissances de 2 inférieures à %d :\n", limite);

    while (puissance < limite) {
        printf("%d ", puissance);
        puissance *= 2;  // puissance = puissance * 2
    }

    printf("\n");
    return 0;
}
```

---

## La boucle `do-while`

### Concept

La boucle `do-while` est similaire à `while`, mais avec une différence importante : **le corps de la boucle est exécuté au moins une fois**, car la condition est testée **après** l'exécution.

**Syntaxe :**
```c
do {
    // Code à répéter (exécuté au moins une fois)
} while (condition);
```

**⚠️ Attention :** Notez le point-virgule `;` après la parenthèse fermante du `while` !

**Diagramme de flux :**
```
     ┌─────────────┐
     │  Exécution  │
     │  du corps   │
     └──────┬──────┘
            │
            ▼
     ┌─────────────┐
     │  Condition  │
     │   vraie ?   │
     └──────┬──────┘
            │
       OUI  │  NON
            │   └───────> Sortie de la boucle
            │
            └──────┐ (retour au corps)
```

### Exemple simple

```c
#include <stdio.h>

int main(void) {
    int compteur = 1;

    do {
        printf("%d ", compteur);
        compteur++;
    } while (compteur <= 5);

    printf("\n");
    return 0;
}
```

### Différence entre `while` et `do-while`

```c
#include <stdio.h>

int main(void) {
    int a = 10;

    // Boucle while : Ne s'exécute jamais (condition fausse dès le début)
    printf("Boucle while :\n");
    while (a < 5) {
        printf("Cette ligne ne s'affichera pas\n");
        a++;
    }

    // Boucle do-while : S'exécute au moins une fois
    printf("Boucle do-while :\n");
    do {
        printf("Cette ligne s'affiche une fois\n");
        a++;
    } while (a < 5);

    return 0;
}
```

**Résultat :**
```
Boucle while :  
Boucle do-while :  
Cette ligne s'affiche une fois  
```

### Cas d'usage typique : Menu interactif

```c
#include <stdio.h>

int main(void) {
    int choix;

    do {
        printf("\n=== MENU ===\n");
        printf("1. Option A\n");
        printf("2. Option B\n");
        printf("3. Option C\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("Option A sélectionnée\n");
                break;
            case 2:
                printf("Option B sélectionnée\n");
                break;
            case 3:
                printf("Option C sélectionnée\n");
                break;
            case 0:
                printf("Au revoir !\n");
                break;
            default:
                printf("Choix invalide\n");
                break;
        }
    } while (choix != 0);  // Répéter tant que l'utilisateur ne choisit pas 0

    return 0;
}
```

---

## La boucle `for`

### Concept

La boucle `for` est généralement utilisée quand on connaît **à l'avance** le nombre d'itérations. Elle combine l'initialisation, la condition et l'incrémentation en une seule ligne.

**Syntaxe :**
```c
for (initialisation; condition; incrémentation) {
    // Code à répéter
}
```

**Décomposition :**
1. **Initialisation** : Exécutée une seule fois au début
2. **Condition** : Testée avant chaque itération
3. **Incrémentation** : Exécutée après chaque itération
4. **Corps** : Le code à répéter

**Équivalence avec `while` :**
```c
// Boucle for
for (int i = 0; i < 5; i++) {
    printf("%d ", i);
}

// Équivalent en while
int i = 0;           // Initialisation  
while (i < 5) {      // Condition  
    printf("%d ", i);
    i++;             // Incrémentation
}
```

### Exemple simple

```c
#include <stdio.h>

int main(void) {
    // Afficher les nombres de 0 à 4
    for (int i = 0; i < 5; i++) {
        printf("%d ", i);
    }

    printf("\n");
    return 0;
}
```

**Résultat :**
```
0 1 2 3 4
```

### Variations de la boucle `for`

**1. Incrémenter de 2 en 2**
```c
#include <stdio.h>

int main(void) {
    printf("Nombres pairs de 0 à 10 :\n");
    for (int i = 0; i <= 10; i += 2) {
        printf("%d ", i);
    }

    printf("\n");
    return 0;
}
```

**2. Décrémenter (compter à rebours)**
```c
#include <stdio.h>

int main(void) {
    printf("Compte à rebours :\n");
    for (int i = 10; i >= 0; i--) {
        printf("%d ", i);
    }
    printf("Décollage !\n");

    return 0;
}
```

**3. Plusieurs variables**
```c
#include <stdio.h>

int main(void) {
    // Initialiser et manipuler plusieurs variables
    for (int i = 0, j = 10; i < j; i++, j--) {
        printf("i=%d, j=%d\n", i, j);
    }

    return 0;
}
```

**4. Boucle for "infinie"**
```c
#include <stdio.h>

int main(void) {
    int compteur = 0;

    // Boucle for sans condition (équivalent à while(1))
    for (;;) {
        printf("%d ", compteur);
        compteur++;

        if (compteur >= 5) {
            break;  // Sortir de la boucle
        }
    }

    printf("\n");
    return 0;
}
```

### Exemples pratiques

**1. Calculer la factorielle**
```c
#include <stdio.h>

int main(void) {
    int n = 5;
    unsigned long long factorielle = 1;

    for (int i = 1; i <= n; i++) {
        factorielle *= i;
    }

    printf("%d! = %llu\n", n, factorielle);

    return 0;
}
```

**2. Parcourir un tableau**
```c
#include <stdio.h>

int main(void) {
    int nombres[] = {10, 20, 30, 40, 50};
    int taille = sizeof(nombres) / sizeof(nombres[0]);

    printf("Éléments du tableau :\n");
    for (int i = 0; i < taille; i++) {
        printf("nombres[%d] = %d\n", i, nombres[i]);
    }

    return 0;
}
```

**3. Afficher une table de multiplication**
```c
#include <stdio.h>

int main(void) {
    int nombre = 7;

    printf("Table de multiplication de %d :\n", nombre);
    for (int i = 1; i <= 10; i++) {
        printf("%d x %d = %d\n", nombre, i, nombre * i);
    }

    return 0;
}
```

---

## Choisir la bonne boucle

| Boucle | Quand l'utiliser | Exemple typique |
|--------|------------------|-----------------|
| `for` | Nombre d'itérations connu à l'avance | Parcourir un tableau, compter de 1 à N |
| `while` | Nombre d'itérations inconnu, condition testée avant | Lire un fichier jusqu'à la fin |
| `do-while` | Au moins une itération nécessaire, condition testée après | Menu interactif, validation d'entrée |

```c
// ✅ BON : for pour nombre connu d'itérations
for (int i = 0; i < 10; i++) {
    printf("%d ", i);
}

// ✅ BON : while pour condition inconnue à l'avance
while (scanf("%d", &nombre) == 1) {
    // Traiter le nombre
}

// ✅ BON : do-while pour exécuter au moins une fois
do {
    afficher_menu();
    scanf("%d", &choix);
} while (choix != 0);
```

---

## Instructions de contrôle de flux

### 1. L'instruction `break`

`break` permet de **sortir immédiatement** de la boucle en cours, quel que soit l'état de la condition.

**Syntaxe :**
```c
break;
```

**Exemple : Sortir d'une boucle prématurément**
```c
#include <stdio.h>

int main(void) {
    // Chercher le premier nombre négatif
    int nombres[] = {5, 10, 15, -3, 20, 25};
    int taille = sizeof(nombres) / sizeof(nombres[0]);

    for (int i = 0; i < taille; i++) {
        if (nombres[i] < 0) {
            printf("Premier nombre négatif trouvé : %d à l'index %d\n", nombres[i], i);
            break;  // Sortir de la boucle
        }
    }

    return 0;
}
```

**Exemple : Limiter le nombre d'essais**
```c
#include <stdio.h>

int main(void) {
    int secret = 42;
    int tentative;
    int max_essais = 3;

    for (int essai = 1; essai <= max_essais; essai++) {
        printf("Essai %d/%d - Devinez le nombre : ", essai, max_essais);
        scanf("%d", &tentative);

        if (tentative == secret) {
            printf("Bravo ! Vous avez trouvé !\n");
            break;  // Sortir si le nombre est trouvé
        } else {
            printf("Raté !\n");
        }
    }

    return 0;
}
```

**⚠️ Important :** `break` ne sort que de la boucle **la plus proche**. Si vous avez des boucles imbriquées, `break` ne sortira que de la boucle intérieure.

---

### 2. L'instruction `continue`

`continue` permet de **sauter l'itération actuelle** et de passer directement à la suivante.

**Syntaxe :**
```c
continue;
```

**Différence entre `break` et `continue` :**
- `break` : Sort complètement de la boucle
- `continue` : Passe à l'itération suivante

**Exemple : Sauter les nombres pairs**
```c
#include <stdio.h>

int main(void) {
    printf("Nombres impairs de 1 à 10 :\n");

    for (int i = 1; i <= 10; i++) {
        if (i % 2 == 0) {
            continue;  // Sauter les nombres pairs
        }
        printf("%d ", i);
    }

    printf("\n");
    return 0;
}
```

**Résultat :**
```
Nombres impairs de 1 à 10 :
1 3 5 7 9
```

**Exemple : Traiter seulement certains éléments**
```c
#include <stdio.h>

int main(void) {
    int notes[] = {15, -1, 18, -1, 12, 16, -1, 14};
    int taille = sizeof(notes) / sizeof(notes[0]);
    int somme = 0;
    int compteur = 0;

    printf("Notes valides :\n");
    for (int i = 0; i < taille; i++) {
        // Ignorer les valeurs invalides (-1)
        if (notes[i] == -1) {
            continue;
        }

        printf("%d ", notes[i]);
        somme += notes[i];
        compteur++;
    }

    if (compteur > 0) {
        printf("\nMoyenne : %.2f\n", (float)somme / compteur);
    }

    return 0;
}
```

---

### 3. L'instruction `goto` (à éviter)

`goto` permet de sauter à un label (étiquette) spécifique dans le code.

**Syntaxe :**
```c
goto label;

// ...

label:
    // Code à exécuter
```

**⚠️ Attention :** L'utilisation de `goto` est **fortement déconseillée** car elle rend le code difficile à lire et à maintenir. C'est ce qu'on appelle le "spaghetti code".

**Cas où `goto` est acceptable (rare) :**

**1. Sortir de boucles imbriquées profondes**
```c
#include <stdio.h>

int main(void) {
    int trouve = 0;

    // Chercher un élément dans une matrice
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                if (/* condition trouvée */) {
                    trouve = 1;
                    goto sortie;  // Sortir de toutes les boucles
                }
            }
        }
    }

sortie:
    if (trouve) {
        printf("Élément trouvé\n");
    }

    return 0;
}
```

**💡 Alternative recommandée :** Utiliser une fonction avec `return` est généralement préférable.

```c
#include <stdio.h>
#include <stdbool.h>

bool chercher_element(int matrice[10][10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                if (/* condition trouvée */) {
                    return true;  // Plus propre que goto
                }
            }
        }
    }
    return false;
}

int main(void) {
    int matrice[10][10][10] = {0};

    if (chercher_element(matrice)) {
        printf("Élément trouvé\n");
    }

    return 0;
}
```

**2. Gestion d'erreurs et nettoyage (pattern Linux kernel)**
```c
#include <stdio.h>
#include <stdlib.h>

int fonction_complexe(void) {
    int *buffer1 = NULL;
    int *buffer2 = NULL;
    int *buffer3 = NULL;
    int resultat = -1;

    buffer1 = malloc(100);
    if (!buffer1) {
        goto cleanup;
    }

    buffer2 = malloc(200);
    if (!buffer2) {
        goto cleanup;
    }

    buffer3 = malloc(300);
    if (!buffer3) {
        goto cleanup;
    }

    // Traitement...
    resultat = 0;  // Succès

cleanup:
    free(buffer3);
    free(buffer2);
    free(buffer1);

    return resultat;
}

int main(void) {
    if (fonction_complexe() == 0) {
        printf("Succès\n");
    } else {
        printf("Erreur\n");
    }

    return 0;
}
```

**💡 Règle générale :** Évitez `goto` sauf si vous avez une très bonne raison et que les alternatives sont vraiment pires.

---

## Boucles imbriquées

### Concept

Une boucle imbriquée est une boucle **à l'intérieur** d'une autre boucle. La boucle intérieure s'exécute complètement pour chaque itération de la boucle extérieure.

**Structure :**
```c
for (int i = 0; i < n; i++) {        // Boucle extérieure
    for (int j = 0; j < m; j++) {    // Boucle intérieure
        // Code exécuté n × m fois
    }
}
```

### Exemple : Table de multiplication complète

```c
#include <stdio.h>

int main(void) {
    printf("Table de multiplication de 1 à 5 :\n\n");
    printf("    ");

    // Afficher l'en-tête
    for (int j = 1; j <= 5; j++) {
        printf("%4d", j);
    }
    printf("\n");
    printf("   ──────────────────────\n");

    // Afficher la table
    for (int i = 1; i <= 5; i++) {
        printf("%2d |", i);
        for (int j = 1; j <= 5; j++) {
            printf("%4d", i * j);
        }
        printf("\n");
    }

    return 0;
}
```

**Résultat :**
```
Table de multiplication de 1 à 5 :

       1   2   3   4   5
   ──────────────────────
 1 |   1   2   3   4   5
 2 |   2   4   6   8  10
 3 |   3   6   9  12  15
 4 |   4   8  12  16  20
 5 |   5  10  15  20  25
```

### Exemple : Parcourir une matrice

```c
#include <stdio.h>

int main(void) {
    int matrice[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    int lignes = 3;
    int colonnes = 4;

    printf("Contenu de la matrice :\n");
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%3d ", matrice[i][j]);
        }
        printf("\n");
    }

    return 0;
}
```

### Exemple : Dessiner des motifs

**1. Triangle d'étoiles**
```c
#include <stdio.h>

int main(void) {
    int hauteur = 5;

    for (int i = 1; i <= hauteur; i++) {
        for (int j = 1; j <= i; j++) {
            printf("* ");
        }
        printf("\n");
    }

    return 0;
}
```

**Résultat :**
```
*
* *
* * *
* * * *
* * * * *
```

**2. Rectangle d'étoiles**
```c
#include <stdio.h>

int main(void) {
    int largeur = 7;
    int hauteur = 4;

    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            printf("* ");
        }
        printf("\n");
    }

    return 0;
}
```

**3. Pyramide centrée**
```c
#include <stdio.h>

int main(void) {
    int hauteur = 5;

    for (int i = 1; i <= hauteur; i++) {
        // Espaces avant les étoiles
        for (int j = 1; j <= hauteur - i; j++) {
            printf(" ");
        }
        // Étoiles
        for (int k = 1; k <= 2 * i - 1; k++) {
            printf("*");
        }
        printf("\n");
    }

    return 0;
}
```

**Résultat :**
```
    *
   ***
  *****
 *******
*********
```

### Contrôle des boucles imbriquées

**Utiliser `break` dans des boucles imbriquées**

`break` ne sort que de la boucle la plus proche :

```c
#include <stdio.h>

int main(void) {
    for (int i = 0; i < 3; i++) {
        printf("Boucle externe : i = %d\n", i);

        for (int j = 0; j < 5; j++) {
            if (j == 3) {
                printf("  Break dans la boucle interne à j = %d\n", j);
                break;  // Sort seulement de la boucle intérieure
            }
            printf("  Boucle interne : j = %d\n", j);
        }
    }

    return 0;
}
```

**Sortir de toutes les boucles avec un flag**

```c
#include <stdio.h>
#include <stdbool.h>

int main(void) {
    bool trouve = false;

    for (int i = 0; i < 5 && !trouve; i++) {
        for (int j = 0; j < 5; j++) {
            if (i * j == 12) {
                printf("Trouvé : %d × %d = 12\n", i, j);
                trouve = true;
                break;  // Sort de la boucle intérieure
            }
        }
    }

    return 0;
}
```

---

## Optimisation des boucles

### 1. Déplacer les calculs invariants hors de la boucle

```c
#include <stdio.h>

int calculer_limite(int n) {
    // Simulation d'un calcul coûteux
    return n * n;
}

int main(void) {
    int n = 10;

    // ❌ NON OPTIMISÉ : calculer_limite() appelé à chaque itération
    for (int i = 0; i < calculer_limite(n); i++) {
        printf("%d ", i);
    }

    printf("\n");

    // ✅ OPTIMISÉ : Résultat calculé une seule fois
    int limite = calculer_limite(n);
    for (int i = 0; i < limite; i++) {
        printf("%d ", i);
    }

    printf("\n");
    return 0;
}
```

### 2. Minimiser le travail dans la boucle

```c
#include <stdio.h>
#include <string.h>

// ❌ NON OPTIMISÉ : strlen() est recalculé à chaque itération
void traiter_v1(const char *chaine) {
    for (int i = 0; i < (int)strlen(chaine); i++) {
        printf("%c", chaine[i]);
    }
}

// ✅ OPTIMISÉ : Le calcul invariant est sorti de la boucle
void traiter_v2(const char *chaine) {
    int longueur = (int)strlen(chaine);
    for (int i = 0; i < longueur; i++) {
        printf("%c", chaine[i]);
    }
}
```

**⚠️ Note :** Les compilateurs modernes optimisent déjà très bien. Ne faites des micro-optimisations que si le profiling montre un vrai problème. En particulier, ne séparez pas une boucle unique en plusieurs boucles : cela nuit à la localité de cache (plusieurs passes sur les mêmes données).

### 3. Dérouler les boucles (loop unrolling) - Avancé

Pour les petites boucles critiques en performance :

```c
#include <stdio.h>

// ❌ BOUCLE NORMALE
void copier_v1(int *dest, int *src, int taille) {
    for (int i = 0; i < taille; i++) {
        dest[i] = src[i];
    }
}

// ✅ BOUCLE DÉROULÉE (pour taille multiple de 4)
void copier_v2(int *dest, int *src, int taille) {
    int i;
    for (i = 0; i < taille - 3; i += 4) {
        dest[i] = src[i];
        dest[i+1] = src[i+1];
        dest[i+2] = src[i+2];
        dest[i+3] = src[i+3];
    }
    // Copier les éléments restants
    for (; i < taille; i++) {
        dest[i] = src[i];
    }
}
```

**💡 Conseil :** Laissez généralement le compilateur faire ces optimisations avec les flags `-O2` ou `-O3`.

---

## Erreurs courantes à éviter

### 1. Modification du compteur dans le corps de la boucle

```c
#include <stdio.h>

int main(void) {
    // ❌ DANGEREUX : Modification de i dans le corps
    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
        if (i == 5) {
            i = 8;  // Peut causer des comportements inattendus
        }
    }

    printf("\n");
    return 0;
}
```

**💡 Conseil :** Évitez de modifier la variable de boucle dans le corps. Utilisez `break` ou `continue` à la place.

### 2. Erreur off-by-one (décalage d'un)

```c
#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};

    // ❌ ERREUR : Accès hors limites (i <= 5 au lieu de i < 5)
    for (int i = 0; i <= 5; i++) {  // BUG !
        printf("%d ", tableau[i]);  // Accès à tableau[5] qui n'existe pas
    }

    // ✅ CORRECT
    for (int i = 0; i < 5; i++) {
        printf("%d ", tableau[i]);
    }

    return 0;
}
```

### 3. Oublier d'incrémenter dans une boucle while

```c
#include <stdio.h>

int main(void) {
    int i = 0;

    // ❌ BOUCLE INFINIE : Oubli de i++
    /*
    while (i < 5) {
        printf("%d ", i);
        // Oubli de i++ !
    }
    */

    // ✅ CORRECT
    while (i < 5) {
        printf("%d ", i);
        i++;
    }

    printf("\n");
    return 0;
}
```

### 4. Utiliser le mauvais type de boucle

```c
// ❌ MALADROIT : while pour un nombre connu d'itérations
int i = 0;  
while (i < 10) {  
    printf("%d ", i);
    i++;
}

// ✅ MEILLEUR : for est plus approprié
for (int i = 0; i < 10; i++) {
    printf("%d ", i);
}
```

### 5. Portée de la variable de boucle

```c
#include <stdio.h>

int main(void) {
    // ❌ i n'est plus accessible après la boucle (C99+)
    for (int i = 0; i < 5; i++) {
        printf("%d ", i);
    }
    // printf("%d", i);  // ERREUR : i n'existe plus ici

    // ✅ Si vous devez utiliser i après la boucle
    int j;
    for (j = 0; j < 5; j++) {
        printf("%d ", j);
    }
    printf("\nDernière valeur : %d\n", j);  // OK, j existe encore

    return 0;
}
```

---

## Bonnes pratiques

### 1. Nommer clairement les variables de boucle

```c
// ❌ PEU CLAIR
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        // Que représentent i et j ?
    }
}

// ✅ CLAIR
for (int ligne = 0; ligne < nb_lignes; ligne++) {
    for (int colonne = 0; colonne < nb_colonnes; colonne++) {
        printf("Cellule [%d][%d]\n", ligne, colonne);
    }
}
```

### 2. Limiter la complexité des boucles imbriquées

```c
// ❌ TROP COMPLEXE : 4 niveaux d'imbrication
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        for (int k = 0; k < p; k++) {
            for (int l = 0; l < q; l++) {
                // Très difficile à comprendre et maintenir
            }
        }
    }
}

// ✅ MEILLEUR : Extraire dans des fonctions
void traiter_niveau3(int k, int p, int q) {
    for (int l = 0; l < q; l++) {
        // Traitement
    }
}

void traiter_niveau2(int j, int m, int p, int q) {
    for (int k = 0; k < p; k++) {
        traiter_niveau3(k, p, q);
    }
}

// Puis dans main :
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        traiter_niveau2(j, m, p, q);
    }
}
```

### 3. Utiliser des constantes pour les limites

```c
#include <stdio.h>

#define TAILLE_MAX 100

int main(void) {
    int tableau[TAILLE_MAX];

    // ✅ BON : Utiliser la constante
    for (int i = 0; i < TAILLE_MAX; i++) {
        tableau[i] = 0;
    }

    return 0;
}
```

### 4. Commenter les boucles complexes

```c
#include <stdio.h>

int main(void) {
    int matrice[10][10];

    // Initialiser seulement la diagonale principale à 1
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i == j) {
                matrice[i][j] = 1;  // Diagonale
            } else {
                matrice[i][j] = 0;  // Reste
            }
        }
    }

    return 0;
}
```

---

## Exemples pratiques récapitulatifs

### 1. Recherche linéaire dans un tableau

```c
#include <stdio.h>
#include <stdbool.h>

int rechercher(int *tableau, int taille, int valeur) {
    for (int i = 0; i < taille; i++) {
        if (tableau[i] == valeur) {
            return i;  // Retourner l'index si trouvé
        }
    }
    return -1;  // Retourner -1 si non trouvé
}

int main(void) {
    int nombres[] = {10, 25, 30, 47, 52, 61, 78, 89};
    int taille = sizeof(nombres) / sizeof(nombres[0]);
    int cherche = 47;

    int index = rechercher(nombres, taille, cherche);

    if (index != -1) {
        printf("%d trouvé à l'index %d\n", cherche, index);
    } else {
        printf("%d non trouvé\n", cherche);
    }

    return 0;
}
```

### 2. Tri à bulles (Bubble Sort)

```c
#include <stdio.h>

void tri_bulle(int *tableau, int taille) {
    // Boucle externe : nombre de passages
    for (int i = 0; i < taille - 1; i++) {
        // Boucle interne : comparaisons
        for (int j = 0; j < taille - i - 1; j++) {
            // Échanger si l'élément actuel est plus grand que le suivant
            if (tableau[j] > tableau[j + 1]) {
                int temp = tableau[j];
                tableau[j] = tableau[j + 1];
                tableau[j + 1] = temp;
            }
        }
    }
}

void afficher_tableau(int *tableau, int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");
}

int main(void) {
    int nombres[] = {64, 34, 25, 12, 22, 11, 90};
    int taille = sizeof(nombres) / sizeof(nombres[0]);

    printf("Avant tri : ");
    afficher_tableau(nombres, taille);

    tri_bulle(nombres, taille);

    printf("Après tri : ");
    afficher_tableau(nombres, taille);

    return 0;
}
```

### 3. Calculer les nombres premiers jusqu'à N

```c
#include <stdio.h>
#include <stdbool.h>

bool est_premier(int n) {
    if (n <= 1) {
        return false;
    }

    // Vérifier la divisibilité jusqu'à la racine carrée de n
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

int main(void) {
    int limite = 50;

    printf("Nombres premiers jusqu'à %d :\n", limite);

    for (int i = 2; i <= limite; i++) {
        if (est_premier(i)) {
            printf("%d ", i);
        }
    }

    printf("\n");
    return 0;
}
```

### 4. Simulation d'un jeu de devinette

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    int secret = rand() % 100 + 1;  // Nombre entre 1 et 100
    int tentative;
    int nb_essais = 0;
    int max_essais = 7;

    printf("=== JEU DE DEVINETTE ===\n");
    printf("Devinez le nombre entre 1 et 100.\n");
    printf("Vous avez %d essais.\n\n", max_essais);

    while (nb_essais < max_essais) {
        nb_essais++;
        printf("Essai %d/%d : ", nb_essais, max_essais);
        scanf("%d", &tentative);

        if (tentative == secret) {
            printf("🎉 Bravo ! Vous avez trouvé en %d essais !\n", nb_essais);
            break;
        } else if (tentative < secret) {
            printf("↑ Trop petit !\n");
        } else {
            printf("↓ Trop grand !\n");
        }

        if (nb_essais == max_essais) {
            printf("😞 Perdu ! Le nombre était %d.\n", secret);
        }
    }

    return 0;
}
```

### 5. Affichage d'un calendrier mensuel simplifié

```c
#include <stdio.h>

void afficher_calendrier(int jours_dans_mois, int jour_debut) {
    printf("Lun Mar Mer Jeu Ven Sam Dim\n");
    printf("────────────────────────────\n");

    // Afficher des espaces pour les jours avant le début du mois
    for (int i = 0; i < jour_debut; i++) {
        printf("    ");
    }

    // Afficher les jours du mois
    for (int jour = 1; jour <= jours_dans_mois; jour++) {
        printf("%3d ", jour);

        // Nouvelle ligne après dimanche
        if ((jour + jour_debut) % 7 == 0) {
            printf("\n");
        }
    }

    printf("\n");
}

int main(void) {
    printf("Février 2024 (année bissextile)\n\n");
    afficher_calendrier(29, 3);  // Février commence un jeudi (index 3)

    return 0;
}
```

---

## Résumé

### Types de boucles

| Boucle | Syntaxe | Quand l'utiliser |
|--------|---------|------------------|
| `while` | `while (condition) { ... }` | Nombre d'itérations inconnu, condition testée avant |
| `do-while` | `do { ... } while (condition);` | Au moins une itération nécessaire |
| `for` | `for (init; cond; incr) { ... }` | Nombre d'itérations connu à l'avance |

### Instructions de contrôle

| Instruction | Effet |
|-------------|-------|
| `break` | Sort immédiatement de la boucle |
| `continue` | Passe à l'itération suivante |
| `goto` | Saute à un label (à éviter sauf cas rares) |

### Points clés à retenir

- ✅ Toujours s'assurer que la condition de boucle finira par devenir fausse
- ✅ Utiliser le bon type de boucle pour chaque situation
- ✅ Attention aux boucles infinies et aux erreurs off-by-one
- ✅ Dans les boucles imbriquées, `break` ne sort que de la boucle la plus proche
- ✅ Privilégier les noms de variables clairs
- ✅ Extraire les boucles complexes dans des fonctions
- ✅ Commenter les boucles non triviales

### Erreurs à éviter

- ❌ Modifier la variable de boucle dans le corps
- ❌ Oublier d'incrémenter dans une boucle `while`
- ❌ Accès hors limites (off-by-one)
- ❌ Point-virgule après le `for` ou `while`
- ❌ Boucles imbriquées trop profondes sans modularisation
- ❌ Abuser de `goto`

---

## Pour aller plus loin

Dans les prochaines sections, vous découvrirez :
- **5.6 Bonnes pratiques de formatage** : Conventions de style pour le code
- **5.7 Formatage automatique avec clang-format** : Outils d'automatisation
- **Module 6** : Programmation système avec des boucles d'événements avancées

Les boucles sont l'un des concepts les plus fondamentaux en programmation. Maîtriser leur utilisation vous permettra de résoudre une grande variété de problèmes et d'écrire des programmes efficaces !

⏭️ [Bonnes pratiques de formatage](/05-operateurs-et-controle/06-bonnes-pratiques.md)
