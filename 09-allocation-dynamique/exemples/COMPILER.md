# Chapitre 09 - Allocation dynamique : Compilation et sorties attendues

Compilation standard :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <programme> <fichier.c>
```

Exception : `18_buddy_allocator.c` nécessite `-lm` (math.h) :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 18_buddy_allocator 18_buddy_allocator.c -lm
```

Note : La section 9.6 (double free / corruption du heap) contient des exemples
intentionnellement dangereux (comportement indéfini) et a été volontairement omise.

---

## 01_heap_tableau.c
- **Section** : 9.1 - La pile (Stack) vs le tas (Heap)
- **Description** : Allocation d'un tableau sur le heap avec fonction creer_tableau
- **Fichier source** : 01-stack-vs-heap.md
- **Sortie attendue** :
```
Premiere valeur : 0
```

## 02_stack_vs_heap.c
- **Section** : 9.1 - La pile (Stack) vs le tas (Heap)
- **Description** : Comparaison stack/heap dans une meme fonction
- **Fichier source** : 01-stack-vs-heap.md
- **Sortie attendue** :
```
Stack - Age: 25, Prenom: Alice
Heap - Description: Ceci est une description sur le Heap
```

## 03_duree_vie.c
- **Section** : 9.2 - Duree de vie et performance
- **Description** : Persistance des donnees heap entre fonctions
- **Fichier source** : 02-duree-vie-performance.md
- **Sortie attendue** :
```
=== Debut du main ===
Dans creer_donnees : donnees creees a l'adresse 0x...
Dans main : les donnees existent toujours !
Valeur : 42 a l'adresse 0x...
Nouvelle valeur : 100
=== Fin du main ===
```

## 04_benchmark.c
- **Section** : 9.2 - Duree de vie et performance
- **Description** : Benchmark comparatif Stack vs Heap (1M iterations)
- **Fichier source** : 02-duree-vie-performance.md
- **Sortie attendue** :
```
=== Benchmark Stack vs Heap ===
Stack : X.XXXXXX secondes pour 1000000 allocations
Heap  : X.XXXXXX secondes pour 1000000 allocations
```

## 05_malloc_tableau.c
- **Section** : 9.3 - Les fonctions d'allocation
- **Description** : Allocation et remplissage d'un tableau avec malloc
- **Fichier source** : 03-fonctions-allocation.md
- **Sortie attendue** :
```
tableau[0] = 0
tableau[1] = 10
tableau[2] = 20
tableau[3] = 30
tableau[4] = 40
tableau[5] = 50
tableau[6] = 60
tableau[7] = 70
tableau[8] = 80
tableau[9] = 90
```

## 06_calloc_demo.c
- **Section** : 9.3 - Les fonctions d'allocation
- **Description** : Demonstration de calloc (initialisation a zero)
- **Fichier source** : 03-fonctions-allocation.md
- **Sortie attendue** :
```
tableau[0] = 0
tableau[1] = 0
tableau[2] = 0
tableau[3] = 0
tableau[4] = 0
```

## 07_realloc_liste.c
- **Section** : 9.3 - Les fonctions d'allocation
- **Description** : Liste dynamique avec realloc (doublement de capacite)
- **Fichier source** : 03-fonctions-allocation.md
- **Sortie attendue** :
```
Redimensionne a capacite 4
Redimensionne a capacite 8
Redimensionne a capacite 16
Liste finale : 0 1 2 3 4 5 6 7 8 9
```

## 08_liberation_structure.c
- **Section** : 9.3 - Les fonctions d'allocation
- **Description** : Liberation correcte d'une structure avec pointeurs internes
- **Fichier source** : 03-fonctions-allocation.md
- **Sortie attendue** :
```
Marie Dupont a 25 ans
```

## 09_tableau_dynamique.c
- **Section** : 9.3 - Les fonctions d'allocation
- **Description** : Gestionnaire de tableau dynamique complet (create/add/display/free)
- **Fichier source** : 03-fonctions-allocation.md
- **Sortie attendue** :
```
Tableau [taille=1, capacite=2] : 0
Tableau [taille=2, capacite=2] : 0 10
Tableau redimensionne : capacite 4
Tableau [taille=3, capacite=4] : 0 10 20
...
Tableau redimensionne : capacite 16
Tableau [taille=9, capacite=16] : 0 10 20 30 40 50 60 70 80
Tableau [taille=10, capacite=16] : 0 10 20 30 40 50 60 70 80 90
```

## 10_erreurs_enum.c
- **Section** : 9.4 - Gestion des erreurs d'allocation
- **Description** : Codes d'erreur avec enum et parametre de sortie
- **Fichier source** : 04-gestion-erreurs.md
- **Sortie attendue** :
```
Tableau cree avec succes : 42
```

## 11_goto_cleanup.c
- **Section** : 9.4 - Gestion des erreurs d'allocation
- **Description** : Nettoyage automatique avec goto en cas d'erreur multiple
- **Fichier source** : 04-gestion-erreurs.md
- **Sortie attendue** :
```
Etudiant cree : Marie Dupont
```

## 12_test_robustesse.c
- **Section** : 9.4 - Gestion des erreurs d'allocation
- **Description** : Tests de robustesse avec simulation d'echecs d'allocation
- **Fichier source** : 04-gestion-erreurs.md
- **Sortie attendue** :
```
=== Test 1 : Tout reussit ===
Structure creee avec succes

=== Test 2 : Echec a la 2e allocation ===
[TEST] Simulation d'echec a l'allocation #2
Echec allocation data

=== Test 3 : Echec a la 4e allocation ===
[TEST] Simulation d'echec a l'allocation #4
Echec allocation values
```

## 13_liste_sans_fuites.c
- **Section** : 9.5 - Fuites memoire : causes et prevention
- **Description** : Liste chainee complete sans fuites memoire
- **Fichier source** : 05-fuites-memoire.md
- **Sortie attendue** :
```
Liste (3 elements) : Troisieme Deuxieme Premier
Programme termine sans fuite
```

## 14_pool_allocator.c
- **Section** : 9.7 - Strategies d'allocation personnalisees
- **Description** : Pool Allocator (blocs de taille fixe, O(1) alloc/free)
- **Fichier source** : 07-strategies-allocation.md
- **Sortie attendue** :
```
Pool cree : 10 blocs de 8 octets = 80 octets
a=10, b=20, c=30
=== Pool Stats ===
Taille bloc    : 8 octets
Nombre blocs   : 10
Blocs utilises : 3 (30.0%)
Blocs libres   : 7
=== Pool Stats ===
Taille bloc    : 8 octets
Nombre blocs   : 10
Blocs utilises : 1 (10.0%)
Blocs libres   : 9
d=40 (bloc reutilise)
```

## 15_arena_allocator.c
- **Section** : 9.7 - Strategies d'allocation personnalisees
- **Description** : Arena/Bump Allocator (allocation lineaire, liberation en masse)
- **Fichier source** : 07-strategies-allocation.md
- **Sortie attendue** :
```
Arena creee : 1048576 octets

--- Parsing fichier 1 ---
Token: Token0 (ligne 1)
Token: Token1 (ligne 2)
Token: Token2 (ligne 3)
=== Arena Stats ===
Taille totale  : 1048576 octets
Utilise        : 232 octets (0.0%)
Libre          : 1048344 octets
Peak usage     : 232 octets (0.0%)
Arena reset : 232 octets liberes

--- Parsing fichier 2 ---
(meme sortie, 3 iterations)
```

## 16_stack_allocator.c
- **Section** : 9.7 - Strategies d'allocation personnalisees
- **Description** : Stack Allocator LIFO (allocation/liberation en ordre inverse)
- **Fichier source** : 07-strategies-allocation.md
- **Sortie attendue** :
```
a=42, b=Hello, c=3.14
```

## 17_freelist_allocator.c
- **Section** : 9.7 - Strategies d'allocation personnalisees
- **Description** : Free List Allocator (first-fit, tailles variables)
- **Fichier source** : 07-strategies-allocation.md
- **Sortie attendue** :
```
a=42
b=Hello from free list
c=3.14159
d=100 (bloc reutilise)
```

## 18_buddy_allocator.c
- **Section** : 9.7 - Strategies d'allocation personnalisees
- **Description** : Buddy Allocator (puissances de 2, division/fusion de blocs)
- **Fichier source** : 07-strategies-allocation.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 18_buddy_allocator 18_buddy_allocator.c -lm`
- **Sortie attendue** :
```
Buddy allocator cree : 1024 octets (5 niveaux)
Allocations effectuees : 0x..., 0x..., 0x...
```
