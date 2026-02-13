🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.1 Stack vs Heap : Diagramme et différences

## Introduction

Lorsque vous écrivez un programme en C, la mémoire utilisée par votre application est organisée en plusieurs zones distinctes. Deux d'entre elles sont particulièrement importantes pour comprendre comment fonctionne l'allocation mémoire : **la pile (Stack)** et **le tas (Heap)**.

Comprendre la différence entre ces deux zones est essentiel pour :
- Écrire du code performant
- Éviter les bugs liés à la mémoire
- Gérer correctement la durée de vie des variables
- Choisir la bonne stratégie d'allocation

---

## Organisation de la mémoire d'un programme C

Avant de détailler Stack et Heap, visualisons l'organisation globale de la mémoire :

```
Adresses hautes (0xFFFFFFFF...)
┌─────────────────────────────┐
│    Arguments & Variables    │
│      d'environnement        │
├─────────────────────────────┤
│          STACK              │ ← Grandit vers le bas (↓)
│     (Pile d'exécution)      │
│                             │
│            ↓                │
│                             │
│         (espace libre)      │
│                             │
│            ↑                │
│                             │
│          HEAP               │ ← Grandit vers le haut (↑)
│      (Tas dynamique)        │
├─────────────────────────────┤
│    Données non initialisées │
│          (.bss)             │
├─────────────────────────────┤
│     Données initialisées    │
│         (.data)             │
├─────────────────────────────┤
│      Code du programme      │
│         (.text)             │
└─────────────────────────────┘
Adresses basses (0x00000000...)
```

---

## La Stack (Pile d'exécution)

### Définition

La **Stack** est une zone de mémoire organisée selon le principe **LIFO** (Last In, First Out - dernier entré, premier sorti), comme une pile d'assiettes.

### Caractéristiques principales

| Aspect | Description |
|--------|-------------|
| **Gestion** | Automatique par le compilateur |
| **Taille** | Limitée (généralement 1-8 MB sur Linux) |
| **Vitesse** | Très rapide (allocation/libération instantanée) |
| **Durée de vie** | Variables détruites à la fin de leur portée |
| **Organisation** | Contiguë et ordonnée |
| **Fragmentation** | Aucune |

### Ce qui est stocké sur la Stack

1. **Variables locales** déclarées dans les fonctions
2. **Paramètres des fonctions**
3. **Adresses de retour** (pour savoir où revenir après un appel de fonction)
4. **Contexte d'exécution** (registres sauvegardés)

### Exemple de code

```c
#include <stdio.h>

void fonction_exemple(void) {
    int a = 10;        // Variable locale sur la Stack
    char c = 'X';      // Variable locale sur la Stack
    double pi = 3.14;  // Variable locale sur la Stack

    printf("a = %d, c = %c, pi = %.2f\n", a, c, pi);
    // Toutes ces variables seront automatiquement
    // détruites à la fin de cette fonction
}

int main(void) {
    int x = 5;         // Variable locale sur la Stack
    printf("x = %d\n", x);
    fonction_exemple();
    // Ici, les variables de fonction_exemple() n'existent plus
    return 0;
}
```

### Visualisation de la Stack en action

```
État initial du main() :
┌──────────────┐
│   x = 5      │ ← Stack Pointer (SP)
└──────────────┘

Après l'appel à fonction_exemple() :
┌──────────────┐
│  pi = 3.14   │ ← SP
├──────────────┤
│   c = 'X'    │
├──────────────┤
│   a = 10     │
├──────────────┤
│ @retour main │ (adresse pour revenir dans main)
├──────────────┤
│   x = 5      │
└──────────────┘

Après le retour de fonction_exemple() :
┌──────────────┐
│   x = 5      │ ← SP (les variables de la fonction sont "dépilées")
└──────────────┘
```

### Avantages de la Stack

- ✅ **Gestion automatique** : Pas besoin de `free()`, tout est nettoyé automatiquement
- ✅ **Performance optimale** : Allocation et libération ultra-rapides
- ✅ **Pas de fragmentation** : Mémoire toujours contiguë
- ✅ **Sécurité** : Pas de risque de fuite mémoire

### Inconvénients de la Stack

- ❌ **Taille limitée** : Risque de Stack Overflow si trop de données
- ❌ **Durée de vie courte** : Les données disparaissent à la fin de la fonction
- ❌ **Taille fixe à la compilation** : Impossible d'allouer une taille déterminée au runtime

---

## Le Heap (Tas dynamique)

### Définition

Le **Heap** est une grande zone de mémoire libre que votre programme peut utiliser pour allouer dynamiquement de la mémoire pendant son exécution.

### Caractéristiques principales

| Aspect | Description |
|--------|-------------|
| **Gestion** | Manuelle par le programmeur (`malloc`/`free`) |
| **Taille** | Beaucoup plus grande (limitée par la RAM disponible) |
| **Vitesse** | Plus lente que la Stack |
| **Durée de vie** | Jusqu'à ce que vous appeliez `free()` |
| **Organisation** | Peut être fragmentée |
| **Fragmentation** | Possible avec le temps |

### Ce qui est stocké sur le Heap

1. **Données dont la taille n'est connue qu'à l'exécution**
2. **Grandes structures de données** (tableaux de taille importante)
3. **Données qui doivent survivre** au-delà de la fonction qui les a créées
4. **Structures dynamiques** (listes chaînées, arbres, etc.)

### Exemple de code

```c
#include <stdio.h>
#include <stdlib.h>

int* creer_tableau(int taille) {
    // Allocation sur le Heap
    int* tableau = malloc(taille * sizeof(int));

    if (tableau == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return NULL;
    }

    // Initialisation
    for (int i = 0; i < taille; i++) {
        tableau[i] = i * 10;
    }

    return tableau; // Le tableau survit à la fin de la fonction !
}

int main(void) {
    int taille = 100;  // Taille connue seulement à l'exécution

    int* mon_tableau = creer_tableau(taille);

    if (mon_tableau != NULL) {
        printf("Première valeur : %d\n", mon_tableau[0]);

        // ⚠️ IMPORTANT : Libération manuelle nécessaire
        free(mon_tableau);
    }

    return 0;
}
```

### Visualisation du Heap

```
Heap avant malloc() :
┌────────────────────────────────────┐
│     (mémoire libre)                │
│                                    │
│                                    │
└────────────────────────────────────┘

Heap après malloc(100 * sizeof(int)) :
┌────────────────────────────────────┐
│  [Zone allouée: 400 octets]        │ ← Pointeur retourné
│  [metadata de l'allocateur]        │
├────────────────────────────────────┤
│     (mémoire libre restante)       │
│                                    │
└────────────────────────────────────┘

Heap après plusieurs malloc() :
┌────────────────────────────────────┐
│  [Zone 1: 400 octets]              │
├────────────────────────────────────┤
│  [Zone 2: 200 octets]              │
├────────────────────────────────────┤
│  (trou libéré par free)            │ ← Fragmentation !
├────────────────────────────────────┤
│  [Zone 3: 1000 octets]             │
├────────────────────────────────────┤
│     (mémoire libre)                │
└────────────────────────────────────┘
```

### Avantages du Heap

- ✅ **Taille flexible** : Allocation de la taille exacte nécessaire
- ✅ **Durée de vie contrôlée** : Les données persistent tant que vous ne les libérez pas
- ✅ **Grande capacité** : Peut stocker de très grandes quantités de données
- ✅ **Partage de données** : Permet de passer des données entre fonctions facilement

### Inconvénients du Heap

- ❌ **Gestion manuelle** : Vous devez appeler `free()` (risque de fuite mémoire)
- ❌ **Performance** : Plus lent que la Stack
- ❌ **Fragmentation** : Le Heap peut se fragmenter au fil du temps
- ❌ **Complexité** : Plus d'erreurs possibles (double free, use after free, etc.)

---

## Comparaison directe : Stack vs Heap

### Tableau récapitulatif

| Critère | Stack | Heap |
|---------|-------|------|
| **Allocation** | Automatique | Manuelle (`malloc`, `calloc`, `realloc`) |
| **Libération** | Automatique | Manuelle (`free`) |
| **Taille** | Petite (1-8 MB) | Grande (limitée par RAM) |
| **Vitesse d'accès** | Très rapide | Plus lente |
| **Durée de vie** | Limitée à la portée | Contrôlée par le programmeur |
| **Organisation** | LIFO (ordonnée) | Non ordonnée |
| **Fragmentation** | Non | Oui (possible) |
| **Sécurité** | Plus sûre | Risques de fuites et erreurs |
| **Utilisation** | Variables locales | Grandes données, durée indéterminée |

### Exemple illustrant les deux

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exemple_complet(void) {
    // ===== STACK =====
    int age = 25;                    // Sur la Stack
    char prenom[20] = "Alice";       // Sur la Stack (taille fixe)

    printf("Stack - Age: %d, Prénom: %s\n", age, prenom);

    // ===== HEAP =====
    // Allocation dynamique pour une chaîne de taille variable
    int longueur = 100;
    char* description = malloc(longueur * sizeof(char));

    if (description == NULL) {
        printf("Erreur d'allocation\n");
        return;
    }

    strcpy(description, "Ceci est une description sur le Heap");
    printf("Heap - Description: %s\n", description);

    // ⚠️ Libération obligatoire
    free(description);

    // À la sortie de cette fonction :
    // - age et prenom (Stack) sont automatiquement détruits
    // - description (Heap) a été manuellement libéré avec free()
}

int main(void) {
    exemple_complet();
    return 0;
}
```

---

## Durée de vie et performance

### Durée de vie

#### Stack
```c
void exemple_stack(void) {
    int x = 10;  // x existe UNIQUEMENT dans cette fonction
    // ...
}  // ← x est DÉTRUITE ici automatiquement
```

#### Heap
```c
int* exemple_heap(void) {
    int* ptr = malloc(sizeof(int));
    *ptr = 10;
    return ptr;  // ptr survit et peut être utilisée ailleurs
}

int main(void) {
    int* valeur = exemple_heap();
    printf("%d\n", *valeur);
    free(valeur);  // Libération quand on n'en a plus besoin
    return 0;
}
```

### Performance en chiffres (ordre de grandeur)

| Opération | Stack | Heap |
|-----------|-------|------|
| **Allocation** | ~1 nanoseconde | ~100 nanosecondes |
| **Accès** | ~1-2 ns (cache L1) | ~1-10 ns (dépend de la localité) |
| **Libération** | Instantanée | ~100 nanosecondes |

**La Stack est environ 100 fois plus rapide pour l'allocation/libération !**

---

## Quand utiliser Stack ou Heap ?

### Utilisez la Stack quand :

- ✅ Vous connaissez la taille à la compilation
- ✅ Les données sont petites (quelques Ko maximum)
- ✅ Les données n'ont besoin d'exister que dans la fonction actuelle
- ✅ Vous voulez des performances optimales

**Exemples :**
- Compteurs, indices de boucle
- Petits tableaux de taille fixe
- Variables temporaires
- Paramètres de fonction

### Utilisez le Heap quand :

- ✅ La taille n'est connue qu'à l'exécution
- ✅ Les données sont volumineuses (>quelques Ko)
- ✅ Les données doivent survivre au-delà de la fonction
- ✅ Vous créez des structures dynamiques (listes, arbres)

**Exemples :**
- Lecture d'un fichier de taille inconnue
- Images, vidéos, gros tableaux
- Structures de données partagées entre fonctions
- Objets créés dynamiquement

---

## Erreurs courantes liées à Stack et Heap

### ❌ Erreur 1 : Retourner un pointeur vers une variable locale (Stack)

```c
// ⚠️ CODE DANGEREUX - NE PAS FAIRE !
int* mauvaise_fonction(void) {
    int x = 42;
    return &x;  // ❌ x sera détruite à la fin de la fonction !
}

int main(void) {
    int* ptr = mauvaise_fonction();
    printf("%d\n", *ptr);  // ⚠️ Comportement indéfini ! (dangling pointer)
    return 0;
}
```

**Solution : Utiliser le Heap**
```c
// ✅ CODE CORRECT
int* bonne_fonction(void) {
    int* x = malloc(sizeof(int));
    *x = 42;
    return x;  // ✅ La mémoire sur le Heap persiste
}

int main(void) {
    int* ptr = bonne_fonction();
    printf("%d\n", *ptr);  // ✅ Fonctionne correctement
    free(ptr);             // N'oubliez pas de libérer !
    return 0;
}
```

### ❌ Erreur 2 : Stack Overflow (dépassement de la pile)

```c
// ⚠️ Risque de Stack Overflow
void recursion_infinie(void) {
    int tableau[1000000];  // Énorme tableau sur la Stack
    recursion_infinie();   // Appel récursif sans condition d'arrêt
}
```

**Solutions :**
- Limiter la récursion
- Utiliser le Heap pour les grandes données
- Augmenter la taille de la Stack (pas recommandé)

### ❌ Erreur 3 : Oublier de libérer le Heap (fuite mémoire)

```c
// ⚠️ FUITE MÉMOIRE
void fuite(void) {
    char* data = malloc(1000);
    strcpy(data, "Hello");
    // ❌ Oubli de free(data) !
}  // La mémoire reste allouée indéfiniment
```

**Solution :**
```c
// ✅ CODE CORRECT
void correct(void) {
    char* data = malloc(1000);
    if (data == NULL) return;

    strcpy(data, "Hello");

    free(data);  // ✅ Libération de la mémoire
}
```

---

## Diagramme complet : Cycle de vie d'une variable

```
Programme C avec Stack et Heap :

┌──────────────────────────────────────────────────────────┐
│                    MÉMOIRE DU PROCESSUS                  │
├──────────────────────────────────────────────────────────┤
│                                                          │
│  STACK (1-8 MB)                    HEAP (flexible)       │
│  ┌────────────┐                    ┌─────────────────┐   │
│  │ main()     │                    │ malloc(1000)    │   │
│  │  int a=5   │◄─────auto          │   ↑             │   │
│  │  int b=10  │      gestion       │   │ persiste    │   │
│  └────────────┘                    │   │ jusqu'à     │   │
│       │                            │   │ free()      │   │
│       │ appel fonction             │   │             │   │
│       ↓                            └───┼─────────────┘   │
│  ┌────────────┐                        │                 │
│  │ func()     │                        │                 │
│  │  int x=7   │                        │                 │
│  │  int y=9   │                   retourne le            │
│  └────────────┘                   pointeur vers          │
│       │                           cette zone             │
│       │ retour                                           │
│       ↓                                                  │
│  ┌────────────┐                                          │
│  │ main()     │                                          │
│  │  int a=5   │                                          │
│  │  int b=10  │                                          │
│  └────────────┘                                          │
│                                                          │
│  x et y ont disparu automatiquement                      │
│  La mémoire Heap reste allouée tant qu'on ne free() pas  │
└──────────────────────────────────────────────────────────┘
```

---

## Récapitulatif

### Points clés à retenir

1. **Stack** = Mémoire automatique, rapide, limitée, locale
2. **Heap** = Mémoire manuelle, flexible, grande, persistante
3. **Stack** pour les variables locales de petite taille
4. **Heap** pour les données dynamiques et volumineuses
5. Toujours **libérer la mémoire Heap** avec `free()`
6. Ne jamais retourner l'adresse d'une **variable locale Stack**

### Règle d'or

> **Si vous ne savez pas quelle taille vous aurez besoin à la compilation, utilisez le Heap. Sinon, préférez la Stack pour les performances et la simplicité.**

---

## Ressources pour aller plus loin

- `man malloc` : Documentation des fonctions d'allocation dynamique
- `man free` : Libération de mémoire
- Outils de détection : Valgrind (Module 5, Section 15.4)
- Prochaine section : **9.2 Durée de vie et performance** (approfondissement)

---


⏭️ [Durée de vie et performance](/09-allocation-dynamique/02-duree-vie-performance.md)
