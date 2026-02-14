# Chapitre 12 - Organisation du code : Guide de compilation

Compilation standard (fichiers mono-source) :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 fichier.c -o fichier
```

Les exemples multi-fichiers nécessitent une compilation séparée ou multi-sources (voir les instructions spécifiques ci-dessous).

---

## 01_include_guards/ (7 fichiers)

- **Section** : 12.1 - Fichiers d'en-tête et gardes d'inclusion
- **Description** : Projet multi-fichiers démontrant les include guards avec trois modules (Point, Rectangle, Math_utils) ; point.h est inclus deux fois (directement + via rectangle.h) sans erreur de redéfinition
- **Fichier source** : 01-fichiers-entete.md
- **Fichiers** : `point.h`, `point.c`, `rectangle.h`, `rectangle.c`, `math_utils.h`, `math_utils.c`, `main.c`
- **Compilation** :
```bash
cd 01_include_guards/
gcc -Wall -Wextra -Werror -pedantic -std=c17 point.c rectangle.c math_utils.c main.c -o 01_include_guards -lm
```
- **Sortie attendue** :
```
=== Module Point ===
Point(3, 4)
Distance a l'origine : 5.00

=== Module Rectangle ===
Aire du rectangle : 50
Perimetre du rectangle : 30

=== Module Math Utils ===
Carre de 7 : 49
Racine de 2 : 1.414214
PI = 3.1415926536
```

## 02_shadowing.c

- **Section** : 12.2 - La portée des variables - Masquage (shadowing)
- **Description** : Démonstration du masquage de variables : une variable interne masque temporairement la variable externe du même nom
- **Fichier source** : 02-portee-variables.md
- **Sortie attendue** :
```
x externe : 10
x interne : 20
x externe apres : 10
```

## 03_static_local.c

- **Section** : 12.2 - La portée des variables - Variables static locales
- **Description** : Variables static locales : persistance de la valeur entre les appels de fonction (compteur d'appels et moyenne avec suivi)
- **Fichier source** : 02-portee-variables.md
- **Sortie attendue** :
```
=== Compteur d'appels ===
Appel numero 1
Appel numero 2
Appel numero 3

=== Moyenne avec suivi ===
Appel #1 : moyenne de 10 et 20 = 15
Appel #2 : moyenne de 5 et 15 = 10
Appel #3 : moyenne de 8 et 12 = 10
```

## 04_static_module/ (2 fichiers)

- **Section** : 12.2 - La portée des variables - Portée de fichier (static)
- **Description** : Module avec état interne privé (variable static au niveau fichier) : encapsulation en C où la variable n'est accessible que via les fonctions publiques
- **Fichier source** : 02-portee-variables.md
- **Fichiers** : `compteur.c`, `main.c`
- **Compilation** :
```bash
cd 04_static_module/
gcc -Wall -Wextra -Werror -pedantic -std=c17 compteur.c main.c -o 04_static_module
```
- **Sortie attendue** :
```
Compteur : 0
Compteur : 2
Compteur : 1
Valeur recuperee : 1
```

## 05_extern_config/ (4 fichiers)

- **Section** : 12.2 - La portée des variables - Variables globales (extern)
- **Description** : Configuration globale partagée entre fichiers via extern : les déclarations sont dans le header, les définitions dans un seul .c
- **Fichier source** : 02-portee-variables.md
- **Fichiers** : `config.h`, `config.c`, `serveur.c`, `main.c`
- **Compilation** :
```bash
cd 05_extern_config/
gcc -Wall -Wextra -Werror -pedantic -std=c17 config.c serveur.c main.c -o 05_extern_config
```
- **Sortie attendue** :
```
Demarrage de MonServeur sur le port 8080
Mode debug active
```

## 06_compilation_separee/ (3 fichiers)

- **Section** : 12.3 - Compilation séparée
- **Description** : Exemple simple de compilation séparée : un module mathématique (header + implémentation) compilé en .o puis lié avec main
- **Fichier source** : 03-compilation-separee.md
- **Fichiers** : `math.h`, `math.c`, `main.c`
- **Compilation** (séparée) :
```bash
cd 06_compilation_separee/
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c math.c
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c main.c
gcc math.o main.o -o 06_compilation_separee
```
- **Compilation** (directe) :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 math.c main.c -o 06_compilation_separee
```
- **Sortie attendue** :
```
10 + 5 = 15
10 * 5 = 50
```

## 07_multi_modules/ (7 fichiers)

- **Section** : 12.3 - Compilation séparée - Projet multi-modules
- **Description** : Projet complet avec trois modules (math_utils, string_utils, config) organisé en sous-dossiers include/ et src/, démontrant la compilation avec -I pour les headers
- **Fichier source** : 03-compilation-separee.md
- **Fichiers** : `include/math_utils.h`, `include/string_utils.h`, `include/config.h`, `src/math_utils.c`, `src/string_utils.c`, `src/config.c`, `src/main.c`
- **Compilation** (séparée) :
```bash
cd 07_multi_modules/
gcc -Wall -Wextra -Werror -pedantic -std=c17 -Iinclude -c src/math_utils.c -o math_utils.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -Iinclude -c src/string_utils.c -o string_utils.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -Iinclude -c src/config.c -o config.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -Iinclude -c src/main.c -o main.o
gcc math_utils.o string_utils.o config.o main.o -o 07_multi_modules
```
- **Compilation** (directe) :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -Iinclude src/*.c -o 07_multi_modules
```
- **Sortie attendue** :
```
=== MonApplication v1.0.0 ===

Tests mathematiques :
5 + 3 = 8
5 * 3 = 15
2^10 = 1024

Tests de chaines :
Texte original : Bonjour
Longueur : 7
Texte inverse : ruojnoB
```

## 08_biblio_statique/ (3 fichiers)

- **Section** : 12.3 - Compilation séparée - Bibliothèques statiques
- **Description** : Création et utilisation d'une bibliothèque statique (.a) avec `ar rcs` : deux fonctions compilées en .o, archivées dans libmath.a, puis liées avec main
- **Fichier source** : 03-compilation-separee.md
- **Fichiers** : `addition.c`, `multiplication.c`, `main.c`
- **Compilation** :
```bash
cd 08_biblio_statique/
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c addition.c
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c multiplication.c
ar rcs libmath.a addition.o multiplication.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c -L. -lmath -o 08_biblio_statique
```
- **Sortie attendue** :
```
5 + 3 = 8
5 * 3 = 15
```

## 09_forward_declarations/ (8 fichiers)

- **Section** : 12.6 - Forward declarations et dépendances circulaires
- **Description** : Système de gestion de bibliothèque avec trois modules interdépendants (Book, Member, Loan) utilisant un header commun types.h pour les forward declarations et typedefs, évitant les dépendances circulaires
- **Fichier source** : 06-forward-declarations.md
- **Fichiers** : `types.h`, `book.h`, `book.c`, `member.h`, `member.c`, `loan.h`, `loan.c`, `main.c`
- **Compilation** :
```bash
cd 09_forward_declarations/
gcc -Wall -Wextra -Werror -pedantic -std=c17 book.c member.c loan.c main.c -o 09_forward_declarations
```
- **Vérification mémoire** :
```bash
valgrind --leak-check=full ./09_forward_declarations
```
- **Sortie attendue** :
```
=== Systeme de gestion de bibliotheque ===
(Demonstration des forward declarations)

=== Livres ===
  "Le Petit Prince" par Saint-Exupery - disponible
  "Les Miserables" par Victor Hugo - disponible

=== Membre ===
  Alice Dupont (ID: 1001) - peut emprunter : oui

=== Emprunt ===
  Emprunt #1 : "Le Petit Prince" par Alice Dupont

=== Apres emprunt ===
  "Le Petit Prince" - emprunte
  Emprunts de Alice Dupont : 1
  Emprunt du meme livre : refuse (deja emprunte)
  Emprunt #2 : "Les Miserables" par Alice Dupont

=== Retour ===
  "Le Petit Prince" retourne - disponible

=== Liberation memoire ===
Memoire liberee.
```

---

## Notes

- Les fichiers `04-organisation-projet.md` et `05-conventions-nommage.md` sont purement théoriques (structures de répertoires, conventions de nommage, standards de codage) et ne contiennent pas d'exemples compilables.
- Tous les programmes multi-fichiers ont été vérifiés en compilation séparée avec `gcc -c`.
- L'exemple `01_include_guards` nécessite `-lm` pour la fonction `sqrt()`.
- L'exemple `09_forward_declarations` a été vérifié avec Valgrind : zéro fuite mémoire.
