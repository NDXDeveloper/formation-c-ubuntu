# Chapitre 28 - Interopérabilité : Compilation des exemples

## Flags standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17
g++ -Wall -Wextra -Werror -pedantic
```

---

## Section 28.1 : C depuis Python (01-06)

### 01_addition.c
- **Section** : 28.1 - C depuis Python (ctypes)
- **Description** : Fonction addition compilée en bibliothèque partagée .so
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -shared -fPIC -o libaddition.so 01_addition.c`
- **Test** : Via `05_test_ctypes.py`

### 02_somme_tableau.c
- **Section** : 28.1 - C depuis Python (ctypes)
- **Description** : Fonction somme de tableau compilée en .so (paramètre pointeur + size_t)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -shared -fPIC -o libsomme.so 02_somme_tableau.c`
- **Test** : Via `05_test_ctypes.py`

### 03_strings.c
- **Section** : 28.1 - C depuis Python (ctypes)
- **Description** : Fonctions de manipulation de chaînes (majuscules, longueur) en .so
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -shared -fPIC -o libstrings.so 03_strings.c`
- **Test** : Via `05_test_ctypes.py`

### 04_module_python.c
- **Section** : 28.1 - C depuis Python (Python C API)
- **Description** : Module d'extension Python en C (addition, division avec ZeroDivisionError)
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 -shared -fPIC $(python3-config --includes) -o module_exemple$(python3-config --extension-suffix) 04_module_python.c`
- **Note** : Sans `-pedantic` (headers Python)
- **Test** : Via `06_test_extension.py`

### 05_test_ctypes.py
- **Section** : 28.1 - C depuis Python (ctypes)
- **Description** : Script Python testant les 3 bibliothèques .so via ctypes
- **Exécution** : `python3 05_test_ctypes.py` (nécessite 01-03 compilés en .so)
- **Sortie attendue** :
```
=== Test ctypes : libaddition ===
  addition(3, 7) = 10
  addition(-5, 15) = 10
=== Test ctypes : libsomme ===
  somme_tableau([1, 2, 3, 4, 5]) = 15
  somme_tableau([10, 20, 30]) = 60
=== Test ctypes : libstrings ===
  longueur_chaine("Bonjour") = 7
  mettre_en_majuscules("hello world") = HELLO WORLD
```

### 06_test_extension.py
- **Section** : 28.1 - C depuis Python (Python C API)
- **Description** : Script Python testant le module d'extension C
- **Exécution** : `python3 06_test_extension.py` (nécessite 04 compilé)
- **Sortie attendue** :
```
=== Test module C extension ===
  addition(3, 7) = 10
  addition(-5, 15) = 10
  diviser(10.0, 3.0) = 3.3333333333333335
  diviser(10.0, 0.0) -> ZeroDivisionError: Division par zero
```

---

## Section 28.2 : C depuis Go (07)

### 07_cgo_mathlib/ (multi-fichiers)
- **Section** : 28.2 - C depuis Go (cgo)
- **Fichiers** : `mathlib.h`, `mathlib.c`, `main.go`
- **Description** : Bibliothèque C (factorielle, racine cubique) appelée depuis Go via cgo
- **Compilation** :
```bash
cd 07_cgo_mathlib
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c mathlib.c -o mathlib.o
ar rcs libmathlib.a mathlib.o
CGO_LDFLAGS="-L. -lmathlib -lm" go build -o cgo_mathlib main.go
```
- **Exécution** : `./cgo_mathlib`
- **Sortie attendue** :
```
=== cgo : C depuis Go ===
  Factorielle(5) = 120
  Factorielle(10) = 3628800
  Racine cubique(27.0) = 3.000000
  Racine cubique(125.0) = 5.000000
=== Fin ===
```

---

## Section 28.3 : Interfaçage Rust (08)

### 08_rust_ffi/ (multi-fichiers)
- **Section** : 28.3 - Interfaçage Rust
- **Fichiers** : `Cargo.toml`, `src/lib.rs`, `rustlib.h`, `main.c`
- **Description** : Bibliothèque Rust (factorielle, fibonacci, division sûre) appelée depuis C via FFI
- **Compilation** :
```bash
cd 08_rust_ffi
cargo build --release
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c -L./target/release -lrustlib -lpthread -ldl -lm -lgcc_s -o rust_ffi_test
```
- **Exécution** : `./rust_ffi_test`
- **Sortie attendue** :
```
=== Rust FFI depuis C ===
  factorielle_rust(5) = 120
  factorielle_rust(10) = 3628800
  fibonacci_rust(10) = 55
  fibonacci_rust(20) = 6765
  diviser_entiers(10, 3) : succes = 1, resultat = 3
  diviser_entiers(10, 0) : succes = 0, resultat = 0
=== Fin ===
```

---

## Section 28.4 : ABI et compatibilité (09)

### 09_struct_layout.c
- **Section** : 28.4 - ABI et compatibilité
- **Description** : Démonstration du padding, alignement, offsetof et types portables (stdint.h)
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 -o 09_struct_layout 09_struct_layout.c`
- **Note** : Sans `-pedantic` (`__attribute__((packed))` est une extension GCC)
- **Sortie attendue** : Tailles et offsets des membres de structs avec et sans packed

---

## Section 28.5 : extern "C" pour C++ (10-13)

### 10_extern_c_math/ (multi-fichiers)
- **Section** : 28.5 - extern "C" pour C++
- **Fichiers** : `math.h`, `math.cpp`, `main.c`
- **Description** : Programme C appelant des fonctions C++ via extern "C" (pattern #ifdef __cplusplus)
- **Compilation** :
```bash
cd 10_extern_c_math
g++ -Wall -Wextra -Werror -pedantic -c math.cpp -o math.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c math.o -lstdc++ -o extern_c_math
```
- **Sortie attendue** :
```
=== extern "C" : C appelle C++ ===
  5 + 3 = 8
  10 - 4 = 6
  -3 + 10 = 7
  100 - 99 = 1
=== Fin ===
```

### 11_image_lib/ (multi-fichiers)
- **Section** : 28.5 - extern "C" pour C++ (PIMPL)
- **Fichiers** : `image_lib.h`, `image_lib.cpp`, `main.c`
- **Description** : Bibliothèque C++ (vector, string) exposée via API C avec pointeur opaque (PIMPL pattern)
- **Compilation** :
```bash
cd 11_image_lib
g++ -Wall -Wextra -Werror -pedantic -c image_lib.cpp -o image_lib.o
gcc -Wall -Wextra -Werror -pedantic -std=c17 main.c image_lib.o -lstdc++ -o image_lib_demo
```
- **Sortie attendue** :
```
=== Bibliotheque C++ via API C (PIMPL) ===
  Image 800x600 creee
  Pixel (10,20) = 0xFF0000 (rouge)
  Apres grayscale : 0x555555
  Image2 pixel (0,0) = 0x00FF00 (vert)
  Apres invert : 0xFFFF00FF
  Pixel hors limites (-1,0) = 0x000000
  Images detruites
=== Fin ===
```

### 12_legacy/ (multi-fichiers)
- **Section** : 28.5 - extern "C" (C++ appelle C)
- **Fichiers** : `legacy.h`, `legacy.c`, `modern.cpp`
- **Description** : Programme C++ moderne appelant du code C legacy via extern "C"
- **Compilation** :
```bash
cd 12_legacy
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c legacy.c -o legacy.o
g++ -Wall -Wextra -Werror -pedantic modern.cpp legacy.o -o legacy_demo
```
- **Sortie attendue** :
```
=== C++ appelle du code C legacy ===
  compute(3, 4) :
  Resultat a*a + b*b = 25
Fonction C legacy : 25
  compute(5, 12) :
  Resultat a*a + b*b = 169
Fonction C legacy : 169
=== Fin ===
```

### 13_plugin/ (multi-fichiers)
- **Section** : 28.5 - extern "C" (système de plugins)
- **Fichiers** : `plugin_api.h`, `plugin_c.c`, `plugin_cpp.cpp`, `host.cpp`
- **Description** : Application hôte C++ chargeant des plugins C et C++ via dlopen
- **Compilation** :
```bash
cd 13_plugin
gcc -Wall -Wextra -Werror -pedantic -std=c17 -shared -fPIC plugin_c.c -o plugin_c.so
g++ -Wall -Wextra -Werror -pedantic -shared -fPIC plugin_cpp.cpp -o plugin_cpp.so
g++ -Wall -Wextra -Werror -pedantic host.cpp -ldl -o host
```
- **Exécution** : `./host` (nécessite plugin_c.so et plugin_cpp.so dans le même dossier)
- **Sortie attendue** :
```
=== Systeme de plugins (dlopen) ===
--- Plugin C ---
  Plugin : Plugin C v1
  Plugin C initialise
  Sortie : Traite par C : hello world
  Plugin C nettoye
--- Plugin C++ ---
  Plugin : Plugin C++ v1
  Plugin C++ initialise
  Sortie : Traite par C++ : HELLO WORLD
  Plugin C++ nettoye
=== Fin ===
```

---

## Section 28.6 : Name Mangling (14-15)

### 14_demangle.cpp
- **Section** : 28.6 - Name Mangling
- **Description** : Demangling de symboles C++ via abi::__cxa_demangle (cxxabi.h) - fonctions, namespaces, classes, templates, opérateurs, vtables
- **Compilation** : `g++ -Wall -Wextra -Werror -pedantic 14_demangle.cpp -o demangle`
- **Sortie attendue** :
```
=== Demangling de symboles C++ ===
--- Fonctions simples ---
  _Z8additionii       -> addition(int, int)
  _Z8additiondd       -> addition(double, double)
  _Z8additioniii      -> addition(int, int, int)
  _Z7traiteridc       -> traiter(int, double, char)
--- Namespaces ---
  _ZN4math8additionEii   -> math::addition(int, int)
  _ZN6calcul8additionEii -> calcul::addition(int, int)
--- Classes ---
  _ZN9Rectangle13calculer_aireEii -> Rectangle::calculer_aire(int, int)
  _ZN6Cercle13calculer_aireEd     -> Cercle::calculer_aire(double)
  _ZN10Calculator3addEii           -> Calculator::add(int, int)
--- Namespace + Classe ---
  _ZN3app7Service7traiterEid -> app::Service::traiter(int, double)
--- Constructeurs/Destructeurs ---
  _ZN8MaClasseC1Ev  -> MaClasse::MaClasse()
  _ZN8MaClasseC1Ei  -> MaClasse::MaClasse(int)
  _ZN8MaClasseD1Ev  -> MaClasse::~MaClasse()
--- Pointeurs et references ---
  _Z8fonctionPi   -> fonction(int*)
  _Z8fonctionRi   -> fonction(int&)
  _Z8fonctionPKi  -> fonction(int const*)
--- Operateurs ---
  _ZN5PointplERKS_ -> Point::operator+(Point const&)
  _ZN5PointeqERKS_ -> Point::operator==(Point const&)
--- Vtables ---
  _ZTV4Base    -> vtable for Base
  _ZTV7Derivee -> vtable for Derivee
--- Cas d'erreur ---
  addition (nom C) -> (nom invalide)
=== Fin ===
```

### 15_bug_gdb.cpp
- **Section** : 28.6 - Name Mangling (débogage GDB)
- **Description** : Bug intentionnel (déréférencement nullptr) pour démontrer le demangling dans GDB
- **Compilation** : `g++ -Wall -Wextra -g 15_bug_gdb.cpp -o bug_gdb`
- **Note** : Sans `-Werror` (bug intentionnel). Compilé avec `-g` pour symboles de debug.
- **Exécution** : `gdb ./bug_gdb` puis `run` puis `bt` (backtrace avec noms démanglés)
- **Ne pas exécuter directement** : crash SIGSEGV intentionnel

---

## Résumé des exceptions de compilation

| Fichier | Exception | Raison |
|---------|-----------|--------|
| 04_module_python.c | Sans `-pedantic` | Headers Python non conformes ISO |
| 09_struct_layout.c | Sans `-pedantic` | `__attribute__((packed))` extension GCC |
| 15_bug_gdb.cpp | Sans `-Werror` | Bug intentionnel (nullptr dereference) |

## Dépendances externes

| Fichier | Dépendance | Installation |
|---------|-----------|--------------|
| 04-06 | Python 3 + python3-dev | `apt install python3-dev` |
| 07 | Go | `apt install golang` |
| 08 | Rust + Cargo | Via rustup |
| 13 | libdl | Disponible par défaut (glibc) |
