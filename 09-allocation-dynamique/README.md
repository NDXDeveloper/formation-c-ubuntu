🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9. Allocation Dynamique

## Introduction au chapitre

L'**allocation dynamique** est l'une des fonctionnalités les plus puissantes et les plus fondamentales du langage C. Elle vous permet de demander de la mémoire **pendant l'exécution** de votre programme, au moment où vous en avez réellement besoin, et non pas à la compilation.

Sans allocation dynamique, vous seriez limité à :
- Des tableaux de taille fixe décidée à la compilation
- Des structures de données rigides
- Une impossibilité de gérer des données dont la taille n'est connue qu'à l'exécution

Avec l'allocation dynamique, vous pouvez :
- ✅ Créer des tableaux dont la taille est déterminée par l'utilisateur
- ✅ Construire des structures de données complexes (listes, arbres, graphes)
- ✅ Gérer efficacement la mémoire en n'allouant que ce dont vous avez besoin
- ✅ Partager des données entre différentes fonctions
- ✅ Créer des programmes flexibles et évolutifs

> **Ce chapitre est crucial** : la maîtrise de l'allocation dynamique distingue un développeur débutant d'un développeur confirmé en C.

---

## Pourquoi l'allocation dynamique ?

### Problème 1 : Taille inconnue à la compilation

Imaginez que vous écrivez un programme qui lit un fichier. Comment savoir à l'avance combien d'octets contient ce fichier ?

**Sans allocation dynamique :**
```c
// ❌ Approche rigide et limitée
char buffer[1000];  // Et si le fichier fait 2000 octets ?
                    // Et si le fichier fait 100 octets ? (gaspillage)
```

**Avec allocation dynamique :**
```c
// ✅ Approche flexible
size_t taille = obtenir_taille_fichier("data.txt");  
char* buffer = malloc(taille);  // Taille exacte !  
```

### Problème 2 : Données qui doivent survivre à une fonction

Les variables locales sont automatiquement détruites quand une fonction se termine. Comment créer des données qui persistent ?

**Sans allocation dynamique :**
```c
int* creer_tableau(void) {
    int tableau[10] = {1, 2, 3, 4, 5};
    return tableau;  // ❌ ERREUR : tableau sera détruit !
}
```

**Avec allocation dynamique :**
```c
int* creer_tableau(void) {
    int* tableau = malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        tableau[i] = i + 1;
    }
    return tableau;  // ✅ OK : la mémoire persiste
}
```

### Problème 3 : Structures de données complexes

Comment créer une liste chaînée sans allocation dynamique ? C'est impossible !

```c
typedef struct Node {
    int valeur;
    struct Node* suivant;  // Pointe vers un autre nœud
} Node;

// On ne peut pas créer une liste de taille arbitraire
// sans allocation dynamique !
```

---

## Les deux zones de mémoire

Pour comprendre l'allocation dynamique, il faut d'abord comprendre qu'un programme C utilise deux zones principales de mémoire :

### La Stack (Pile)

```
┌─────────────────────────┐
│   Variables locales     │  ← Gérée AUTOMATIQUEMENT
│   Paramètres fonctions  │  ← Rapide
│   Adresses de retour    │  ← Limitée en taille
└─────────────────────────┘
```

**Caractéristiques :**
- ✅ Gestion automatique (pas besoin de libérer)
- ✅ Très rapide
- ❌ Taille limitée (quelques MB)
- ❌ Durée de vie limitée au scope

**Exemple :**
```c
void fonction(void) {
    int x = 10;        // Sur la Stack
    char nom[50];      // Sur la Stack
    double pi = 3.14;  // Sur la Stack
}  // x, nom et pi sont automatiquement détruits ici
```

### Le Heap (Tas)

```
┌─────────────────────────┐
│   Allocation dynamique  │  ← Gérée MANUELLEMENT
│   malloc(), free()      │  ← Plus lent que Stack
│   Grandes quantités     │  ← Flexible en taille
└─────────────────────────┘
```

**Caractéristiques :**
- ✅ Taille flexible (limité par la RAM disponible)
- ✅ Durée de vie contrôlée par le programmeur
- ✅ Peut être partagée entre fonctions
- ❌ Plus lent que la Stack
- ❌ Gestion manuelle (risque de fuites mémoire)

**Exemple :**
```c
void fonction(void) {
    int* ptr = malloc(sizeof(int));  // Alloué sur le Heap
    *ptr = 10;

    // ptr (le pointeur) est sur la Stack
    // La mémoire pointée est sur le Heap

    free(ptr);  // ⚠️ Libération MANUELLE obligatoire
}
```

---

## Les fonctions clés de l'allocation dynamique

Le C fournit quatre fonctions principales pour gérer l'allocation dynamique :

### malloc() - Allouer

```c
void* malloc(size_t size);
```

Alloue `size` octets sur le Heap et retourne un pointeur vers le début de cette zone.

**Exemple :**
```c
int* entier = malloc(sizeof(int));  
char* chaine = malloc(100 * sizeof(char));  
```

### calloc() - Allouer et initialiser

```c
void* calloc(size_t nmemb, size_t size);
```

Alloue de la mémoire pour `nmemb` éléments de `size` octets chacun, et **initialise tout à zéro**.

**Exemple :**
```c
int* tableau = calloc(10, sizeof(int));  // 10 entiers, tous = 0
```

### realloc() - Redimensionner

```c
void* realloc(void* ptr, size_t size);
```

Redimensionne un bloc précédemment alloué.

**Exemple :**
```c
int* tableau = malloc(5 * sizeof(int));
// ... besoin de plus d'espace ...
tableau = realloc(tableau, 10 * sizeof(int));  // Maintenant 10 entiers
```

### free() - Libérer

```c
void free(void* ptr);
```

Libère la mémoire précédemment allouée.

**Exemple :**
```c
int* ptr = malloc(sizeof(int));
// ... utilisation ...
free(ptr);  // ⚠️ CRUCIAL : toujours libérer !
```

---

## Premier exemple complet

Voici un exemple simple qui illustre les concepts de base :

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // 1. Demander la taille à l'utilisateur
    printf("Combien d'entiers voulez-vous stocker ? ");
    int n;
    scanf("%d", &n);

    // 2. Allouer dynamiquement un tableau de cette taille
    int* tableau = malloc(n * sizeof(int));

    // 3. Vérifier que l'allocation a réussi
    if (tableau == NULL) {
        printf("Erreur : mémoire insuffisante\n");
        return 1;
    }

    // 4. Remplir le tableau
    printf("Entrez %d entiers :\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &tableau[i]);
    }

    // 5. Afficher le tableau
    printf("Vous avez entré : ");
    for (int i = 0; i < n; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");

    // 6. Libérer la mémoire
    free(tableau);

    return 0;
}
```

**Exécution :**
```
Combien d'entiers voulez-vous stocker ? 5  
Entrez 5 entiers :  
10 20 30 40 50
Vous avez entré : 10 20 30 40 50
```

**Points clés de cet exemple :**
- La taille du tableau n'est connue qu'à l'exécution
- Nous allouons exactement la quantité de mémoire nécessaire
- Nous vérifions que l'allocation a réussi
- Nous libérons la mémoire quand nous n'en avons plus besoin

---

## Les risques de l'allocation dynamique

L'allocation dynamique est puissante, mais elle comporte des risques que nous allons apprendre à gérer dans ce chapitre :

### 1. Fuites mémoire (Memory Leaks)

**Problème :** Oublier de libérer la mémoire allouée.

```c
void fonction(void) {
    int* ptr = malloc(1000 * sizeof(int));
    // ... utilisation ...
    // ❌ OUBLI : pas de free(ptr) !
}  // La mémoire reste allouée indéfiniment → FUITE
```

**Conséquence :** Le programme consomme de plus en plus de mémoire jusqu'à l'épuisement.

### 2. Double free

**Problème :** Libérer la même mémoire deux fois.

```c
int* ptr = malloc(sizeof(int));  
free(ptr);  
free(ptr);  // ❌ ERREUR : double free → Corruption mémoire  
```

**Conséquence :** Corruption du gestionnaire de mémoire, crash du programme.

### 3. Use After Free

**Problème :** Utiliser un pointeur après avoir libéré la mémoire.

```c
int* ptr = malloc(sizeof(int));
*ptr = 42;
free(ptr);

printf("%d\n", *ptr);  // ❌ ERREUR : lecture de mémoire libérée
```

**Conséquence :** Comportement indéfini, crash possible, corruption de données.

### 4. Buffer Overflow

**Problème :** Écrire en dehors de la zone allouée.

```c
char* buffer = malloc(10);  
strcpy(buffer, "Ceci est une très longue chaîne");  // ❌ DÉBORDEMENT !  
```

**Conséquence :** Corruption de mémoire, crash, faille de sécurité.

---

## La règle d'or de l'allocation dynamique

> **Pour chaque malloc(), il DOIT y avoir un free() correspondant.**

```c
// Pattern correct
void* ptr = malloc(size);  
if (ptr != NULL) {  
    // ... utilisation de ptr ...
    free(ptr);  // ✅ Libération
    ptr = NULL; // ✅ Bonne pratique : éviter les erreurs
}
```

**Mnémotechnique :** "Allouer = Emprunter. Libérer = Rendre. Toujours rendre ce qu'on emprunte !"

---

## Vue d'ensemble du chapitre

Ce chapitre est structuré pour vous guider progressivement vers la maîtrise de l'allocation dynamique :

### Section 9.1 : Stack vs Heap
Comprendre les différences fondamentales entre les deux zones de mémoire, leurs avantages et inconvénients respectifs.

### Section 9.2 : Durée de vie et performance
Apprendre comment la durée de vie des variables diffère entre Stack et Heap, et l'impact sur les performances.

### Section 9.3 : Les fonctions malloc, calloc, realloc et free
Maîtriser l'utilisation des quatre fonctions essentielles de l'allocation dynamique.

### Section 9.4 : Gestion des erreurs d'allocation
Apprendre à gérer les échecs d'allocation et à écrire du code robuste.

### Section 9.5 : Fuites mémoire
Identifier, détecter et prévenir les fuites mémoire avec des outils comme Valgrind.

### Section 9.6 : Double free et corruption de tas
Comprendre les erreurs graves qui corrompent le gestionnaire de mémoire et comment les éviter.

### Section 9.7 : Stratégies d'allocation personnalisées
Découvrir les allocateurs personnalisés (pool, arena) pour des cas d'usage spécifiques.

---

## Analogie du monde réel

Pour mieux comprendre l'allocation dynamique, voici une analogie :

### La Stack = Votre bureau

```
┌─────────────────────────────────────┐
│  Votre bureau (Stack)               │
│  - Espace limité                    │
│  - Objets temporaires               │
│  - Nettoyage automatique            │
│  - Accès immédiat                   │
└─────────────────────────────────────┘
```

Quand vous travaillez, vous posez des feuilles, des stylos sur votre bureau. Quand vous avez fini une tâche, vous rangez automatiquement. L'espace est limité mais très rapide d'accès.

### Le Heap = Un entrepôt

```
┌─────────────────────────────────────┐
│  Entrepôt (Heap)                    │
│  - Espace immense                   │
│  - Stockage long terme              │
│  - Gestion manuelle                 │
│  - Accès plus lent                  │
└─────────────────────────────────────┘
```

Quand vous avez besoin de stocker quelque chose à long terme ou de grande taille, vous allez à l'entrepôt. Vous devez :
1. **Réserver un emplacement** (malloc)
2. **Y ranger vos affaires** (utilisation)
3. **Libérer l'emplacement** quand vous n'en avez plus besoin (free)

Si vous oubliez de libérer, l'entrepôt se remplit de choses inutiles (fuite mémoire) !

---

## Prérequis pour ce chapitre

Pour tirer le meilleur parti de ce chapitre, vous devriez être à l'aise avec :

✅ **Les pointeurs** (Module 3, Section 7)
- Comprendre ce qu'est un pointeur
- Savoir déréférencer un pointeur
- Connaître l'arithmétique des pointeurs

✅ **Les tableaux** (Module 3, Section 8)
- Relation entre tableaux et pointeurs
- Manipulation de tableaux

✅ **Les structures** (Module 4, Section 10)
- Créer et utiliser des structures
- Pointeurs vers structures

Si ces notions ne sont pas claires, il est recommandé de les réviser avant de continuer.

---

## Outils essentiels

Pour travailler efficacement avec l'allocation dynamique, vous aurez besoin de ces outils :

### Valgrind

**Détecte les fuites mémoire et erreurs d'accès.**

```bash
# Installation
sudo apt-get install valgrind

# Utilisation
gcc -g programme.c -o programme  
valgrind --leak-check=full ./programme  
```

### AddressSanitizer

**Détection rapide des erreurs mémoire.**

```bash
# Compilation avec ASan
gcc -fsanitize=address -g programme.c -o programme
./programme
```

### GDB

**Débogueur pour analyser les crashs.**

```bash
gdb ./programme
(gdb) run
(gdb) backtrace  # Après un crash
```

Nous verrons comment utiliser ces outils en détail dans les sections suivantes.

---

## Conseils pour réussir ce chapitre

### 1. Pratiquez régulièrement

L'allocation dynamique demande de la pratique. Chaque exemple du cours devrait être :
- ✅ Tapé et compilé par vous-même
- ✅ Testé avec différentes valeurs
- ✅ Vérifié avec Valgrind

### 2. Développez des réflexes

Créez ces habitudes dès maintenant :
- Toujours vérifier si `malloc()` retourne NULL
- Toujours appeler `free()` pour chaque `malloc()`
- Mettre les pointeurs à NULL après `free()`

### 3. Utilisez les outils

Ne vous fiez pas uniquement à votre jugement :
- Exécutez Valgrind sur TOUS vos programmes
- Compilez avec les warnings (`-Wall -Wextra`)
- Testez les cas limites

### 4. Comprenez avant de mémoriser

Ne vous contentez pas d'apprendre par cœur les fonctions :
- Comprenez POURQUOI la Stack est limitée
- Comprenez COMMENT fonctionne malloc() internement
- Comprenez les CONSÉQUENCES de chaque erreur

---

## Objectifs d'apprentissage

À la fin de ce chapitre, vous serez capable de :

- ✅ **Comprendre** la différence entre Stack et Heap
- ✅ **Utiliser** correctement malloc(), calloc(), realloc() et free()
- ✅ **Gérer** les erreurs d'allocation de manière robuste
- ✅ **Détecter** les fuites mémoire avec Valgrind
- ✅ **Éviter** les erreurs courantes (double free, use after free)
- ✅ **Créer** des structures de données dynamiques (listes, arbres)
- ✅ **Optimiser** l'utilisation de la mémoire avec des allocateurs personnalisés

---

## Exemple motivant : Gestionnaire de contacts

Voici un aperçu de ce que vous pourrez créer après ce chapitre :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char* nom;
    char* telephone;
    struct Contact* suivant;
} Contact;

typedef struct {
    Contact* tete;
    size_t nombre;
} Carnet;

Carnet* creer_carnet(void) {
    Carnet* carnet = malloc(sizeof(Carnet));
    if (carnet == NULL) return NULL;

    carnet->tete = NULL;
    carnet->nombre = 0;
    return carnet;
}

int ajouter_contact(Carnet* carnet, const char* nom, const char* tel) {
    if (carnet == NULL) return 0;

    Contact* nouveau = malloc(sizeof(Contact));
    if (nouveau == NULL) return 0;

    nouveau->nom = malloc(strlen(nom) + 1);
    nouveau->telephone = malloc(strlen(tel) + 1);

    if (nouveau->nom == NULL || nouveau->telephone == NULL) {
        free(nouveau->telephone);
        free(nouveau->nom);
        free(nouveau);
        return 0;
    }

    strcpy(nouveau->nom, nom);
    strcpy(nouveau->telephone, tel);
    nouveau->suivant = carnet->tete;
    carnet->tete = nouveau;
    carnet->nombre++;

    return 1;
}

void afficher_carnet(const Carnet* carnet) {
    if (carnet == NULL) return;

    printf("=== Carnet (%zu contacts) ===\n", carnet->nombre);
    Contact* courant = carnet->tete;

    while (courant != NULL) {
        printf("%s : %s\n", courant->nom, courant->telephone);
        courant = courant->suivant;
    }
}

void liberer_carnet(Carnet* carnet) {
    if (carnet == NULL) return;

    Contact* courant = carnet->tete;
    while (courant != NULL) {
        Contact* suivant = courant->suivant;
        free(courant->telephone);
        free(courant->nom);
        free(courant);
        courant = suivant;
    }

    free(carnet);
}

int main(void) {
    Carnet* mon_carnet = creer_carnet();

    if (mon_carnet == NULL) {
        fprintf(stderr, "Erreur de création du carnet\n");
        return 1;
    }

    ajouter_contact(mon_carnet, "Alice", "06-12-34-56-78");
    ajouter_contact(mon_carnet, "Bob", "06-98-76-54-32");
    ajouter_contact(mon_carnet, "Charlie", "06-11-22-33-44");

    afficher_carnet(mon_carnet);

    liberer_carnet(mon_carnet);

    return 0;
}
```

Ce programme illustre tous les concepts que nous allons étudier : allocation dynamique, structures chaînées, gestion d'erreurs, et libération propre de la mémoire.

---

## Résumé

L'allocation dynamique est une fonctionnalité essentielle qui permet :
- De créer des programmes flexibles et évolutifs
- De gérer des données de taille variable
- De construire des structures de données complexes

Elle nécessite :
- Une compréhension de la Stack et du Heap
- Une gestion rigoureuse de la mémoire
- L'utilisation d'outils de débogage

Les risques principaux sont :
- Les fuites mémoire
- Les double free
- Les use after free
- Les buffer overflows

> **Règle d'or :** Un malloc = Un free

---

## Prêt à commencer ?

Vous avez maintenant une vue d'ensemble de l'allocation dynamique. Les sections suivantes vont détailler chaque aspect de manière progressive et pratique.

Commençons par comprendre les différences fondamentales entre la Stack et le Heap dans la section 9.1 !

---


⏭️ [Stack vs Heap : Diagramme et différences](/09-allocation-dynamique/01-stack-vs-heap.md)
