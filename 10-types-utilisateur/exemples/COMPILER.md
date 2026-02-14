# Chapitre 10 - Types utilisateur : Compilation et sorties attendues

Compilation standard :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <programme> <fichier.c>
```

Exceptions (extensions GNU `__attribute__((packed))`) - **sans `-pedantic`** :
- `09_packed_comparaison.c`
- `11_benchmark_packed.c`

```bash
gcc -Wall -Wextra -Werror -std=c17 -o <programme> <fichier.c>
```

---

## 01_etudiant.c
- **Section** : 10.1 - struct : Creation d'objets complexes
- **Description** : Gestion d'un etudiant avec structure
- **Fichier source** : 01-structures.md
- **Sortie attendue** :
```
Informations de l'etudiant :
Nom : Alice Dupont
Age : 20 ans
Moyenne : 15.50/20
```

## 02_tableau_structures.c
- **Section** : 10.1 - struct : Creation d'objets complexes
- **Description** : Tableau de structures (classe d'etudiants)
- **Fichier source** : 01-structures.md
- **Sortie attendue** :
```
Etudiant 1 : Alice, 20 ans, moyenne 15.50
Etudiant 2 : Bob, 22 ans, moyenne 14.20
Etudiant 3 : Charlie, 21 ans, moyenne 16.80
```

## 03_carnet_adresses.c
- **Section** : 10.1 - struct : Creation d'objets complexes
- **Description** : Carnet d'adresses simple avec structures
- **Fichier source** : 01-structures.md
- **Sortie attendue** :
```
=== CARNET D'ADRESSES ===

Nom       : Alice Dupont
Telephone : 06.12.34.56.78
Email     : alice@example.com
-------------------
Nom       : Bob Martin
Telephone : 06.98.76.54.32
Email     : bob@example.com
-------------------
```

## 04_ordre_membres.c
- **Section** : 10.2 - Alignement memoire et padding
- **Description** : Impact de l'ordre des membres sur la taille
- **Fichier source** : 02-alignement-padding.md
- **Sortie attendue** :
```
MauvaisOrdre : 12 octets
BonOrdre     : 8 octets
```

## 05_optimisation_padding.c
- **Section** : 10.2 - Alignement memoire et padding
- **Description** : Comparaison avant/apres optimisation de l'ordre des membres
- **Fichier source** : 02-alignement-padding.md
- **Sortie attendue** :
```
Avant optimisation : 24 octets
Apres optimisation : 16 octets
Gain               : 8 octets (33.3%)
```

## 06_offsetof_padding.c
- **Section** : 10.2 - Alignement memoire et padding
- **Description** : Visualiser le padding avec offsetof
- **Fichier source** : 02-alignement-padding.md
- **Sortie attendue** :
```
Taille totale : 12 octets

Offset de c  : 0
Offset de i  : 4
Offset de c2 : 8

Padding apres c  : 3 octets
Padding apres c2 : 3 octets
```

## 07_alignof.c
- **Section** : 10.2 - Alignement memoire et padding
- **Description** : Verifier l'alignement des types avec alignof (C11)
- **Fichier source** : 02-alignement-padding.md
- **Sortie attendue** :
```
Alignement de char   : 1
Alignement de short  : 2
Alignement de int    : 4
Alignement de long   : 8
Alignement de double : 8
Alignement de void*  : 8
```

## 08_benchmark_padding.c
- **Section** : 10.2 - Alignement memoire et padding
- **Description** : Impact du padding sur l'utilisation memoire
- **Fichier source** : 02-alignement-padding.md
- **Sortie attendue** :
```
NonOptimise : 16 octets x 10000000 = 152.59 Mo
Optimise    : 12 octets x 10000000 = 114.44 Mo
```

## 09_packed_comparaison.c
- **Section** : 10.3 - Packed structures
- **Description** : Comparaison tailles Normal vs Packed (attribut et pragma)
- **Fichier source** : 03-packed-structures.md
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17` (sans -pedantic)
- **Sortie attendue** :
```
=== COMPARAISON DES TAILLES ===

Structure normale     : 12 octets
Packed (attribut)     : 7 octets
Packed (pragma)       : 7 octets

=== DISPOSITION MEMOIRE ===

Normal - Offset c : 0
Normal - Offset i : 4
Normal - Offset s : 8

Packed - Offset c : 0
Packed - Offset i : 1
Packed - Offset s : 5
```

## 10_packed_bmp.c
- **Section** : 10.3 - Packed structures
- **Description** : Verification des tailles d'en-tetes BMP avec pragma pack
- **Fichier source** : 03-packed-structures.md
- **Sortie attendue** :
```
=== VERIFICATION DES TAILLES ===

BMPFileHeader : 14 octets (attendu : 14)
BMPInfoHeader : 16 octets (attendu : 16)

Les structures sont correctement packed !
Elles correspondent au format BMP standard.
```

## 11_benchmark_packed.c
- **Section** : 10.3 - Packed structures
- **Description** : Benchmark performance Normal vs Packed
- **Fichier source** : 03-packed-structures.md
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17` (sans -pedantic)
- **Sortie attendue** :
```
=== BENCHMARK (10 millions d'acces) ===

Normal : X.XXX secondes
Packed : X.XXX secondes
Ratio  : X.XXx plus lent

=== UTILISATION MEMOIRE ===

Normal : 76.29 Mo
Packed : 47.68 Mo
Gain   : 28.61 Mo (37.5%)
```

## 12_union_base.c
- **Section** : 10.4 - union : Optimisation memoire
- **Description** : Exemple basique d'union (partage memoire entre types)
- **Fichier source** : 04-unions.md
- **Sortie attendue** :
```
Taille de l'union : 4 octets

Entier : 42
Float  : 3.14
Entier : 1078523331 (valeur invalide !)

Char   : A
Float  : 3.14 (valeur corrompue, seul le 1er octet a change !)
```

## 13_tagged_union.c
- **Section** : 10.4 - union : Optimisation memoire
- **Description** : Tagged union (union avec champ type pour securite)
- **Fichier source** : 04-unions.md
- **Sortie attendue** :
```
Entier : 42
Float  : 3.14
String : Bonjour
```

## 14_union_messages.c
- **Section** : 10.4 - union : Optimisation memoire
- **Description** : Systeme de messages avec types differents via union
- **Fichier source** : 04-unions.md
- **Sortie attendue** :
```
[TEXTE] Connexion etablie
[NOMBRE] 42
[ERREUR 404] Fichier introuvable
```

## 15_endianness.c
- **Section** : 10.4 - union : Optimisation memoire
- **Description** : Detection de l'endianness via union
- **Fichier source** : 04-unions.md
- **Sortie attendue** :
```
Valeur : 0x12345678
Octets : 78 56 34 12
Architecture : Little Endian
```

## 16_union_tailles.c
- **Section** : 10.4 - union : Optimisation memoire
- **Description** : Comparaison de tailles struct sans/avec union
- **Fichier source** : 04-unions.md
- **Sortie attendue** :
```
Taille struct Personne1 : 108 octets
Taille struct Personne2 : 72 octets
Gain : 36 octets
```

## 17_systeme_variables.c
- **Section** : 10.4 - union : Optimisation memoire
- **Description** : Systeme de gestion de variables typees avec tagged union
- **Fichier source** : 04-unions.md
- **Sortie attendue** :
```
Variable 'compteur' = 42 (int)
Variable 'prix' = 19.99 (double)
Variable 'nom' = "Alice" (string)
Variable 'actif' = true (bool)
```

## 18_enum_jour.c
- **Section** : 10.5 - enum : Code lisible et gestion d'etats
- **Description** : Premier exemple d'enumeration (jours de la semaine)
- **Fichier source** : 05-enumerations.md
- **Sortie attendue** :
```
Aujourd'hui est le jour numero : 2
C'est mercredi !
```

## 19_machine_etats.c
- **Section** : 10.5 - enum : Code lisible et gestion d'etats
- **Description** : Machine a etats de connexion avec enum
- **Fichier source** : 05-enumerations.md
- **Sortie attendue** :
```
Transition : DECONNECTE -> CONNEXION_EN_COURS
Transition : CONNEXION_EN_COURS -> CONNECTE
Transition : CONNECTE -> DECONNEXION_EN_COURS
Transition : DECONNEXION_EN_COURS -> DECONNECTE
```

## 20_enum_options.c
- **Section** : 10.5 - enum : Code lisible et gestion d'etats
- **Description** : Options de configuration avec enum et operateurs bit-a-bit
- **Fichier source** : 05-enumerations.md
- **Sortie attendue** :
```
Mode verbose active
Mode debug active
Couleurs desactivees
```

## 21_enum_boucle.c
- **Section** : 10.5 - enum : Code lisible et gestion d'etats
- **Description** : Iteration sur enumeration avec compteur NB_JOURS
- **Fichier source** : 05-enumerations.md
- **Sortie attendue** :
```
Jours de la semaine :
0 : Lundi
1 : Mardi
2 : Mercredi
3 : Jeudi
4 : Vendredi
5 : Samedi
6 : Dimanche
```

## 22_analyseur_commandes.c
- **Section** : 10.5 - enum : Code lisible et gestion d'etats
- **Description** : Analyseur de commandes complet avec enum et tableau
- **Fichier source** : 05-enumerations.md
- **Sortie attendue** :
```
> aide

=== COMMANDES DISPONIBLES ===

  aide         - Affiche l'aide
  quitter      - Quitte le programme
  liste        - Liste tous les elements
  ajouter      - Ajoute un element
  supprimer    - Supprime un element


> liste
Liste des elements : [element1, element2, element3]

> ajouter
Ajout d'un nouvel element...

> blabla
Commande inconnue. Tapez 'aide' pour voir les commandes.

> quitter
Au revoir !
```

## 23_typedef_struct.c
- **Section** : 10.6 - typedef : Simplification de la syntaxe
- **Description** : typedef avec struct pour simplifier les declarations
- **Fichier source** : 06-typedef.md
- **Sortie attendue** :
```
Nom : Alice Dupont
Age : 20 ans
Moyenne : 15.50/20

Nom : Bob Martin
Age : 22 ans
Moyenne : 14.20/20
```

## 24_typedef_union.c
- **Section** : 10.6 - typedef : Simplification de la syntaxe
- **Description** : Tagged union avec typedef (enum + union + struct)
- **Fichier source** : 06-typedef.md
- **Sortie attendue** :
```
Entier : 42
Float : 3.14
String : Bonjour
```

## 25_calculatrice.c
- **Section** : 10.6 - typedef : Simplification de la syntaxe
- **Description** : Calculatrice avec typedef de pointeurs de fonctions
- **Fichier source** : 06-typedef.md
- **Sortie attendue** :
```
10 + 5 = 15
10 - 5 = 5
10 * 5 = 50
10 / 5 = 2
```

## 26_callbacks.c
- **Section** : 10.6 - typedef : Simplification de la syntaxe
- **Description** : Callbacks avec typedef de pointeurs de fonctions
- **Fichier source** : 06-typedef.md
- **Sortie attendue** :
```
Valeurs : 1 2 3 4 5
Doubles : 2 4 6 8 10
```

## 27_tri_generique.c
- **Section** : 10.6 - typedef : Simplification de la syntaxe
- **Description** : Tri bulle generique avec typedef CompareFn et void*
- **Fichier source** : 06-typedef.md
- **Sortie attendue** :
```
Avant tri : 5 2 8 1 9 3
Apres tri : 1 2 3 5 8 9
```

## 28_machine_etats_typedef.c
- **Section** : 10.6 - typedef : Simplification de la syntaxe
- **Description** : Machine a etats avec typedef de pointeurs de fonctions
- **Fichier source** : 06-typedef.md
- **Sortie attendue** :
```
Demarrage...
Mise en pause...
Reprise...
Arret...
```

## 29_bitfields_base.c
- **Section** : 10.7 - Structures bit-fields
- **Description** : Drapeaux simples avec bit-fields (1 bit par flag)
- **Fichier source** : 07-bit-fields.md
- **Sortie attendue** :
```
Taille : 4 octet(s)
flag1 : 1
flag2 : 0
flag3 : 1
```

## 30_options_affichage.c
- **Section** : 10.7 - Structures bit-fields
- **Description** : Options d'affichage avec bit-fields (gras, italique, etc.)
- **Fichier source** : 07-bit-fields.md
- **Sortie attendue** :
```
Taille : 4 octet(s)
Options actives : Gras Italique
Options actives : Gras Italique Souligne
```

## 31_etat_joueur.c
- **Section** : 10.7 - Structures bit-fields
- **Description** : Etat d'un joueur avec bit-fields (vie, niveau, arme, etc.)
- **Fichier source** : 07-bit-fields.md
- **Sortie attendue** :
```
=== ETAT DU JOUEUR ===
Vie : 100/127
Niveau : 5
Invincible : Non
Ennemis tues : 42
Arme : 3
Taille : 4 octets
```

## 32_config_systeme.c
- **Section** : 10.7 - Structures bit-fields
- **Description** : Configuration systeme complete avec bit-fields
- **Fichier source** : 07-bit-fields.md
- **Sortie attendue** :
```
=== CONFIGURATION SYSTEME ===
Verbose       : Non
Debug         : Non
Couleur       : Oui
Niveau log    : 3
Threads       : 4
Cache         : Actif
Compression   : Rapide
Protocole     : v2
Taille config : 4 octets

=== Activation mode debug ===
=== CONFIGURATION SYSTEME ===
Verbose       : Oui
Debug         : Oui
Couleur       : Oui
Niveau log    : 7
Threads       : 4
Cache         : Actif
Compression   : Rapide
Protocole     : v2
Taille config : 4 octets
```
