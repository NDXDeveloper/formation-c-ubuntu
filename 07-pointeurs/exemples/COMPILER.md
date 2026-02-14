# Chapitre 07 — Pointeurs : Compilation des exemples

> Compilateur : `gcc -Wall -Wextra -Werror -pedantic -std=c17`

## Résumé

| # | Fichier(s) | Section | Description | Source | Sortie attendue |
|---|-----------|---------|-------------|--------|----------------|
| 01 | `01_adresses_variables.c` | 7.1 : Concept d'adresse mémoire | Adresses de plusieurs variables en mémoire | `01-adresse-memoire.md` | `Adresse de a : 0x... (valeur : 10)` / `Adresse de b : 0x... (valeur : 20)` / `Adresse de c : 0x... (valeur : 30)` |
| 02 | `02_types_et_adresses.c` | 7.1 : Concept d'adresse mémoire | Types de données et leurs adresses/tailles | `01-adresse-memoire.md` | `Adresse de lettre (char) : 0x... (taille : 1 octet)` / ... / `Taille d'une adresse : 8 octets` |
| 03 | `03_pointeur_complet.c` | 7.2 : Déclaration, déréférencement et adresse | Exemple complet (declaration, dereferencement, modification) | `02-declaration-dereferencement.md` | `Valeur de nombre : 42` / `Valeur pointee (*ptr_nombre) : 42` / `Apres (*ptr_nombre = 100) : nombre = 100` |
| 04 | `04_pointeurs_multiples.c` | 7.2 : Déclaration, déréférencement et adresse | Plusieurs pointeurs vers la même variable | `02-declaration-dereferencement.md` | `valeur = 50` / `*ptr1 = 50` / `*ptr2 = 50` / `valeur = 60` / `*ptr2 = 60` |
| 05 | `05_equivalence_tableau.c` | 7.3 : L'arithmétique des pointeurs | Equivalence tableau[i] et *(tableau + i) | `03-arithmetique-pointeurs.md` | `tableau[0] = 10` / `*(tableau + 0) = 10` / ... |
| 06 | `06_parcours_pointeur.c` | 7.3 : L'arithmétique des pointeurs | Parcourir un tableau avec un pointeur | `03-arithmetique-pointeurs.md` | `Adresse : 0x..., Valeur : 10` / ... / `Valeur : 50` |
| 07 | `07_recherche_pointeur.c` | 7.3 : L'arithmétique des pointeurs | Recherche d'un élément avec pointeurs | `03-arithmetique-pointeurs.md` | `Valeur trouvee a l'index : 2` / `Valeur : 30` / `Valeur 99 non trouvee` |
| 08 | `08_arithmetique_complet.c` | 7.3 : L'arithmétique des pointeurs | Exemple récapitulatif complet | `03-arithmetique-pointeurs.md` | `[ 100 200 300 400 500 600 ]` / navigation / `[ 100 200 300 999 500 600 ]` |
| 09 | `09_afficher_octets.c` | 7.4 : Pointeurs void* et casting | Fonction générique afficher_octets | `04-pointeurs-void.md` | `Contenu en octets : 78 56 34 12` / `Contenu en octets : C3 F5 48 40` |
| 10 | `10_swap_generique.c` | 7.4 : Pointeurs void* et casting | Swap générique avec void* et memcpy | `04-pointeurs-void.md` | `x = 20, y = 10` / `f1 = 2.71, f2 = 3.14` |
| 11 | `11_qsort_demo.c` | 7.4 : Pointeurs void* et casting | Tri avec qsort et callbacks void* | `04-pointeurs-void.md` | `Avant tri : 50 10 40 20 30` / `Apres tri : 10 20 30 40 50` |
| 12 | `12_liste_chainee.c` | 7.5 : Pointeurs NULL et dangling pointers | Liste chaînée sûre avec gestion NULL | `05-pointeurs-null.md` | `Liste : 10 20 30` / `Liste correctement liberee` |
| 13 | `13_const_pointeurs.c` | 7.6 : Pointeurs constants et const | Les 3 cas de const avec pointeurs | `06-pointeurs-const.md` | Cas 1 (valeur constante) / Cas 2 (pointeur constant) / Cas 3 (tout constant) |
| 14 | `14_recherche_const.c` | 7.6 : Pointeurs constants et const | Recherche avec const-correctness | `06-pointeurs-const.md` | `Valeur trouvee : 30` / `Index : 2` / `Valeur 99 non trouvee` |
| 15 | `15_produit_scalaire.c` | 7.7 : Pointeurs restreints (restrict) | Produit scalaire avec restrict | `07-pointeurs-restrict.md` | `Produit scalaire : 70.0` |
| 16 | `16_multiplication_matrices.c` | 7.7 : Pointeurs restreints (restrict) | Multiplication de matrices 2x2 avec restrict | `07-pointeurs-restrict.md` | `19 22` / `43 50` |

## Commandes de compilation

**Fichier unique :**
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <nom_programme> <fichier>.c
```

## Adaptations

- Aucune adaptation nécessaire. Tous les programmes compilent directement avec les flags stricts.

## Statistiques

- **16 programmes** (16 fichiers) couvrant 7 sections sur 7 fichiers .md
- **0 correction** nécessaire (compilation stricte)
- **Sections couvertes** : 7.1 (Adresse mémoire), 7.2 (Déclaration/déréférencement), 7.3 (Arithmétique des pointeurs), 7.4 (Pointeurs void*), 7.5 (NULL/dangling), 7.6 (Pointeurs const), 7.7 (Pointeurs restrict)
