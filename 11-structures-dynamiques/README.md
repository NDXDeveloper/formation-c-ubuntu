🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Chapitre 11 : Structures de données dynamiques

## Introduction au chapitre

Jusqu'à présent, vous avez travaillé principalement avec des **tableaux statiques** : des structures de taille fixe, définie à la compilation. Bien que simples et efficaces, les tableaux présentent une limitation majeure : **leur taille ne peut pas changer** pendant l'exécution du programme.

```c
int tableau[100];  // Taille fixe : 100 éléments, ni plus ni moins
```

**Problème** : Que faire si vous ne connaissez pas à l'avance le nombre d'éléments dont vous aurez besoin ? Ou si ce nombre change constamment pendant l'exécution ?

Les **structures de données dynamiques** résolvent ce problème en permettant de faire croître ou rétrécir vos collections de données selon les besoins du programme.

---

## Pourquoi les structures dynamiques ?

### Scénario 1 : Gestion d'une liste de contacts

Imaginez que vous développez une application de gestion de contacts.

**Avec un tableau statique** :
```c
Contact contacts[1000];  // Combien prévoir ? 100 ? 1000 ? 10000 ?
```

**Problèmes** :
- Si l'utilisateur a seulement 10 contacts → **Gaspillage** de mémoire (990 emplacements vides)
- Si l'utilisateur a 1500 contacts → **Dépassement** impossible à gérer

**Avec une structure dynamique** :
```c
ContactList* contacts = create_list();  // Commence vide  
add_contact(contacts, alice);           // Grandit automatiquement  
add_contact(contacts, bob);  
add_contact(contacts, charlie);  
// ... autant de contacts que nécessaire
```

### Scénario 2 : File d'attente d'impression

Une imprimante reçoit des documents à imprimer.

**Avec un tableau** :
```c
Document queue[50];  
int front = 0, rear = 0;  

// Que faire quand la file est pleine mais qu'on a imprimé la moitié ?
// Les emplacements du début sont "perdus"
```

**Avec une structure dynamique** :
```c
Queue* print_queue = create_queue();  
enqueue(print_queue, doc1);  // Ajoute à la fin  
Document* next = dequeue(print_queue);  // Retire du début  
// La mémoire s'ajuste automatiquement
```

### Scénario 3 : Arbre généalogique

Comment représenter une famille avec un nombre variable d'enfants ?

**Impossible avec un tableau simple** : Chaque personne peut avoir 0, 1, 2, 3... enfants.

**Avec une structure dynamique** :
```c
typedef struct Person {
    char* name;
    struct Person* parent;
    struct Person** children;  // Nombre variable d'enfants
    int num_children;
} Person;
```

---

## Avantages et inconvénients

### ✅ Avantages des structures dynamiques

**1. Flexibilité de taille**
- La structure grandit et rétrécit selon les besoins
- Pas de gaspillage de mémoire
- Pas de limite artificielle

**2. Insertion/Suppression efficace**
- Certaines structures permettent d'ajouter/retirer des éléments en temps constant O(1)
- Pas besoin de décaler tous les éléments comme dans un tableau

**3. Organisation naturelle**
- Certaines structures (arbres, graphes) représentent naturellement des relations hiérarchiques
- Modélisation plus intuitive de problèmes réels

**4. Utilisation optimale de la mémoire**
- Alloue exactement ce qui est nécessaire
- Libère la mémoire devenue inutile

### ❌ Inconvénients des structures dynamiques

**1. Complexité accrue**
- Gestion manuelle de la mémoire (malloc/free)
- Risques de fuites mémoire
- Plus de code à écrire et maintenir

**2. Overhead mémoire**
- Besoin de pointeurs pour relier les éléments
- Chaque pointeur consomme 8 bytes (sur système 64-bit)

**3. Performance**
- Accès séquentiel plutôt que direct
- Moins favorable au cache CPU (données dispersées en mémoire)
- Allocations dynamiques plus lentes que l'accès à la stack

**4. Complexité algorithmique**
- Certaines opérations deviennent plus complexes
- Nécessite une bonne compréhension des pointeurs

---

## Prérequis essentiels

Avant d'aborder les structures dynamiques, assurez-vous de maîtriser :

### 1. Les pointeurs

```c
int* ptr;                    // Déclaration  
ptr = &variable;             // Adresse  
*ptr = 10;                   // Déréférencement
ptr->membre;                 // Accès membre via pointeur
```

**Pourquoi ?** Les structures dynamiques utilisent intensivement les pointeurs pour relier les éléments.

### 2. L'allocation dynamique

```c
int* array = (int*)malloc(sizeof(int) * 10);  // Allocation  
if (array == NULL) {  
    // Gérer l'erreur
}
// ... utilisation ...
free(array);  // Libération  
array = NULL;  
```

**Pourquoi ?** Chaque élément d'une structure dynamique est alloué sur le tas (heap).

### 3. Les structures

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;
```

**Pourquoi ?** Les structures définissent la forme des éléments et leurs liens.

### 4. La récursivité (utile mais pas obligatoire)

```c
void process(Node* node) {
    if (node == NULL) return;
    printf("%d ", node->data);
    process(node->next);  // Appel récursif
}
```

**Pourquoi ?** Beaucoup d'algorithmes sur structures dynamiques sont naturellement récursifs.

---

## Vue d'ensemble du chapitre

Dans ce chapitre, nous allons explorer **7 structures de données dynamiques** fondamentales, organisées par complexité croissante.

### Structures linéaires

#### 11.1 Listes chaînées simples
**Concept** : Séquence d'éléments reliés par des pointeurs `next`.

```
[10|•] → [20|•] → [30|•] → NULL
```

**Opérations clés** : Insertion O(1) au début, parcours O(n)

**Cas d'usage** : Collections à taille variable, insertions fréquentes au début

---

#### 11.2 Listes doublement chaînées
**Concept** : Éléments avec pointeurs `prev` et `next` (navigation bidirectionnelle).

```
NULL ← [•|10|•] ↔ [•|20|•] ↔ [•|30|•] → NULL
```

**Opérations clés** : Suppression O(1), navigation dans les deux sens

**Cas d'usage** : Historique de navigation, undo/redo, playlists

---

#### 11.3 Piles et Files
**Piles (LIFO)** : Dernier entré, premier sorti (comme une pile d'assiettes)

```
push(10) → push(20) → push(30)
   |          |           |
  [10]    [20]         [30]  ← Sommet
          [10]         [20]
                       [10]
```

**Files (FIFO)** : Premier entré, premier sorti (comme une file d'attente)

```
enqueue(10) → enqueue(20) → enqueue(30)  
Front → [10] [20] [30] ← Rear  
```

**Cas d'usage** :
- Piles : Évaluation d'expressions, gestion des appels de fonctions, undo
- Files : Gestion de tâches, buffers, algorithmes de parcours

---

### Structures par accès

#### 11.4 Tables de hachage
**Concept** : Association clé-valeur avec accès ultra-rapide O(1).

```
Clé "Alice" → [Fonction de hachage] → Index 3 → Valeur "06 12 34 56 78"
```

**Opérations clés** : Recherche, insertion, suppression en O(1) moyen

**Cas d'usage** : Dictionnaires, caches, comptage de fréquences, sets

---

### Structures hiérarchiques

#### 11.5 Arbres binaires
**Concept** : Structure hiérarchique où chaque nœud a au maximum 2 enfants.

```
       [50]
      /    \
   [30]    [70]
   /  \      \
[20] [40]   [80]
```

**Arbre Binaire de Recherche (BST)** : Gauche < Racine < Droit

**Opérations clés** : Recherche, insertion, suppression en O(log n) si équilibré

**Cas d'usage** : Données triées, indexation, arbres de décision

---

### Guide de sélection

#### 11.6 Choix de la structure de données
**Objectif** : Apprendre à choisir la structure adaptée à chaque situation.

**Critères de décision** :
- Type d'opérations principales
- Contraintes de performance
- Besoins en mémoire
- Nécessité d'ordre ou non

**Arbres de décision** : Guides pour choisir rapidement la bonne structure.

---

### Gestion mémoire

#### 11.7 Gestion mémoire des structures dynamiques
**Objectif** : Maîtriser l'allocation et la libération correcte de la mémoire.

**Sujets couverts** :
- Cycle de vie de la mémoire
- Erreurs classiques (fuites, double free, use after free)
- Outils de détection (Valgrind, AddressSanitizer)
- Bonnes pratiques

---

## Comparaison visuelle

### Tableau vs Structure dynamique

**Tableau statique** :
```
┌─────┬─────┬─────┬─────┬─────┐
│  10 │  20 │  30 │  40 │  50 │  (mémoire contiguë)
└─────┴─────┴─────┴─────┴─────┘
Index: 0     1     2     3     4

✓ Accès direct : array[3] → O(1)
✗ Taille fixe : Définie à la compilation
✗ Insertion milieu : Décalage de tous les éléments → O(n)
```

**Liste chaînée** :
```
[10|•]───→[20|•]───→[30|•]───→[40|•]───→[50|•]───→NULL
  ↑                                              (mémoire dispersée)
HEAD

✓ Taille dynamique : Grandit selon les besoins
✓ Insertion début : O(1)
✗ Accès : Parcours séquentiel → O(n)
✗ Overhead : Pointeurs supplémentaires
```

---

## Analogies pour comprendre

### Liste chaînée = Train
Chaque wagon (nœud) est relié au suivant. On peut ajouter ou retirer des wagons facilement, mais pour atteindre le wagon 10, il faut passer par tous les précédents.

### Pile = Pile d'assiettes
On empile (push) et on dépile (pop) toujours par le haut. L'assiette du dessous est inaccessible sans retirer celles du dessus.

### File = File d'attente à la caisse
Les gens entrent par la fin (enqueue) et sortent par le début (dequeue). Premier arrivé, premier servi.

### Table de hachage = Annuaire téléphonique magique
Au lieu de chercher page par page, on calcule instantanément où trouver le numéro. "Alice" → Calcul → Page 42 → Numéro trouvé !

### Arbre = Organigramme d'entreprise
PDG au sommet, managers en dessous, employés aux feuilles. Structure hiérarchique naturelle.

---

## Progression pédagogique du chapitre

Le chapitre est organisé selon une **difficulté croissante** :

### Niveau 1 : Fondamentaux (Sections 11.1-11.3)
- Listes chaînées simples
- Listes doublement chaînées
- Piles et Files

**Objectif** : Comprendre le concept de liaison par pointeurs, maîtriser l'insertion/suppression.

### Niveau 2 : Accès optimisé (Section 11.4)
- Tables de hachage

**Objectif** : Apprendre à associer clés et valeurs avec un accès O(1).

### Niveau 3 : Structures hiérarchiques (Section 11.5)
- Arbres binaires de recherche

**Objectif** : Comprendre les structures arborescentes et la récursivité.

### Niveau 4 : Synthèse (Sections 11.6-11.7)
- Choix de la structure adaptée
- Gestion mémoire rigoureuse

**Objectif** : Développer un jugement critique pour choisir et implémenter correctement.

---

## Ce que vous saurez faire à la fin

Après avoir complété ce chapitre, vous serez capable de :

- ✅ **Implémenter** les 7 structures de données principales  
- ✅ **Analyser** la complexité des opérations (O(1), O(n), O(log n))  
- ✅ **Choisir** la structure adaptée à un problème donné  
- ✅ **Gérer** correctement la mémoire dynamique sans fuites  
- ✅ **Déboguer** les problèmes mémoire avec Valgrind  
- ✅ **Comparer** les avantages/inconvénients de chaque structure  
- ✅ **Résoudre** des problèmes algorithmiques classiques

---

## Conseils pour réussir

### 1. Visualisez !
Dessinez toujours les structures sur papier avant de coder. Représentez les pointeurs avec des flèches.

```
Avant insertion :
[10] → [30] → NULL

Après insertion de 20 :
[10] → [20] → [30] → NULL
         ↑
    nouveau nœud
```

### 2. Commencez simple
Ne cherchez pas à tout optimiser dès le début. Une version simple qui fonctionne vaut mieux qu'une version "optimale" boguée.

### 3. Testez systématiquement
Pour chaque structure, testez les cas limites :
- Structure vide
- Un seul élément
- Insertion au début, milieu, fin
- Suppression dans tous les cas

### 4. Utilisez Valgrind dès le début
Ne pas attendre d'avoir un problème pour vérifier la mémoire.

```bash
valgrind --leak-check=full ./mon_programme
```

### 5. Comprenez avant de mémoriser
Ne cherchez pas à mémoriser le code. Comprenez la logique, le reste suivra naturellement.

### 6. Implémentez plusieurs fois
La première implémentation sera difficile. La deuxième sera plus facile. À la troisième, ce sera naturel.

---

## Convention de notation

Dans ce chapitre, nous utiliserons les conventions suivantes :

### Représentation graphique

**Pointeurs** :
```
[data|•] → [data|•]
       ↑         ↑
    pointeur  pointeur
```

**NULL** :
```
[data|•] → NULL
```

**Pointeurs bidirectionnels** :
```
[•|data|•] ↔ [•|data|•]
```

### Complexité

- **O(1)** : Temps constant (meilleur cas)
- **O(n)** : Temps linéaire (proportionnel au nombre d'éléments)
- **O(log n)** : Temps logarithmique (division par 2 à chaque étape)
- **O(n²)** : Temps quadratique (à éviter si possible)

### Code

```c
// Commentaires expliquent le "pourquoi"
typedef struct Node {
    int data;            // Donnée stockée
    struct Node* next;   // Pointeur vers le suivant
} Node;
```

---

## Ressources complémentaires

### Pour aller plus loin

Une fois les bases maîtrisées, explorez :
- **Structures avancées** : AVL, Red-Black Trees, B-Trees, Tries
- **Algorithmes** : Tri, recherche, parcours de graphes
- **Complexité** : Analyse amortie, master theorem
- **Implémentations** : STL en C++, collections Java

### Sites recommandés

- **VisuAlgo** : Visualisation animée des structures de données
- **GeeksforGeeks** : Tutoriels et exercices
- **LeetCode** : Problèmes pratiques

### Livres de référence

- *Introduction to Algorithms* (CLRS) - Référence académique
- *The Algorithm Design Manual* (Skiena) - Approche pratique
- *Data Structures and Algorithms in C* (Tanenbaum) - Spécifique au C

---

## Structure type d'une section

Chaque section de ce chapitre suivra cette structure :

1. **Introduction** : Concept et motivation
2. **Structure de données** : Définition et diagrammes
3. **Opérations de base** : Insertion, suppression, recherche, parcours
4. **Complexité** : Analyse des performances
5. **Exemple complet** : Code fonctionnel
6. **Applications** : Cas d'usage réels
7. **Comparaison** : Avec d'autres structures
8. **Erreurs courantes** : Pièges à éviter
9. **Récapitulatif** : Points clés

---

## Prêt à commencer ?

Les structures de données dynamiques sont au cœur de la programmation moderne. Elles vous permettront de :

🎯 Résoudre des problèmes complexes élégamment  
🚀 Écrire du code plus flexible et maintenable  
💡 Comprendre le fonctionnement interne de bibliothèques et frameworks  
🔧 Optimiser les performances de vos applications

**La route sera parfois difficile**, surtout au début avec les pointeurs et la gestion mémoire. Mais une fois ces concepts maîtrisés, vous aurez franchi un cap majeur dans votre apprentissage du C.

**Conseil final** : Prenez votre temps. Ces structures sont fondamentales et méritent d'être bien comprises. N'hésitez pas à revenir sur les sections précédentes si nécessaire, et surtout, **pratiquez, pratiquez, pratiquez** !

---

## Prochaine étape

Nous commençons par la structure la plus fondamentale : **les listes chaînées simples**.

Cette structure vous servira de base pour comprendre toutes les autres. Une fois les listes chaînées maîtrisées, le reste du chapitre coulera naturellement.

**Objectif de la section 11.1** : Comprendre comment créer une séquence d'éléments reliés par des pointeurs, et effectuer les opérations de base (insertion, suppression, parcours).

Prêt ? Allons-y ! 🚀

---

*💡 "La seule façon de vraiment comprendre les structures de données est de les implémenter soi-même." - Proverbe de programmeur*

⏭️ [Listes chaînées simples](/11-structures-dynamiques/01-listes-chainees.md)
