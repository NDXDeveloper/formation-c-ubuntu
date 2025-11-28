🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 28.4 ABI et compatibilité

## Introduction

### Qu'est-ce qu'une ABI ?

**ABI** signifie **Application Binary Interface** (Interface Binaire d'Application). C'est un concept fondamental mais souvent mal compris en programmation système.

**Définition simple :** L'ABI définit comment les programmes compilés communiquent au niveau binaire (machine).

**Analogie :**
- **API (Application Programming Interface)** = Les règles de la langue que vous parlez (syntaxe, grammaire)
- **ABI (Application Binary Interface)** = L'accent et la prononciation (comment les sons sont produits physiquement)

**Exemple concret :** Vous pouvez écrire du code C qui compile sur Linux et Windows, mais les binaires générés ne sont PAS compatibles. Pourquoi ? Parce qu'ils ont des ABIs différentes !

### Pourquoi l'ABI est-elle importante ?

L'ABI est cruciale dans plusieurs situations :

1. **Interopérabilité entre langages**
   - Appeler du C depuis Python, Go, Rust
   - L'ABI C est le "langage universel" des binaires

2. **Bibliothèques partagées (.so, .dll)**
   - Une bibliothèque compilée avec gcc 9 fonctionnera-t-elle avec votre programme compilé avec gcc 11 ?
   - Mise à jour de bibliothèques sans recompiler

3. **Compatibilité de versions**
   - Peut-on remplacer `libexemple.so.1.0` par `libexemple.so.1.1` ?
   - Forward/backward compatibility

4. **Distribution de logiciels**
   - Distribuer des binaires qui fonctionnent sur différentes machines
   - Plugins et extensions

**Cas d'usage réel :** Vous développez un plugin pour une application. Le plugin doit fonctionner avec l'application même si compilé avec des versions de compilateurs différentes. L'ABI garantit cette compatibilité.

---

## API vs ABI : Comprendre la différence

### API : Le code source

**API (Application Programming Interface)** concerne le code source.

```c
// API : Déclaration dans un header (.h)
int addition(int a, int b);
```

**Ce que définit l'API :**
- Noms des fonctions
- Types des paramètres
- Valeurs de retour
- Comportement attendu (documentation)

**Niveau :** Code source (avant compilation)

### ABI : Le binaire

**ABI (Application Binary Interface)** concerne le code compilé.

**Ce que définit l'ABI :**
- Comment les paramètres sont passés (registres ou pile ?)
- Dans quel ordre ?
- Qui nettoie la pile (appelant ou appelé) ?
- Comment les structures sont alignées en mémoire ?
- Comment les noms sont encodés (name mangling) ?
- Taille des types de données
- Format des exceptions
- Layout des v-tables (C++)

**Niveau :** Code binaire (après compilation)

### Exemple illustratif

**Fichier source `exemple.c` :**
```c
int addition(int a, int b) {
    return a + b;
}
```

**Compilation :**
```bash
gcc -c exemple.c -o exemple.o
```

**API :** Le code source définit une fonction `addition` qui prend deux `int`.

**ABI :** Le binaire `exemple.o` contient :
- Instructions machine spécifiques (x86-64, ARM, etc.)
- Convention d'appel (les paramètres dans les registres `edi` et `esi` sur x86-64)
- Symbole exporté `addition` (sans mangling en C)
- Alignement des données

**Démonstration :**
```bash
# Voir le code assembleur (représentation de l'ABI)
objdump -d exemple.o

# Sortie (x86-64) :
# 0000000000000000 <addition>:
#    0:   8d 04 37                lea    (%rdi,%rsi,1),%eax
#    3:   c3                      retq
```

Les paramètres `a` et `b` sont dans `%rdi` et `%rsi` (partie de l'ABI x86-64).

---

## Composants de l'ABI

### 1. Convention d'appel (Calling Convention)

La convention d'appel définit **comment les fonctions sont appelées** au niveau binaire.

#### Registres vs Pile

**Question :** Où sont placés les paramètres d'une fonction ?

**Réponse :** Ça dépend de la convention d'appel !

**Sur x86-64 Linux (System V ABI) :**
- Les 6 premiers paramètres entiers : `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`
- Les paramètres flottants : `xmm0` à `xmm7`
- Les paramètres supplémentaires : sur la pile

**Sur x86-64 Windows (Microsoft x64) :**
- Les 4 premiers paramètres : `rcx`, `rdx`, `r8`, `r9`
- Les paramètres supplémentaires : sur la pile
- Shadow space : 32 bytes réservés sur la pile

**Exemple :**

```c
int fonction(int a, int b, int c, int d, int e, int f, int g) {
    return a + b + c + d + e + f + g;
}
```

**Linux x86-64 :**
- `a` → `%rdi`
- `b` → `%rsi`
- `c` → `%rdx`
- `d` → `%rcx`
- `e` → `%r8`
- `f` → `%r9`
- `g` → pile

**Windows x86-64 :**
- `a` → `%rcx`
- `b` → `%rdx`
- `c` → `%r8`
- `d` → `%r9`
- `e` → pile
- `f` → pile
- `g` → pile

**Pourquoi c'est important ?** Si vous appelez une fonction Windows depuis Linux ou vice-versa, ça ne fonctionnera PAS directement.

#### Conventions d'appel courantes

| Convention | Plateforme | Paramètres | Nettoyage pile |
|------------|-----------|------------|----------------|
| **cdecl** | x86 (32 bits) | Pile (droite à gauche) | Appelant |
| **stdcall** | Windows x86 | Pile (droite à gauche) | Appelé |
| **fastcall** | x86 | Premiers dans registres | Appelé |
| **System V** | Linux x86-64 | Registres (rdi, rsi, ...) | N/A |
| **Microsoft x64** | Windows x64 | Registres (rcx, rdx, ...) | N/A |

**En C, on peut spécifier :**
```c
// Utiliser explicitement cdecl
int __attribute__((cdecl)) fonction(int a);

// Utiliser stdcall (Windows)
int __attribute__((stdcall)) fonction(int a);
```

### 2. Taille et alignement des types

La taille des types dépend de l'ABI !

**Sur la plupart des systèmes 64 bits :**

| Type C | Linux x86-64 | Windows x64 | ARM64 |
|--------|-------------|-------------|-------|
| `char` | 1 byte | 1 byte | 1 byte |
| `short` | 2 bytes | 2 bytes | 2 bytes |
| `int` | 4 bytes | 4 bytes | 4 bytes |
| `long` | **8 bytes** | **4 bytes** ❗ | 8 bytes |
| `long long` | 8 bytes | 8 bytes | 8 bytes |
| `void*` | 8 bytes | 8 bytes | 8 bytes |

**Problème de portabilité :**
```c
void fonction(long valeur) {
    // Sur Linux : 8 bytes
    // Sur Windows : 4 bytes
    // Incompatibilité ABI !
}
```

**Solution :** Utiliser des types de taille fixe :
```c
#include <stdint.h>

void fonction(int64_t valeur) {
    // Toujours 8 bytes sur toutes les plateformes
}
```

#### Alignement mémoire

Les structures doivent être alignées selon des règles spécifiques.

```c
struct Exemple1 {
    char a;      // 1 byte
    int b;       // 4 bytes
    char c;      // 1 byte
};
// Taille réelle : 12 bytes (avec padding)
// Layout : [a][___][bbbb][c][___]
//           1  +3   4    1  +3

struct Exemple2 {
    char a;      // 1 byte
    char c;      // 1 byte
    int b;       // 4 bytes
};
// Taille réelle : 8 bytes
// Layout : [a][c][__][bbbb]
//           1  1  +2   4
```

**Visualisation du padding :**
```bash
# Afficher la taille et l'alignement
gcc -c exemple.c
objdump -h exemple.o

# Ou en C :
printf("sizeof(Exemple1) = %zu\n", sizeof(struct Exemple1));
```

**Règles d'alignement typiques :**
- `char` : aligné sur 1 byte
- `short` : aligné sur 2 bytes
- `int` : aligné sur 4 bytes
- `long` : aligné sur 4 ou 8 bytes (dépend de l'ABI)
- `double` : aligné sur 8 bytes
- Structures : alignement du plus grand membre

**Forcer un alignement spécifique :**
```c
// Pas de padding (dangereux pour la performance)
struct __attribute__((packed)) SansPadding {
    char a;
    int b;
    char c;
};  // Taille : 6 bytes exactement

// Aligner sur 16 bytes
struct __attribute__((aligned(16))) Aligne16 {
    int a;
};
```

### 3. Name Mangling

Le name mangling transforme les noms de fonctions pour encoder des informations supplémentaires.

#### En C : Pas de mangling (simple)

```c
int addition(int a, int b) {
    return a + b;
}
```

**Symbole dans le binaire :** `addition` (nom préservé)

```bash
nm exemple.o
# 0000000000000000 T addition
```

#### En C++ : Mangling complexe

```cpp
int addition(int a, int b) {
    return a + b;
}

double addition(double a, double b) {
    return a + b;
}
```

**Symboles dans le binaire (exemple g++) :**
```
_Z8additionii       # addition(int, int)
_Z8additiondd       # addition(double, double)
```

Le nom encode :
- `_Z` : Préfixe de mangling
- `8` : Longueur du nom (addition = 8 caractères)
- `addition` : Nom de la fonction
- `ii` : Deux paramètres `int`
- `dd` : Deux paramètres `double`

**Problème d'interopérabilité :**
```cpp
// C++ : fichier lib.cpp
int fonction_cpp() {
    return 42;
}
```

```c
// C : fichier main.c
int fonction_cpp();  // Déclaration

int main() {
    return fonction_cpp();  // ❌ LINKER ERROR !
}
```

**Erreur :** `undefined reference to 'fonction_cpp'`

Le linker cherche `fonction_cpp` mais le symbole réel est `_Z12fonction_cppv`.

**Solution : `extern "C"`**
```cpp
// C++ : lib.cpp
extern "C" int fonction_cpp() {
    return 42;
}
// Symbole : fonction_cpp (pas de mangling)
```

Maintenant C peut appeler la fonction !

#### Vérifier le mangling

```bash
# Voir les symboles manglés
nm -C lib.o

# -C : Démangle les noms
# Sans -C : affiche les noms manglés
# Avec -C : affiche les noms lisibles

# Exemple :
nm lib.o
# _Z8additionii
nm -C lib.o
# addition(int, int)
```

**Outil `c++filt` :**
```bash
# Démangle un nom
echo "_Z8additionii" | c++filt
# Sortie : addition(int, int)
```

### 4. Valeurs de retour

Comment les valeurs de retour sont-elles transmises ?

#### Petites valeurs (≤ registre)

```c
int fonction() {
    return 42;
}
```

**ABI x86-64 :** Retour dans le registre `%rax`.

```assembly
movl    $42, %eax
ret
```

#### Grandes valeurs (structures)

```c
struct Resultat {
    int a, b, c, d, e;
};

struct Resultat fonction() {
    struct Resultat r = {1, 2, 3, 4, 5};
    return r;
}
```

**Deux approches selon l'ABI :**

**Approche 1 : Retour dans registres**
- Si la structure est petite (≤ 16 bytes sur x86-64)
- Utilise plusieurs registres (`rax`, `rdx`)

**Approche 2 : Retour via pointeur caché**
- Si la structure est grande
- L'appelant passe un pointeur caché en premier paramètre
- L'appelé écrit le résultat à cette adresse

```c
// Code source :
struct Big fonction();

// Devient en interne :
void fonction(struct Big *retval_ptr);
```

### 5. Layout des structures

Le layout (disposition en mémoire) des structures fait partie de l'ABI.

```c
struct Point {
    int x;
    int y;
};
```

**En mémoire :**
```
Offset 0 : x (4 bytes)
Offset 4 : y (4 bytes)
Total : 8 bytes
```

**Accès :**
```c
struct Point p;
// &p.x = (char*)&p + 0
// &p.y = (char*)&p + 4
```

Le compilateur **doit** respecter ces offsets pour la compatibilité binaire.

---

## Compatibilité ABI

### Qu'est-ce que la compatibilité ABI ?

**Définition :** Deux binaires sont ABI-compatibles s'ils peuvent interagir sans recompilation.

**Exemple :** Vous avez un programme qui utilise `libexemple.so.1.0`. Vous mettez à jour vers `libexemple.so.1.1`. Le programme fonctionne-t-il toujours ? Si oui, il y a compatibilité ABI.

### Types de compatibilité

#### 1. Compatibilité ascendante (Forward Compatibility)

**Définition :** Un ancien binaire fonctionne avec une nouvelle bibliothèque.

```
Programme (compilé avec lib v1.0) + lib v1.1 = ✅ Fonctionne
```

**Exemple :**
```c
// Version 1.0 de libmath.so
int addition(int a, int b);
int soustraction(int a, int b);

// Version 1.1 de libmath.so (ajout d'une fonction)
int addition(int a, int b);
int soustraction(int a, int b);
int multiplication(int a, int b);  // ← NOUVELLE
```

Un programme compilé avec v1.0 fonctionne avec v1.1 car les fonctions existantes n'ont pas changé.

#### 2. Compatibilité descendante (Backward Compatibility)

**Définition :** Un nouveau binaire fonctionne avec une ancienne bibliothèque.

```
Programme (compilé avec lib v1.1) + lib v1.0 = ❌ Peut échouer
```

**Exemple :**
Si le programme utilise `multiplication()` (de v1.1), il ne trouvera pas cette fonction dans v1.0.

### Changements qui cassent l'ABI

#### ❌ Changer la signature d'une fonction

```c
// Version 1.0
int fonction(int a, int b);

// Version 2.0 - ❌ CASSE L'ABI
int fonction(int a, int b, int c);  // Paramètre ajouté
```

**Pourquoi ?** L'appelant passe 2 paramètres, mais la fonction attend 3.

#### ❌ Changer l'ordre des champs d'une structure

```c
// Version 1.0
struct Personne {
    int age;
    double salaire;
};

// Version 2.0 - ❌ CASSE L'ABI
struct Personne {
    double salaire;  // Inversé !
    int age;
};
```

**Pourquoi ?** L'ancien code accède à `salaire` à l'offset 4, mais maintenant il est à l'offset 0.

#### ❌ Changer la taille d'une structure

```c
// Version 1.0
struct Config {
    int option1;
    int option2;
};  // Taille : 8 bytes

// Version 2.0 - ❌ CASSE L'ABI
struct Config {
    int option1;
    int option2;
    int option3;  // Ajout
};  // Taille : 12 bytes
```

**Pourquoi ?** L'allocation mémoire diffère (`sizeof(struct Config)`).

#### ❌ Changer le type d'un champ

```c
// Version 1.0
struct Stats {
    int compteur;  // 4 bytes
};

// Version 2.0 - ❌ CASSE L'ABI
struct Stats {
    long compteur;  // 8 bytes sur x86-64 Linux
};
```

#### ❌ Supprimer une fonction exportée

```c
// Version 1.0
void fonction_a();
void fonction_b();

// Version 2.0 - ❌ CASSE L'ABI
void fonction_a();
// fonction_b supprimée !
```

### Changements qui préservent l'ABI

#### ✅ Ajouter de nouvelles fonctions

```c
// Version 1.0
int addition(int a, int b);

// Version 1.1 - ✅ ABI préservée
int addition(int a, int b);
int multiplication(int a, int b);  // Nouvelle fonction
```

Les anciens binaires n'utilisent pas `multiplication`, donc pas de problème.

#### ✅ Ajouter des champs à la fin d'une structure

**Avec précaution :**
```c
// Version 1.0
struct Config {
    int option1;
    int option2;
};

// Version 1.1 - ⚠️ Risqué mais possible
struct Config {
    int option1;
    int option2;
    int option3;  // À LA FIN
};
```

**Conditions :**
- La structure n'est jamais allouée par l'appelant (uniquement par la bibliothèque)
- Pas de tableau de structures

#### ✅ Changer l'implémentation sans toucher l'interface

```c
// Version 1.0
int addition(int a, int b) {
    return a + b;
}

// Version 1.1 - ✅ ABI préservée
int addition(int a, int b) {
    // Implémentation différente mais même signature
    int result = a;
    result += b;
    return result;
}
```

Le code machine peut changer tant que l'interface binaire reste identique.

---

## Versioning de bibliothèques

### SONAME (Shared Object Name)

Sur Linux, les bibliothèques utilisent un système de versioning sophistiqué.

**Format :** `libnom.so.MAJOR.MINOR.PATCH`

**Exemple :** `libmath.so.2.1.5`
- **MAJOR (2)** : Version majeure (changements incompatibles ABI)
- **MINOR (1)** : Version mineure (ajout de fonctionnalités, ABI compatible)
- **PATCH (5)** : Corrections de bugs (pas de changement API/ABI)

#### Liens symboliques

```bash
ls -l /usr/lib/x86_64-linux-gnu/libm.so*

lrwxrwxrwx libm.so -> libm.so.6               # Lien pour la compilation
lrwxrwxrwx libm.so.6 -> libm.so.6.2.29        # SONAME
-rwxr-xr-x libm.so.6.2.29                     # Fichier réel
```

**Explication :**
- **`libm.so`** : Utilisé par le compilateur (compilation)
- **`libm.so.6`** : SONAME, utilisé au runtime (chargement dynamique)
- **`libm.so.6.2.29`** : Fichier réel (version complète)

#### Création avec GCC

```bash
# Créer une bibliothèque avec version
gcc -shared -fPIC -Wl,-soname,libmath.so.1 -o libmath.so.1.0.0 math.c

# -Wl,-soname,libmath.so.1 : Définit le SONAME

# Créer les liens symboliques
ln -s libmath.so.1.0.0 libmath.so.1
ln -s libmath.so.1 libmath.so
```

**Utilisation :**
```bash
# Compiler un programme
gcc main.c -L. -lmath -o main

# L'exécutable cherchera libmath.so.1 (SONAME) au runtime
ldd main
# libmath.so.1 => ./libmath.so.1 (0x00007f...)
```

### Politique de versioning

**Quand incrémenter MAJOR ?**
- Changements incompatibles ABI
- Suppression de fonctions
- Changement de signatures

**Quand incrémenter MINOR ?**
- Ajout de nouvelles fonctions (ABI compatible)
- Nouvelles fonctionnalités

**Quand incrémenter PATCH ?**
- Corrections de bugs
- Optimisations internes

**Exemple de cycle de vie :**
```
libmath.so.1.0.0   # Version initiale
libmath.so.1.0.1   # Correction de bug
libmath.so.1.1.0   # Nouvelle fonction addition_rapide()
libmath.so.1.1.1   # Correction de bug
libmath.so.2.0.0   # Changement : int→long (incompatible ABI)
```

---

## ABI selon les plateformes

### Linux (System V ABI)

**Caractéristiques :**
- ABI bien définie et stable
- Convention d'appel : System V AMD64 ABI (x86-64)
- Name mangling : Simple en C, complexe en C++
- Versioning : SONAME
- Symboles : Visibilité par défaut

**Documentation :** [System V ABI](https://www.uclibc.org/docs/psABI-x86_64.pdf)

### Windows

**Caractéristiques :**
- Convention d'appel : Microsoft x64 calling convention
- `long` est 32 bits (même sur 64 bits !)
- DLL versioning : Moins sophistiqué que Linux
- Name mangling : Différent de Linux
- DEF files pour contrôler les exports

**Spécificités :**
```c
// Windows : attributs d'export/import
#ifdef _WIN32
    #ifdef BUILD_DLL
        #define API __declspec(dllexport)
    #else
        #define API __declspec(dllimport)
    #endif
#else
    #define API
#endif

API int fonction();
```

### macOS

**Caractéristiques :**
- ABI similaire à Linux (mais pas identique)
- Convention d'appel : System V AMD64
- Format : Mach-O (pas ELF)
- Bibliothèques : `.dylib` (pas `.so`)
- Two-level namespace

**Versioning :**
```bash
# Créer une dylib avec version
gcc -dynamiclib -o libmath.1.0.dylib math.c \
    -current_version 1.0 -compatibility_version 1.0
```

### Architecture ARM

**Spécificités :**
- Différentes versions d'ABI : AAPCS, EABI
- Passage de paramètres : Registres R0-R3
- Alignement : Souvent 8 bytes pour `double`
- Hard-float vs soft-float

```c
// Vérifier l'ABI ARM
#if defined(__ARM_EABI__)
    // EABI (moderne)
#elif defined(__ARM_APCS__)
    // Ancien ABI
#endif
```

---

## Problèmes courants et solutions

### Problème 1 : Incompatibilité entre compilateurs

**Symptôme :**
```bash
g++ prog.cpp -o prog -L. -lmylib
./prog
# Segmentation fault ou comportement étrange
```

**Cause :** Bibliothèque compilée avec GCC 7, programme avec GCC 11.

**Solution :**
```bash
# Recompiler la bibliothèque avec le même compilateur
gcc --version  # Vérifier la version
gcc -shared -o libmylib.so mylib.c
```

**Meilleure solution :** Distribuer les bibliothèques avec une ABI stable (C, pas C++).

### Problème 2 : Mixing C et C++

**Symptôme :**
```
undefined reference to `fonction'
```

**Cause :** Oubli de `extern "C"`.

**Solution :**
```cpp
// Dans le header .h
#ifdef __cplusplus
extern "C" {
#endif

int fonction(int a);

#ifdef __cplusplus
}
#endif
```

**Pourquoi ?** Cela fonctionne depuis C et C++ :
- Depuis C : Pas de `__cplusplus`, donc pas de `extern "C"` (normal)
- Depuis C++ : `extern "C"` désactive le mangling

### Problème 3 : Structure alignment mismatch

**Symptôme :** Données corrompues lors du passage de structures.

**Cause :** Compilateurs avec options d'alignement différentes.

**Solution :**
```c
// Forcer un alignement spécifique
#pragma pack(push, 1)
struct Config {
    char a;
    int b;
};
#pragma pack(pop)

// Ou avec attributs
struct __attribute__((packed)) Config {
    char a;
    int b;
};
```

### Problème 4 : Version mismatch

**Symptôme :**
```
./prog: error while loading shared libraries: libmath.so.2:
cannot open shared object file: No such file or directory
```

**Cause :** Le programme demande `libmath.so.2` mais seul `libmath.so.1` est installé.

**Solutions :**
```bash
# Solution 1 : Installer la bonne version
sudo apt-get install libmath2

# Solution 2 : Recompiler avec la version disponible
gcc main.c -lmath  # Utilise libmath.so.1

# Solution 3 : Lien symbolique (TEMPORAIRE, risqué)
sudo ln -s /usr/lib/libmath.so.1 /usr/lib/libmath.so.2
```

### Problème 5 : Symbol visibility

**Symptôme :** Conflit de symboles entre bibliothèques.

```
multiple definition of `fonction_interne'
```

**Cause :** Tous les symboles sont publics par défaut.

**Solution :**
```c
// Contrôler la visibilité
__attribute__((visibility("hidden")))
void fonction_interne() {
    // Pas visible en dehors de la bibliothèque
}

__attribute__((visibility("default")))
void fonction_publique() {
    // Visible et exportée
}
```

**Compilation avec option :**
```bash
gcc -fvisibility=hidden -shared -o lib.so lib.c
# Par défaut : hidden
# Seuls les symboles marqués "default" sont exportés
```

---

## Outils pour analyser l'ABI

### 1. `nm` - Liste des symboles

```bash
# Voir les symboles exportés
nm -D libexemple.so

# Types de symboles :
# T : Fonction dans .text (code)
# D : Variable dans .data (initialisée)
# B : Variable dans .bss (non initialisée)
# U : Symbole non défini (externe)

# Exemple de sortie :
# 0000000000001234 T fonction_publique
# 0000000000001245 t fonction_locale
# 0000000000001256 D variable_globale
#                  U printf  # Symbole externe
```

### 2. `readelf` - Analyse ELF

```bash
# Voir les sections
readelf -S libexemple.so

# Voir les symboles dynamiques
readelf -s libexemple.so

# Voir les dépendances
readelf -d libexemple.so | grep NEEDED

# Voir le SONAME
readelf -d libexemple.so | grep SONAME
```

### 3. `ldd` - Dépendances dynamiques

```bash
# Lister les dépendances
ldd programme

# Sortie :
# linux-vdso.so.1 (0x00007fff...)
# libmath.so.1 => /usr/lib/libmath.so.1 (0x00007f...)
# libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
```

**⚠️ Attention :** Ne jamais exécuter `ldd` sur des binaires non fiables (risque de sécurité).

### 4. `objdump` - Désassemblage

```bash
# Désassembler
objdump -d programme

# Voir les headers
objdump -h programme

# Voir les symboles
objdump -t programme
```

### 5. `c++filt` - Démangle

```bash
# Démangle un symbole C++
echo "_Z8additionii" | c++filt
# Sortie : addition(int, int)

# Utilisable avec nm
nm libexemple.so | c++filt
```

### 6. `abidiff` - Comparer des ABIs

```bash
# Comparer deux versions d'une bibliothèque
abidiff libmath.so.1.0 libmath.so.1.1

# Sortie : Changements détaillés dans l'ABI
```

**Installation :**
```bash
sudo apt-get install abigail-tools
```

### 7. `abi-compliance-checker`

```bash
# Vérifier la compatibilité ABI
abi-compliance-checker -lib libmath \
    -old libmath.so.1.0 \
    -new libmath.so.1.1

# Génère un rapport HTML
```

---

## Bonnes pratiques pour la stabilité ABI

### 1. Utiliser une interface C

```c
// ✅ Interface stable en C
// mon_api.h
#ifdef __cplusplus
extern "C" {
#endif

typedef struct MonObjet MonObjet;  // Pointeur opaque

MonObjet* creer_objet();
void detruire_objet(MonObjet* obj);
int obtenir_valeur(MonObjet* obj);

#ifdef __cplusplus
}
#endif
```

**Implémentation C++ interne :**
```cpp
// mon_api.cpp (interne, pas dans l'API)
extern "C" {

class MonObjetImpl {
    int valeur;
public:
    int getValeur() { return valeur; }
};

MonObjet* creer_objet() {
    return reinterpret_cast<MonObjet*>(new MonObjetImpl());
}

void detruire_objet(MonObjet* obj) {
    delete reinterpret_cast<MonObjetImpl*>(obj);
}

int obtenir_valeur(MonObjet* obj) {
    return reinterpret_cast<MonObjetImpl*>(obj)->getValeur();
}

}  // extern "C"
```

### 2. Structures opaques (PIMPL)

```c
// Public API (stable)
typedef struct MaBiblio MaBiblio;

MaBiblio* biblio_create();
void biblio_destroy(MaBiblio* b);
int biblio_operation(MaBiblio* b, int param);
```

```c
// Implémentation privée (peut changer)
struct MaBiblio {
    int field1;
    int field2;
    char* internal_buffer;
    // On peut ajouter des champs sans casser l'ABI !
};
```

**Avantage :** L'utilisateur ne voit qu'un pointeur opaque, la structure interne peut changer.

### 3. Versioning explicite

```c
// Version 1.0
#define API_VERSION_MAJOR 1
#define API_VERSION_MINOR 0

int api_get_version(int* major, int* minor);
```

**Utilisation :**
```c
int major, minor;
api_get_version(&major, &minor);

if (major != 1) {
    fprintf(stderr, "Version incompatible!\n");
    exit(1);
}
```

### 4. Structures extensibles

```c
// Technique de structure extensible
typedef struct {
    size_t size;        // Toujours en premier !
    int option1;
    int option2;
    // Champs futurs...
} Config;

void initialiser_config(Config* cfg) {
    cfg->size = sizeof(Config);
    cfg->option1 = 0;
    cfg->option2 = 0;
}

int utiliser_config(const Config* cfg) {
    // Vérifier la taille
    if (cfg->size < offsetof(Config, option2) + sizeof(int)) {
        // Ancienne version, option2 n'existe pas
        return -1;
    }

    // Utiliser option2 en toute sécurité
    printf("Option2: %d\n", cfg->option2);
    return 0;
}
```

### 5. Éviter les inline et templates dans l'API

```cpp
// ❌ Problématique
// fichier.h (public)
class MaClasse {
public:
    inline int getValeur() { return valeur_; }
private:
    int valeur_;
};
```

**Problème :** Si `valeur_` change de position, l'ABI est cassée.

```cpp
// ✅ Meilleur
// fichier.h (public)
class MaClasse {
public:
    int getValeur();  // Pas inline
private:
    class Impl;
    Impl* pimpl_;  // Pointeur vers implémentation
};
```

### 6. Documentation des garanties ABI

```c
/**
 * Bibliothèque MonLib - Garanties ABI
 *
 * Version : 2.x.x
 *
 * Garanties :
 * - Les fonctions existantes conservent leur signature
 * - Les structures opaques peuvent évoluer
 * - Les enum peuvent avoir de nouvelles valeurs
 * - Compatible avec versions 2.0+
 *
 * Incompatibilités :
 * - Non compatible avec version 1.x (changement majeur)
 * - Fonction ancienne_fonction() supprimée en 2.0
 */
```

---

## Cas pratiques

### Cas 1 : Mise à jour d'une bibliothèque

**Situation :** Vous maintenez `libcalcul.so` utilisée par 100 programmes.

**Version actuelle : 1.5.0**
```c
// libcalcul.h
int addition(int a, int b);
int soustraction(int a, int b);
```

**Changement souhaité : Ajouter multiplication**

```c
// Version 1.6.0 - ✅ ABI compatible
int addition(int a, int b);
int soustraction(int a, int b);
int multiplication(int a, int b);  // Nouvelle
```

**Versioning :**
```bash
gcc -shared -Wl,-soname,libcalcul.so.1 -o libcalcul.so.1.6.0 calcul.c
ln -sf libcalcul.so.1.6.0 libcalcul.so.1
```

**Résultat :** Les anciens programmes fonctionnent toujours !

### Cas 2 : Changement incompatible

**Changement souhaité : `int` → `long long` pour éviter overflow**

```c
// Version 1.x
int addition(int a, int b);

// Version 2.0 - ❌ Incompatible ABI
long long addition(long long a, long long b);
```

**Stratégie :**
```c
// Garder l'ancienne fonction (deprecated)
int addition(int a, int b) __attribute__((deprecated));

// Nouvelle fonction avec nouveau nom
long long addition_ll(long long a, long long b);
```

**Ou version 2.0 avec SONAME différent :**
```bash
gcc -shared -Wl,-soname,libcalcul.so.2 -o libcalcul.so.2.0.0 calcul.c
```

Les programmes doivent être recompilés pour utiliser v2.

### Cas 3 : Plugin system

**Objectif :** Application qui charge des plugins dynamiquement.

**Interface stable :**
```c
// plugin_api.h
#define PLUGIN_API_VERSION 1

typedef struct {
    int version;
    const char* name;
    int (*init)();
    void (*cleanup)();
    int (*process)(void* data);
} PluginInterface;

// Fonction que chaque plugin doit exporter
PluginInterface* get_plugin_interface();
```

**Plugin :**
```c
// mon_plugin.c
#include "plugin_api.h"

static int mon_init() { /* ... */ }
static void mon_cleanup() { /* ... */ }
static int mon_process(void* data) { /* ... */ }

static PluginInterface interface = {
    .version = PLUGIN_API_VERSION,
    .name = "MonPlugin",
    .init = mon_init,
    .cleanup = mon_cleanup,
    .process = mon_process
};

PluginInterface* get_plugin_interface() {
    return &interface;
}
```

**Application hôte :**
```c
#include <dlfcn.h>

void charger_plugin(const char* path) {
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Erreur dlopen: %s\n", dlerror());
        return;
    }

    typedef PluginInterface* (*get_interface_func)();
    get_interface_func get_interface = dlsym(handle, "get_plugin_interface");

    if (!get_interface) {
        fprintf(stderr, "Symbole non trouvé\n");
        dlclose(handle);
        return;
    }

    PluginInterface* plugin = get_interface();

    if (plugin->version != PLUGIN_API_VERSION) {
        fprintf(stderr, "Version incompatible: %d vs %d\n",
                plugin->version, PLUGIN_API_VERSION);
        dlclose(handle);
        return;
    }

    printf("Chargé: %s\n", plugin->name);
    plugin->init();
}
```

---

## Récapitulatif

### Points clés à retenir

1. **ABI vs API**
   - **API** : Interface au niveau du code source
   - **ABI** : Interface au niveau binaire
   - L'ABI est ce qui permet la compatibilité binaire

2. **Composants de l'ABI**
   - Convention d'appel (registres vs pile)
   - Taille et alignement des types
   - Name mangling
   - Layout des structures
   - Valeurs de retour

3. **Compatibilité**
   - **Forward compatible** : Ancien binaire + nouvelle bibliothèque
   - **Backward compatible** : Nouveau binaire + ancienne bibliothèque
   - Changements cassant l'ABI : signature, structures, suppression

4. **Versioning**
   - SONAME sur Linux : `libnom.so.MAJOR.MINOR.PATCH`
   - MAJOR : Changements incompatibles
   - MINOR : Ajouts compatibles
   - PATCH : Corrections

5. **Bonnes pratiques**
   - Interface C pour stabilité
   - Structures opaques (PIMPL)
   - Versioning explicite
   - `extern "C"` pour C++
   - Documentation des garanties

6. **Outils**
   - `nm` : Symboles
   - `readelf` : Analyse ELF
   - `ldd` : Dépendances
   - `abidiff` : Comparaison d'ABI

### Checklist ABI-safe

- [ ] Utiliser `extern "C"` pour les exports C++
- [ ] Marquer les structures avec `#[repr(C)]` (Rust) ou équivalent
- [ ] Utiliser des types de taille fixe (`int32_t`, pas `int`)
- [ ] Ne jamais changer les signatures de fonctions existantes
- [ ] Ne pas réorganiser les champs de structures publiques
- [ ] Versioning SONAME correct
- [ ] Documenter les garanties de compatibilité
- [ ] Tester avec plusieurs versions de compilateurs
- [ ] Utiliser des structures opaques quand possible
- [ ] Vérifier avec `abidiff` avant release

### Le mot de la fin

L'ABI est le contrat invisible qui permet aux logiciels de communiquer au niveau binaire. Bien la comprendre est essentiel pour :
- Créer des bibliothèques stables
- Éviter les cauchemars de compatibilité
- Distribuer des logiciels portables
- Maintenir du code sur le long terme

Une bonne gestion de l'ABI transforme une bibliothèque fragile en un composant robuste et réutilisable pour des années.

**Pensez toujours : "Si je change ceci, vais-je casser l'ABI ?" 🤔**

---


⏭️ [extern "C" pour C++](/28-interoperabilite/05-extern-c.md)
