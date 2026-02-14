# Chapitre 08 — Tableaux et chaînes : Compilation des exemples

> Compilateur : `gcc -Wall -Wextra -Werror -pedantic -std=c17`

## Résumé

| # | Fichier(s) | Section | Description | Source | Sortie attendue |
|---|-----------|---------|-------------|--------|----------------|
| 01 | `01_relation_tableaux_pointeurs.c` | 8.1 : Relation entre tableaux et pointeurs | Manipulation d'un tableau avec pointeurs (affichage, adresses, doublement) | `01-relation-tableaux-pointeurs.md` | `Valeurs : 10 20 30 40 50` / adresses / `Taille du tableau : 20 octets` / `Valeurs : 20 40 60 80 100` |
| 02 | `02_somme_matrice.c` | 8.2 : Tableaux multidimensionnels | Somme des éléments d'une matrice 3×4 | `02-tableaux-multidimensionnels.md` | `Somme totale : 78` |
| 03 | `03_transposition.c` | 8.2 : Tableaux multidimensionnels | Transposition d'une matrice (3×4 → 4×3) | `02-tableaux-multidimensionnels.md` | Matrice originale puis matrice transposée |
| 04 | `04_recherche_matrice.c` | 8.2 : Tableaux multidimensionnels | Recherche d'un élément dans une matrice | `02-tableaux-multidimensionnels.md` | `7 trouve a la position [1][2]` |
| 05 | `05_matrice_identite.c` | 8.2 : Tableaux multidimensionnels | Création et affichage d'une matrice d'identité 5×5 | `02-tableaux-multidimensionnels.md` | Matrice 5×5 avec 1 sur la diagonale |
| 06 | `06_compter_voyelles.c` | 8.3 : Les chaînes de caractères en C | Fonction qui compte les voyelles d'une chaîne | `03-chaines-en-c.md` | `Voyelles : 6` |
| 07 | `07_manipulation_chaines.c` | 8.3 : Les chaînes de caractères en C | Manipulation complète (compter_mots, inverser, est_palindrome, trim) | `03-chaines-en-c.md` | `Nombre de mots : 5` / `ruojnoB` / palindromes Oui / `texte` |
| 08 | `08_strtok_demo.c` | 8.4 : Manipulation avancée des chaînes | Découpage de chaîne avec strtok | `04-manipulation-chaines.md` | `Bonjour` / `le` / `monde` / `en` / `C` |
| 09 | `09_trouver_occurrences.c` | 8.4 : Manipulation avancée des chaînes | Trouver toutes les occurrences d'un caractère avec strchr | `04-manipulation-chaines.md` | `Trouve 'o' a la position 1` / `position 4` / `position 12` |
| 10 | `10_remplacer_char.c` | 8.4 : Manipulation avancée des chaînes | Remplacer un caractère dans une chaîne | `04-manipulation-chaines.md` | `Avant : Bonjour le monde` / `Apres : B0nj0ur le m0nde` |
| 11 | `11_capitalize.c` | 8.4 : Manipulation avancée des chaînes | Première lettre de chaque mot en majuscule | `04-manipulation-chaines.md` | `Bonjour Le Monde En C` |
| 12 | `12_analyseur_logs.c` | 8.4 : Manipulation avancée des chaînes | Analyseur de logs avec parsing de lignes structurées | `04-manipulation-chaines.md` | Parsing de 5 lignes + `Erreurs: 1` / `Warnings: 1` / `Infos: 3` |
| 13 | `13_snprintf_construction.c` | 8.6 : Fonctions sécurisées | Construction progressive d'un message avec snprintf | `06-fonctions-securisees.md` | `Ligne 1` / `Ligne 2` / `Ligne 3` |
| 14 | `14_litterales_demo.c` | 8.7 : Strings littérales en C | Utilisation correcte des chaînes littérales (const, fonctions, buffer) | `07-strings-litterales.md` | `=== MonApp v1.0.0 ===` / `[INFO] Bonjour` / `[ERROR] Permission refusee` / `user_123` |

## Commandes de compilation

**Fichier unique :**
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <nom_programme> <fichier>.c
```

## Adaptations

- Aucune adaptation nécessaire. Tous les programmes compilent directement avec les flags stricts.
- Section 8.5 (dangers et buffer overflows) contient des exemples intentionnellement vulnérables — non inclus dans les exemples compilables.

## Statistiques

- **14 programmes** (14 fichiers) couvrant 6 sections sur 7 fichiers .md
- **0 correction** nécessaire (compilation stricte)
- **Sections couvertes** : 8.1 (Relation tableaux/pointeurs), 8.2 (Tableaux multidimensionnels), 8.3 (Chaînes en C), 8.4 (Manipulation avancée), 8.6 (Fonctions sécurisées), 8.7 (Strings littérales)
- **Section non couverte** : 8.5 (Dangers et buffer overflows — exemples volontairement vulnérables)
