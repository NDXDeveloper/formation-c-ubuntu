🔝 Retour au [Sommaire](/SOMMAIRE.md)

# B.1 Options GCC (Cheat Sheet)

## Introduction

**GCC** (GNU Compiler Collection) est le compilateur C le plus utilisé sur Linux. Connaître ses options principales est essentiel pour :
- ✅ Compiler correctement vos programmes
- ✅ Détecter les bugs tôt (warnings)
- ✅ Optimiser les performances
- ✅ Débugger efficacement
- ✅ Assurer la portabilité

💡 **Pour les débutants :** Vous n'avez pas besoin de connaître toutes ces options ! Ce document est une **référence** à consulter selon vos besoins.

---

## Syntaxe de base

### Compilation simple

```bash
gcc fichier.c -o programme
```

**Explication :**
- `gcc` : Le compilateur
- `fichier.c` : Votre code source
- `-o programme` : Nom de l'exécutable généré (output)

**Sans `-o` :**
```bash
gcc fichier.c
# Génère "a.out" par défaut
```

### Compilation avec plusieurs fichiers

```bash
gcc fichier1.c fichier2.c fichier3.c -o programme
```

---

## 📊 Options par catégorie

## 🔧 Options de compilation de base

### `-c` : Compiler sans lier
```bash
gcc -c fichier.c
# Génère fichier.o (fichier objet)
```

**Quand l'utiliser :**
- Compilation séparée (grands projets)
- Pas besoin de relinkage après modification d'un seul fichier

**Exemple complet :**
```bash
# Étape 1 : Compiler séparément
gcc -c main.c       # → main.o  
gcc -c utils.c      # → utils.o  

# Étape 2 : Lier (linker)
gcc main.o utils.o -o programme
```

---

### `-o` : Spécifier le nom de sortie
```bash
gcc fichier.c -o mon_programme
```

**Sans `-o` :**
- Exécutable : `a.out`
- Fichier objet : `fichier.o`

---

### `-E` : Arrêter après le préprocesseur
```bash
gcc -E fichier.c
# Affiche le code après expansion des macros et #include
```

**Utilité :**
- Débugger les problèmes de macros
- Voir ce que fait réellement le préprocesseur

**Rediriger vers un fichier :**
```bash
gcc -E fichier.c > fichier.i
```

---

### `-S` : Générer l'assembleur
```bash
gcc -S fichier.c
# Génère fichier.s (code assembleur)
```

**Utilité :**
- Comprendre ce que le compilateur génère
- Optimisation bas niveau
- Apprentissage de l'assembleur

---

### `-v` : Mode verbose (détails de compilation)
```bash
gcc -v fichier.c -o programme
```

**Affiche :**
- Version de GCC
- Chemins de recherche
- Commandes exactes exécutées
- Options implicites

---

## ⚠️ Options de warnings (Avertissements)

### `-Wall` : Activer les warnings standards
```bash
gcc -Wall fichier.c -o programme
```

**Active :**
- Variables non utilisées
- Fonctions sans retour
- Comparaisons douteuses
- Conversions implicites dangereuses
- Et ~30 autres warnings

**🔥 TOUJOURS UTILISER `-Wall` !**

**Exemple :**
```c
int main() {
    int x;
    printf("%d\n", x);  // Warning : x non initialisé
    return 0;
}
```

```bash
$ gcc -Wall test.c
test.c:3:20: warning: 'x' is used uninitialized [-Wuninitialized]
```

---

### `-Wextra` : Warnings supplémentaires
```bash
gcc -Wall -Wextra fichier.c -o programme
```

**Active en plus :**
- Paramètres non utilisés
- Comparaisons signées/non signées
- Champs manquants dans les initialisations
- Et ~20 warnings additionnels

**Recommandation : Utilisez toujours `-Wall -Wextra`**

---

### `-Werror` : Transformer warnings en erreurs
```bash
gcc -Wall -Wextra -Werror fichier.c -o programme
```

**Effet :** Le code ne compile pas s'il y a des warnings.

**Quand l'utiliser :**
- ✅ CI/CD (forcer un code propre)
- ✅ Projets critiques
- ✅ Apprentissage (se forcer à corriger tout)

**Attention :** Peut être frustrant pour débuter.

---

### `-Wpedantic` ou `-pedantic` : Conformité stricte aux standards
```bash
gcc -Wall -Wextra -pedantic fichier.c -o programme
```

**Vérifie :**
- Conformité ISO C
- Extensions GNU non standard
- Syntaxe non portable

**Exemple :**
```c
// Extension GNU : expression entre accolades (statement expression)
int x = ({ int y = 5; y + 1; });  // Warning avec -pedantic
```

---

### Autres warnings utiles

#### `-Wshadow` : Variables masquées
```bash
gcc -Wall -Wshadow fichier.c
```

```c
int x = 10;  
void func() {  
    int x = 20;  // Warning : masque la variable globale x
}
```

---

#### `-Wconversion` : Conversions de types dangereuses
```bash
gcc -Wall -Wconversion fichier.c
```

```c
int x = 3.14;  // Warning : perte de précision
```

**Attention :** Génère BEAUCOUP de warnings (peut être trop verbeux).

---

#### `-Wuninitialized` : Variables non initialisées
```bash
gcc -Wall -Wuninitialized fichier.c
```

Déjà activé par `-Wall`, mais peut être forcé.

---

#### `-Wcast-align` : Alignement des casts
```bash
gcc -Wall -Wcast-align fichier.c
```

Détecte les casts qui peuvent causer des problèmes d'alignement mémoire.

---

### 🎯 Combinaison recommandée pour la production

```bash
gcc -std=c17 -Wall -Wextra -Werror -pedantic -O2 fichier.c -o programme
```

---

## 🔍 Options de debugging

### `-g` : Inclure les informations de debug
```bash
gcc -g fichier.c -o programme
```

**Permet :**
- Utiliser GDB pour debugger
- Voir les noms de variables
- Voir les numéros de ligne
- Inspecter la stack

**Utilisation avec GDB :**
```bash
gcc -g programme.c -o programme  
gdb ./programme  
```

**Taille du binaire :** Augmente significativement.

**Solution :** Compiler avec `-g` pour debug, sans pour production.

---

### `-g3` : Maximum d'informations de debug
```bash
gcc -g3 fichier.c -o programme
```

**Inclut en plus :**
- Définitions de macros
- Informations supplémentaires pour GDB

---

### `-ggdb` : Optimiser pour GDB
```bash
gcc -ggdb fichier.c -o programme
```

Génère des infos de debug spécifiques à GDB (plus détaillées).

---

### `-pg` : Profiling avec gprof
```bash
gcc -pg fichier.c -o programme
./programme
gprof ./programme gmon.out > analyse.txt
```

**Permet :**
- Mesurer le temps passé dans chaque fonction
- Identifier les bottlenecks
- Optimiser les performances

---

## ⚡ Options d'optimisation

### Niveaux d'optimisation

#### `-O0` : Aucune optimisation (par défaut)
```bash
gcc -O0 fichier.c -o programme
```

**Caractéristiques :**
- Compilation rapide
- Code facile à débugger
- Performance minimale

**Quand l'utiliser :** Développement et debugging

---

#### `-O1` : Optimisations basiques
```bash
gcc -O1 fichier.c -o programme
```

**Active :**
- Optimisations simples qui ne prennent pas trop de temps
- Équilibre compilation/performance

---

#### `-O2` : Optimisations recommandées
```bash
gcc -O2 fichier.c -o programme
```

**Active :**
- Toutes les optimisations de `-O1`
- Plus ~40 optimisations additionnelles
- N'augmente pas la taille du code
- Pas de compromis vitesse/espace

**🔥 NIVEAU RECOMMANDÉ POUR LA PRODUCTION**

**Exemples d'optimisations :**
- Élimination de code mort
- Déroulage de boucles (loop unrolling)
- Optimisation des registres
- Inlining de fonctions

---

#### `-O3` : Optimisations agressives
```bash
gcc -O3 fichier.c -o programme
```

**Active :**
- Toutes les optimisations de `-O2`
- Optimisations plus agressives
- Peut augmenter la taille du code
- Vectorisation automatique

**Quand l'utiliser :**
- Performance critique (calcul scientifique, jeux)
- Après avoir vérifié que ça n'introduit pas de bugs

**Attention :** Peut casser du code non conforme aux standards.

---

#### `-Os` : Optimiser pour la taille
```bash
gcc -Os fichier.c -o programme
```

**Objectif :** Minimiser la taille de l'exécutable

**Quand l'utiliser :**
- Systèmes embarqués avec mémoire limitée
- Contraintes de taille strictes

---

#### `-Ofast` : Performance maximale (non standard)
```bash
gcc -Ofast fichier.c -o programme
```

**Active :**
- Toutes les optimisations de `-O3`
- Optimisations qui peuvent violer les standards (calculs flottants)

**⚠️ Attention :** Peut produire des résultats incorrects dans certains cas.

---

### Optimisations spécifiques

#### `-finline-functions` : Forcer l'inlining
```bash
gcc -O2 -finline-functions fichier.c
```

#### `-funroll-loops` : Dérouler les boucles
```bash
gcc -O2 -funroll-loops fichier.c
```

#### `-ffast-math` : Optimisations mathématiques agressives
```bash
gcc -O2 -ffast-math fichier.c
```

**⚠️ Attention :** Peut affecter la précision des calculs flottants.

---

## 📏 Options de standard C

### `-std=` : Spécifier le standard C
```bash
gcc -std=c89 fichier.c    # C89/C90 (ANSI C)  
gcc -std=c99 fichier.c    # C99  
gcc -std=c11 fichier.c    # C11
gcc -std=c17 fichier.c    # C17 (recommandé)
gcc -std=c2x fichier.c    # C23 (expérimental)  
```

**Recommandation 2025 :** `-std=c17` (ou `-std=c17` minimum)

---

### `-std=gnu17` : C17 + extensions GNU
```bash
gcc -std=gnu17 fichier.c
```

**Différence avec `-std=c17` :**
- Autorise les extensions GNU (typeof, __attribute__, etc.)
- Moins portable
- Plus de fonctionnalités

**Par défaut :** GCC utilise `gnu17` (ou variante GNU) depuis GCC 8+

---

## 🔗 Options de linkage (édition de liens)

### `-l` : Lier avec une bibliothèque
```bash
gcc fichier.c -o programme -lm
```

**Exemple :** `-lm` lie avec `libm.so` (bibliothèque mathématique)

**Syntaxe :** `-l<nom>` cherche `lib<nom>.so` ou `lib<nom>.a`

**Bibliothèques courantes :**
```bash
-lm         # Mathématiques (sin, cos, sqrt)
-lpthread   # Threads POSIX
-lrt        # Real-time (timers, etc.)
-ldl        # Dynamic loading
-lcrypto    # OpenSSL crypto
-lssl       # OpenSSL SSL
```

---

### `-L` : Ajouter un chemin de recherche de bibliothèques
```bash
gcc fichier.c -o programme -L/usr/local/lib -lmylib
```

**Utilité :** Chercher des bibliothèques dans des dossiers non standard.

---

### `-I` : Ajouter un chemin de recherche d'en-têtes
```bash
gcc -I/usr/local/include fichier.c -o programme
```

**Utilité :** Inclure des fichiers d'en-tête hors chemins standards.

**Exemple :**
```c
#include "mylib.h"  // Cherché dans /usr/local/include si -I est spécifié
```

---

### `-static` : Linkage statique
```bash
gcc -static fichier.c -o programme
```

**Effet :**
- Lie les bibliothèques statiquement (tout dans le binaire)
- Exécutable plus gros
- Pas de dépendances externes (.so)
- Portable mais volumineux

---

### `-shared` : Créer une bibliothèque partagée
```bash
gcc -shared -fPIC fichier.c -o libmylib.so
```

**Options combinées :**
- `-shared` : Créer un .so
- `-fPIC` : Position Independent Code (requis pour .so)

---

## 🛡️ Options de sécurité et hardening

### `-fstack-protector-strong` : Protection contre buffer overflow
```bash
gcc -fstack-protector-strong fichier.c -o programme
```

**Effet :**
- Détecte les corruptions de stack
- Ajoute des "canaries" (valeurs sentinelles)
- Performance impact minimal

**Recommandé pour la production.**

---

### `-D_FORTIFY_SOURCE=2` : Vérifications runtime
```bash
gcc -O2 -D_FORTIFY_SOURCE=2 fichier.c -o programme
```

**Effet :**
- Vérifie les buffer overflows (strcpy, memcpy, etc.)
- Nécessite `-O1` ou plus

**🔥 Recommandé pour la production.**

---

### `-fPIE -pie` : Position Independent Executable
```bash
gcc -fPIE -pie fichier.c -o programme
```

**Effet :**
- Exécutable à position indépendante
- Améliore la sécurité (ASLR)
- Standard sur distributions modernes

---

### `-Wformat -Wformat-security` : Sécurité des format strings
```bash
gcc -Wall -Wformat -Wformat-security fichier.c
```

**Détecte :**
```c
char *user_input = get_input();  
printf(user_input);  // DANGER ! Format string vulnerability  
// Correct :
printf("%s", user_input);
```

---

### Combinaison hardening complète

```bash
gcc -std=c17 -Wall -Wextra -Werror \
    -O2 -D_FORTIFY_SOURCE=2 \
    -fstack-protector-strong \
    -fPIE -pie \
    -Wformat -Wformat-security \
    fichier.c -o programme
```

---

## 🧪 Options pour les sanitizers

### AddressSanitizer (ASan)
```bash
gcc -fsanitize=address -g fichier.c -o programme
```

**Détecte :**
- Buffer overflows
- Use-after-free
- Double free
- Memory leaks

**Exemple d'utilisation :**
```bash
gcc -fsanitize=address -g test.c -o test
./test
# Si bug mémoire → rapport détaillé
```

---

### UndefinedBehaviorSanitizer (UBSan)
```bash
gcc -fsanitize=undefined -g fichier.c -o programme
```

**Détecte :**
- Integer overflow
- Division par zéro
- Déréférencement de NULL
- Comportement indéfini

---

### ThreadSanitizer (TSan)
```bash
gcc -fsanitize=thread -g fichier.c -o programme -lpthread
```

**Détecte :**
- Race conditions
- Deadlocks
- Problèmes de synchronisation

**Note :** Ne peut pas être combiné avec ASan.

---

### LeakSanitizer (LSan)
```bash
gcc -fsanitize=leak -g fichier.c -o programme
```

**Détecte :**
- Fuites mémoire (memory leaks)

**Note :** Inclus dans AddressSanitizer.

---

### Combiner plusieurs sanitizers
```bash
# ASan + UBSan + LSan
gcc -fsanitize=address,undefined,leak -g fichier.c -o programme
```

**Attention :** TSan ne peut pas être combiné avec ASan.

---

## 📦 Options de génération de code

### `-march=` : Architecture cible
```bash
gcc -march=native fichier.c -o programme
```

**Options courantes :**
- `-march=native` : Optimise pour le CPU actuel
- `-march=x86-64` : Compatible x86-64 générique
- `-march=armv8-a` : ARM 64 bits

**Utilité :** Performance maximale sur architecture spécifique.

---

### `-mtune=` : Optimiser pour une architecture
```bash
gcc -mtune=native fichier.c -o programme
```

**Différence avec `-march` :**
- `-march` : Code spécifique à l'archi (peut ne pas fonctionner ailleurs)
- `-mtune` : Optimise pour l'archi mais reste compatible

---

### `-m32` / `-m64` : Architecture 32/64 bits
```bash
gcc -m32 fichier.c -o programme32  
gcc -m64 fichier.c -o programme64  
```

---

## 🔬 Options d'analyse

### `-save-temps` : Garder les fichiers intermédiaires
```bash
gcc -save-temps fichier.c -o programme
```

**Génère :**
- `fichier.i` : Après préprocesseur
- `fichier.s` : Code assembleur
- `fichier.o` : Fichier objet

**Utilité :** Comprendre la chaîne de compilation.

---

### `-dumpversion` : Afficher la version de GCC
```bash
gcc -dumpversion
# Exemple : 11.4.0
```

---

### `-dumpmachine` : Afficher la cible
```bash
gcc -dumpmachine
# Exemple : x86_64-linux-gnu
```

---

### `-Q --help=optimizers` : Lister les optimisations actives
```bash
gcc -O2 -Q --help=optimizers | grep enabled
```

---

## 🎨 Options de format de sortie

### `-M` : Générer les dépendances
```bash
gcc -M fichier.c
```

**Affiche :** Liste des fichiers dont dépend `fichier.c` (tous les #include).

**Utilité :** Générer des règles pour Makefile.

---

### `-MM` : Dépendances (sans fichiers système)
```bash
gcc -MM fichier.c
```

**Affiche :** Comme `-M` mais ignore les headers système (`<stdio.h>`, etc.).

---

## 🧩 Options pour projets multi-fichiers

### Compilation séparée complète

```bash
# Étape 1 : Compiler chaque .c en .o
gcc -c -Wall -Wextra -std=c17 main.c -o main.o  
gcc -c -Wall -Wextra -std=c17 utils.c -o utils.o  
gcc -c -Wall -Wextra -std=c17 data.c -o data.o  

# Étape 2 : Linker tous les .o
gcc main.o utils.o data.o -o programme

# Étape 3 : Exécuter
./programme
```

---

### Avec bibliothèque statique

```bash
# Créer la bibliothèque statique
gcc -c utils.c -o utils.o  
gcc -c data.c -o data.o  
ar rcs libmylib.a utils.o data.o  

# Compiler le programme principal
gcc -c main.c -o main.o  
gcc main.o -L. -lmylib -o programme  
```

---

### Avec bibliothèque partagée

```bash
# Créer la bibliothèque partagée
gcc -c -fPIC utils.c -o utils.o  
gcc -c -fPIC data.c -o data.o  
gcc -shared utils.o data.o -o libmylib.so  

# Compiler le programme principal
gcc main.c -L. -lmylib -o programme

# Exécuter (avec LD_LIBRARY_PATH)
LD_LIBRARY_PATH=. ./programme
```

---

## 🎯 Combinaisons recommandées

### Pour débuter (apprentissage)
```bash
gcc -std=c17 -Wall -Wextra -g fichier.c -o programme
```

**Explications :**
- `-std=c17` : Standard C17
- `-Wall -Wextra` : Tous les warnings
- `-g` : Infos de debug pour GDB

---

### Pour le développement quotidien
```bash
gcc -std=c17 -Wall -Wextra -Werror -g -O0 fichier.c -o programme
```

**Ajouts :**
- `-Werror` : Forcer la correction des warnings
- `-O0` : Pas d'optimisation (debug facile)

---

### Pour les tests (avec sanitizers)
```bash
gcc -std=c17 -Wall -Wextra -Werror \
    -g -O1 \
    -fsanitize=address,undefined \
    fichier.c -o programme
```

**Ajouts :**
- `-O1` : Optimisation minimale (sanitizers fonctionnent mieux)
- `-fsanitize=address,undefined` : Détection de bugs

---

### Pour la production (release)
```bash
gcc -std=c17 -Wall -Wextra -Werror \
    -O2 -DNDEBUG \
    -D_FORTIFY_SOURCE=2 \
    -fstack-protector-strong \
    -fPIE -pie \
    fichier.c -o programme
```

**Explications :**
- `-O2` : Optimisations recommandées
- `-DNDEBUG` : Désactive les assert()
- Flags de sécurité (hardening)

---

### Pour la performance maximale
```bash
gcc -std=c17 -Wall -Wextra \
    -O3 -march=native -flto \
    fichier.c -o programme
```

**Ajouts :**
- `-O3` : Optimisations agressives
- `-march=native` : Optimise pour votre CPU
- `-flto` : Link-Time Optimization

---

### Pour le code portable
```bash
gcc -std=c17 -Wall -Wextra -pedantic -Werror \
    -O2 \
    fichier.c -o programme
```

**Ajouts :**
- `-pedantic` : Conformité stricte ISO C

---

## 📊 Tableau récapitulatif des options essentielles

| Option | Catégorie | Description | Quand l'utiliser |
|--------|-----------|-------------|------------------|
| **-Wall** | Warnings | Warnings standards | **TOUJOURS** |
| **-Wextra** | Warnings | Warnings supplémentaires | **TOUJOURS** |
| **-Werror** | Warnings | Warnings = erreurs | CI/CD, production |
| **-pedantic** | Warnings | Conformité stricte | Code portable |
| **-g** | Debug | Informations debug | Développement |
| **-O0** | Optim | Aucune optimisation | Debug |
| **-O1** | Optim | Optimisations basiques | Tests |
| **-O2** | Optim | Optimisations recommandées | **Production** |
| **-O3** | Optim | Optimisations agressives | Performance |
| **-Os** | Optim | Optimiser taille | Embarqué |
| **-std=c17** | Standard | Standard C17 | **Production** |
| **-fsanitize=address** | Sanitizer | Détection mémoire | Tests |
| **-fsanitize=undefined** | Sanitizer | Comportement indéfini | Tests |
| **-fstack-protector-strong** | Sécurité | Protection stack | Production |
| **-D_FORTIFY_SOURCE=2** | Sécurité | Vérifications runtime | Production |
| **-fPIE -pie** | Sécurité | PIE | Production |
| **-c** | Compilation | Compiler sans lier | Projets multi-fichiers |
| **-o** | Compilation | Nom de sortie | **TOUJOURS** |
| **-I** | Include | Chemin headers | Bibliothèques externes |
| **-L** | Link | Chemin bibliothèques | Bibliothèques externes |
| **-l** | Link | Lier bibliothèque | Librairies (math, pthread) |

---

## 🎓 Exemples pratiques complets

### Exemple 1 : Programme simple avec mathématiques
```bash
# hello_math.c utilise sqrt() de <math.h>
gcc -std=c17 -Wall -Wextra -O2 hello_math.c -o hello_math -lm

# Explications :
# -lm : lier avec libm (bibliothèque mathématique)
```

---

### Exemple 2 : Programme multi-threadé
```bash
# threaded.c utilise pthread
gcc -std=c17 -Wall -Wextra -g \
    -fsanitize=thread \
    threaded.c -o threaded -lpthread

# Explications :
# -fsanitize=thread : détecter race conditions
# -lpthread : lier avec libpthread
```

---

### Exemple 3 : Projet avec structure
```
projet/
├── src/
│   ├── main.c
│   ├── utils.c
│   └── data.c
├── include/
│   ├── utils.h
│   └── data.h
└── build/
```

```bash
# Compiler chaque fichier source
gcc -c -std=c17 -Wall -Wextra -Iinclude \
    src/main.c -o build/main.o

gcc -c -std=c17 -Wall -Wextra -Iinclude \
    src/utils.c -o build/utils.o

gcc -c -std=c17 -Wall -Wextra -Iinclude \
    src/data.c -o build/data.o

# Linker
gcc build/main.o build/utils.o build/data.o \
    -o build/programme

# Explications :
# -Iinclude : chercher les .h dans include/
```

---

### Exemple 4 : Debug avec Valgrind et ASan
```bash
# Version ASan (rapide, infos détaillées)
gcc -std=c17 -Wall -Wextra -g \
    -fsanitize=address \
    program.c -o program_asan

./program_asan

# Version Valgrind (plus lent, plus complet)
gcc -std=c17 -Wall -Wextra -g \
    program.c -o program_valgrind

valgrind --leak-check=full ./program_valgrind
```

---

### Exemple 5 : Cross-compilation pour ARM
```bash
# Installer le cross-compiler
sudo apt install gcc-arm-linux-gnueabihf

# Compiler pour ARM
arm-linux-gnueabihf-gcc -std=c17 -Wall -O2 \
    program.c -o program_arm

# Vérifier l'architecture
file program_arm
# Output : program_arm: ELF 32-bit LSB executable, ARM, ...
```

---

## 🔍 Débugger les options de compilation

### Voir les options par défaut de GCC
```bash
gcc -Q --help=target  
gcc -Q --help=optimizers  
gcc -Q --help=warnings  
```

---

### Voir ce que fait une option précise
```bash
gcc --help=warning | grep Wshadow
```

---

### Vérifier les symboles dans un binaire
```bash
# Avec informations de debug
gcc -g program.c -o program  
file program  
# Output : ..., with debug_info, not stripped

# Sans informations de debug
gcc program.c -o program  
strip program  
file program  
# Output : ..., stripped
```

---

### Comparer la taille des binaires
```bash
# Sans optimisation
gcc program.c -o program_o0  
ls -lh program_o0  

# Avec -O2
gcc -O2 program.c -o program_o2  
ls -lh program_o2  

# Avec -Os (taille)
gcc -Os program.c -o program_os  
ls -lh program_os  
```

---

## 💡 Bonnes pratiques

### 1. Variables d'environnement
Définir `CFLAGS` pour ne pas répéter :

```bash
# Dans ~/.bashrc ou script
export CFLAGS="-std=c17 -Wall -Wextra -Werror -g"

# Utilisation
gcc $CFLAGS program.c -o program
```

---

### 2. Makefile avec options
```makefile
CC = gcc  
CFLAGS = -std=c17 -Wall -Wextra -Werror -O2  
LDFLAGS = -lm -lpthread  

program: main.o utils.o
	$(CC) $(CFLAGS) main.o utils.o -o program $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o program
```

---

### 3. Scripts de compilation
```bash
#!/bin/bash
# build.sh

# Mode debug ou release
MODE=${1:-debug}

if [ "$MODE" = "debug" ]; then
    FLAGS="-std=c17 -Wall -Wextra -g -O0 -fsanitize=address"
elif [ "$MODE" = "release" ]; then
    FLAGS="-std=c17 -Wall -Wextra -Werror -O2 -DNDEBUG -D_FORTIFY_SOURCE=2"
else
    echo "Usage: $0 [debug|release]"
    exit 1
fi

gcc $FLAGS src/*.c -o build/program -lm

echo "Compiled in $MODE mode"
```

---

### 4. Options par type de fichier
```bash
# Headers
gcc -std=c17 -Wall -Wextra -Iinclude -c src/file.c

# Bibliothèque partagée
gcc -std=c17 -Wall -fPIC -shared lib.c -o libmylib.so

# Exécutable
gcc -std=c17 -Wall -Wextra main.c -L. -lmylib -o program
```

---

## ⚠️ Pièges courants

### 1. Oublier `-lm` pour les fonctions mathématiques
```bash
# ❌ Erreur de link
gcc program.c -o program
# undefined reference to `sqrt'

# ✅ Correct
gcc program.c -o program -lm
```

---

### 2. Ordre des options `-l`
```bash
# ❌ Mauvais ordre
gcc -lm program.c -o program
# Peut causer des erreurs de link

# ✅ Bon ordre
gcc program.c -o program -lm
```

**Règle :** Les bibliothèques (`-l`) doivent venir **après** les fichiers sources.

---

### 3. Oublier `-fPIC` pour les .so
```bash
# ❌ Sans -fPIC
gcc -shared lib.c -o libmylib.so
# Peut causer des erreurs au runtime

# ✅ Avec -fPIC
gcc -fPIC -shared lib.c -o libmylib.so
```

---

### 4. Mélanger `-O3` et `-g` en production
```bash
# ⚠️ Moins optimal
gcc -O3 -g program.c -o program

# ✅ Debug
gcc -g -O0 program.c -o program_debug

# ✅ Release
gcc -O2 program.c -o program_release
```

**Raison :** `-g` augmente la taille, `-O3` peut rendre le debug difficile.

---

### 5. Négliger les warnings
```bash
# ❌ Mauvaise pratique
gcc program.c -o program
# Ignore les warnings

# ✅ Bonne pratique
gcc -Wall -Wextra -Werror program.c -o program
```

---

## 🎯 Checklist : Options à connaître absolument

### ✅ Niveau Débutant
- [ ] `-o` : Spécifier le nom de sortie
- [ ] `-Wall` : Activer les warnings
- [ ] `-Wextra` : Warnings supplémentaires
- [ ] `-g` : Informations de debug
- [ ] `-std=c17` : Standard C17
- [ ] `-lm` : Lier avec libmath

### ✅ Niveau Intermédiaire
- [ ] `-c` : Compiler sans lier
- [ ] `-Werror` : Warnings = erreurs
- [ ] `-O2` : Optimisations production
- [ ] `-I` : Chemin d'include
- [ ] `-L` : Chemin de bibliothèques
- [ ] `-fsanitize=address` : Détecter bugs mémoire

### ✅ Niveau Avancé
- [ ] `-fPIC -shared` : Créer bibliothèques partagées
- [ ] `-static` : Linkage statique
- [ ] `-march=native` : Optimisations CPU
- [ ] `-flto` : Link-Time Optimization
- [ ] `-D_FORTIFY_SOURCE=2` : Hardening
- [ ] `-fstack-protector-strong` : Protection stack

---

## 📝 Aide-mémoire rapide (copier-coller)

```bash
# Développement
gcc -std=c17 -Wall -Wextra -g program.c -o program

# Tests avec sanitizers
gcc -std=c17 -Wall -Wextra -g -fsanitize=address,undefined program.c -o program

# Production
gcc -std=c17 -Wall -Wextra -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong program.c -o program

# Performance maximale
gcc -std=c17 -Wall -O3 -march=native -flto program.c -o program

# Avec bibliothèque math
gcc -std=c17 -Wall -O2 program.c -o program -lm

# Avec threads
gcc -std=c17 -Wall -O2 program.c -o program -lpthread

# Créer bibliothèque partagée
gcc -fPIC -shared -std=c17 -Wall lib.c -o libmylib.so

# Compiler avec bibliothèque perso
gcc -std=c17 -Wall program.c -L. -lmylib -o program
```

---

## 🔗 Pour aller plus loin

### Documentation officielle
```bash
man gcc  
info gcc  
gcc --help  
```

### En ligne
- GCC Manual : [https://gcc.gnu.org/onlinedocs/gcc/](https://gcc.gnu.org/onlinedocs/gcc/)
- Options Index : [https://gcc.gnu.org/onlinedocs/gcc/Option-Index.html](https://gcc.gnu.org/onlinedocs/gcc/Option-Index.html)

---

## 📝 Conclusion

Les options GCC sont **nombreuses** mais vous n'avez pas besoin de toutes les connaître.

**Les 5 options essentielles :**
1. `-Wall -Wextra` : Détecter les bugs
2. `-g` : Permettre le debugging
3. `-O2` : Optimiser pour la production
4. `-std=c17` : Utiliser un standard moderne
5. `-o` : Nommer votre programme

**Tout le reste est du bonus** que vous apprendrez au fur et à mesure de vos besoins.

**Gardez ce document comme référence** et consultez-le quand vous avez besoin d'une option spécifique !

---


⏭️ [Commandes GDB](/annexes/antiseches/02-commandes-gdb.md)
