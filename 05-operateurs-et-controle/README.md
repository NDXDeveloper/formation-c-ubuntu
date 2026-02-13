🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5. Opérateurs et Structures de contrôle

## Introduction

Félicitations d'avoir atteint ce chapitre ! Vous maîtrisez maintenant les bases du C : les variables, les types de données, les fonctions d'entrée/sortie. Mais jusqu'à présent, vos programmes se contentaient d'exécuter des instructions de manière séquentielle, de haut en bas, sans possibilité de faire des calculs complexes, de prendre des décisions ou de répéter des actions.

**Dans ce chapitre, tout change.** Vous allez apprendre à :
- 🧮 **Effectuer des calculs** avec les opérateurs arithmétiques
- 🤔 **Prendre des décisions** avec les conditions (if, else, switch)
- 🔁 **Répéter des actions** avec les boucles (while, for, do-while)
- 🎯 **Manipuler les données** au niveau des bits
- 📝 **Écrire du code propre** avec de bonnes pratiques de formatage

Ce sont les **briques fondamentales** qui transforment un simple script en un véritable programme capable de résoudre des problèmes complexes.

---

## Qu'est-ce qu'un opérateur ?

Un **opérateur** est un symbole qui indique au compilateur d'effectuer une opération spécifique sur une ou plusieurs valeurs (appelées **opérandes**).

### Exemple simple

```c
int resultat = 5 + 3;
```

Dans cette expression :
- `+` est l'**opérateur** (addition)
- `5` et `3` sont les **opérandes**
- `resultat` reçoit la **valeur** calculée (8)

### Catégories d'opérateurs

Le C propose plusieurs catégories d'opérateurs :

| Catégorie | Exemples | Usage |
|-----------|----------|-------|
| **Arithmétiques** | `+`, `-`, `*`, `/`, `%` | Calculs mathématiques |
| **Logiques** | `&&`, `||`, `!` | Conditions et tests |
| **Relationnels** | `==`, `!=`, `<`, `>`, `<=`, `>=` | Comparaisons |
| **Bit-à-bit** | `&`, `|`, `^`, `~`, `<<`, `>>` | Manipulation des bits |
| **Affectation** | `=`, `+=`, `-=`, `*=`, `/=` | Attribution de valeurs |
| **Incrémentation** | `++`, `--` | Augmenter/diminuer de 1 |

**Chaque catégorie a son rôle et ses cas d'usage.** Ce chapitre les explore en détail.

---

## Qu'est-ce qu'une structure de contrôle ?

Une **structure de contrôle** est une instruction qui permet de modifier le **flux d'exécution** d'un programme. Au lieu d'exécuter les instructions ligne par ligne, vous pouvez :
- **Choisir** quelles instructions exécuter (conditions)
- **Répéter** des instructions plusieurs fois (boucles)
- **Sauter** à différentes parties du code

### Le flux séquentiel (sans structure de contrôle)

**Programme simple sans contrôle :**
```c
#include <stdio.h>

int main(void)
{
    printf("Ligne 1\n");
    printf("Ligne 2\n");
    printf("Ligne 3\n");
    return 0;
}
```

**Exécution :** Ligne 1 → Ligne 2 → Ligne 3 → Fin

Toutes les instructions sont exécutées, dans l'ordre, une seule fois.

### Avec des structures de contrôle

**Programme avec condition :**
```c
#include <stdio.h>

int main(void)
{
    int age = 25;

    printf("Début du programme\n");

    if (age >= 18) {
        printf("Vous êtes majeur\n");
    } else {
        printf("Vous êtes mineur\n");
    }

    printf("Fin du programme\n");
    return 0;
}
```

**Exécution :** Le programme **décide** quelle ligne afficher selon la valeur de `age`.

**Programme avec boucle :**
```c
#include <stdio.h>

int main(void)
{
    for (int i = 1; i <= 3; i++) {
        printf("Répétition numéro %d\n", i);
    }
    return 0;
}
```

**Exécution :** La ligne `printf` est **répétée** 3 fois automatiquement.

---

## Pourquoi ce chapitre est crucial

### 1. Créer des programmes interactifs

Sans structures de contrôle, impossible de créer des programmes qui :
- Réagissent aux entrées de l'utilisateur
- S'adaptent aux différentes situations
- Prennent des décisions intelligentes

**Exemple :** Une calculatrice qui change de comportement selon l'opération choisie.

### 2. Résoudre des problèmes réels

La plupart des problèmes nécessitent :
- **Calculs** (opérateurs arithmétiques)
- **Décisions** (if, else, switch)
- **Répétitions** (boucles)

**Exemple :** Calculer la moyenne d'un ensemble de notes.

```c
// Sans boucle : impossible si on a 100 notes
int note1 = 15, note2 = 18, note3 = 12;  
float moyenne = (note1 + note2 + note3) / 3.0;  

// Avec boucle : possible pour n'importe quel nombre de notes
float somme = 0;  
for (int i = 0; i < nb_notes; i++) {  
    somme += notes[i];
}
float moyenne = somme / nb_notes;
```

### 3. Optimiser et améliorer les performances

Comprendre les opérateurs et structures de contrôle permet d'écrire du code :
- Plus rapide (choix des bons opérateurs)
- Plus élégant (conditions bien structurées)
- Plus maintenable (code lisible)

### 4. Base pour les concepts avancés

Tout ce que vous apprendrez ensuite repose sur ce chapitre :
- Les tableaux nécessitent des boucles
- Les pointeurs utilisent des opérateurs spéciaux
- Les structures de données utilisent des conditions
- Les algorithmes combinent tout cela

**Sans maîtriser ce chapitre, impossible de progresser !**

---

## Ce que vous allez apprendre

Ce chapitre est organisé en 7 sections progressives :

### 5.1 Opérateurs arithmétiques
Les bases des calculs en C.
- Addition, soustraction, multiplication, division
- Modulo (reste de division)
- Incrémentation et décrémentation
- Priorité des opérateurs
- Erreurs courantes (division par zéro, dépassement)

**Pourquoi :** Tout programme fait des calculs. C'est la base absolue.

### 5.2 Opérateurs logiques
Combiner des conditions et prendre des décisions complexes.
- ET logique (`&&`), OU logique (`||`), NON logique (`!`)
- Tables de vérité
- Court-circuit d'évaluation
- Différence avec les opérateurs bit-à-bit

**Pourquoi :** Permet de créer des conditions sophistiquées pour contrôler le programme.

### 5.3 Opérateurs bit-à-bit
Manipuler les données au niveau le plus bas : les bits.
- Opérations sur les bits individuels
- Masquage et décalage
- Applications pratiques (flags, permissions, optimisations)

**Pourquoi :** Essentiel pour la programmation système, l'embarqué et les optimisations.

### 5.4 Conditions et optimisation
Faire des choix dans vos programmes.
- `if`, `else if`, `else`
- Opérateur ternaire (`? :`)
- `switch` et `case`
- Optimisation des conditions

**Pourquoi :** Permet au programme de s'adapter et de réagir aux différentes situations.

### 5.5 Boucles et contrôle de flux
Répéter des actions et contrôler l'exécution.
- `while`, `do-while`, `for`
- `break` et `continue`
- Boucles imbriquées
- Choix de la bonne boucle

**Pourquoi :** Automatise les tâches répétitives et traite des ensembles de données.

### 5.6 Bonnes pratiques de formatage
Écrire du code lisible et professionnel.
- Indentation et espacement
- Conventions de nommage
- Commentaires efficaces
- Organisation du code

**Pourquoi :** Un code bien formaté est plus facile à lire, comprendre et maintenir.

### 5.7 Formatage automatique avec clang-format
Automatiser le formatage pour garantir la cohérence.
- Installation et configuration de clang-format
- Styles de formatage
- Intégration dans l'éditeur
- Pre-commit hooks et CI/CD

**Pourquoi :** Économise du temps et garantit un code toujours bien formaté automatiquement.

---

## Progression pédagogique

Ce chapitre suit une progression logique :

```
Opérateurs de base (5.1)
    ↓
Opérateurs logiques (5.2) ← Nécessaires pour les conditions
    ↓
Opérateurs bit-à-bit (5.3) ← Complémentaires
    ↓
Conditions (5.4) ← Utilise les opérateurs logiques
    ↓
Boucles (5.5) ← Utilise souvent des conditions
    ↓
Formatage (5.6-5.7) ← Rend tout plus lisible
```

**Chaque section s'appuie sur les précédentes.** Il est important de les étudier dans l'ordre.

---

## Exemples de ce que vous pourrez faire

À la fin de ce chapitre, vous serez capable d'écrire des programmes comme :

### 1. Calculatrice interactive

```c
#include <stdio.h>

int main(void)
{
    float a, b;
    char operation;

    printf("Entrez une opération (ex: 5 + 3) : ");
    scanf("%f %c %f", &a, &operation, &b);

    if (operation == '+') {
        printf("Résultat : %.2f\n", a + b);
    } else if (operation == '-') {
        printf("Résultat : %.2f\n", a - b);
    } else if (operation == '*') {
        printf("Résultat : %.2f\n", a * b);
    } else if (operation == '/' && b != 0) {
        printf("Résultat : %.2f\n", a / b);
    } else {
        printf("Opération invalide\n");
    }

    return 0;
}
```

### 2. Détecteur de nombre premier

```c
#include <stdio.h>
#include <stdbool.h>

int main(void)
{
    int n;
    bool est_premier = true;

    printf("Entrez un nombre : ");
    scanf("%d", &n);

    if (n <= 1) {
        est_premier = false;
    } else {
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                est_premier = false;
                break;
            }
        }
    }

    if (est_premier) {
        printf("%d est un nombre premier\n", n);
    } else {
        printf("%d n'est pas un nombre premier\n", n);
    }

    return 0;
}
```

### 3. Jeu de devinette

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand(time(NULL));
    int nombre_secret = rand() % 100 + 1;
    int tentative;
    int nb_essais = 0;

    printf("J'ai choisi un nombre entre 1 et 100. Devinez !\n");

    do {
        printf("Votre tentative : ");
        scanf("%d", &tentative);
        nb_essais++;

        if (tentative < nombre_secret) {
            printf("Trop petit !\n");
        } else if (tentative > nombre_secret) {
            printf("Trop grand !\n");
        } else {
            printf("Bravo ! Vous avez trouvé en %d essais !\n", nb_essais);
        }
    } while (tentative != nombre_secret);

    return 0;
}
```

### 4. Gestion de permissions (avec bits)

```c
#include <stdio.h>

#define READ    (1 << 0)  // 0x01 — Permission de lecture
#define WRITE   (1 << 1)  // 0x02 — Permission d'écriture
#define EXECUTE (1 << 2)  // 0x04 — Permission d'exécution

int main(void)
{
    int permissions = READ | WRITE;  // Lecture et écriture

    if (permissions & READ) {
        printf("Vous pouvez lire\n");
    }

    if (permissions & WRITE) {
        printf("Vous pouvez écrire\n");
    }

    if (permissions & EXECUTE) {
        printf("Vous pouvez exécuter\n");
    } else {
        printf("Vous ne pouvez pas exécuter\n");
    }

    return 0;
}
```

**Ces exemples combinent opérateurs, conditions et boucles pour créer des programmes utiles et interactifs !**

---

## Concepts clés à maîtriser

À la fin de ce chapitre, vous devrez maîtriser :

### Opérateurs
- ✅ Savoir quand utiliser chaque type d'opérateur
- ✅ Comprendre la priorité des opérateurs
- ✅ Éviter les pièges courants (division entière, dépassement)
- ✅ Utiliser les opérateurs d'affectation composée (`+=`, `-=`, etc.)

### Conditions
- ✅ Écrire des conditions simples et complexes
- ✅ Choisir entre `if` et `switch`
- ✅ Utiliser l'opérateur ternaire quand approprié
- ✅ Éviter les imbrications excessives

### Boucles
- ✅ Choisir la bonne boucle pour chaque situation
- ✅ Éviter les boucles infinies
- ✅ Utiliser `break` et `continue` à bon escient
- ✅ Optimiser les boucles pour la performance

### Formatage
- ✅ Indenter correctement votre code
- ✅ Utiliser des conventions de nommage cohérentes
- ✅ Écrire des commentaires utiles
- ✅ Automatiser le formatage avec des outils

---

## Approche pédagogique

### Théorie et pratique

Chaque section combine :
- **Explications théoriques** : Comprendre les concepts
- **Exemples concrets** : Voir comment ça marche
- **Cas d'usage** : Savoir quand les utiliser
- **Erreurs courantes** : Éviter les pièges
- **Bonnes pratiques** : Écrire du code professionnel

### Exemples progressifs

Les exemples commencent simples et deviennent progressivement plus complexes :

**Niveau 1 :** Exemples basiques
```c
int x = 5 + 3;  // Addition simple
```

**Niveau 2 :** Exemples intermédiaires
```c
float moyenne = (note1 + note2 + note3) / 3.0;
```

**Niveau 3 :** Exemples avancés
```c
for (int i = 0; i < n; i++) {
    if (tableau[i] > max) {
        max = tableau[i];
        index_max = i;
    }
}
```

### Comparaisons visuelles

De nombreux tableaux et comparaisons pour visualiser les différences :

| Opérateur | Signification | Exemple | Résultat |
|-----------|---------------|---------|----------|
| `+` | Addition | `5 + 3` | `8` |
| `-` | Soustraction | `5 - 3` | `2` |
| `*` | Multiplication | `5 * 3` | `15` |
| `/` | Division | `5 / 3` | `1` (division entière) |

---

## Pièges courants à éviter

Ce chapitre vous aidera à éviter des erreurs classiques :

### 1. Confusion entre `=` et `==`

```c
// ❌ ERREUR : Affectation au lieu de comparaison
if (x = 5) {  // Affecte 5 à x et teste si 5 est vrai (toujours vrai)
    printf("x vaut 5\n");
}

// ✅ CORRECT : Comparaison
if (x == 5) {  // Compare x avec 5
    printf("x vaut 5\n");
}
```

### 2. Division entière inattendue

```c
// ❌ Résultat inattendu
int a = 5, b = 2;  
float resultat = a / b;  // resultat = 2.0 (pas 2.5 !)  

// ✅ CORRECT
float resultat = (float)a / b;  // resultat = 2.5
```

### 3. Boucle infinie

```c
// ❌ Boucle infinie
int i = 0;  
while (i < 10) {  
    printf("%d\n", i);
    // Oubli d'incrémenter i !
}

// ✅ CORRECT
int i = 0;  
while (i < 10) {  
    printf("%d\n", i);
    i++;  // Incrémentation
}
```

### 4. Priorité des opérateurs

```c
// ❌ Résultat inattendu
int resultat = 5 + 3 * 2;  // resultat = 11 (pas 16 !)
// Explication : * a priorité sur +

// ✅ CORRECT si on veut 16
int resultat = (5 + 3) * 2;  // resultat = 16
```

**Chaque piège sera expliqué en détail dans les sections concernées.**

---

## Outils et ressources

### Compilateur

Tout au long de ce chapitre, vous utiliserez GCC :

```bash
# Compiler un programme
gcc -Wall -Wextra -std=c17 mon_programme.c -o mon_programme

# Exécuter
./mon_programme
```

Les options `-Wall -Wextra` sont **essentielles** : elles affichent les avertissements qui peuvent révéler des erreurs subtiles.

### Débogueur

Pour comprendre comment les conditions et boucles fonctionnent :

```bash
# Compiler avec symboles de débogage
gcc -g mon_programme.c -o mon_programme

# Déboguer
gdb ./mon_programme
```

### Formatage automatique

À partir de la section 5.7, vous utiliserez `clang-format` :

```bash
# Installer
sudo apt install clang-format

# Formater un fichier
clang-format -i mon_fichier.c
```

---

## Prérequis

Avant de commencer ce chapitre, assurez-vous de maîtriser :

- ✅ **Les variables** : Déclaration, initialisation, types de base
- ✅ **Les fonctions de base** : `printf()`, `scanf()`
- ✅ **La compilation** : Compiler et exécuter un programme C
- ✅ **Le terminal** : Naviguer et exécuter des commandes

**Si ces notions ne sont pas claires, révisez les chapitres précédents avant de continuer.**

---

## Comment aborder ce chapitre

### 1. Lire activement

Ne vous contentez pas de lire. Pour chaque exemple :
- Lisez le code
- Essayez de prédire le résultat
- Tapez le code vous-même
- Compilez et exécutez
- Comparez avec votre prédiction

### 2. Expérimenter

Modifiez les exemples :
- Changez les valeurs
- Ajoutez des conditions
- Testez les cas limites
- Provoquez des erreurs volontairement pour comprendre

### 3. Pratiquer régulièrement

La programmation s'apprend par la pratique :
- Faites les exemples de chaque section
- Essayez de créer vos propres variations
- Résolvez de petits problèmes
- Revenez sur les sections difficiles

### 4. Prendre des notes

Notez :
- Les concepts clés
- Les pièges à éviter
- Vos propres exemples
- Les questions à approfondir

---

## Résumé de l'introduction

Avant de plonger dans les détails, retenez ces points essentiels :

### Ce que sont les opérateurs
Des symboles qui effectuent des opérations sur des valeurs. Ils permettent de :
- Faire des calculs (arithmétiques)
- Prendre des décisions (logiques, relationnels)
- Manipuler les données (bit-à-bit)

### Ce que sont les structures de contrôle
Des instructions qui modifient le flux d'exécution. Elles permettent de :
- Exécuter du code conditionnellement (`if`, `switch`)
- Répéter du code (`while`, `for`, `do-while`)
- Contrôler les répétitions (`break`, `continue`)

### Pourquoi c'est crucial
- Base de toute la programmation
- Permet de créer des programmes interactifs
- Résout des problèmes réels
- Fondation pour les concepts avancés

### Ce que vous allez apprendre
1. **Opérateurs** : Arithmétiques, logiques, bit-à-bit
2. **Conditions** : if, else, switch, optimisations
3. **Boucles** : while, for, do-while, contrôle de flux
4. **Formatage** : Bonnes pratiques et automatisation

**À la fin de ce chapitre, vous pourrez écrire des programmes complets qui calculent, décident et répètent des actions intelligemment !**

---

## Prêt à commencer ?

Vous avez maintenant une vue d'ensemble de ce chapitre crucial. Vous comprenez :
- L'importance des opérateurs et structures de contrôle
- Ce que vous allez apprendre
- Comment aborder le contenu
- Les pièges à éviter

**Il est temps de passer à la pratique !**

La prochaine section (**5.1 - Opérateurs arithmétiques**) démarre avec les bases des calculs en C. Vous allez apprendre à :
- Effectuer les quatre opérations de base (+, -, *, /)
- Utiliser le modulo (%) pour calculer des restes
- Comprendre la priorité des opérateurs
- Éviter les erreurs courantes de calcul

**Prêt à écrire vos premiers programmes qui calculent ?** Passons à la section 5.1 !

---

## Motivation finale

Rappelez-vous : **tout programme que vous utilisez quotidiennement** (navigateur web, système d'exploitation, jeux vidéo, applications mobiles) utilise massivement les concepts de ce chapitre.

En maîtrisant les opérateurs et structures de contrôle, vous franchissez la frontière entre :
- ❌ Écrire des scripts basiques qui suivent des instructions séquentielles
- ✅ Créer de vrais programmes qui **pensent**, **décident** et **s'adaptent**

**C'est ici que votre voyage en programmation C devient vraiment passionnant !**

Bonne lecture et bon apprentissage ! 🚀

⏭️ [Opérateurs arithmétiques](/05-operateurs-et-controle/01-operateurs-arithmetiques.md)
