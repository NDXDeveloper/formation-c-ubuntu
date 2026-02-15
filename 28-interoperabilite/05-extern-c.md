🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 28.5 extern "C" pour C++

## Introduction

### Le problème : C++ et C ne parlent pas la même langue binaire

Imaginez deux personnes qui parlent des langues différentes. Elles peuvent comprendre les mots écrits (le code source), mais quand elles parlent (code compilé), elles ne se comprennent plus. C'est exactement ce qui se passe entre C et C++ au niveau binaire.

**Situation typique :**
```cpp
// Fichier C++ : math.cpp
int addition(int a, int b) {
    return a + b;
}
```

```c
// Fichier C : main.c
int addition(int a, int b);  // Déclaration

int main() {
    int resultat = addition(5, 7);
    return 0;
}
```

**Compilation :**
```bash
g++ -c math.cpp -o math.o  
gcc main.c math.o -o programme  

# Erreur du linker :
# undefined reference to `addition'
```

**Pourquoi ça ne marche pas ?** Le code C++ et le code C ne génèrent pas les mêmes symboles binaires, même si le code source est identique !

**La solution :** `extern "C"` !

### Qu'est-ce que `extern "C"` ?

`extern "C"` est une instruction qui dit au compilateur C++ :
> "Traite ces fonctions comme si elles étaient du C pur, sans les transformations spéciales de C++"

**Avec `extern "C"` :**
```cpp
// Fichier C++ : math.cpp
extern "C" int addition(int a, int b) {
    return a + b;
}
```

Maintenant le code C peut appeler cette fonction ! ✅

---

## Le name mangling : La racine du problème

### Qu'est-ce que le name mangling ?

**Name mangling** (ou "mutilation de noms") est un processus où le compilateur C++ transforme les noms de fonctions pour encoder des informations supplémentaires.

**Pourquoi C++ fait ça ?** Pour supporter la surcharge de fonctions (overloading).

### Exemple de name mangling

**Code source C++ :**
```cpp
int addition(int a, int b) {
    return a + b;
}

double addition(double a, double b) {
    return a + b;
}
```

En C++, vous pouvez avoir deux fonctions `addition` avec des paramètres différents. Comment le linker les différencie-t-il ?

**Symboles générés par le compilateur C++ :**
```
_Z8additionii       # addition(int, int)
_Z8additiondd       # addition(double, double)
```

Le nom encode :
- `_Z` : Préfixe de mangling
- `8` : Longueur du nom ("addition" = 8 caractères)
- `addition` : Nom de la fonction
- `ii` : Deux paramètres `int`
- `dd` : Deux paramètres `double`

### Comparaison C vs C++

**Code identique :**
```c
// math.c (compilé avec gcc)
int addition(int a, int b) {
    return a + b;
}
```

```cpp
// math.cpp (compilé avec g++)
int addition(int a, int b) {
    return a + b;
}
```

**Compilation :**
```bash
gcc -c math.c -o math_c.o  
g++ -c math.cpp -o math_cpp.o  
```

**Symboles générés :**
```bash
nm math_c.o
# 0000000000000000 T addition    ← Nom simple en C

nm math_cpp.o
# 0000000000000000 T _Z8additionii    ← Nom manglé en C++
```

**Conséquence :** Un programme C qui cherche `addition` ne trouvera pas `_Z8additionii` !

### Visualiser le mangling

```bash
# Voir les symboles manglés
nm -C math_cpp.o

# -C : "demangle" (retraduire en nom lisible)
# Sortie : addition(int, int)

# Sans -C :
nm math_cpp.o
# Sortie : _Z8additionii

# Outil c++filt pour démangle
echo "_Z8additionii" | c++filt
# Sortie : addition(int, int)
```

---

## Utilisation de base de `extern "C"`

### Syntaxe 1 : Fonction unique

```cpp
extern "C" int addition(int a, int b) {
    return a + b;
}
```

**Effet :** La fonction `addition` sera compilée avec un nom simple (`addition`) au lieu d'un nom manglé (`_Z8additionii`).

### Syntaxe 2 : Bloc de fonctions

```cpp
extern "C" {
    int addition(int a, int b) {
        return a + b;
    }

    int soustraction(int a, int b) {
        return a - b;
    }

    double multiplication(double a, double b) {
        return a * b;
    }
}
```

**Toutes les fonctions dans le bloc** sont traitées comme du C.

### Syntaxe 3 : Déclarations dans un header

```cpp
// math.h
#ifndef MATH_H
#define MATH_H

extern "C" {
    int addition(int a, int b);
    int soustraction(int a, int b);
    double multiplication(double a, double b);
}

#endif
```

---

## Pattern standard : Header compatible C et C++

### Le problème

Vous voulez créer un header qui fonctionne :
- Avec du code C (compilé avec `gcc`)
- Avec du code C++ (compilé avec `g++`)

**Solution :** Utiliser la macro `__cplusplus`.

### Pattern recommandé

```cpp
// math.h
#ifndef MATH_H
#define MATH_H

#ifdef __cplusplus
extern "C" {
#endif

int addition(int a, int b);  
int soustraction(int a, int b);  
double multiplication(double a, double b);  

#ifdef __cplusplus
}
#endif

#endif
```

**Comment ça marche ?**

**Quand compilé avec C (gcc) :**
- `__cplusplus` n'est PAS défini
- Les `#ifdef` sont ignorés
- Le header devient simplement :
  ```c
  int addition(int a, int b);
  int soustraction(int a, int b);
  double multiplication(double a, double b);
  ```

**Quand compilé avec C++ (g++) :**
- `__cplusplus` EST défini
- Les `extern "C" {` et `}` sont inclus
- Les fonctions sont traitées comme du C

### Exemple complet

**Header : `math.h`**
```c
#ifndef MATH_H
#define MATH_H

#ifdef __cplusplus
extern "C" {
#endif

int addition(int a, int b);  
int soustraction(int a, int b);  

#ifdef __cplusplus
}
#endif

#endif
```

**Implémentation C++ : `math.cpp`**
```cpp
#include "math.h"

int addition(int a, int b) {
    return a + b;
}

int soustraction(int a, int b) {
    return a - b;
}
```

**Utilisation depuis C : `main.c`**
```c
#include <stdio.h>
#include "math.h"

int main() {
    printf("5 + 3 = %d\n", addition(5, 3));
    printf("10 - 4 = %d\n", soustraction(10, 4));
    return 0;
}
```

**Utilisation depuis C++ : `main.cpp`**
```cpp
#include <iostream>
#include "math.h"

int main() {
    std::cout << "5 + 3 = " << addition(5, 3) << std::endl;
    std::cout << "10 - 4 = " << soustraction(10, 4) << std::endl;
    return 0;
}
```

**Compilation :**
```bash
# Compiler l'implémentation C++
g++ -c math.cpp -o math.o

# Programme C utilisant la bibliothèque C++
gcc main.c math.o -lstdc++ -o prog_c
./prog_c

# Programme C++ utilisant la bibliothèque C++
g++ main.cpp math.o -o prog_cpp
./prog_cpp
```

Les deux fonctionnent ! ✅

---

## Ce que `extern "C"` affecte

### ✅ Ce qui est affecté

**1. Name mangling**
```cpp
extern "C" void fonction();
// Symbole : fonction (pas de mangling)
```

**2. Convention d'appel**
```cpp
extern "C" int addition(int a, int b);
// Utilise la convention d'appel C
```

### ❌ Ce qui n'est PAS affecté

**1. Le code peut rester du C++ moderne**
```cpp
extern "C" int traiter_vecteur(int* data, int size) {
    // Vous pouvez utiliser du C++ moderne à l'intérieur !
    std::vector<int> vec(data, data + size);
    std::sort(vec.begin(), vec.end());
    return vec[0];  // Plus petit élément
}
```

**2. Les types C++ ne sont PAS autorisés dans la signature**
```cpp
// ❌ ERREUR : std::string n'existe pas en C
extern "C" std::string obtenir_nom();

// ❌ ERREUR : std::vector n'existe pas en C
extern "C" void traiter(std::vector<int>& vec);

// ✅ OK : Types C compatibles
extern "C" const char* obtenir_nom();  
extern "C" void traiter(int* data, int size);  
```

---

## Cas d'usage détaillés

### Cas 1 : Bibliothèque C++ utilisable depuis C

**Objectif :** Créer une bibliothèque de traitement d'images en C++ avec une API C.

**Header public : `image_lib.h`**
```c
#ifndef IMAGE_LIB_H
#define IMAGE_LIB_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Pointeur opaque (cachée implémentation C++)
typedef struct Image Image;

// API C
Image* image_create(int width, int height);  
void image_destroy(Image* img);  
void image_set_pixel(Image* img, int x, int y, uint32_t color);  
uint32_t image_get_pixel(Image* img, int x, int y);  
void image_apply_filter(Image* img, const char* filter_name);  

#ifdef __cplusplus
}
#endif

#endif
```

**Implémentation C++ : `image_lib.cpp`**
```cpp
#include "image_lib.h"
#include <vector>
#include <string>
#include <algorithm>

// Classe C++ interne (pas visible depuis C)
class ImageImpl {  
public:  
    int width, height;
    std::vector<uint32_t> pixels;

    ImageImpl(int w, int h) : width(w), height(h) {
        pixels.resize(w * h, 0);
    }

    void setPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = color;
        }
    }

    uint32_t getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return pixels[y * width + x];
        }
        return 0;
    }

    void applyFilter(const std::string& name) {
        if (name == "invert") {
            for (auto& pixel : pixels) {
                pixel = ~pixel;
            }
        } else if (name == "grayscale") {
            for (auto& pixel : pixels) {
                uint8_t r = (pixel >> 16) & 0xFF;
                uint8_t g = (pixel >> 8) & 0xFF;
                uint8_t b = pixel & 0xFF;
                uint8_t gray = (r + g + b) / 3;
                pixel = (gray << 16) | (gray << 8) | gray;
            }
        }
    }
};

// Implémentation des fonctions C
extern "C" {

Image* image_create(int width, int height) {
    return reinterpret_cast<Image*>(new ImageImpl(width, height));
}

void image_destroy(Image* img) {
    delete reinterpret_cast<ImageImpl*>(img);
}

void image_set_pixel(Image* img, int x, int y, uint32_t color) {
    reinterpret_cast<ImageImpl*>(img)->setPixel(x, y, color);
}

uint32_t image_get_pixel(Image* img, int x, int y) {
    return reinterpret_cast<ImageImpl*>(img)->getPixel(x, y);
}

void image_apply_filter(Image* img, const char* filter_name) {
    reinterpret_cast<ImageImpl*>(img)->applyFilter(filter_name);
}

}  // extern "C"
```

**Utilisation depuis C : `main.c`**
```c
#include <stdio.h>
#include "image_lib.h"

int main() {
    Image* img = image_create(800, 600);

    image_set_pixel(img, 10, 20, 0xFF0000);  // Rouge
    image_apply_filter(img, "grayscale");

    uint32_t color = image_get_pixel(img, 10, 20);
    printf("Couleur après filtre : 0x%X\n", color);

    image_destroy(img);
    return 0;
}
```

**Compilation :**
```bash
g++ -c image_lib.cpp -o image_lib.o  
gcc main.c image_lib.o -lstdc++ -o programme  
./programme
```

**Avantages de cette approche :**
- ✅ C peut utiliser une bibliothèque C++ puissante
- ✅ L'implémentation C++ utilise STL, classes, etc.
- ✅ L'API C reste simple et stable
- ✅ Pointeur opaque cache les détails d'implémentation

### Cas 2 : Appeler du code C depuis C++

**Bibliothèque C existante : `legacy.c`**
```c
#include <stdio.h>

void ancienne_fonction(int x) {
    printf("Fonction C legacy : %d\n", x);
}

int calcul_complexe(int a, int b) {
    return a * a + b * b;
}
```

**Header : `legacy.h`**
```c
#ifndef LEGACY_H
#define LEGACY_H

#ifdef __cplusplus
extern "C" {
#endif

void ancienne_fonction(int x);  
int calcul_complexe(int a, int b);  

#ifdef __cplusplus
}
#endif

#endif
```

**Nouveau code C++ : `modern.cpp`**
```cpp
#include <iostream>
#include "legacy.h"

class Calculator {  
public:  
    void compute(int a, int b) {
        // Appeler du code C depuis du code C++ !
        int resultat = calcul_complexe(a, b);
        std::cout << "Résultat : " << resultat << std::endl;

        ancienne_fonction(resultat);
    }
};

int main() {
    Calculator calc;
    calc.compute(3, 4);
    return 0;
}
```

**Compilation :**
```bash
gcc -c legacy.c -o legacy.o  
g++ modern.cpp legacy.o -o programme  
./programme
```

### Cas 3 : Plugin system

**Application hôte C++ qui charge des plugins (C ou C++).**

**Interface plugin : `plugin_api.h`**
```c
#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* name;
    int version;
} PluginInfo;

// Fonctions que chaque plugin doit exporter
PluginInfo get_plugin_info();  
int plugin_init();  
void plugin_cleanup();  
int plugin_process(const char* input, char* output, int output_size);  

#ifdef __cplusplus
}
#endif

#endif
```

**Plugin écrit en C : `plugin_c.c`**
```c
#include "plugin_api.h"
#include <string.h>
#include <stdio.h>

PluginInfo get_plugin_info() {
    PluginInfo info = {"Plugin C", 1};
    return info;
}

int plugin_init() {
    printf("Plugin C initialisé\n");
    return 0;
}

void plugin_cleanup() {
    printf("Plugin C nettoyé\n");
}

int plugin_process(const char* input, char* output, int output_size) {
    snprintf(output, output_size, "Traité par C : %s", input);
    return 0;
}
```

**Plugin écrit en C++ : `plugin_cpp.cpp`**
```cpp
#include "plugin_api.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>

extern "C" {

PluginInfo get_plugin_info() {
    return {"Plugin C++", 1};
}

int plugin_init() {
    std::cout << "Plugin C++ initialisé" << std::endl;
    return 0;
}

void plugin_cleanup() {
    std::cout << "Plugin C++ nettoyé" << std::endl;
}

int plugin_process(const char* input, char* output, int output_size) {
    std::string str(input);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    std::snprintf(output, output_size, "Traité par C++ : %s", str.c_str());
    return 0;
}

}  // extern "C"
```

**Application hôte : `host.cpp`**
```cpp
#include <dlfcn.h>
#include <iostream>
#include "plugin_api.h"

void charger_et_utiliser_plugin(const char* path) {
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        std::cerr << "Erreur : " << dlerror() << std::endl;
        return;
    }

    // Charger les symboles
    auto get_info = (PluginInfo(*)())dlsym(handle, "get_plugin_info");
    auto init = (int(*)())dlsym(handle, "plugin_init");
    auto process = (int(*)(const char*, char*, int))dlsym(handle, "plugin_process");
    auto cleanup = (void(*)())dlsym(handle, "plugin_cleanup");

    if (!get_info || !init || !process || !cleanup) {
        std::cerr << "Symboles manquants" << std::endl;
        dlclose(handle);
        return;
    }

    // Utiliser le plugin
    PluginInfo info = get_info();
    std::cout << "Plugin : " << info.name << " v" << info.version << std::endl;

    init();

    char output[256];
    process("hello world", output, sizeof(output));
    std::cout << "Sortie : " << output << std::endl;

    cleanup();

    dlclose(handle);
}

int main() {
    charger_et_utiliser_plugin("./plugin_c.so");
    charger_et_utiliser_plugin("./plugin_cpp.so");
    return 0;
}
```

**Compilation :**
```bash
# Plugins
gcc -shared -fPIC plugin_c.c -o plugin_c.so  
g++ -shared -fPIC plugin_cpp.cpp -o plugin_cpp.so  

# Hôte
g++ host.cpp -ldl -o host

# Exécution
./host
```

Les deux plugins (C et C++) fonctionnent avec la même application ! ✅

---

## Limitations et restrictions

### 1. Pas de surcharge de fonctions

```cpp
// ❌ ERREUR : Impossible avec extern "C"
extern "C" {
    int addition(int a, int b);
    double addition(double a, double b);  // Erreur !
}
```

**Pourquoi ?** `extern "C"` désactive le name mangling. Sans mangling, le compilateur ne peut pas différencier les deux fonctions.

**Solution :** Noms différents
```cpp
extern "C" {
    int addition_int(int a, int b);
    double addition_double(double a, double b);
}
```

### 2. Pas de fonctions membres de classes

```cpp
class MaClasse {  
public:  
    // ❌ ERREUR : extern "C" impossible pour méthode
    extern "C" void methode();
};
```

**Pourquoi ?** Les méthodes de classes ont un paramètre caché `this`. Ce n'est pas compatible avec C.

**Solution :** Fonctions libres qui utilisent la classe en interne (comme dans l'exemple image_lib).

### 3. Pas de templates

```cpp
// ❌ ERREUR : Templates incompatibles avec extern "C"
extern "C" template <typename T>  
T addition(T a, T b) {  
    return a + b;
}
```

**Solution :** Instancier les templates puis exposer avec extern "C"
```cpp
template <typename T>  
T addition_impl(T a, T b) {  
    return a + b;
}

extern "C" {
    int addition_int(int a, int b) {
        return addition_impl(a, b);
    }

    double addition_double(double a, double b) {
        return addition_impl(a, b);
    }
}
```

### 4. Pas d'exceptions

```cpp
extern "C" int fonction_dangereuse() {
    // ❌ PROBLÈME : L'exception ne peut pas traverser la frontière C
    throw std::runtime_error("Erreur !");
}
```

**Pourquoi ?** C ne comprend pas les exceptions C++.

**Solution :** Codes d'erreur
```cpp
extern "C" int fonction_safe() {
    try {
        // Code qui peut lever une exception
        operation_risquee();
        return 0;  // Succès
    } catch (const std::exception& e) {
        fprintf(stderr, "Erreur : %s\n", e.what());
        return -1;  // Erreur
    } catch (...) {
        return -2;  // Erreur inconnue
    }
}
```

### 5. Types C++ dans les signatures

```cpp
// ❌ ERREUR : std::string n'existe pas en C
extern "C" std::string obtenir_texte();

// ❌ ERREUR : std::vector n'existe pas en C
extern "C" void traiter(const std::vector<int>& vec);
```

**Solution :** Utiliser des types C compatibles
```cpp
// ✅ OK : const char* existe en C
extern "C" const char* obtenir_texte();

// ✅ OK : int* et size existe en C
extern "C" void traiter(const int* data, int size);
```

---

## Bonnes pratiques

### 1. Toujours utiliser le pattern `#ifdef __cplusplus`

```cpp
// ✅ BON
#ifdef __cplusplus
extern "C" {
#endif

void fonction();

#ifdef __cplusplus
}
#endif
```

Ce pattern garantit que le header fonctionne en C et en C++.

### 2. Documenter les fonctions extern "C"

```cpp
/**
 * Crée une nouvelle image.
 *
 * @param width Largeur en pixels
 * @param height Hauteur en pixels
 * @return Pointeur vers l'image, ou NULL en cas d'erreur
 *
 * @note La mémoire doit être libérée avec image_destroy()
 */
extern "C" Image* image_create(int width, int height);
```

### 3. Préfixer les fonctions pour éviter les collisions

```cpp
// ✅ BON : Préfixe unique
extern "C" {
    int mylib_init();
    void mylib_cleanup();
    int mylib_process(const char* data);
}

// ❌ RISQUE : Noms génériques
extern "C" {
    int init();      // Peut entrer en conflit avec d'autres bibliothèques
    void cleanup();
    int process(const char* data);
}
```

### 4. Gérer les erreurs avec des codes de retour

```cpp
// ✅ BON : Codes d'erreur clairs
extern "C" {
    enum {
        MYLIB_SUCCESS = 0,
        MYLIB_ERROR_INVALID_PARAM = -1,
        MYLIB_ERROR_OUT_OF_MEMORY = -2,
        MYLIB_ERROR_IO = -3
    };

    int mylib_operation(int param);
}
```

### 5. Pointeurs opaques pour cacher l'implémentation

```cpp
// Header public
typedef struct MyObject MyObject;

extern "C" {
    MyObject* myobject_create();
    void myobject_destroy(MyObject* obj);
    int myobject_do_something(MyObject* obj);
}

// Implémentation .cpp
struct MyObject {
    std::string data;
    std::vector<int> values;
    // Implémentation C++ cachée !
};
```

### 6. Toujours capturer les exceptions

```cpp
extern "C" int fonction_safe(const char* input) {
    if (!input) {
        return -1;  // Erreur : paramètre null
    }

    try {
        // Code C++ qui peut lever des exceptions
        std::string str(input);
        // ... traitement ...
        return 0;  // Succès
    } catch (const std::bad_alloc&) {
        return -2;  // Erreur mémoire
    } catch (const std::exception& e) {
        fprintf(stderr, "Exception : %s\n", e.what());
        return -3;  // Erreur générique
    } catch (...) {
        return -4;  // Erreur inconnue
    }
}
```

---

## Pièges courants et solutions

### Piège 1 : Oublier `extern "C"` dans l'implémentation

```cpp
// math.h
#ifdef __cplusplus
extern "C" {
#endif
int addition(int a, int b);
#ifdef __cplusplus
}
#endif

// math.cpp
// ❌ OUBLI : Pas de extern "C" ici !
int addition(int a, int b) {
    return a + b;
}
```

**Résultat :** Le symbole sera manglé malgré la déclaration dans le header.

**Solution :**
```cpp
// math.cpp
extern "C" int addition(int a, int b) {
    return a + b;
}

// Ou inclure le header
#include "math.h"
int addition(int a, int b) {  // extern "C" vient du header
    return a + b;
}
```

### Piège 2 : Variables globales

```cpp
// ❌ Problématique
extern "C" {
    extern int compteur;  // Variable globale
}

// Peut causer des problèmes d'initialisation
```

**Solution :** Utiliser des fonctions accesseurs
```cpp
extern "C" {
    int get_compteur();
    void set_compteur(int valeur);
}

// Implémentation
static int compteur = 0;  // Variable interne

extern "C" int get_compteur() {
    return compteur;
}

extern "C" void set_compteur(int valeur) {
    compteur = valeur;
}
```

### Piège 3 : Passer des objets C++ par valeur

```cpp
// ❌ ERREUR
struct MonStruct {
    std::string data;  // Type C++ !
};

extern "C" void fonction(MonStruct s);  // Impossible
```

**Solution :** Pointeurs opaques
```cpp
typedef struct MonStruct MonStruct;

extern "C" {
    MonStruct* monstruct_create();
    void monstruct_destroy(MonStruct* s);
}
```

### Piège 4 : Callback C++ vers C

```cpp
// Callback C
typedef void (*callback_t)(int x);

extern "C" void register_callback(callback_t cb);

// ❌ Problème : Passer une lambda C++
register_callback([](int x) {
    std::cout << x << std::endl;
});  // Erreur !
```

**Solution :** Fonction libre ou fonction statique
```cpp
static void mon_callback(int x) {
    std::cout << x << std::endl;
}

extern "C" void mon_callback_wrapper(int x);

void mon_callback_wrapper(int x) {
    mon_callback(x);
}

register_callback(mon_callback_wrapper);
```

### Piège 5 : Inline functions dans les headers

```cpp
// ❌ Problématique
extern "C" {
    inline int addition(int a, int b) {
        return a + b;
    }
}
```

**Pourquoi ?** Les fonctions inline sont expansées au point d'appel. Cela peut causer des problèmes de linkage.

**Solution :** Éviter inline pour extern "C", ou utiliser `static inline`
```cpp
extern "C" {
    // Déclaration
    int addition(int a, int b);
}

// Implémentation dans .cpp
extern "C" int addition(int a, int b) {
    return a + b;
}
```

---

## Outils de vérification

### 1. Vérifier les symboles exportés

```bash
# Compiler
g++ -c math.cpp -o math.o

# Voir les symboles
nm math.o

# Avec démangling
nm -C math.o
```

**Exemple de sortie :**
```
0000000000000000 T addition           # Sans extern "C" (nom simple)
0000000000000010 T _Z14fonction_cppii # Avec mangling (C++)
```

### 2. Vérifier qu'un header compile en C

```bash
# Tester la compilation en C
gcc -c -xc test.h -o /dev/null

# Si ça compile, le header est compatible C
```

### 3. Utiliser `c++filt` pour démangle

```bash
# Démangle un symbole
echo "_Z8additionii" | c++filt
# Sortie : addition(int, int)
```

### 4. Vérifier avec `objdump`

```bash
# Voir les symboles avec objdump
objdump -t math.o

# Avec démangle
objdump -C -t math.o
```

---

## Récapitulatif

### Points clés à retenir

1. **Le problème**
   - C++ fait du name mangling (transformation des noms)
   - C ne fait pas de name mangling
   - Résultat : incompatibilité binaire

2. **La solution : `extern "C"`**
   - Désactive le name mangling
   - Utilise la convention d'appel C
   - Permet l'interopérabilité C/C++

3. **Pattern standard**
   ```cpp
   #ifdef __cplusplus
   extern "C" {
   #endif

   // Déclarations

   #ifdef __cplusplus
   }
   #endif
   ```

4. **Ce qui est possible**
   - ✅ Fonctions libres
   - ✅ Pointeurs opaques
   - ✅ Types C dans les signatures
   - ✅ Utiliser du C++ à l'intérieur des fonctions

5. **Ce qui n'est PAS possible**
   - ❌ Surcharge de fonctions
   - ❌ Méthodes de classes
   - ❌ Templates dans les signatures
   - ❌ Exceptions traversant la frontière
   - ❌ Types C++ dans les signatures

6. **Bonnes pratiques**
   - Toujours utiliser `#ifdef __cplusplus`
   - Préfixer les fonctions
   - Pointeurs opaques pour l'implémentation
   - Codes d'erreur au lieu d'exceptions
   - Capturer toutes les exceptions

### Checklist extern "C"

- [ ] Header utilise `#ifdef __cplusplus`
- [ ] Pas de types C++ dans les signatures
- [ ] Pas de surcharge de fonctions
- [ ] Les exceptions sont capturées
- [ ] Documentation claire de l'API
- [ ] Préfixe de namespace pour éviter collisions
- [ ] Pointeurs opaques pour cacher l'implémentation
- [ ] Tests depuis code C ET C++
- [ ] Symboles vérifiés avec `nm`

### Le mot de la fin

`extern "C"` est le pont qui permet à C et C++ de communiquer harmonieusement. Il permet de :
- **Réutiliser** du code C legacy depuis du code C++ moderne
- **Exposer** des bibliothèques C++ puissantes via une API C simple
- **Créer** des plugins et systèmes extensibles
- **Garantir** la compatibilité binaire

Maîtriser `extern "C"` est essentiel pour tout projet qui mélange C et C++, ce qui est extrêmement courant dans le monde réel.

**Rappelez-vous :** `extern "C"` n'est pas une limitation, c'est une fonctionnalité ! 🌉

---


⏭️ [Name mangling](/28-interoperabilite/06-name-mangling.md)
