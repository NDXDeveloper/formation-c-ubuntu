# Chapitre 05 — Opérateurs et contrôle de flux : Compilation des exemples

> Compilateur : `gcc -Wall -Wextra -Werror -pedantic -std=c17`

## Résumé

| # | Fichier | Section | Description | Source | Sortie attendue |
|---|---------|---------|-------------|--------|----------------|
| 01 | `01_moyenne.c` | 5.1 : Opérateurs arithmétiques | Calcul de moyenne avec division flottante | `01-operateurs-arithmetiques.md` | `Moyenne : 15.00` |
| 02 | `02_temperature.c` | 5.1 : Opérateurs arithmétiques | Conversion Celsius vers Fahrenheit | `01-operateurs-arithmetiques.md` | `25.0°C = 77.0°F` |
| 03 | `03_pair_impair.c` | 5.1 : Opérateurs arithmétiques | Vérification pair/impair avec modulo | `01-operateurs-arithmetiques.md` | `42 est pair` |
| 04 | `04_division_reste.c` | 5.1 : Opérateurs arithmétiques | Division entière et reste | `01-operateurs-arithmetiques.md` | `17 / 5 = 3 reste 2` |
| 05 | `05_conditions_combinees.c` | 5.2 : Opérateurs logiques | Conditions combinées avec &&, \|\|, ! | `02-operateurs-logiques.md` | `Excellentes conditions pour une randonnée !` |
| 06 | `06_valeurs_booleennes.c` | 5.2 : Opérateurs logiques | Valeurs booléennes en C (non-zéro = vrai) | `02-operateurs-logiques.md` | `valeur1 est considérée comme vraie` / `valeur3 est considérée comme vraie` |
| 07 | `07_categories_age.c` | 5.2 : Opérateurs logiques | Catégories d'âge avec opérateurs logiques | `02-operateurs-logiques.md` | `Catégorie : Adolescent` |
| 08 | `08_extraction_rgb.c` | 5.3 : Opérateurs bit à bit | Extraction composantes RGB avec AND et décalage | `03-operateurs-bit-a-bit.md` | `Rouge: 255, Vert: 58, Bleu: 124` |
| 09 | `09_permissions_flags.c` | 5.3 : Opérateurs bit à bit | Drapeaux de permissions avec OR et AND | `03-operateurs-bit-a-bit.md` | Permissions: 5, Lecture/Exécution autorisées |
| 10 | `10_xor_echange.c` | 5.3 : Opérateurs bit à bit | Échange de variables avec XOR | `03-operateurs-bit-a-bit.md` | `Avant : a=10, b=20` / `Après : a=20, b=10` |
| 11 | `11_status_flags.c` | 5.3 : Opérateurs bit à bit | Drapeaux de status avec \|=, &=, ^= | `03-operateurs-bit-a-bit.md` | Status initial 0x09, modifications successives |
| 12 | `12_puissance_de_deux.c` | 5.3 : Opérateurs bit à bit | Vérification puissance de 2 avec n & (n-1) | `03-operateurs-bit-a-bit.md` | 1, 2, 4, 8, 16, 32 sont des puissances de 2 |
| 13 | `13_compter_bits.c` | 5.3 : Opérateurs bit à bit | Comptage de bits (simple + algorithme de Kernighan) | `03-operateurs-bit-a-bit.md` | `Nombre de bits à 1 : 3` |
| 14 | `14_notes_mention.c` | 5.4 : Conditions | Mentions selon la note avec if/else if/else | `04-conditions.md` | `Mention Bien` |
| 15 | `15_switch_choix.c` | 5.4 : Conditions | Instruction switch pour choix de menu | `04-conditions.md` | `Vous avez choisi B` |
| 16 | `16_ternaire.c` | 5.4 : Conditions | Opérateur ternaire (majorité et maximum) | `04-conditions.md` | `Vous êtes majeur` / `Le maximum est : 20` |
| 17 | `17_validation_date.c` | 5.4 : Conditions | Validation de date avec années bissextiles | `04-conditions.md` | 29/02/2024 Valide, 29/02/2023 Invalide, 31/04/2024 Invalide |
| 18 | `18_calculatrice.c` | 5.4 : Conditions | Calculatrice avec switch et gestion d'erreurs | `04-conditions.md` | Résultats des 4 opérations + erreur division par zéro |
| 19 | `19_somme_while.c` | 5.5 : Boucles | Somme des nombres de 1 à N avec while | `05-boucles.md` | `La somme des nombres de 1 à 10 est : 55` |
| 20 | `20_puissances_deux.c` | 5.5 : Boucles | Puissances de 2 inférieures à 1000 | `05-boucles.md` | `1 2 4 8 16 32 64 128 256 512` |
| 21 | `21_do_while_vs_while.c` | 5.5 : Boucles | Comparaison do-while vs while | `05-boucles.md` | do-while affiche une ligne, while n'affiche rien |
| 22 | `22_factorielle.c` | 5.5 : Boucles | Calcul de factorielle avec boucle for | `05-boucles.md` | `5! = 120` |
| 23 | `23_filtrer_notes.c` | 5.5 : Boucles | Filtrage de notes invalides avec continue | `05-boucles.md` | Notes valides + `Moyenne : 15.00` |
| 24 | `24_table_multiplication.c` | 5.5 : Boucles | Table de multiplication avec boucles imbriquées | `05-boucles.md` | Table 5×5 formatée |
| 25 | `25_tri_bulle.c` | 5.5 : Boucles | Tri à bulle (bubble sort) | `05-boucles.md` | `Avant tri : 64 34 25 12 22 11 90` / `Après tri : 11 12 22 25 34 64 90` |
| 26 | `26_nombres_premiers.c` | 5.5 : Boucles | Nombres premiers jusqu'à N avec boucles imbriquées | `05-boucles.md` | `2 3 5 7 11 13 17 19 23 29 31 37 41 43 47` |

## Commandes de compilation

Tous les fichiers se compilent avec la même commande :

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <nom_programme> <fichier>.c
```

## Corrections appliquées

**Aucune correction nécessaire.** Les 26 programmes compilent sans erreur ni avertissement avec les flags stricts.

## Statistiques

- **26 programmes** couvrant 5 sections sur 13 fichiers .md
- **0 correction** nécessaire
- **Sections couvertes** : 5.1 (Arithmétiques), 5.2 (Logiques), 5.3 (Bit à bit), 5.4 (Conditions), 5.5 (Boucles)
- **Sections sans programme compilable** : 5.6 (goto), 5.7 (clang-format), 5.8-5.13 (documentation outils/bonnes pratiques)
