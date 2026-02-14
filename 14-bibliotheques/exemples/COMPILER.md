# Chapitre 14 - Bibliothèques : Compilation des exemples

## 01_biblio_statique/ (Section 01.1/01.2)
**Description** : Création et utilisation d'une bibliothèque statique (.a) avec ar et ranlib
**Fichier source** : 01.1-compilation-ar-ranlib.md
**Fichiers** : math_utils.h, addition.c, multiplication.c, main.c

```bash
cd 01_biblio_statique

# Compiler les fichiers objets
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c addition.c -o addition.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c multiplication.c -o multiplication.o

# Créer la bibliothèque statique
ar rcs libmath_utils.a addition.o multiplication.o

# Compiler le programme avec la bibliothèque
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c -L. -lmath_utils -o programme
./programme

# Sortie attendue :
# Addition : 10 + 5 = 15
# Multiplication : 10 * 5 = 50

# Nettoyage
rm -f *.o *.a programme
```

---

## 02_biblio_dynamique/ (Section 02.1)
**Description** : Création et utilisation d'une bibliothèque dynamique (.so) avec -fPIC
**Fichier source** : 02.1-compilation-fpic.md
**Fichiers** : math_utils.h, addition.c, multiplication.c, main.c

```bash
cd 02_biblio_dynamique

# Compiler avec -fPIC (Position Independent Code)
gcc -Wall -Wextra -Werror -pedantic -std=c17 -fPIC -c addition.c -o addition.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 -fPIC -c multiplication.c -o multiplication.o

# Créer la bibliothèque partagée
gcc -shared addition.o multiplication.o -o libmath_utils.so

# Compiler le programme
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c -L. -lmath_utils -o programme

# Exécuter (nécessite LD_LIBRARY_PATH)
LD_LIBRARY_PATH=. ./programme

# Sortie attendue :
# Addition : 10 + 5 = 15
# Multiplication : 10 * 5 = 50

# Nettoyage
rm -f *.o *.so programme
```

---

## 03_dlopen/ (Section 02.3)
**Description** : Chargement dynamique de bibliothèque avec dlopen/dlsym
**Fichier source** : 02.3-resolution-symboles.md
**Fichiers** : math_utils.c, main.c

```bash
cd 03_dlopen

# Compiler la bibliothèque partagée
gcc -Wall -Wextra -Werror -pedantic -std=c17 -fPIC -shared math_utils.c -o libmath.so

# Compiler le programme (lié avec -ldl pour dlopen/dlsym)
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c -ldl -o programme
./programme

# Sortie attendue :
# 5 + 3 = 8

# Nettoyage
rm -f *.so programme
```

---

## 04_plugin_system/ (Section 02.3)
**Description** : Système de plugins dynamiques avec dlopen/dlsym
**Fichier source** : 02.3-resolution-symboles.md
**Fichiers** : plugin_interface.h, plugin_compression.c, main.c

```bash
cd 04_plugin_system

# Compiler le plugin en .so
gcc -Wall -Wextra -Werror -pedantic -std=c17 -fPIC -shared plugin_compression.c -o plugin_compression.so

# Compiler le chargeur de plugins
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c -ldl -o loader
./loader ./plugin_compression.so

# Sortie attendue :
# Plugin chargé : Compression v1.0
# Plugin Compression initialisé
# Compression de : Données de test
# Plugin Compression nettoyé

# Nettoyage
rm -f *.so loader
```

---

## 05_queue_api/ (Section 06)
**Description** : API propre avec type opaque - file d'attente (queue)
**Fichier source** : 06-creation-api-propre.md
**Fichiers** : queue.h, queue.c, main.c

```bash
cd 05_queue_api

# Compiler la bibliothèque
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c queue.c -o queue.o

# Compiler et lier le programme
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c queue.o -o programme
./programme

# Sortie attendue :
# Taille apres 3 push : 3
# Premier element (peek) : 10
# Pop : 10
# Pop : 20
# Pop : 30
# File vide : oui

# Nettoyage
rm -f *.o programme
```

---

## 06_symbol_visibility/ (Section 07)
**Description** : Contrôle de la visibilité des symboles avec -fvisibility=hidden
**Fichier source** : 07-symbol-visibility.md
**Fichiers** : mylib_export.h, mylib.h, mylib.c, main.c
**Note** : Compilé sans -pedantic (attribut GCC __attribute__((visibility)))

```bash
cd 06_symbol_visibility

# Compiler la bibliothèque avec visibilité cachée par défaut
gcc -Wall -Wextra -Werror -std=c17 -fPIC -fvisibility=hidden -DMYLIB_BUILDING -shared mylib.c -o libmylib.so

# Vérifier les symboles exportés (seules les fonctions MYLIB_API sont visibles)
nm -D libmylib.so | grep " T "
# Attendu : mylib_init, mylib_process, mylib_get_version
# internal_helper n'apparaît PAS

# Compiler et exécuter le programme
gcc -Wall -Wextra -Werror -std=c17 main.c -L. -lmylib -o programme
LD_LIBRARY_PATH=. ./programme

# Sortie attendue :
# Version de la bibliotheque : 1
# Library initialized
# Internal function
# Processing: Hello visibility

# Nettoyage
rm -f *.so programme
```

---

## Fichiers .md sans exemples compilables

Les fichiers suivants sont théoriques ou ne contiennent que des fragments :
- **README.md** : Vue d'ensemble des bibliothèques
- **01-bibliotheques-statiques.md** : Concepts des bibliothèques statiques
- **01.2-linking-statique.md** : Processus de linking (même bibliothèque que 01.1)
- **02-bibliotheques-dynamiques.md** : Concepts des bibliothèques dynamiques
- **02.2-versioning-soname.md** : Versioning SONAME (processus d'installation système)
- **03-differences-statiques-dynamiques.md** : Comparaison statique vs dynamique
- **04-ld-library-path.md** : Configuration LD_LIBRARY_PATH
- **05-ldd-ldconfig.md** : Outils de diagnostic (ldd, ldconfig)
