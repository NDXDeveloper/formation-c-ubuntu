# Chapitre 11 - Structures dynamiques : Guide de compilation

Compilation standard :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 fichier.c -o fichier
```

Les fichiers utilisant `strdup()` nécessitent `#define _POSIX_C_SOURCE 200809L` (déjà inclus dans le code source).

---

## 01_liste_chainee_simple.c

- **Section** : 11.1 - Listes chaînées simples
- **Description** : Exemple complet combinant création, insertion (début/fin), recherche, suppression et libération d'une liste chaînée simple
- **Fichier source** : 01-listes-chainees.md
- **Sortie attendue** :
```
=== Insertion au debut ===
Liste : 10 --> 20 --> 30 --> NULL

=== Insertion a la fin ===
Liste : 10 --> 20 --> 30 --> 40 --> 50 --> NULL

=== Recherche ===
30 trouve dans la liste
100 non trouve dans la liste

=== Suppression ===
Noeud avec valeur 20 supprime
Liste : 10 --> 30 --> 40 --> 50 --> NULL
Noeud avec valeur 10 supprime
Liste : 30 --> 40 --> 50 --> NULL

=== Liberation de la memoire ===
Liste liberee
Liste : NULL
```

## 02_liste_chainee_tail.c

- **Section** : 11.1 - Listes chaînées simples - Variantes et optimisations
- **Description** : Liste chaînée avec pointeur tail pour insertion en O(1) à la fin
- **Fichier source** : 01-listes-chainees.md
- **Sortie attendue** :
```
=== Insertion a la fin optimisee (O(1)) ===
Liste (5 elements) : 10 --> 20 --> 30 --> 40 --> 50 --> NULL

Head = 10, Tail = 50

=== Liberation ===
Liste liberee
```

## 03_liste_doublement_chainee.c

- **Section** : 11.2 - Listes doublement chaînées
- **Description** : Exemple complet avec insertion, parcours bidirectionnel, recherche, suppression et libération
- **Fichier source** : 02-listes-doublement-chainees.md
- **Sortie attendue** :
```
=== Insertion au debut ===
Liste (avant) : 10 <-> 20 <-> 30 -> NULL

=== Insertion a la fin ===
Liste (avant) : 10 <-> 20 <-> 30 <-> 40 <-> 50 -> NULL

=== Parcours arriere ===
Liste (arriere) : 50 <-> 40 <-> 30 <-> 20 <-> 10 -> NULL

=== Recherche ===
Valeur 30 trouvee : 30
  Precedent : 20
  Suivant : 40

=== Suppression de 20 ===
Liste (avant) : 10 <-> 30 <-> 40 <-> 50 -> NULL

=== Suppression du premier (10) ===
Liste (avant) : 30 <-> 40 <-> 50 -> NULL

=== Suppression du dernier (50) ===
Liste (avant) : 30 <-> 40 -> NULL

=== Taille de la liste : 2 ===

=== Liberation ===
Liste liberee
Liste (avant) :  -> NULL
```

## 04_liste_double_avancee.c

- **Section** : 11.2 - Listes doublement chaînées - Opérations avancées
- **Description** : Inversion, concaténation et accès par index optimisé
- **Fichier source** : 02-listes-doublement-chainees.md
- **Sortie attendue** :
```
=== Liste 1 ===
Liste (3 elements) : 10 <-> 20 <-> 30 -> NULL

=== Liste 2 ===
Liste (3 elements) : 40 <-> 50 <-> 60 -> NULL

=== Concatenation (list1 + list2) ===
Liste (6 elements) : 10 <-> 20 <-> 30 <-> 40 <-> 50 <-> 60 -> NULL
Liste 2 apres concatenation : Liste (0 elements) :  -> NULL

=== Acces par index ===
  Element[0] = 10
  Element[1] = 20
  Element[2] = 30
  Element[3] = 40
  Element[4] = 50
  Element[5] = 60

=== Inversion ===
Avant : Liste (6 elements) : 10 <-> 20 <-> 30 <-> 40 <-> 50 <-> 60 -> NULL
Apres : Liste (6 elements) : 60 <-> 50 <-> 40 <-> 30 <-> 20 <-> 10 -> NULL

Head = 60, Tail = 10
```

## 05_pile_tableau.c

- **Section** : 11.3 - Piles et Files - Pile avec tableau statique
- **Description** : Implémentation d'une pile LIFO avec un tableau de taille fixe
- **Fichier source** : 03-piles-et-files.md
- **Sortie attendue** :
```
Push : 10
Push : 20
Push : 30
Pile (sommet -> base) : 30 20 10
Sommet : 30
Pop : 30
Pop : 20
Pile (sommet -> base) : 10
Push : 40
Pile (sommet -> base) : 40 10
```

## 06_pile_liste_chainee.c

- **Section** : 11.3 - Piles et Files - Pile avec liste chaînée
- **Description** : Implémentation d'une pile LIFO dynamique avec liste chaînée
- **Fichier source** : 03-piles-et-files.md
- **Sortie attendue** :
```
Push : 10
Push : 20
Push : 30
Pile (sommet -> base) : 30 20 10
Taille : 3
Pop : 30
Pop : 20
Pile (sommet -> base) : 10
Push : 40
Push : 50
Pile (sommet -> base) : 50 40 10

Liberation de la pile
Pile : [vide]
```

## 07_pile_parentheses.c

- **Section** : 11.3 - Piles et Files - Application : parenthèses équilibrées
- **Description** : Vérification de parenthèses/crochets/accolades avec une pile
- **Fichier source** : 03-piles-et-files.md
- **Sortie attendue** :
```
"(())" -> equilibre (OK)
"({[]})" -> equilibre (OK)
"(()" -> desequilibre (OK)
"({)}" -> desequilibre (OK)
```

## 08_pile_postfixe.c

- **Section** : 11.3 - Piles et Files - Application : notation polonaise inversée
- **Description** : Évaluation d'expressions postfixes (RPN) avec une pile
- **Fichier source** : 03-piles-et-files.md
- **Sortie attendue** :
```
"34+" = 7 (attendu: 7)
"53*2+" = 17 (attendu: 17)
"82-" = 6 (attendu: 6)
"93/" = 3 (attendu: 3)
```

## 09_file_tableau_circulaire.c

- **Section** : 11.3 - Piles et Files - File avec tableau circulaire
- **Description** : Implémentation d'une file FIFO avec tableau circulaire (modulo)
- **Fichier source** : 03-piles-et-files.md
- **Sortie attendue** :
```
Enqueue : 10
Enqueue : 20
Enqueue : 30
File (front -> rear) : 10 20 30
Front : 10
Dequeue : 10
File (front -> rear) : 20 30
Enqueue : 40
File (front -> rear) : 20 30 40
```

## 10_file_liste_chainee.c

- **Section** : 11.3 - Piles et Files - File avec liste chaînée
- **Description** : Implémentation d'une file FIFO dynamique avec liste chaînée
- **Fichier source** : 03-piles-et-files.md
- **Sortie attendue** :
```
=== Enqueue ===
Enqueue : 10
Enqueue : 20
Enqueue : 30
File (front -> rear) : 10 20 30 (taille: 3)

=== Dequeue ===
Dequeue : 10
File (front -> rear) : 20 30 (taille: 2)

=== Enqueue supplementaire ===
Enqueue : 40
Enqueue : 50
File (front -> rear) : 20 30 40 50 (taille: 4)

=== Vider la file un par un ===
Dequeue : 20
Dequeue : 30
Dequeue : 40
Dequeue : 50
File : [vide]
```

## 11_table_hachage_chainage.c

- **Section** : 11.4 - Tables de hachage - Chaînage
- **Description** : Table de hachage avec gestion des collisions par chaînage (listes chaînées dans chaque bucket)
- **Fichier source** : 04-tables-hachage.md
- **Sortie attendue** (les buckets peuvent varier selon la fonction de hachage) :
```
=== Insertions ===
=== Table de hachage ===
Bucket 0: [vide]
Bucket 1: ["Zoe" -> 901234]
Bucket 2: ["Bob" -> 789012]
Bucket 3: [vide]
Bucket 4: [vide]
Bucket 5: ["Charlie" -> 345678]
Bucket 6: [vide]
Bucket 7: ["Alice" -> 123456]
Bucket 8: [vide]
Bucket 9: [vide]
Taille totale : 4 elements

=== Recherche ===
Alice trouvee : 123456
David non trouve

=== Mise a jour ===
Alice apres mise a jour : 999999

=== Suppression de Bob ===
=== Table de hachage ===
...
Taille totale : 3 elements
```

## 12_table_hachage_adressage_ouvert.c

- **Section** : 11.4 - Tables de hachage - Adressage ouvert (sondage linéaire)
- **Description** : Table de hachage avec sondage linéaire et tombstones pour la suppression
- **Fichier source** : 04-tables-hachage.md
- **Sortie attendue** :
```
=== Insertions ===
=== Table de hachage (adressage ouvert) ===
  [0] [vide]
  [1] "Zoe" -> 901234
  [2] "Bob" -> 789012
  ...
Taille : 4 / 10

=== Recherche ===
Charlie trouve : 345678
David non trouve

=== Suppression de Bob (tombstone) ===
...
  [2] [SUPPRIME (tombstone)]
...

=== Recherche apres suppression ===
Bob non trouve (supprime)
Charlie toujours accessible : 345678
```

## 13_arbre_binaire_recherche.c

- **Section** : 11.5 - Arbres binaires
- **Description** : Arbre binaire de recherche (BST) complet avec insertion, recherche, parcours (préfixe, infixe, postfixe, largeur), propriétés, suppression et affichage visuel
- **Fichier source** : 05-arbres-binaires.md
- **Sortie attendue** :
```
=== Construction de l'arbre ===
Arbre cree avec 7 noeuds

=== Parcours ===
Prefixe  : 50 30 20 10 40 70 80
Infixe   : 10 20 30 40 50 70 80
Postfixe : 10 20 40 30 80 70 50
Largeur  : 50 30 70 20 40 80 10

=== Proprietes ===
Hauteur : 3
Nombre de noeuds : 7
Nombre de feuilles : 3
Est un BST valide ? Oui

=== Recherche ===
40 trouve dans l'arbre
100 non trouve dans l'arbre

=== Min/Max ===
Minimum : 10
Maximum : 80

=== Affichage visuel ===
          80

     70

50

          40

     30

          20

               10

=== Suppression ===
Suppression de 20 (feuille)...
Infixe apres suppression : 10 30 40 50 70 80

Suppression de 30 (deux enfants)...
Infixe apres suppression : 10 40 50 70 80

=== Liberation de la memoire ===
Memoire liberee
```

## 14_gestion_memoire_liste.c

- **Section** : 11.7 - Gestion mémoire des structures dynamiques
- **Description** : Liste chaînée avec chaînes allouées dynamiquement, libération correcte vérifiable avec Valgrind
- **Fichier source** : 07-gestion-memoire.md
- **Sortie attendue** :
```
Liste :
  -> "Premier"
  -> "Deuxieme"
  -> "Troisieme"

Liberation de la liste...
Memoire liberee correctement.
(Verifier avec : valgrind --leak-check=full ./14_gestion_memoire_liste)
```

## 15_gestion_memoire_hashtable.c

- **Section** : 11.7 - Gestion mémoire des structures dynamiques
- **Description** : Table de hachage avec libération correcte de toutes les allocations (clés + nœuds)
- **Fichier source** : 07-gestion-memoire.md
- **Sortie attendue** :
```
Insertions...

Contenu de la table :
  Bucket 2: ["Bob"=456]
  Bucket 3: ["Eve"=202]
  Bucket 5: ["Charlie"=789]
  Bucket 7: ["David"=101] ["Alice"=123]

Liberation de la table...
Memoire liberee correctement.
(Verifier avec : valgrind --leak-check=full ./15_gestion_memoire_hashtable)
```

## 16_safe_malloc_free.c

- **Section** : 11.7 - Gestion mémoire des structures dynamiques
- **Description** : Wrappers safe_malloc/safe_free pour sécuriser les allocations et éviter les fuites mémoire
- **Fichier source** : 07-gestion-memoire.md
- **Sortie attendue** :
```
=== Demonstration safe_malloc / safe_free ===

Tableau : 10 20 30 40 50

Avant safe_free : array = 0x... (adresse variable)
Apres safe_free : array = (nil) (NULL)
Double safe_free : pas de crash !

=== Liste chainee avec safe_malloc ===
Liste : 10 20 30
Liste liberee.
```

---

## Notes

- Le fichier `06-choix-structure.md` est purement théorique (guide de décision, comparaisons) et ne contient pas d'exemples compilables.
- Tous les programmes ont été vérifiés avec Valgrind : zéro fuite mémoire.
- Les fichiers `11_*` et `12_*` (tables de hachage) et `14_*`, `15_*` (gestion mémoire) utilisent `strdup()` via `#define _POSIX_C_SOURCE 200809L`.
