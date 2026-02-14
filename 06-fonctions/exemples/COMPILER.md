# Chapitre 06 — Fonctions : Compilation des exemples

> Compilateur : `gcc -Wall -Wextra -Werror -pedantic -std=c17`

## Résumé

| # | Fichier(s) | Section | Description | Source | Sortie attendue |
|---|-----------|---------|-------------|--------|----------------|
| 01 | `01_carre.c` | 6.1 : Déclaration, définition et prototypes | Prototypes et fonctions simples (carré) | `01-declaration-definition.md` | `Le carré de 7 est 49` |
| 02 | `02_types_retour.c` | 6.1 : Déclaration, définition et prototypes | Fonctions avec différents types de retour (int, bool, void) | `01-declaration-definition.md` | `Maximum entre 15 et 23 : 23` / `15 est impair` / `====================` |
| 03 | `03a_operations.h` `03b_operations.c` `03c_main_operations.c` | 6.1 : Déclaration, définition et prototypes | Compilation séparée (header + implémentation + main) | `01-declaration-definition.md` | `10 + 5 = 15` / `10 - 5 = 5` / `10 * 5 = 50` |
| 04 | `04_puissance.c` | 6.1 : Déclaration, définition et prototypes | Programme complet avec prototypes, main et définitions | `01-declaration-definition.md` | `=== Calculateur de puissance ===` / `2^10 = 1024` |
| 05 | `05_appels_imbriques.c` | 6.2 : La Pile - Stack Frames | Appels de fonctions imbriqués (main → calculer → multiplier) | `02-stack-frames.md` | `Résultat = 30` |
| 06 | `06_tracer_pile.c` | 6.2 : La Pile - Stack Frames | Tracer manuellement la pile avec indentation | `02-stack-frames.md` | `[main] Début` / ` [triple_nombre] n = 5` / `  [double_nombre] n = 5` / ` [triple_nombre] result = 10` / `[main] y = 30` |
| 07 | `07_passage_valeur.c` | 6.3 : Passage par valeur | La modification du paramètre n'affecte pas l'original | `03-passage-par-valeur.md` | `nombre` reste à 42 après appel de `modifier` |
| 08 | `08_echanger_copie.c` | 6.3 : Passage par valeur | Échec de l'échange par passage par valeur | `03-passage-par-valeur.md` | `x=10, y=20` inchangés après `echanger_copie` |
| 09 | `09_passage_complet.c` | 6.3 : Passage par valeur | Exemple complet : passage par valeur et par pointeur | `03-passage-par-valeur.md` | Carré, tripler, distance Manhattan, déplacement de point |
| 10 | `10_division_securisee.c` | 6.4 : Valeurs de retour et codes d'erreur | Division sécurisée avec paramètres de sortie | `04-valeurs-retour.md` | `17 / 5 = 3 reste 2` |
| 11 | `11_errno_demo.c` | 6.4 : Valeurs de retour et codes d'erreur | Utilisation de errno et strerror | `04-valeurs-retour.md` | `Code errno : 2` / `Message : No such file or directory` |
| 12 | `12_strtol_demo.c` | 6.4 : Valeurs de retour et codes d'erreur | Conversion sécurisée avec strtol | `04-valeurs-retour.md` | `Valeur convertie : 12345` / `Caractères non convertis : abc` |
| 13 | `13_afficher_arguments.c` | 6.5 : La fonction main et ses paramètres | Affichage des arguments de la ligne de commande | `05-fonction-main.md` | `argv[0] = ./13_afficher_arguments` / `argv[1] = arg1` / ... |
| 14 | `14_calculatrice_cli.c` | 6.5 : La fonction main et ses paramètres | Calculatrice en ligne de commande avec argc/argv | `05-fonction-main.md` | `10 + 5 = 15` / `20 / 4 = 5` / `7 x 6 = 42` |
| 15 | `15_getopt_demo.c` | 6.5 : La fonction main et ses paramètres | Parsing d'options avec getopt (POSIX) | `05-fonction-main.md` | `Verbose : oui` / `Fichier : output.txt` / `Count : 15` |

## Commandes de compilation

**Fichier unique :**
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <nom_programme> <fichier>.c
```

**Multi-fichiers (programme 03) :**
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 03_operations 03b_operations.c 03c_main_operations.c
```

## Commandes de test (programmes avec arguments CLI)

```bash
./13_afficher_arguments arg1 arg2 "argument avec espaces"
./14_calculatrice_cli 10 + 5
./14_calculatrice_cli 20 / 4
./14_calculatrice_cli 7 x 6
./15_getopt_demo -v -o output.txt -n 15 fichier1 fichier2
```

## Adaptations

- **14_calculatrice_cli.c** : L'opérateur multiplication utilise `x` au lieu de `*` pour éviter le globbing shell (le .md suggère `"*"` avec guillemets)
- **15_getopt_demo.c** : Ajout de `#define _POSIX_C_SOURCE 200809L` en tête de fichier pour rendre `getopt`, `optarg` et `optind` visibles avec `-std=c17` (fonctions POSIX non exposées en mode C strict)

## Statistiques

- **15 programmes** (17 fichiers) couvrant 5 sections sur 5 fichiers .md
- **0 correction** nécessaire (compilation stricte)
- **Sections couvertes** : 6.1 (Déclaration/définition), 6.2 (Stack Frames), 6.3 (Passage par valeur), 6.4 (Valeurs de retour), 6.5 (Fonction main)
