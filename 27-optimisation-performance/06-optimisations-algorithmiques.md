🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.6 Optimisations algorithmiques

## Introduction

Les **optimisations algorithmiques** consistent à choisir ou concevoir des algorithmes plus efficaces pour résoudre un problème donné. C'est souvent **le levier d'optimisation le plus puissant** : un bon algorithme peut être **100 à 1000 fois plus rapide** qu'un mauvais algorithme, même sans aucune autre optimisation !

### La hiérarchie des optimisations

```
1. Algorithme efficace         ← Impact : 100-1000x
2. Structures de données       ← Impact : 10-100x
3. Optimisations cache         ← Impact : 2-10x
4. Optimisations compilateur   ← Impact : 1.5-3x
5. Optimisations assembleur    ← Impact : 1.1-1.5x
```

**Citation célèbre :**

> "Premature optimization is the root of all evil, but choosing a bad algorithm is the root of all performance problems" — Adapté de Donald Knuth

### Analogie : Choisir le bon véhicule

Imaginez que vous devez aller de Paris à Marseille (800 km) :

- **Marche à pied** : 200 heures (mauvais algorithme)
- **Vélo** : 40 heures (algorithme lent)
- **Voiture** : 8 heures (bon algorithme)
- **TGV** : 3 heures (algorithme optimal)

Optimiser la marche à pied (chaussures plus légères, meilleure technique) ne vous fera **jamais** battre le TGV. Il faut d'abord choisir le bon moyen de transport (algorithme) !

---

## Complexité algorithmique : Notation Big-O

La **notation Big-O** décrit comment le temps d'exécution d'un algorithme évolue quand la taille des données augmente.

### Notations courantes

| Notation | Nom | Exemple | Évolution |
|----------|-----|---------|-----------|
| **O(1)** | Constant | Accès tableau par index | Toujours le même temps |
| **O(log n)** | Logarithmique | Recherche dichotomique | Très lent |
| **O(n)** | Linéaire | Parcours de tableau | Proportionnel à n |
| **O(n log n)** | Linéarithmique | Tri rapide (quicksort) | Acceptable |
| **O(n²)** | Quadratique | Tri à bulles | Rapide pour petites données |
| **O(n³)** | Cubique | Multiplication matrices naïve | Lent |
| **O(2ⁿ)** | Exponentiel | Problème du voyageur (brute force) | Catastrophique |
| **O(n!)** | Factoriel | Permutations complètes | Inutilisable au-delà de n=10 |

### Visualisation de la croissance

```
Temps (pour n éléments)

      │
1000s │                                                    O(2ⁿ)
      │                                                 ╱
      │                                              ╱
100s  │                                          O(n²)
      │                                       ╱
      │                                   ╱
10s   │                              ╱
      │                          ╱ O(n log n)
1s    │                     ╱──────
      │              ╱──────
0.1s  │      O(log n)  O(n)
      │  ──────────────
      └─────────────────────────────────────────────────> n
      10   100  1000  10000  100000
```

### Exemple concret

Pour **n = 1,000,000** éléments :

- **O(1)** : 1 opération (instantané)
- **O(log n)** : ~20 opérations (instantané)
- **O(n)** : 1,000,000 opérations (~1 ms)
- **O(n log n)** : ~20,000,000 opérations (~20 ms)
- **O(n²)** : 1,000,000,000,000 opérations (~17 minutes !)
- **O(2ⁿ)** : ... l'univers n'est pas assez vieux

**Conclusion :** Avec de grandes données, seuls les algorithmes O(1), O(log n), O(n) et O(n log n) sont viables.

---

## Exemples classiques : Bons vs mauvais algorithmes

### Exemple 1 : Recherche dans un tableau

#### Recherche linéaire : O(n)

```c
// Chercher une valeur dans un tableau non trié
int recherche_lineaire(int *tableau, int taille, int cible) {
    for (int i = 0; i < taille; i++) {
        if (tableau[i] == cible) {
            return i;  // Trouvé !
        }
    }
    return -1;  // Non trouvé
}
```

**Complexité :** O(n) — Dans le pire cas, on doit parcourir tout le tableau.

**Performance :**
- 1,000 éléments : ~1,000 comparaisons
- 1,000,000 éléments : ~1,000,000 comparaisons

#### Recherche dichotomique : O(log n)

**Condition :** Le tableau doit être **trié** !

```c
// Chercher dans un tableau trié
int recherche_dichotomique(int *tableau, int taille, int cible) {
    int gauche = 0;
    int droite = taille - 1;

    while (gauche <= droite) {
        int milieu = gauche + (droite - gauche) / 2;

        if (tableau[milieu] == cible) {
            return milieu;  // Trouvé !
        }

        if (tableau[milieu] < cible) {
            gauche = milieu + 1;  // Chercher à droite
        } else {
            droite = milieu - 1;  // Chercher à gauche
        }
    }

    return -1;  // Non trouvé
}
```

**Complexité :** O(log n) — On divise l'espace de recherche par 2 à chaque étape.

**Performance :**
- 1,000 éléments : ~10 comparaisons
- 1,000,000 éléments : ~20 comparaisons
- 1,000,000,000 éléments : ~30 comparaisons

**Benchmark :**

```c
// test_recherche.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 1000000

int main() {
    int *tableau = malloc(TAILLE * sizeof(int));

    // Remplir avec valeurs triées
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = i;
    }

    clock_t debut, fin;
    int resultat;

    // Recherche linéaire
    debut = clock();
    for (int i = 0; i < 1000; i++) {  // 1000 recherches
        resultat = recherche_lineaire(tableau, TAILLE, TAILLE - 1);
    }
    fin = clock();
    printf("Recherche linéaire: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    // Recherche dichotomique
    debut = clock();
    for (int i = 0; i < 1000; i++) {  // 1000 recherches
        resultat = recherche_dichotomique(tableau, TAILLE, TAILLE - 1);
    }
    fin = clock();
    printf("Recherche dichotomique: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    free(tableau);
    return 0;
}
```

**Résultats typiques :**
```
Recherche linéaire: 3250.5 ms
Recherche dichotomique: 0.8 ms  ← 4000x plus rapide !
```

**Conclusion :** Si vous faites beaucoup de recherches, **triez d'abord** le tableau, puis utilisez la recherche dichotomique.

---

### Exemple 2 : Tri de données

#### Tri à bulles : O(n²)

```c
void tri_bulles(int *tableau, int taille) {
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (tableau[j] > tableau[j + 1]) {
                // Échanger
                int temp = tableau[j];
                tableau[j] = tableau[j + 1];
                tableau[j + 1] = temp;
            }
        }
    }
}
```

**Complexité :** O(n²) — Double boucle imbriquée.

**Performance :**
- 1,000 éléments : ~1,000,000 comparaisons (~10 ms)
- 10,000 éléments : ~100,000,000 comparaisons (~1 seconde)
- 100,000 éléments : ~10,000,000,000 comparaisons (~2 minutes)

#### Quicksort : O(n log n)

```c
int partition(int *tableau, int bas, int haut) {
    int pivot = tableau[haut];
    int i = bas - 1;

    for (int j = bas; j < haut; j++) {
        if (tableau[j] < pivot) {
            i++;
            // Échanger tableau[i] et tableau[j]
            int temp = tableau[i];
            tableau[i] = tableau[j];
            tableau[j] = temp;
        }
    }

    // Placer le pivot à sa position finale
    int temp = tableau[i + 1];
    tableau[i + 1] = tableau[haut];
    tableau[haut] = temp;

    return i + 1;
}

void quicksort(int *tableau, int bas, int haut) {
    if (bas < haut) {
        int pi = partition(tableau, bas, haut);
        quicksort(tableau, bas, pi - 1);
        quicksort(tableau, pi + 1, haut);
    }
}

// Fonction d'entrée
void tri_rapide(int *tableau, int taille) {
    quicksort(tableau, 0, taille - 1);
}
```

**Complexité :** O(n log n) en moyenne.

**Performance :**
- 1,000 éléments : ~10,000 comparaisons (~0.1 ms)
- 10,000 éléments : ~133,000 comparaisons (~1.3 ms)
- 100,000 éléments : ~1,700,000 comparaisons (~17 ms)

**Benchmark :**

```c
// test_tri.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAILLE 10000

void generer_aleatoire(int *tableau, int taille) {
    for (int i = 0; i < taille; i++) {
        tableau[i] = rand();
    }
}

int main() {
    int *tableau1 = malloc(TAILLE * sizeof(int));
    int *tableau2 = malloc(TAILLE * sizeof(int));

    generer_aleatoire(tableau1, TAILLE);
    memcpy(tableau2, tableau1, TAILLE * sizeof(int));

    clock_t debut, fin;

    // Tri à bulles
    debut = clock();
    tri_bulles(tableau1, TAILLE);
    fin = clock();
    printf("Tri à bulles: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    // Quicksort
    debut = clock();
    tri_rapide(tableau2, TAILLE);
    fin = clock();
    printf("Quicksort: %.3f ms\n",
           (double)(fin - debut) * 1000 / CLOCKS_PER_SEC);

    free(tableau1);
    free(tableau2);
    return 0;
}
```

**Résultats typiques :**
```
Tri à bulles: 485.3 ms
Quicksort: 1.2 ms  ← 400x plus rapide !
```

**Conclusion :** Pour trier, utilisez toujours `qsort()` de la stdlib (ou un algorithme O(n log n)). Jamais de tri à bulles en production !

---

### Exemple 3 : Vérifier les doublons

#### Algorithme naïf : O(n²)

```c
// Vérifier si un tableau contient des doublons
int contient_doublons_naif(int *tableau, int taille) {
    for (int i = 0; i < taille; i++) {
        for (int j = i + 1; j < taille; j++) {
            if (tableau[i] == tableau[j]) {
                return 1;  // Doublon trouvé
            }
        }
    }
    return 0;  // Pas de doublon
}
```

**Complexité :** O(n²) — Double boucle pour comparer chaque paire.

#### Avec tri : O(n log n)

```c
int contient_doublons_tri(int *tableau, int taille) {
    // Copier pour ne pas modifier l'original
    int *copie = malloc(taille * sizeof(int));
    memcpy(copie, tableau, taille * sizeof(int));

    // Trier
    qsort(copie, taille, sizeof(int), compare);

    // Vérifier les éléments adjacents
    int resultat = 0;
    for (int i = 0; i < taille - 1; i++) {
        if (copie[i] == copie[i + 1]) {
            resultat = 1;  // Doublon trouvé
            break;
        }
    }

    free(copie);
    return resultat;
}
```

**Complexité :** O(n log n) pour le tri + O(n) pour la vérification = O(n log n)

#### Avec table de hachage : O(n)

```c
#include <stdbool.h>

// Utiliser une table de hachage simple (limitée aux valeurs positives < MAX)
#define MAX_VAL 1000000

int contient_doublons_hash(int *tableau, int taille) {
    bool *vu = calloc(MAX_VAL, sizeof(bool));

    for (int i = 0; i < taille; i++) {
        if (tableau[i] >= 0 && tableau[i] < MAX_VAL) {
            if (vu[tableau[i]]) {
                free(vu);
                return 1;  // Doublon trouvé
            }
            vu[tableau[i]] = true;
        }
    }

    free(vu);
    return 0;  // Pas de doublon
}
```

**Complexité :** O(n) — Un seul parcours du tableau.

**Performance :**

| Taille | Naïf O(n²) | Tri O(n log n) | Hash O(n) |
|--------|------------|----------------|-----------|
| 1,000 | 10 ms | 0.5 ms | 0.1 ms |
| 10,000 | 1000 ms | 5 ms | 1 ms |
| 100,000 | 100,000 ms | 60 ms | 10 ms |

**Conclusion :** La table de hachage est la plus rapide, mais nécessite de la mémoire supplémentaire.

---

## Structures de données et performances

Le choix de la structure de données a un impact énorme sur les performances.

### Tableau dynamique vs Liste chaînée

| Opération | Tableau | Liste chaînée |
|-----------|---------|---------------|
| Accès par index | O(1) ✅ | O(n) ❌ |
| Insertion début | O(n) ❌ | O(1) ✅ |
| Insertion fin | O(1)* | O(1)* |
| Suppression début | O(n) ❌ | O(1) ✅ |
| Suppression milieu | O(n) | O(n) |
| Recherche | O(n) | O(n) |
| Cache-friendly | Oui ✅ | Non ❌ |

\* Amortie pour le tableau (avec reallocation), O(1) si on garde un pointeur vers la fin pour la liste.

**Règle générale :**
- **Tableau** : Si vous accédez souvent par index et ajoutez/supprimez rarement
- **Liste chaînée** : Si vous insérez/supprimez fréquemment au début

**En pratique :** Les tableaux sont presque toujours plus rapides grâce à la localité cache, même pour des insertions/suppressions.

### Table de hachage vs Arbre binaire de recherche

| Opération | Table de hachage | Arbre équilibré (AVL, Red-Black) |
|-----------|------------------|----------------------------------|
| Recherche | O(1) ✅ | O(log n) |
| Insertion | O(1) ✅ | O(log n) |
| Suppression | O(1) ✅ | O(log n) |
| Ordre trié | Non ❌ | Oui ✅ |
| Parcours ordonné | O(n log n) | O(n) ✅ |
| Mémoire | Plus ❌ | Moins ✅ |

**Règle générale :**
- **Table de hachage** : Si vous n'avez pas besoin d'ordre
- **Arbre** : Si vous devez maintenir un ordre ou faire des parcours triés

---

## Cas d'étude : Comptage de mots

### Problème : Compter les occurrences de chaque mot dans un texte

#### Solution 1 : Tableau de structures (naïf)

```c
struct Mot {
    char texte[50];
    int compte;
};

void compter_mots_naif(char **mots, int nb_mots) {
    struct Mot *compteurs = malloc(nb_mots * sizeof(struct Mot));
    int nb_uniques = 0;

    for (int i = 0; i < nb_mots; i++) {
        // Chercher si le mot existe déjà
        int trouve = 0;
        for (int j = 0; j < nb_uniques; j++) {
            if (strcmp(mots[i], compteurs[j].texte) == 0) {
                compteurs[j].compte++;
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            strcpy(compteurs[nb_uniques].texte, mots[i]);
            compteurs[nb_uniques].compte = 1;
            nb_uniques++;
        }
    }

    free(compteurs);
}
```

**Complexité :** O(n²) — Pour chaque mot, on parcourt le tableau des mots uniques.

**Performance :**
- 10,000 mots : ~5 secondes
- 100,000 mots : ~500 secondes (8 minutes !)

#### Solution 2 : Tri + comptage

```c
void compter_mots_tri(char **mots, int nb_mots) {
    // Trier les mots
    qsort(mots, nb_mots, sizeof(char*), compare_strings);

    // Compter les mots adjacents identiques
    char *mot_courant = mots[0];
    int compte = 1;

    for (int i = 1; i < nb_mots; i++) {
        if (strcmp(mots[i], mot_courant) == 0) {
            compte++;
        } else {
            printf("%s: %d\n", mot_courant, compte);
            mot_courant = mots[i];
            compte = 1;
        }
    }
    printf("%s: %d\n", mot_courant, compte);
}
```

**Complexité :** O(n log n) pour le tri + O(n) pour le comptage = O(n log n)

**Performance :**
- 10,000 mots : ~10 ms
- 100,000 mots : ~120 ms

**Amélioration :** 500x plus rapide !

#### Solution 3 : Table de hachage (optimal)

```c
#include <uthash.h>  // Bibliothèque de table de hachage

struct MotHash {
    char texte[50];      // Clé
    int compte;          // Valeur
    UT_hash_handle hh;   // Handle pour uthash
};

void compter_mots_hash(char **mots, int nb_mots) {
    struct MotHash *hash = NULL;

    for (int i = 0; i < nb_mots; i++) {
        struct MotHash *element;

        // Chercher dans la table de hachage
        HASH_FIND_STR(hash, mots[i], element);

        if (element == NULL) {
            // Nouveau mot
            element = malloc(sizeof(struct MotHash));
            strcpy(element->texte, mots[i]);
            element->compte = 1;
            HASH_ADD_STR(hash, texte, element);
        } else {
            // Mot existant
            element->compte++;
        }
    }

    // Afficher les résultats
    struct MotHash *element, *tmp;
    HASH_ITER(hh, hash, element, tmp) {
        printf("%s: %d\n", element->texte, element->compte);
        HASH_DEL(hash, element);
        free(element);
    }
}
```

**Complexité :** O(n) — Un seul parcours avec des opérations O(1) sur la table de hachage.

**Performance :**
- 10,000 mots : ~2 ms
- 100,000 mots : ~20 ms

**Amélioration :** 25000x plus rapide que la solution naïve !

---

## Memoization : Éviter les calculs redondants

### Exemple : Suite de Fibonacci

#### Version récursive naïve : O(2ⁿ)

```c
int fibonacci_naif(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci_naif(n - 1) + fibonacci_naif(n - 2);
}
```

**Problème :** Calcule plusieurs fois les mêmes valeurs !

```
fibonacci(5)
├── fibonacci(4)
│   ├── fibonacci(3)
│   │   ├── fibonacci(2)
│   │   │   ├── fibonacci(1) ← Calculé
│   │   │   └── fibonacci(0) ← Calculé
│   │   └── fibonacci(1) ← Recalculé !
│   └── fibonacci(2)
│       ├── fibonacci(1) ← Recalculé !
│       └── fibonacci(0) ← Recalculé !
└── fibonacci(3)
    ├── fibonacci(2)
    │   ├── fibonacci(1) ← Recalculé !
    │   └── fibonacci(0) ← Recalculé !
    └── fibonacci(1) ← Recalculé !
```

**Performance :**
- fibonacci(10) : ~177 appels
- fibonacci(20) : ~21,891 appels
- fibonacci(40) : ~2,692,537,283 appels (~10 secondes !)
- fibonacci(50) : ... des heures

#### Version avec memoization : O(n)

```c
#define MAX_N 100
long long memo[MAX_N];

void init_memo() {
    for (int i = 0; i < MAX_N; i++) {
        memo[i] = -1;  // -1 = pas encore calculé
    }
}

long long fibonacci_memo(int n) {
    if (n <= 1) {
        return n;
    }

    // Déjà calculé ?
    if (memo[n] != -1) {
        return memo[n];
    }

    // Calculer et mémoriser
    memo[n] = fibonacci_memo(n - 1) + fibonacci_memo(n - 2);
    return memo[n];
}
```

**Performance :**
- fibonacci(10) : 19 appels
- fibonacci(20) : 39 appels
- fibonacci(40) : 79 appels (~0.0001 secondes)
- fibonacci(50) : 99 appels (~0.0001 secondes)

**Amélioration :** 100,000x plus rapide pour n=40 !

#### Version itérative : O(n), encore plus efficace

```c
long long fibonacci_iteratif(int n) {
    if (n <= 1) return n;

    long long prev = 0, curr = 1;

    for (int i = 2; i <= n; i++) {
        long long next = prev + curr;
        prev = curr;
        curr = next;
    }

    return curr;
}
```

**Avantages :**
- Pas de récursion (pas de risque de stack overflow)
- Pas besoin de mémoire supplémentaire (juste 2 variables)
- Plus cache-friendly

---

## Algorithmes de recherche de motifs

### Recherche naïve dans une chaîne : O(n×m)

```c
// Chercher un motif dans un texte
int recherche_naive(const char *texte, const char *motif) {
    int n = strlen(texte);
    int m = strlen(motif);

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (texte[i + j] != motif[j]) {
                break;
            }
        }
        if (j == m) {
            return i;  // Trouvé à la position i
        }
    }

    return -1;  // Non trouvé
}
```

**Complexité :** O(n×m) où n = longueur du texte, m = longueur du motif

### Algorithme KMP (Knuth-Morris-Pratt) : O(n+m)

**Idée :** Utiliser les informations déjà comparées pour éviter de recomparer.

```c
// Calculer la table de préfixes
void calculer_lps(const char *motif, int m, int *lps) {
    int longueur = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m) {
        if (motif[i] == motif[longueur]) {
            longueur++;
            lps[i] = longueur;
            i++;
        } else {
            if (longueur != 0) {
                longueur = lps[longueur - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

int recherche_kmp(const char *texte, const char *motif) {
    int n = strlen(texte);
    int m = strlen(motif);

    int *lps = malloc(m * sizeof(int));
    calculer_lps(motif, m, lps);

    int i = 0;  // Index pour texte
    int j = 0;  // Index pour motif

    while (i < n) {
        if (motif[j] == texte[i]) {
            i++;
            j++;
        }

        if (j == m) {
            free(lps);
            return i - j;  // Trouvé
        } else if (i < n && motif[j] != texte[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    return -1;  // Non trouvé
}
```

**Complexité :** O(n + m) — Plus efficace pour de longs textes et motifs.

**Performance :**
- Texte de 1,000,000 caractères, motif de 100 caractères
- Naïf : ~100 ms
- KMP : ~15 ms

**Note :** En pratique, `strstr()` de la stdlib utilise souvent Boyer-Moore ou d'autres algorithmes optimisés.

---

## Diviser pour régner (Divide and Conquer)

### Principe

Diviser un gros problème en petits sous-problèmes plus faciles à résoudre.

**Exemples d'algorithmes :**
- Quicksort
- Mergesort
- Recherche dichotomique
- Multiplication de matrices de Strassen

### Cas d'étude : Trouver le maximum dans un tableau

#### Version linéaire : O(n)

```c
int trouver_max(int *tableau, int taille) {
    int max = tableau[0];
    for (int i = 1; i < taille; i++) {
        if (tableau[i] > max) {
            max = tableau[i];
        }
    }
    return max;
}
```

#### Version diviser pour régner : O(n) aussi, mais parallélisable

```c
int trouver_max_divide(int *tableau, int debut, int fin) {
    // Cas de base : un seul élément
    if (debut == fin) {
        return tableau[debut];
    }

    // Diviser
    int milieu = debut + (fin - debut) / 2;

    // Conquérir récursivement
    int max_gauche = trouver_max_divide(tableau, debut, milieu);
    int max_droite = trouver_max_divide(tableau, milieu + 1, fin);

    // Combiner
    return (max_gauche > max_droite) ? max_gauche : max_droite;
}
```

**Avantage :** Cette version est facilement parallélisable (on peut calculer max_gauche et max_droite en parallèle).

---

## Programmation dynamique

### Principe

Résoudre un problème en le décomposant en sous-problèmes **qui se chevauchent**, et mémoriser les résultats (memoization ou tabulation).

**Différence avec diviser pour régner :**
- **Diviser pour régner** : Sous-problèmes **indépendants**
- **Programmation dynamique** : Sous-problèmes **qui se chevauchent**

### Exemple : Problème du sac à dos (Knapsack)

**Problème :** Vous avez un sac avec une capacité maximale et des objets avec poids et valeurs. Maximiser la valeur totale sans dépasser la capacité.

#### Solution récursive naïve : O(2ⁿ)

```c
int knapsack_naif(int capacite, int *poids, int *valeurs, int n) {
    // Cas de base
    if (n == 0 || capacite == 0) {
        return 0;
    }

    // Si le dernier objet est trop lourd
    if (poids[n - 1] > capacite) {
        return knapsack_naif(capacite, poids, valeurs, n - 1);
    }

    // Comparer : inclure vs exclure l'objet
    int avec = valeurs[n - 1] +
               knapsack_naif(capacite - poids[n - 1], poids, valeurs, n - 1);
    int sans = knapsack_naif(capacite, poids, valeurs, n - 1);

    return (avec > sans) ? avec : sans;
}
```

**Performance :** Exponentiel, inutilisable pour n > 20.

#### Solution avec programmation dynamique : O(n×capacité)

```c
int knapsack_dp(int capacite, int *poids, int *valeurs, int n) {
    // Table pour mémoriser les résultats
    int **dp = malloc((n + 1) * sizeof(int*));
    for (int i = 0; i <= n; i++) {
        dp[i] = calloc(capacite + 1, sizeof(int));
    }

    // Remplir la table de manière itérative
    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= capacite; w++) {
            if (poids[i - 1] <= w) {
                // On peut inclure l'objet
                int avec = valeurs[i - 1] + dp[i - 1][w - poids[i - 1]];
                int sans = dp[i - 1][w];
                dp[i][w] = (avec > sans) ? avec : sans;
            } else {
                // Trop lourd
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    int resultat = dp[n][capacite];

    // Libérer la mémoire
    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    return resultat;
}
```

**Performance :**
- n=20, capacité=100 : ~0.001 secondes (vs plusieurs minutes pour la version naïve)
- n=100, capacité=1000 : ~0.1 secondes

---

## Algorithmes gloutons (Greedy)

### Principe

À chaque étape, faire le choix **localement optimal** en espérant qu'il mène au résultat global optimal.

**Avantages :**
- ✅ Simples à implémenter
- ✅ Rapides (souvent O(n log n))

**Inconvénients :**
- ❌ Ne garantissent pas toujours l'optimum
- ❌ Difficile de prouver la correction

### Exemple : Problème du rendu de monnaie

**Problème :** Rendre une somme avec le minimum de pièces.

**Système de pièces européen :** 1, 2, 5, 10, 20, 50 centimes, 1€, 2€

#### Algorithme glouton

```c
void rendre_monnaie(int montant, int *pieces, int nb_pieces) {
    int compte[nb_pieces];
    for (int i = 0; i < nb_pieces; i++) {
        compte[i] = 0;
    }

    // Trier les pièces par ordre décroissant
    qsort(pieces, nb_pieces, sizeof(int), compare_desc);

    // Prendre la plus grosse pièce possible à chaque fois
    for (int i = 0; i < nb_pieces; i++) {
        while (montant >= pieces[i]) {
            montant -= pieces[i];
            compte[i]++;
        }
    }

    // Afficher le résultat
    for (int i = 0; i < nb_pieces; i++) {
        if (compte[i] > 0) {
            printf("%d pièce(s) de %d centimes\n", compte[i], pieces[i]);
        }
    }
}
```

**Exemple :** Rendre 87 centimes
- 1 pièce de 50
- 1 pièce de 20
- 1 pièce de 10
- 1 pièce de 5
- 1 pièce de 2
**Total : 5 pièces** (optimal ✅)

**Note :** L'algorithme glouton fonctionne pour le système européen, mais pas pour tous les systèmes de pièces !

**Contre-exemple :**
Pièces : {1, 3, 4}, montant : 6
- Glouton : 4 + 1 + 1 = **3 pièces**
- Optimal : 3 + 3 = **2 pièces** ❌

---

## Checklist pour choisir un algorithme

Avant d'implémenter :

1. ✅ **Quelle est la taille typique des données ?**
   - Petite (n < 100) : Même un O(n²) va bien
   - Moyenne (100 < n < 10,000) : O(n log n) recommandé
   - Grande (n > 10,000) : O(n) ou O(n log n) obligatoire

2. ✅ **Quelle opération est la plus fréquente ?**
   - Recherche → Tri + recherche dichotomique ou table de hachage
   - Insertion/suppression → Liste ou arbre équilibré
   - Accès par index → Tableau

3. ✅ **Y a-t-il des contraintes mémoire ?**
   - Peu de mémoire → Algorithme en place (quicksort vs mergesort)
   - Beaucoup de mémoire → Table de hachage, memoization

4. ✅ **Les données ont-elles des propriétés particulières ?**
   - Déjà triées → Profiter du tri
   - Beaucoup de doublons → Compression, table de hachage
   - Pattern prévisible → Profiter du pattern

5. ✅ **Ai-je besoin de l'optimum ou d'une approximation suffit ?**
   - Optimum → Algorithme exact (parfois coûteux)
   - Approximation acceptable → Algorithme glouton ou heuristique

---

## Résumé des algorithmes par problème

| Problème | Algorithme | Complexité | Notes |
|----------|-----------|------------|-------|
| **Tri** | Quicksort, Mergesort | O(n log n) | Utiliser `qsort()` |
| **Recherche (non trié)** | Linéaire | O(n) | Trier si recherches fréquentes |
| **Recherche (trié)** | Dichotomique | O(log n) | Très rapide |
| **Recherche (hash)** | Table de hachage | O(1) | Le plus rapide |
| **Min/Max** | Parcours linéaire | O(n) | Optimal |
| **Kième plus petit** | QuickSelect | O(n) en moyenne | Plus rapide que tri complet |
| **Plus court chemin** | Dijkstra | O((V+E) log V) | Graphes pondérés |
| **Arbre couvrant minimum** | Kruskal, Prim | O(E log V) | Graphes |
| **Multiplication matrices** | Strassen | O(n^2.807) | Pour très grandes matrices |

---

## Outils pour analyser la complexité

### Analyse théorique

Compter les opérations en fonction de n :

```c
// Exemple
for (int i = 0; i < n; i++) {           // n itérations
    for (int j = 0; j < n; j++) {       // n itérations
        tableau[i][j] = i + j;          // O(1)
    }
}
// Complexité : O(n²)
```

### Analyse empirique

Mesurer le temps d'exécution pour différentes valeurs de n :

```c
// test_complexite.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void algorithme(int n) {
    // Votre algorithme ici
}

int main() {
    int tailles[] = {100, 1000, 10000, 100000};

    for (int i = 0; i < 4; i++) {
        int n = tailles[i];

        clock_t debut = clock();
        algorithme(n);
        clock_t fin = clock();

        double temps = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("n = %6d : %.6f secondes\n", n, temps);
    }

    return 0;
}
```

**Analyser la croissance :**
- Si le temps double quand n double → O(n)
- Si le temps quadruple quand n double → O(n²)
- Si le temps augmente faiblement → O(log n) ou O(n log n)

---

## Erreurs courantes à éviter

### ❌ Optimiser prématurément

**Problème :** Passer du temps à micro-optimiser un algorithme O(n) alors qu'un algorithme O(n²) domine le temps d'exécution.

**Solution :** Profiler d'abord, identifier les vrais goulots, puis optimiser.

### ❌ Choisir une structure de données inadaptée

**Exemple :** Utiliser une liste chaînée pour faire beaucoup d'accès par index.

**Solution :** Choisir la structure selon les opérations les plus fréquentes.

### ❌ Ignorer les constantes et le surcoût

Un algorithme O(n log n) avec une grosse constante peut être plus lent qu'un O(n²) bien optimisé pour de petites valeurs de n.

**Exemple :** Quicksort vs Insertion sort
- Quicksort : O(n log n), mais overhead de récursion
- Insertion sort : O(n²), mais très rapide pour n < 20

**Solution pratique :** Hybrides comme Timsort (utilisé en Python) qui combine plusieurs algorithmes selon la taille.

### ❌ Négliger la localité de cache

Un algorithme théoriquement optimal mais cache-hostile peut être plus lent qu'un algorithme moins optimal mais cache-friendly.

**Solution :** Considérer aussi les aspects bas-niveau (cf. section 27.4).

---

## Pour aller plus loin

### Livres recommandés

- **"Introduction to Algorithms"** (CLRS) — Cormen, Leiserson, Rivest, Stein (bible des algorithmes)
- **"The Algorithm Design Manual"** — Steven Skiena (plus pratique)
- **"Algorithms"** — Robert Sedgewick (avec implémentations en C)
- **"Programming Pearls"** — Jon Bentley (optimisations pratiques)

### Ressources en ligne

- **Big-O Cheat Sheet** : https://www.bigocheatsheet.com/
- **VisuAlgo** : https://visualgo.net/ (visualisation d'algorithmes)
- **GeeksforGeeks** : https://www.geeksforgeeks.org/fundamentals-of-algorithms/
- **LeetCode** : https://leetcode.com/ (pratique avec exercices)

### Cours en ligne

- **MIT OpenCourseWare - Introduction to Algorithms**
- **Princeton Algorithms (Coursera)** — Robert Sedgewick
- **Khan Academy - Algorithms**

---

## Conclusion

Les optimisations algorithmiques sont le **levier le plus puissant** pour améliorer les performances :

1. ✅ **Choisir le bon algorithme** peut vous faire gagner 100-1000x
2. ✅ **Big-O n'est pas tout** : Considérer aussi les constantes, le cache, et les cas pratiques
3. ✅ **Profiler avant d'optimiser** : Ne pas deviner, mesurer !
4. ✅ **Trade-offs** : Temps vs mémoire, simplicité vs performance
5. ✅ **Structures de données** : Le bon choix est crucial

**Hiérarchie des optimisations (rappel) :**

```
1. Algorithme O(n) vs O(n²)        → 1000x plus rapide
2. Structure de données adaptée    → 100x plus rapide
3. Optimisations cache             → 10x plus rapide
4. Flags de compilation (-O2 -O3)  → 3x plus rapide
5. Micro-optimisations             → 1.5x plus rapide
```

**Citation finale :**

> "The fastest algorithm is the one that never runs" — Anonymous

Traduction : Le meilleur algorithme est celui qu'on n'exécute pas → Éviter le travail inutile (caching, memoization) est souvent plus efficace que d'optimiser le code !

**Règle d'or :** Commencez toujours par le bon algorithme, puis optimisez si nécessaire. Ne micro-optimisez jamais un algorithme fondamentalement mauvais !

---

*Prochaine section : 27.7 Vectorisation et SIMD*

⏭️ [Vectorisation et SIMD](/27-optimisation-performance/07-vectorisation-simd.md)
