🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 28.6 Name Mangling

## Introduction

### Qu'est-ce que le name mangling ?

**Name mangling** (ou "name decoration", "name encoding") est le processus par lequel un compilateur transforme les noms de fonctions, variables et autres symboles en noms internes uniques utilisés dans le code binaire.

**Analogie simple :** Imaginez une école où plusieurs élèves s'appellent "Marie". Pour les différencier, on utilise leur nom complet : "Marie Dupont de la classe 5A" et "Marie Martin de la classe 3B". Le name mangling fait la même chose : il transforme des noms simples en identifiants uniques.

**Exemple visuel :**
```
Code source : int addition(int a, int b)
         ↓
Name mangling (C++)
         ↓
Symbole binaire : _Z8additionii
```

### Pourquoi le name mangling existe ?

**En C :** Pas de mangling (ou très minimal)
```c
int addition(int a, int b) { return a + b; }
```
**Symbole généré :** `addition` (nom préservé tel quel)

**En C++ :** Mangling complexe nécessaire
```cpp
int addition(int a, int b) { return a + b; }  
double addition(double a, double b) { return a + b; }  
```
**Symboles générés :**
- `_Z8additionii` (version avec int)
- `_Z8additiondd` (version avec double)

**Raisons du name mangling en C++ :**

1. **Surcharge de fonctions (Function Overloading)**
   - Plusieurs fonctions avec le même nom mais paramètres différents
   - Le compilateur doit les différencier dans le binaire

2. **Espaces de noms (Namespaces)**
   - `std::sort` vs `mysort` vs `utils::sort`
   - Le nom complet doit être encodé

3. **Classes et méthodes**
   - `MaClasse::methode()` vs `AutreClasse::methode()`
   - Même nom de méthode, classes différentes

4. **Templates**
   - `vector<int>` vs `vector<double>`
   - Chaque instanciation est une fonction différente

---

## Name mangling en C

### C : Simplicité (presque pas de mangling)

En C, le name mangling est minimal ou inexistant selon la plateforme.

**Code C :**
```c
int addition(int a, int b) {
    return a + b;
}

void afficher(const char* message) {
    printf("%s\n", message);
}
```

**Symboles générés :**
```bash
gcc -c exemple.c -o exemple.o  
nm exemple.o  

# Sortie (Linux) :
# 0000000000000000 T addition
# 0000000000000015 T afficher
```

Les noms sont **préservés tels quels** !

### Variations selon les plateformes

**Linux/Unix :**
- Pas de préfixe ou suffixe
- `addition` reste `addition`

**Windows (32 bits) :**
- Convention cdecl : préfixe `_`
- `addition` devient `_addition`
- Convention stdcall : préfixe `_` et suffixe `@N` (N = taille des arguments)
- `addition` devient `_addition@8` (2 int × 4 bytes)

**macOS :**
- Préfixe underscore `_`
- `addition` devient `_addition`

### Pourquoi C ne fait pas de mangling complexe ?

**Raisons historiques et techniques :**

1. **Pas de surcharge de fonctions**
   ```c
   // ❌ INTERDIT en C
   int addition(int a, int b);
   double addition(double a, double b);  // Erreur !
   ```

2. **Pas de classes ni méthodes**
   - Fonctions au niveau global uniquement
   - Pas d'ambiguïté de contexte

3. **Compatibilité binaire**
   - Linker simple
   - Interopérabilité facile entre bibliothèques

4. **Simplicité**
   - Débogage plus facile
   - Symboles lisibles

---

## Name mangling en C++

### C++ : Nécessité d'un encodage complexe

C++ introduit de nombreuses fonctionnalités qui nécessitent un name mangling sophistiqué.

### 1. Surcharge de fonctions

**Code C++ :**
```cpp
int addition(int a, int b) {
    return a + b;
}

double addition(double a, double b) {
    return a + b;
}

int addition(int a, int b, int c) {
    return a + b + c;
}
```

**Symboles générés (g++) :**
```bash
g++ -c exemple.cpp -o exemple.o  
nm exemple.o  

# Sortie :
# _Z8additionii      # addition(int, int)
# _Z8additiondd      # addition(double, double)
# _Z8additioniii     # addition(int, int, int)
```

**Décodage :**
```bash
c++filt _Z8additionii
# Sortie : addition(int, int)

c++filt _Z8additiondd
# Sortie : addition(double, double)
```

### 2. Espaces de noms (Namespaces)

**Code C++ :**
```cpp
namespace math {
    int addition(int a, int b) {
        return a + b;
    }
}

namespace calcul {
    int addition(int a, int b) {
        return a + b + 1;  // Différente implémentation
    }
}
```

**Symboles générés :**
```bash
# _ZN4math8additionEii    # math::addition(int, int)
# _ZN6calcul8additionEii  # calcul::addition(int, int)
```

**Remarquez :**
- `N` indique un nom qualifié (avec namespace)
- `4math` : namespace "math" (4 caractères)
- `8addition` : fonction "addition" (8 caractères)
- `E` : Fin du nom qualifié
- `ii` : Deux paramètres int

### 3. Méthodes de classes

**Code C++ :**
```cpp
class Rectangle {  
public:  
    int calculer_aire(int largeur, int hauteur) {
        return largeur * hauteur;
    }
};

class Cercle {  
public:  
    double calculer_aire(double rayon) {
        return 3.14159 * rayon * rayon;
    }
};
```

**Symboles générés :**
```bash
# _ZN9Rectangle13calculer_aireEii     # Rectangle::calculer_aire(int, int)
# _ZN6Cercle13calculer_aireEd        # Cercle::calculer_aire(double)
```

### 4. Templates

**Code C++ :**
```cpp
template <typename T>  
T maximum(T a, T b) {  
    return (a > b) ? a : b;
}

// Utilisations
int x = maximum<int>(5, 10);  
double y = maximum<double>(3.14, 2.71);  
```

**Symboles générés :**
```bash
# _Z7maximumIiET_S0_S0_              # maximum<int>(int, int)
# _Z7maximumIdET_S0_S0_              # maximum<double>(double, double)
```

**Décodage :**
```bash
c++filt _Z7maximumIiET_S0_S0_
# Sortie : int maximum<int>(int, int)

c++filt _Z7maximumIdET_S0_S0_
# Sortie : double maximum<double>(double, double)
```

---

## Anatomie d'un nom manglé

### Structure d'un nom manglé (Itanium C++ ABI)

L'Itanium C++ ABI est utilisé par GCC, Clang et la plupart des compilateurs modernes (sauf MSVC).

**Format général :**
```
_Z[N]<nom_qualifié>[E]<types_parametres>
```

**Composants :**

1. **`_Z`** : Préfixe obligatoire
2. **`N`** : Indique un nom qualifié (namespace, classe)
3. **`<longueur><nom>`** : Longueur du nom + nom
4. **`E`** : Fin du nom qualifié
5. **`<types>`** : Encodage des types des paramètres

### Exemples détaillés

#### Exemple 1 : Fonction simple
```cpp
void fonction(int x);
```
**Mangled :** `_Z8fonctioni`
- `_Z` : Préfixe
- `8` : Longueur de "fonction"
- `fonction` : Nom
- `i` : Type `int`

#### Exemple 2 : Fonction avec plusieurs paramètres
```cpp
void traiter(int a, double b, char c);
```
**Mangled :** `_Z7traiteridc`
- `_Z` : Préfixe
- `7` : Longueur de "traiter"
- `traiter` : Nom
- `i` : Type `int`
- `d` : Type `double`
- `c` : Type `char`

#### Exemple 3 : Méthode de classe
```cpp
class MaClasse {
    void methode(int x);
};
```
**Mangled :** `_ZN8MaClasse7methodeEi`
- `_Z` : Préfixe
- `N` : Nom qualifié
- `8` : Longueur de "MaClasse"
- `MaClasse` : Classe
- `7` : Longueur de "methode"
- `methode` : Méthode
- `E` : Fin du nom qualifié
- `i` : Type `int`

#### Exemple 4 : Namespace + Classe
```cpp
namespace app {
    class Service {
        void traiter(int x, double y);
    };
}
```
**Mangled :** `_ZN3app7Service7traiterEid`
- `_Z` : Préfixe
- `N` : Nom qualifié
- `3` : Longueur de "app"
- `app` : Namespace
- `7` : Longueur de "Service"
- `Service` : Classe
- `7` : Longueur de "traiter"
- `traiter` : Méthode
- `E` : Fin du nom qualifié
- `i` : Type `int`
- `d` : Type `double`

### Encodage des types

| Type C++ | Code | Exemple manglé |
|----------|------|----------------|
| `void` | `v` | `_Z8fonctionv` |
| `char` | `c` | `_Z8fonctionc` |
| `signed char` | `a` | `_Z8fonctiona` |
| `unsigned char` | `h` | `_Z8fonctionh` |
| `short` | `s` | `_Z8fonctions` |
| `unsigned short` | `t` | `_Z8fonctiont` |
| `int` | `i` | `_Z8fonctioni` |
| `unsigned int` | `j` | `_Z8fonctionj` |
| `long` | `l` | `_Z8fonctionl` |
| `unsigned long` | `m` | `_Z8fonctionm` |
| `long long` | `x` | `_Z8fonctionx` |
| `unsigned long long` | `y` | `_Z8fonctiony` |
| `float` | `f` | `_Z8fonctionf` |
| `double` | `d` | `_Z8fonctiond` |
| `long double` | `e` | `_Z8fonctione` |
| `bool` | `b` | `_Z8fonctionb` |
| `wchar_t` | `w` | `_Z8fonctionw` |
| `...` (varargs) | `z` | `_Z8fonctioniz` |

### Encodage des types composés

**Pointeurs :**
```cpp
void fonction(int* p);
```
**Mangled :** `_Z8fonctionPi`
- `P` : Pointeur
- `i` : vers `int`

**Références :**
```cpp
void fonction(int& r);
```
**Mangled :** `_Z8fonctionRi`
- `R` : Référence
- `i` : vers `int`

**Pointeurs constants :**
```cpp
void fonction(const int* p);
```
**Mangled :** `_Z8fonctionPKi`
- `P` : Pointeur
- `K` : const
- `i` : vers `int`

**Référence vers tableau :**
```cpp
void fonction(int (&tableau)[10]);
```
**Mangled :** `_Z8fonctionRA10_i`
- `R` : Référence
- `A10_` : Tableau de 10
- `i` : éléments `int`

> **Note :** Un paramètre `int tableau[10]` (sans référence) décaye en pointeur et produit le symbole `_Z8fonctionPi`.

---

## Différences entre compilateurs

### GCC/Clang (Itanium C++ ABI)

**Standard moderne utilisé par :**
- GCC sur Linux
- Clang sur Linux/macOS
- La plupart des compilateurs Unix

**Exemple :**
```cpp
int addition(int a, int b);
```
**Mangled :** `_Z8additionii`

**Caractéristiques :**
- Préfixe `_Z`
- Encodage systématique
- Documentation complète

### MSVC (Microsoft Visual C++)

Microsoft utilise son propre schéma de mangling.

**Exemple :**
```cpp
int addition(int a, int b);
```
**Mangled :** `?addition@@YAHHH@Z`

**Décomposition :**
- `?` : Préfixe
- `addition` : Nom de la fonction
- `@@` : Séparateur
- `YA` : Convention d'appel (cdecl)
- `H` : Type de retour (`int`)
- `HH` : Deux paramètres `int`
- `@Z` : Fin

**Encodage des types MSVC :**

| Type | Code |
|------|------|
| `void` | `X` |
| `char` | `D` |
| `signed char` | `C` |
| `unsigned char` | `E` |
| `short` | `F` |
| `unsigned short` | `G` |
| `int` | `H` |
| `unsigned int` | `I` |
| `long` | `J` |
| `unsigned long` | `K` |
| `float` | `M` |
| `double` | `N` |
| `bool` | `_N` |

**Exemple avec namespace et classe (MSVC) :**
```cpp
namespace math {
    class Calculator {
        int add(int a, int b);
    };
}
```
**Mangled :** `?add@Calculator@math@@QAEHHH@Z`

### Comparaison GCC vs MSVC

| Aspect | GCC/Clang | MSVC |
|--------|-----------|------|
| Préfixe | `_Z` | `?` |
| Lisibilité | Moyenne | Faible |
| Standard | Itanium ABI | Propriétaire |
| Documentation | Excellente | Limitée |
| Compatibilité | Multiplateforme | Windows |

---

## Outils pour démangle

### 1. `c++filt` (GCC/Clang)

**Outil en ligne de commande pour démangle les noms.**

```bash
# Démangle un seul nom
c++filt _Z8additionii
# Sortie : addition(int, int)

# Démangle depuis un fichier
nm programme.o | c++filt

# Démangle stdin
echo "_Z8additionii" | c++filt
```

**Options utiles :**
```bash
# Format simplifié
c++filt -p _Z8additionii
# Sortie : addition(int, int)

# Afficher les types
c++filt -t _Z8additionii
# Sortie : addition(int, int)
```

### 2. `nm` avec démangle automatique

```bash
# Avec démangle
nm -C programme.o

# Comparaison :
# Sans -C :
# _Z8additionii

# Avec -C :
# addition(int, int)
```

### 3. `objdump` avec démangle

```bash
# Désassembler avec noms démanglés
objdump -C -d programme.o

# Sortie :
# 0000000000000000 <addition(int, int)>:
#    0:   8d 04 37                lea    (%rdi,%rsi,1),%eax
#    3:   c3                      retq
```

### 4. Outils en ligne

**cpp.sh/demangle** : Outil web pour démangle en ligne

**demangler.com** : Autre outil en ligne

### 5. Programmation : `abi::__cxa_demangle`

**C++ fournit une API pour démangle :**

```cpp
#include <iostream>
#include <cxxabi.h>
#include <cstdlib>

std::string demangle(const char* mangled) {
    int status;
    char* demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);

    if (status == 0) {
        std::string result(demangled);
        std::free(demangled);
        return result;
    }

    return mangled;  // Échec, retourner le nom original
}

int main() {
    const char* mangled = "_Z8additionii";
    std::cout << "Mangled: " << mangled << std::endl;
    std::cout << "Demangled: " << demangle(mangled) << std::endl;

    return 0;
}
```

**Compilation :**
```bash
g++ demangle.cpp -o demangle
./demangle

# Sortie :
# Mangled: _Z8additionii
# Demangled: addition(int, int)
```

---

## Cas spéciaux et complexes

### 1. Opérateurs surchargés

**Code C++ :**
```cpp
class Point {  
public:  
    Point operator+(const Point& other);
    bool operator==(const Point& other);
};
```

**Symboles manglés :**
```bash
# _ZN5PointplERKS_    # Point::operator+(Point const&)
# _ZN5PointeqERKS_    # Point::operator==(Point const&)
```

**Encodage des opérateurs :**

| Opérateur | Code |
|-----------|------|
| `+` | `pl` (plus) |
| `-` | `mi` (minus) |
| `*` | `ml` (multiply) |
| `/` | `dv` (divide) |
| `%` | `rm` (remainder) |
| `==` | `eq` (equal) |
| `!=` | `ne` (not equal) |
| `<` | `lt` (less than) |
| `>` | `gt` (greater than) |
| `[]` | `ix` (index) |
| `()` | `cl` (call) |
| `->` | `pt` (pointer) |
| `=` | `aS` (assign) |

### 2. Constructeurs et destructeurs

**Code C++ :**
```cpp
class MaClasse {  
public:  
    MaClasse();           // Constructeur par défaut
    MaClasse(int x);      // Constructeur avec paramètre
    ~MaClasse();          // Destructeur
};
```

**Symboles manglés :**
```bash
# _ZN8MaClasseC1Ev      # MaClasse::MaClasse() (constructeur complet)
# _ZN8MaClasseC2Ev      # MaClasse::MaClasse() (constructeur de base)
# _ZN8MaClasseC1Ei      # MaClasse::MaClasse(int)
# _ZN8MaClasseD1Ev      # MaClasse::~MaClasse() (destructeur complet)
# _ZN8MaClasseD2Ev      # MaClasse::~MaClasse() (destructeur de base)
```

**Explication :**
- `C1`/`C2` : Constructeurs (complet/base)
- `D1`/`D2` : Destructeurs (complet/base)
- Nécessaire pour l'héritage et les classes virtuelles

### 3. Templates avec paramètres multiples

**Code C++ :**
```cpp
template <typename T, typename U>  
T convertir(U valeur);  

// Utilisation
int x = convertir<int, double>(3.14);
```

**Symbole manglé :**
```bash
# _Z9convertirIidET_T0_
```

**Décomposition :**
- `_Z` : Préfixe
- `9` : Longueur de "convertir"
- `convertir` : Nom
- `I` : Début des paramètres template
- `i` : Premier paramètre template (`int`)
- `d` : Deuxième paramètre template (`double`)
- `E` : Fin des paramètres template
- `T_` : Type de retour (premier paramètre template)
- `T0_` : Type du paramètre (deuxième paramètre template)

### 4. Templates spécialisés

**Code C++ :**
```cpp
template <typename T>  
class Container {  
    void add(T item);
};

// Spécialisation pour int
template <>  
class Container<int> {  
    void add(int item);
};
```

**Symboles manglés :**
```bash
# Template générique :
# _ZN9ContainerIiE3addEi    # Container<int>::add(int)

# Spécialisation :
# _ZN9ContainerIiE3addEi    # Même symbole !
```

La spécialisation produit le même symbole car c'est la même signature finale.

### 5. Fonctions virtuelles

**Code C++ :**
```cpp
class Base {  
public:  
    virtual void methode();
};

class Derivee : public Base {  
public:  
    void methode() override;
};
```

**Symboles manglés :**
```bash
# _ZN4Base7methodeEv        # Base::methode()
# _ZN7Derivee7methodeEv     # Derivee::methode()

# Tables virtuelles :
# _ZTV4Base                 # vtable pour Base
# _ZTV7Derivee              # vtable pour Derivee
```

---

## Impact du name mangling

### 1. Taille des binaires

**Le name mangling augmente la taille des symboles.**

**Exemple :**
```cpp
// Nom court en C
void f(int a);
// Symbole : f (1 caractère)

// Nom court en C++
void f(int a);
// Symbole : _Z1fi (5 caractères)

// Nom long avec namespace et templates
namespace very_long_namespace_name {
    template <typename T>
    void very_long_function_name(T value);
}
// Symbole : _ZN24very_long_namespace_name23very_long_function_nameIiEEvT_
// (plus de 60 caractères !)
```

**Impact :**
- Table des symboles plus grande
- Temps de linkage plus long
- Fichiers objets plus volumineux

### 2. Débogage

**Symboles manglés dans les traces de pile :**

```
Segmentation fault (core dumped)

gdb programme core
(gdb) bt

#0  _ZN8MaClasse7methodeEi ()
#1  _ZN3app10Controller9traiterEv ()
#2  _Z4mainv ()
```

**Avec démangle (automatique dans GDB moderne) :**
```
#0  MaClasse::methode(int)
#1  app::Controller::traiter()
#2  main()
```

### 3. Compatibilité binaire

**Changement de signature = changement de symbole**

```cpp
// Version 1.0
void traiter(int x);
// Symbole : _Z7traiteri

// Version 2.0
void traiter(int x, int y);
// Symbole : _Z7traiterii  ← DIFFÉRENT !
```

**Conséquence :** Incompatibilité ABI automatique si la signature change.

### 4. Interopérabilité

**Problème :** Différents compilateurs = différents schémas de mangling

```cpp
// Code identique, compilateurs différents

// GCC :
_Z8additionii

// MSVC :
?addition@@YAHHH@Z
```

**Solution :** Utiliser `extern "C"` pour l'interopérabilité.

---

## Problèmes et solutions

### Problème 1 : Symbole non trouvé (undefined reference)

**Erreur typique :**
```
undefined reference to `_Z8additionii'
```

**Causes possibles :**

1. **Oubli de lier avec la bibliothèque**
   ```bash
   # ❌ Erreur
   g++ main.cpp -o programme

   # ✅ Correct
   g++ main.cpp -L. -lmath -o programme
   ```

2. **Incompatibilité de compilation**
   ```bash
   # Fonction compilée en C, appelée depuis C++
   # Symbole attendu : _Z8additionii
   # Symbole fourni : addition

   # Solution : extern "C"
   ```

3. **Symbole manglé différemment**
   ```bash
   # Vérifie les symboles disponibles
   nm libmath.so | grep addition

   # Compare avec ce qui est demandé
   nm main.o | grep -i undefined
   ```

### Problème 2 : Conflit de symboles (multiple definition)

**Erreur typique :**
```
multiple definition of `_Z8additionii'
```

**Causes :**

1. **Fonction définie dans un header inclus plusieurs fois**
   ```cpp
   // ❌ Dans math.h
   int addition(int a, int b) {
       return a + b;
   }

   // ✅ Solution : inline ou extern
   inline int addition(int a, int b) {
       return a + b;
   }
   ```

2. **Même symbole dans plusieurs bibliothèques**
   ```bash
   # Vérifier les symboles exportés
   nm -D lib1.so | grep addition
   nm -D lib2.so | grep addition
   ```

### Problème 3 : Symboles trop longs

**Avec des templates profondément imbriqués :**
```cpp
std::map<std::string, std::vector<std::pair<int, std::string>>>
```

**Le symbole manglé peut être extrêmement long (> 500 caractères) !**

**Conséquences :**
- Limite de longueur de certains linkers
- Erreurs de compilation
- Messages d'erreur illisibles

**Solutions :**

1. **Typedef pour simplifier**
   ```cpp
   using MaCarte = std::map<std::string, std::vector<std::pair<int, std::string>>>;
   ```

2. **Réduire l'imbrication des templates**

3. **Options de compilateur**
   ```bash
   # GCC : augmenter la limite
   g++ -ftemplate-depth=1000 ...
   ```

### Problème 4 : Démangle incorrect

**Symptôme :** `c++filt` ne reconnaît pas le symbole

**Causes :**
- Symbole corrompu
- Format de mangling non standard
- Version différente de l'ABI

**Solution :**
```bash
# Vérifier quel compilateur a généré le binaire
file programme

# Utiliser l'outil correspondant
# GCC/Clang : c++filt
# MSVC : undname (Windows SDK)
```

---

## Bonnes pratiques

### 1. Utiliser `extern "C"` pour les APIs publiques

```cpp
// Interface publique stable
#ifdef __cplusplus
extern "C" {
#endif

int addition(int a, int b);  
void traiter(const char* data);  

#ifdef __cplusplus
}
#endif

// Implémentation interne peut utiliser C++
int addition(int a, int b) {
    std::cout << "Addition appelée" << std::endl;
    return a + b;
}
```

**Avantages :**
- Symboles stables et lisibles
- Compatibilité ABI garantie
- Interopérabilité facile

### 2. Limiter l'utilisation de templates dans les headers publics

```cpp
// ❌ Mauvais : Template dans l'API publique
template <typename T>  
class Container {  
    // ...
};

// ✅ Meilleur : Interface non-template
class Container {
    // Implémentation template cachée
};
```

### 3. Utiliser des namespaces avec préfixes

```cpp
// ✅ BON
namespace mylib {
    void fonction();
}

// ❌ RISQUE : Namespace générique
namespace utils {
    void fonction();  // Conflit possible avec d'autres libs
}
```

### 4. Documenter les garanties ABI

```cpp
/**
 * @file api.h
 * @brief API publique de la bibliothèque
 *
 * Garanties ABI :
 * - Toutes les fonctions utilisent extern "C"
 * - Pas de name mangling
 * - Compatible entre compilateurs
 * - Stabilité de version en version
 */
```

### 5. Outils de vérification

```bash
# Vérifier les symboles exportés
nm -D --demangle libmylib.so

# Comparer deux versions
nm -D --demangle libmylib.so.1.0 > v1.txt  
nm -D --demangle libmylib.so.1.1 > v2.txt  
diff v1.txt v2.txt  

# Utiliser abidiff pour analyse détaillée
abidiff libmylib.so.1.0 libmylib.so.1.1
```

---

## Exemples pratiques

### Exemple 1 : Analyser un binaire

```bash
# Compiler un programme
cat > example.cpp << 'EOF'
#include <iostream>

class Calculator {  
public:  
    int add(int a, int b) {
        return a + b;
    }

    int subtract(int a, int b) {
        return a - b;
    }
};

int main() {
    Calculator calc;
    std::cout << calc.add(5, 3) << std::endl;
    return 0;
}
EOF

g++ example.cpp -o example

# Voir tous les symboles démanglés
nm -C example | grep Calculator

# Sortie :
# 0000000000001234 W Calculator::add(int, int)
# 0000000000001245 W Calculator::subtract(int, int)

# Voir les symboles manglés
nm example | grep Calculator

# Sortie :
# 0000000000001234 W _ZN10Calculator3addEii
# 0000000000001245 W _ZN10Calculator8subtractEii
```

### Exemple 2 : Débogage avec GDB

```cpp
// bug.cpp
#include <iostream>

void fonction_problematique(int x) {
    int* ptr = nullptr;
    *ptr = x;  // Crash !
}

int main() {
    fonction_problematique(42);
    return 0;
}
```

```bash
# Compiler avec symboles de debug
g++ -g bug.cpp -o bug

# Exécuter avec GDB
gdb ./bug

(gdb) run
# Program received signal SIGSEGV

(gdb) bt
# #0  fonction_problematique(int) (x=42) at bug.cpp:5
# #1  main () at bug.cpp:9

# Les noms sont automatiquement démanglés !
```

### Exemple 3 : Identifier le compilateur utilisé

```bash
# Analyser un binaire inconnu
file /usr/bin/programme

# Sortie possible :
# ELF 64-bit LSB executable, x86-64...

# Voir le format des symboles
nm /usr/bin/programme | head

# Si vous voyez _Z... → GCC/Clang
# Si vous voyez ?... → MSVC
```

---

## Récapitulatif

### Points clés à retenir

1. **Définition**
   - Name mangling = transformation des noms en symboles uniques
   - Nécessaire en C++ pour supporter surcharge, templates, etc.
   - Minimal ou absent en C

2. **Pourquoi en C++ ?**
   - Surcharge de fonctions
   - Espaces de noms
   - Méthodes de classes
   - Templates
   - Opérateurs surchargés

3. **Format (Itanium ABI)**
   - Préfixe `_Z`
   - Encodage de la structure complète
   - Types encodés avec lettres simples
   - Noms préfixés par leur longueur

4. **Différences de compilateurs**
   - GCC/Clang : Itanium ABI (`_Z...`)
   - MSVC : Propriétaire (`?...`)
   - Incompatibilité entre compilateurs

5. **Outils**
   - `c++filt` : Démangle les noms
   - `nm -C` : Liste symboles démanglés
   - `objdump -C` : Désassemble avec noms lisibles
   - `abi::__cxa_demangle` : API C++

6. **Impact**
   - Taille des binaires augmentée
   - Compatibilité ABI affectée
   - Débogage plus complexe
   - Interopérabilité limitée

7. **Solutions**
   - `extern "C"` pour APIs publiques
   - Limiter templates publics
   - Namespaces bien nommés
   - Documentation des garanties ABI

### Checklist name mangling

- [ ] Comprendre pourquoi le symbole est manglé
- [ ] Utiliser `c++filt` pour démangle
- [ ] Vérifier les symboles avec `nm -C`
- [ ] Utiliser `extern "C"` pour interopérabilité
- [ ] Éviter templates dans API publique
- [ ] Préfixer namespaces pour éviter conflits
- [ ] Tester avec différents compilateurs
- [ ] Documenter les garanties ABI
- [ ] Utiliser outils de vérification (abidiff)

### Le mot de la fin

Le name mangling est un mécanisme essentiel mais souvent invisible de C++. Il permet toute la richesse du langage (surcharge, templates, classes) tout en maintenant une compatibilité au niveau binaire.

**Comprendre le name mangling vous aide à :**
- 🐛 **Déboguer** les problèmes de linkage
- 🔗 **Créer** des bibliothèques interopérables
- 📊 **Analyser** des binaires
- 🛠️ **Optimiser** la compatibilité ABI

Même si vous ne manipulez pas directement les symboles manglés au quotidien, cette connaissance est précieuse pour tout développeur C++ sérieux.

**Rappelez-vous : Le name mangling n'est pas votre ennemi, c'est ce qui rend C++ possible ! 🎯**

---


⏭️ [Tests et Qualité](/29-tests-et-qualite/README.md)
